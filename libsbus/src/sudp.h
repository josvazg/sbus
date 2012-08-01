/**@file sudp.h
   @brief Interfaz de servicios de red UDP simplificados

  Interfaz simplificada del servicios UDP, se ocupa de manejar los sockets convenientemente<p>
  
  Jos� Luis V�zquez Gonz�lez <josvazg@terra.es><br>
  LGPL
*/


#ifndef SUDP
#define SUDP

#ifdef __cplusplus
extern "C" {
#endif

/**
  Crea un socket UDP normal 

  @param ip es la direcci�n ip local
  @param port es el puerto de escucha/envio de Multicast

  @return El socket multicast creado o -1 en caso de error
*/
int sudp_create(char* ip, int port);

/**
  Crea un socket Multicast 

  #param device es el dispositivo de la interfaz donde se desea asocial al socket
  @param mcip es la direcci�n ip de multicast
  @param port es el puerto multicast, generalmente se deja a 0  

  @return El socket multicast creado o -1 en caso de error
*/
int sudp_mcast(const char *device, const char* mcip, int port);

/**
  Envia al socket Multicast

  @param sockfd es el socket multicast
  @param data son los datos a enviar
  @param len es la longitud a enviar
  @param mcip es la direcci�n ip de multicast
  @param port es el puerto de escucha/envio de Multicast  

  @return 0 si hubo �xito y -1 en caso de error
*/
int sudp_mcsend(int sockfd, char* data, int len, const char* mcip, int port);

/**
  Cierra el socket multicast dejando tambi�n el grupo

  @param sockfd es el socket multicast  
  #param device es el dispositivo de la interfaz donde se desea asociar al socket
  @param mcip es la direcci�n ip de multicast  
  
  @return 0 si fue bien y -1 en caso de error
*/
int sudp_mclose(int sockfd, const char* device, const char* mcip);

/**
  Crea un socket Broadcast

  @param bcip es la direcci�n ip de broadcast
  @param port es el puerto de escucha/envio de Multicast

  @return El socket multicast creado o -1 en caso de error
*/
int sudp_bcast(const char* bcip, int port);

/**
  Envia al socket Broadcast

  @param sockfd es el socket broadcast
  @param data son los datos a enviar
  @param len es la longitud a enviar

  @return 0 si hubo �xito y -1 en caso de error
*/
int sudp_bcsend(int sockfd, char* data, int len);

/**
  Recibe del socket UDP esperando para ello un tiempo determinado

  @param sockfd es el socket 
  @param sender es la direcci�n del enviante
  @param data es el lugar donde copiar los datos recibidos
  @param len es la m�xima longitud a recibir
  @param timeout es el tiempo m�ximo esperado en milisegundos, 0 indica bloqueo total

  @return uno de los siguientes valores:
  - SOCK_TIMEOUT (-2) en caso de timeout
  - SOCK_ERROR   (-1) en caso de error (y errno con el correspondiente valor)
  - SOCK_DISCONN (0) en caso desconexi�n (0 bytes recibidos)
  - Un valor positivo con el n�mero de bytes recibidos
*/
int sudp_trecv(int sockfd, struct sockaddr_in* sender, char* data, int len, int timeout);

/**
  Da la IP local del socket
*/
int sudp_getLocalIP(int sockfd);

/**
  Da el puerto local del socket
*/
int sudp_getLocalPort(int sockfd);


#ifdef __cplusplus
}
#endif

#endif
