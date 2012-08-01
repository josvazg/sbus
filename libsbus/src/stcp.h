/**@file stcp.h
   @brief Interfaz de servicios de red TCP simplificados

  Interfaz simplificada del servicios TCP, se ocupa de manejar los sockets convenientemente<p>
  
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/

#include <sock.h>
#include <sockaddr.h>

#ifndef STCP
#define STCP

#ifdef __cplusplus
extern "C" {
#endif

/// SocketType definition
typedef int SocketType;

/**----------------------------------------------------------------------
 Portable function to set a socket into nonblocking mode.
 Calling this on a socket causes all future read() and write() calls on
 that socket to do only as much as they can immediately, and return
 without waiting.
 If no data can be read or written, they return -1 and set errno
 to EAGAIN (or EWOULDBLOCK).
 Thanks to Bjorn Reese for this code.
----------------------------------------------------------------------*/
int stcp_setNonBlocking(int fd);

/**
  Crea un socket servidor

  @param ip es la IP local de escucha del servidor TCP ("0.0.0.0","127.0.0.1",...)
  @param port es el puerto de conexión del servidor TCP

  @return -1 en caso de error o el socket servidor creado (número positivo)
*/
int stcp_server(const char* ip, int port);

/**
  Crea un socket cliente conectado

  @param ip es la IP remota del servidor TCP
  @param port es el puerto de conexión del servidor TCP

  @return 0 si todo fue bien y -1 en caso de error
*/
int stcp_client(const char *ip, int port, int blocking);

/**
  Espera un tiempo determinado a que se complete la conexión
*/
int stcp_waitConnection(int sockfd, int timeout);

/**
  Comprueba el estado de SO_ERROR
*/
int stcp_soerror(int sockfd);

/**
  Indica si el otro lado esta escuchando, comprueba la conexión

  @param sockfd es el descriptor del socket a comprobar

  @return 1 en caso de que este vivo 0 en caso de que no este y -1 en caso de error
 */
int stcp_isAlive(int sockfd);

/**
  @brief Envia datos por la conexión dada

  @param sockfd es la conexión TCP
  @param data es el contenedor a usar para los datos
  @param len es la longitud de los datos a enviar

  @return en número de bytes enviados, o -1 en caso de error
 */
int stcp_send(int sockfd, char *data, int len);

/**
  @brief Recibe datos de la conexión dada

  @param sockfd es la conexión TCP
  @param data es el contenedor a usar para los datos
  @param maxdata es la longitud maxima esperada, la longitud del contenedor de datos
  @param timeout es el máximo tiempo en ms que se van a esperar los datos, 0 es 'para siempre'

  @return en número de bytes leidos,
    0 en caso de desconexión o un valor negativo en caso de error
 */
int stcp_trecv(int sockfd, char *data, int maxdata, int timeout);

/**
  Da la IP local del socket
*/
int stcp_getLocalIP(int sockfd);

/**
  Da la IP remota del socket
*/
int stcp_getIP(int sockfd);

/**
  Da el puerto local del socket
*/
int stcp_getLocalPort(int sockfd);

/**
  Da el puerto remoto del socket
*/
int stcp_getPort(int sockfd);

#ifdef __cplusplus
}
#endif

#endif
