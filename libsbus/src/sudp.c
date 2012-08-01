/** sudp.c
   @brief Interfaz de servicios de red UDP simplificados

  Interfaz simplificada del servicios UDP, se ocupa de manejar los sockets convenientemente<p>
  
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <netdb.h>
#include <unistd.h>
#include <string.h>
#include <sys/poll.h>

#include <errdefs.h>

#include <sockaddr.h>
#include <sudp.h>

/**
  Crea un socket UDP normal 

  @param ip es la dirección ip local
  @param port es el puerto de escucha/envio de Multicast

  @return El socket multicast creado o -1 en caso de error
*/
int sudp_create(const char* ip, int port) {
  int sock;
  struct sockaddr_in addr;
  // create socket
  RET_ON_PERROR((sock = socket(AF_INET,SOCK_DGRAM,0)));
  // bind port
  sockaddr_set(&addr,ip,port);
  RET_ON_PERROR(bind(sock,(struct sockaddr *) &addr, sizeof(addr)));
  return sock;  
}

/**
  Crea un socket Multicast 

  #param device es el dispositivo de la interfaz donde se desea asociar al socket
  @param mcip es la dirección ip de multicast
  @param port es el puerto multicast, generalmente se deja a 0  

  @return El socket multicast creado o -1 en caso de error
*/
int sudp_mcast(const char *device, const char* mcip, int port) {
  int sock;
  struct in_addr mcaddr;
  struct sockaddr_in addr;
  struct hostent *h;
  struct ip_mreq mreq;
  struct in_addr ifaddr;
  int reuseaddr=1;
  unsigned char ttl = 64;
  unsigned char loop = 1;
  // get mcast address to listen to
  h=gethostbyname(mcip);
  if(h==NULL) {
    ERROR("Unknown group '%s'\n",mcip);
    return -1;
  }
  memcpy(&mcaddr, h->h_addr_list[0],h->h_length);
  // check given address is multicast
  RET_ON_FALSE(IN_MULTICAST(ntohl(mcaddr.s_addr)));
  // create socket
  RET_ON_PERROR((sock = socket(AF_INET,SOCK_DGRAM,0)));
  // Reuseaddr para poderlo usar desde varios procesos/threads
  RET_ON_PERROR(setsockopt(sock,SOL_SOCKET,SO_REUSEADDR,&reuseaddr,sizeof(reuseaddr)));
  // bind port
  sockaddr_set(&addr,ANY_IP_TXT,port);
  RET_ON_PERROR(bind(sock,(struct sockaddr *) &addr, sizeof(struct sockaddr_in)));
  // join multicast group
  mreq.imr_multiaddr.s_addr=mcaddr.s_addr;
  // Calcula dirección a la que asociarse según dispositivo
  if(device!=NULL) {
    ifaddr.s_addr=htonl(sockaddr_device2ip(device));
    mreq.imr_interface.s_addr=htonl(sockaddr_device2ip(device));
  } else {
    ifaddr.s_addr=INADDR_ANY;
    mreq.imr_interface.s_addr=INADDR_ANY;
  }
  RET_ON_PERROR(setsockopt(sock,IPPROTO_IP,IP_ADD_MEMBERSHIP,(void *) &mreq, sizeof(mreq)));
  // Set outgoing interface
  RET_ON_PERROR(setsockopt(sock,IPPROTO_IP,IP_MULTICAST_IF, &ifaddr,sizeof(ifaddr)));  
  // Set TTL
  RET_ON_PERROR(setsockopt(sock,IPPROTO_IP,IP_MULTICAST_TTL, &ttl,sizeof(ttl)));
  // Set Loopback
  RET_ON_PERROR(setsockopt(sock,IPPROTO_IP,IP_MULTICAST_LOOP, &loop,sizeof(loop)));  
  return sock;
}

/**
  Envia al socket Multicast

  @param sockfd es el socket multicast
  @param data son los datos a enviar
  @param len es la longitud a enviar
  @param mcip es la dirección ip de multicast
  @param port es el puerto de escucha/envio de Multicast  

  @return 0 si hubo éxito y -1 en caso de error
*/
int sudp_mcsend(int sockfd, char* data, int len, const char* mcip, int port) {
  struct sockaddr_in addr;
  sockaddr_set(&addr,mcip,port);
  return sendto(sockfd,data,len,0,(struct sockaddr*)&addr,sizeof(addr));
}

/**
  Cierra el socket multicast dejando también el grupo

  @param sockfd es el socket multicast  
  #param device es el dispositivo de la interfaz donde se desea asociar al socket
  @param mcip es la dirección ip de multicast  
  
  @return 0 si fue bien y -1 en caso de error
*/
int sudp_mclose(int sockfd, const char* device, const char* mcip) {
  struct hostent *h;
  struct in_addr mcaddr;  
  struct ip_mreq mreq;   
  // get mcast address to listen to
  h=gethostbyname(mcip);
  if(h==NULL) {
    ERROR("Unknown group '%s'\n",mcip);
    return -1;
  }
  memcpy(&mcaddr, h->h_addr_list[0],h->h_length);
  // check given address is multicast
  RET_ON_FALSE(IN_MULTICAST(ntohl(mcaddr.s_addr)));
  mreq.imr_multiaddr.s_addr=mcaddr.s_addr;
  // Calcula dirección a la que asociarse según dispositivo
  if(device!=NULL) {
    mreq.imr_interface.s_addr=htonl(sockaddr_device2ip(device));
  } else {
    mreq.imr_interface.s_addr=INADDR_ANY;
  }  
  RET_ON_PERROR(setsockopt(sockfd,IPPROTO_IP,IP_DROP_MEMBERSHIP,(void *) &mreq, sizeof(mreq)));
  close(sockfd);
  return 0;
}

/**
  Crea un socket Broadcast

  @param bcip es la dirección ip de broadcast
  @param port es el puerto de escucha/envio de Multicast

  @return El socket multicast creado o -1 en caso de error
*/
int sudp_bcast(const char* bcip, int port) {
  const int on=1;
  int sock;
  struct sockaddr_in addr;
  RET_ON_PERROR((sock=socket(PF_INET,SOCK_DGRAM,0)));
  RET_ON_PERROR(setsockopt(sock,SOL_SOCKET, SO_BROADCAST, &on, sizeof(on)));
  sockaddr_set(&addr,bcip,port);
  RET_ON_PERROR(bind(sock,(struct sockaddr*)&addr,sizeof(addr)));
  return sock;
}

/**
  Envia al socket Broadcast

  @param sockfd es el socket broadcast
  @param data son los datos a enviar
  @param len es la longitud a enviar

  @return 0 si hubo éxito y -1 en caso de error
*/
int sudp_bcsend(int sockfd, char* data, int len) {
  struct sockaddr_in addr;
  int addrlen=sizeof(addr);
  RET_ON_PERROR(getsockname(sockfd, (struct sockaddr*)&addr, (socklen_t*)&addrlen));
  return sendto(sockfd,data,len,0,(struct sockaddr*)&addr,sizeof(addr));
}

/**
  Recibe del socket UDP esperando para ello un tiempo determinado

  @param sockfd es el socket 
  @param sender es la dirección del enviante
  @param data es el lugar donde copiar los datos recibidos
  @param len es la máxima longitud a recibir
  @param timeout es el tiempo máximo esperado en milisegundos, 0 indica bloqueo total

  @return uno de los siguientes valores:
  - SOCK_TIMEOUT (-2) en caso de timeout
  - SOCK_ERROR   (-1) en caso de error (y errno con el correspondiente valor)
  - SOCK_DISCONN (0) en caso desconexión (0 bytes recibidos)
  - Un valor positivo con el número de bytes recibidos
*/
int sudp_trecv(int sockfd, struct sockaddr_in* from, char* data, int len, int timeout) {
  int addrlen;
  int res;
  struct pollfd fds;

  addrlen = sizeof(struct sockaddr_in);
  fds.fd=sockfd;
  fds.events=POLLIN;
  if((res=poll(&fds,1, timeout))<0) {
    return SOCK_ERROR;
  }
  if(res==0) {
    return SOCK_TIMEOUT; 
  }
  // Recibimos los datos
  from->sin_addr.s_addr=0;
  from->sin_port=0;
  if(fds.revents&POLLIN) {
    if((res = recvfrom(sockfd,data,len,0,(struct sockaddr*)from,(socklen_t*)&addrlen))<0) {
      PERROR("Error");
      return SOCK_ERROR;
    }
  } else if(res==0) {
    return SOCK_TIMEOUT;
  }
  return res;
}

/**
  Da la IP local del socket
*/
int sudp_getLocalIP(int sockfd) {
  struct sockaddr_in addr;
  int len=sizeof(addr);
  RET_ON_PERROR(getsockname(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len));
  return addr.sin_addr.s_addr;
}

/**
  Da el puerto local del socket
*/
int sudp_getLocalPort(int sockfd) {
  struct sockaddr_in addr;
  int len=sizeof(addr);
  RET_ON_PERROR(getsockname(sockfd, (struct sockaddr*)&addr, (socklen_t*)&len));
  return sockaddr_getPort(&addr);
}

