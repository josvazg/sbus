/** stcp.c
   @brief Servicios de red TCP

  Interfaz simplificada del servicios TCP, se ocupa de manejar los sockets convenientemente<p>
  
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <wait.h>
#include <errno.h>
#include <string.h>
#include <signal.h>
#include <sys/time.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <sys/poll.h>
#include <assert.h>

#include <memdefs.h>
#include <errdefs.h>

#include <stcp.h>

#define INVALID -1

#undef SINGLE_PROCESS

/// Tamaño por defecto de la cola listen
#define LISTEN_QUEUE_SIZE 20

/*----------------------------------------------------------------------
 Portable function to set a socket into nonblocking mode.
 Calling this on a socket causes all future read() and write() calls on
 that socket to do only as much as they can immediately, and return
 without waiting.
 If no data can be read or written, they return -1 and set errno
 to EAGAIN (or EWOULDBLOCK).
 Thanks to Bjorn Reese for this code.
----------------------------------------------------------------------*/
int stcp_setNonBlocking(int fd) {
  int flags;

  /* If they have O_NONBLOCK, use the Posix way to do it */
#if defined(O_NONBLOCK)
  /* Fixme: O_NONBLOCK is defined but broken on SunOS 4.1.x and AIX 3.2.5. */
  if (-1 == (flags = fcntl(fd, F_GETFL, 0))) {
    flags = 0;
  }
  return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
#else
  /* Otherwise, use the old way of doing it */
  flags = 1;
    return ioctl(fd, FIOBIO, &flags);
#endif
}

/**
  Crea un socket servidor

  @param ip es la IP local de escucha del servidor TCP ("0.0.0.0","127.0.0.1",...)
  @param port es el puerto de conexión del servidor TCP

  @return -1 en caso de error o el socket servidor creado (número positivo)
*/
int stcp_server(const char* ip, int port) {
  struct sockaddr_in addr;
  int sockfd;
  int reuseaddr=1;
  // SOCKET
  RET_ON_PERROR((sockfd=socket(PF_INET,SOCK_STREAM, 0)));
  // SO_REUSEADDR
  RET_ON_PERROR(setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &reuseaddr, sizeof(reuseaddr)));
  // BIND
  sockaddr_set(&addr,ip,port);
  RET_ON_PERROR(bind(sockfd, (struct sockaddr*)&addr, sizeof(addr)));
  // LISTEN
  RET_ON_PERROR(listen(sockfd,LISTEN_QUEUE_SIZE));
  return sockfd;
}

/**
  Crea un socket cliente conectado

  @param ip es la IP remota del servidor TCP
  @param port es el puerto de conexión del servidor TCP

  @return el socket creado si todo fue bien y -1 en caso de error
*/
int stcp_client(const char *ip, int port, int blocking) { 
  int sock;
  struct sockaddr_in remote;
  
  // SOCKET
  RET_ON_PERROR((sock=socket(PF_INET,SOCK_STREAM, 0)));
  // No bloqueante
  if(!blocking) {
    stcp_setNonBlocking(sock);
  }
  // ADDR
  sockaddr_set(&remote,ip,port);
  // CONNECT
  if ( connect(sock,(struct sockaddr*)&remote, sizeof(remote)) != 0 ) {
    if((errno=EAGAIN)||(errno=EWOULDBLOCK)) {
      return sock;
    }
    PERROR("Error connect()");
    return -1;
  }
  return sock;
}

/**
  Espera un tiempo determinado a que se complete la conexión
*/
int stcp_waitConnection(int sockfd, int timeout) {
  struct pollfd fds;  
  fds.fd=sockfd;
  fds.events=POLLOUT | POLLERR | POLLHUP | POLLNVAL;
  RET_ON_PERROR(poll(&fds,1,timeout));
  return 0;
}

/**
  Comprueba el estado de SO_ERROR
*/
int stcp_soerror(int sockfd) {
  int soerror;
  int soerrorlen=sizeof(int);
  RET_ON_PERROR(getsockopt(sockfd, SOL_SOCKET, SO_ERROR, &soerror, (socklen_t*)&soerrorlen));
  return soerror;
}

/**
  Indica si el otro lado esta escuchando, comprueba la conexión

  @param sockfd es el descriptor del socket a comprobar

  @return 1 en caso de que este vivo 0 en caso de que no este y -1 en caso de error
 */
int stcp_isAlive(int sockfd) {
  struct pollfd polledsock;
  int res;
  polledsock.fd=sockfd;
  polledsock.events=POLLHUP|POLLIN;
  res=poll(&polledsock,1,0);
  if(res<0) {
    PERROR("Error");
    return 0;
  } else if(res>0) {
    if(polledsock.revents&POLLHUP) {
      return 0;
    }
    if(polledsock.revents&POLLIN) {
      char buf;
      if(recv(sockfd,&buf,1,MSG_PEEK)==0) {
        return 0;
      }
    }
  }
  return 1;
}

/**
  @brief Envia datos por la conexión dada

  @param sockfd es la conexión TCP
  @param data es el contenedor a usar para los datos
  @param len es la longitud de los datos a enviar

  @return en número de bytes enviados, o -1 en caso de error
 */
int stcp_send(int sockfd, char *data, int len) {
  return send(sockfd,data,len,0);
}

/**
  @brief Recibe datos de la conexión dada

  @param sockfd es el socket es la conexión TCP
  @param data es el contenedor a usar para los datos
  @param len longitud máxima de datos a recibir o tamaño de data
  @param timeout es el máximo tiempo en ms que se van a esperar los datos, 0 es 'para siempre'

  @return uno de los siguientes valores:
  - SOCK_TIMEOUT (-2) en caso de timeout
  - SOCK_ERROR   (-1) en caso de error (y errno con el correspondiente valor)
  - SOCK_DISCONN (0) en caso desconexión (0 bytes recibidos)
  - Un valor positivo con el número de bytes recibidos
 */
int sctp_trecv(int sockfd, char *data, int len, int timeout) {
  struct pollfd fds;
  int res;
  
  fds.fd=sockfd;
  fds.events=POLLIN;
  // poll...
  if((res=poll(&fds,1, timeout))<0) {
    return SOCK_ERROR;
  }
  if(res==0) {
    return SOCK_TIMEOUT; 
  }
  // Recibimos los datos
  if(fds.revents&POLLIN) {
    if((res = read(sockfd,data,len))<0) {
      PERROR("Error");
    }
  } else if(res==0) {
    return SOCK_TIMEOUT;
  }
  return res;
}

/**
  Da la IP local del socket
*/
int stcp_getLocalIP(int sockfd) {
  struct sockaddr_in addr;
  int len=sizeof(addr);
  RET_ON_PERROR(getsockname(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len));
  return ntohl(addr.sin_addr.s_addr);
}

/**
  Da la IP remota del socket
*/
int stcp_getIP(int sockfd) {
  struct sockaddr_in addr;
  int len=sizeof(addr);  
  RET_ON_PERROR(getpeername(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len));
  return ntohl(addr.sin_addr.s_addr);
}

/**
  Da el puerto local del socket
*/
int stcp_getLocalPort(int sockfd) {
  struct sockaddr_in addr;
  int len=sizeof(addr);
  RET_ON_PERROR(getsockname(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len));
  return sockaddr_getPort(&addr);
}

/**
  Da el puerto remoto del socket
*/
int stcp_getPort(int sockfd) {
  struct sockaddr_in addr;
  int len=sizeof(addr);
  RET_ON_PERROR(getpeername(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len));
  return sockaddr_getPort(&addr);
}

