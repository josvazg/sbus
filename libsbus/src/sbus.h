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
/** @file sbus.h
   @brief Interfaz de paso de mensajes sobre SBUS

   Interfaz de paso de mensajes sobre SBUS<p>

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/

/// Tamaño máximo del nombre
#define MAX_NAME_SIZE 512
/// IP Multicast por defecto
#define DEFAULT_MCIP "224.0.23.130"
/// IP Puerto por defecto
#define DEFAULT_MCPORT 10001

/// SBUS por defecto
#define sbus_default() sbus_create(NULL,DEFAULT_MCIP,DEFAULT_MCPORT)
/// SBUS en un puerto determinado
#define sbus_atPort(port) sbus_create(NULL,DEFAULT_MCIP,port)
/// SBUS en un dispositivo determinado
#define sbus_atDevice(device) sbus_create(device,DEFAULT_MCIP,DEFAULT_MCPORT)
/// SBUS en un dispositivo y puerto determinados
#define sbus_atIP(device,port) sbus_create(device,DEFAULT_MCIP,port)

/// PING MSGTAG     
#define SBUSPROTOCOL_PING 0
/// FIND MSGTAG
#define SBUSPROTOCOL_FIND 0xFFFF
/// PING/FIND REPLY MSGTAG
#define SBUSPROTOCOL_PINGFIND_REPLY 0

#ifdef __cplusplus
extern "C" {
#endif

#include <sbusdefs.h>

/// Definición de tipo opaca
typedef struct SBusTypedef *SBusType;

/// Backward compatibility
typedef int SBusChType;

/**
 Crea un enlace con el SBUS (se pueden crear varios, uno por thread, por proceso, etc)
 NOTA: No es thread-safe no es coveniente usar el mismo enlace en 2 o más threads

 @param device es el intefaz de red sobre el que va a trabajar el SBUS 
 @param mcip es la dirección de trabajo del SBUS al que se debe enganchar
 @param port es el puerto de trabajo del SBUS al que se debe enganchar

 @return el enlace SBUS o NULL en caso de no poder crearlo adecuadamente
 */
SBusType sbus_create(char *device, char* mcip, int mcport);

/**
 Envia datos por SBUS Multicast

 @param sbus es el enlace SBUS
 @param msgtag es un código identificativo del tipo de mensaje enviado
 @param data son los datos del mensaje
 @param bytes es el tamaño de data

 @return el número de bytes enviados en caso de éxito o -1 en caso de error
*/
int sbus_mcsend(SBusType sbus, int msgtag, char *data, int bytes);

/**
 Envia datos directamente a otro SBUS (sin multicast, punto a punto, directo)

 @param sbus es el enlace SBUS
 @param msgtag es un código identificativo del tipo de mensaje enviado
 @param peerid es el identificativo del receptor
 @param data son los datos del mensaje
 @param bytes es el tamaño de data

 @return el número de bytes enviados en caso de éxito o -1 en caso de error
*/
int sbus_send(SBusType sbus, int msgtag, int ch2peer, char *data, int bytes);

/**
 Recepción Temporizada (en ms) de datos de SBUS a través de este enlace

 @param sbus es el enlace SBUS
 @param msgtag contendrá el código identificativo del tipo de mensaje enviado
 @param peerid contendrá el canal al enviante del mensaje
 @param data son los datos del mensaje

 @return 0 en caso de timeout, el número de bytes recibidos o -1 en caso de error
*/
int sbus_recv(SBusType sbus, int* pmsgtag, int* pch2peer, char *data, int bytes);

/**
 Registers this Sbus with a (hopefully) unique name

 @param sbus this SBus binding object reference
 @param name to register this Sbus with
 
 @return 0 on success, -1 on error
*/
int sbus_setName(SBusType sbus, char* name);

/**
  Get this SBus registered name

  @param sbus this SBus binding object reference

  @return a copy of the name string
*/
char* sbus_getName(SBusType sbus);

/**
  Transforms binary data messages to null terminated strings

  @param str is the destination string, it has to be at least size+1 bytes long
  @param bin is the binary data array
  @param size is the binary data array size
*/
char* sbus_bin2str(char* str,char* bin, int size);

/**
 Cierra y libera los recursos un enlace SBUS

 @param sbus es el enlace SBUS
 
 @return 0 si fue bien y -1 en caso de error
 */
int sbus_dispose(SBusType sbus);

#ifdef __cplusplus
}
#endif
