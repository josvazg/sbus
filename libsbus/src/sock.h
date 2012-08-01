/** @file sock.h

  @brief Definiciones de socket comunes

  Definiciones de socket comunes

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdlib.h>
#include <sys/poll.h>

#include <errdefs.h>
#include <memdefs.h>

#ifndef SOCK
#define SOCK

/// Respuesta a recepción socket desconectado
#define SOCK_DISCONN       0
/// Respuesta a recepción socket de error
#define SOCK_ERROR        -1
/// Respuesta a recepción socket, vencimiento de temporizador
#define SOCK_TIMEOUT      -2

/// IP localhost
#define LOOPBACK_TXT "127.0.0.1"

/// Cualquier IP
#define ANY_IP_TXT "0.0.0.0"

/// IP localhost
#define LOOPBACK_ADDR 0x0100007F

/// IP 'any'
#define ANY_IP_ADDR 0

/// Cualquier Puerto
#define ANY_PORT 0

/// Longitud de la dirección MAC
#define MAC_ADDR_LENGTH 6

/// Longitud de la dirección MAC en texto
#define MAC_ADDR_STR_LENGTH 13

/// Longitud de la dirección IP
#define IP_ADDR_LENGTH 4

/// Longitud de la dirección IP en texto
#define IP_ADDR_STR_LENGTH 16

/// MAC NULA
#define VOID_MAC	  "\0\0\0\0\0\0"

/// Conversión de MAC binaria a texto
#define MAC2STR(macstr,str) sprintf(macstr,"%02X%02X%02X%02X%02X%02X",str[0],str[1],str[2],str[3],str[4],str[5]);

/// Tipo de datos MACADDRESS
typedef unsigned char macaddress[MAC_ADDR_LENGTH];

#endif
