/*
      SBUS library: Simple BUS communcations library for distributed software developments
      Copyright (C) 2004  José Luis Vázquez González <josvazg@terra.es>

      This library is free software; you can redistribute it and/or modify it
      under the terms of the GNU Lesser General Public License as published by
      the Free Software Foundation; either version 2.1 of the License, or (at
      your option) any later version.

      This library is distributed in the hope that it will be useful, but
      WITHOUT ANY WARRANTY; without even the implied warranty of
      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
      Lesser General Public License for more details.

      You should have received a copy of the GNU Lesser General Public
      License along with this library; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
      USA.
*/
/** @file sockaddr.h

 @brief Funciones de manejo de las direcciones IP

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include <sock.h>

#define LO "lo"

#define ETH0 "eth0"

#define ETH1 "eth1"

#define DEFAULT_DEVICE LO

#define MAX_EXPECTED_INTERFACES 64

#define MAX_IP_ADDR_STR 16

#ifdef __cplusplus
extern "C" {
#endif

/**
  Obtención de la IP en forma de texto asociada a esta dirección
 */
char* sockaddr_getIPSTR(char *str, struct sockaddr_in* addr);

/**
  Conversión de ip numerica a IP en texto
 */
char* sockaddr_int2ip(char *str, int ip);

/**
  Obtención del puerto asociado a esta dirección
 */
int sockaddr_getPort(struct sockaddr_in* addr);

/**
  Obtención de la IP numérica asociada a esta dirección
 */
int sockaddr_getIP(struct sockaddr_in* addr);

/**
 Inicia una direccion IP dadas la IP y el puerto
 */
int sockaddr_set(struct sockaddr_in* addr, const char *ip, int port);

/**
  Obtiene la MAC del dispositivo dado

  @param mac es la MAC del
  @param device es el nombre del dispositivo

  @return 0 si fue bien y -1 en caso de error
*/
int sockaddr_getMAC(macaddress mac, const char *device);

/**
  Obtiene la MAC del dispositivo dado en forma de cadena de caracteres

  @param macstr es la cadena donde escribir la MAC
  @param device es el nombre del dispositivo / interfaz de red

  @return el puntero a macstr o NULL en caso de error
 */
char* sockaddr_getMACStr(char *macstr, const char *device);

/**
  Devuelve el número de interfaces disponibles
*/
int sockaddr_interfaceCount();

/**
  Devuelve las ips locales
*/
int sockaddr_ips(unsigned int *ips);

/**
  Obtiene la IP de red de un dispositivo dado en forma de entero

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_device2ip(const char *device);

/**
  Obtiene la máscara de red de un dispositivo dado en forma de entero

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_getMask(const char *device);

/**
  Obtiene la máscara de red de un dispositivo dado en forma de entero

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_getMask(const char *device);

/**
  Obtiene la dirección de broadcast

  @param device es el nombre del dispositivo / interfaz de red

  @return -1 en caso de error o la máscara
*/
int sockaddr_getBroadcast(const char *device);

#ifdef __cplusplus
}
#endif
