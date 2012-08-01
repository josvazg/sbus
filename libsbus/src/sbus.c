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
      License along with thisobj library; if not, write to the Free Software
      Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA 02111-1307,
      USA.
*/
/** sbus.c
   @brief Interfaz de paso de mensajes sobre SBUS

   Interfaz de paso de mensajes sobre SBUS<p>

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>

#include <errdefs.h>
#include <stcp.h>
#include <sudp.h>
#include <spoll.h>

#include <sbus.h>
#include <SBus.h>

/// Timeout para esperas internas del SBUS
#define SBUS_INTERNAL_TIMEOUT_MS 25

/// Timempo de espera en la búsqueda find
#define SBUS_FIND_TIME_MS 1600

/// Tiempo máximo de expera de conexión
#define SBUS_CONN_TIMEOUT_MS 700

/// Contexto del enlace al SBUS
typedef struct SBusTypedef {
  SBus* sbus;
} SBusTypedef;

/**
 Crea un enlace con el SBUS (se pueden crear varios, uno por thread, por proceso, etc)
 NOTA: No es thread-safe no es coveniente usar el mismo enlace en 2 o más threads

 @param device es el intefaz de red sobre el que va a trabajar el SBUS 
 @param mcip es la dirección de trabajo del SBUS al que se debe enganchar
 @param port es el puerto de trabajo del SBUS al que se debe enganchar

 @return el enlace SBUS o NULL en caso de no poder crearlo adecuadamente
 */
SBusType sbus_create(char *device, char* mcip, int mcport) {
  SBusType sbus=new SBusTypedef;
  bzero(sbus,sizeof(SBusTypedef));
  if(sbus==NULL) {
    ERROR("Could not allocate SBusType");
    return NULL;
  }
  try {
    sbus->sbus=new SBus(device,mcip,mcport);
  } catch(string s) {
    ERROR("Could not instatiate inner Object SBus");
    return NULL;
  }
  return sbus;
}

/**
 Registers this Sbus with a (hopefully) unique name

 @param sbus this SBUS binding object reference  
 @param name to register this SBus with

 @return 0 on success, -1 on error
*/
int sbus_setName(SBusType sbus, char* name) {
  string _name=name;
  return sbus->sbus->setName(_name);
}

/**
  Get this SBus registered name
  @return a copy of the name string
*/
char* sbus_getName(SBusType sbus) {
  return (char*)sbus->sbus->getName().c_str();
}

/**
 Envia datos por SBUS Multicast

 @param libsbus es el enlace SBUS
 @param msgtag es un código identificativo del tipo de mensaje enviado
 @param data son los datos del mensaje

 @param bytes es el tamaño de data

 @return el número de bytes enviados en caso de éxito o -1 en caso de error
*/
int sbus_mcsend(SBusType sbus, int msgtag, char *data, int bytes) {
  string _data(data,bytes);
  return sbus->sbus->send(msgtag,_data);    
}

/**
 Envia datos directamente a otro SBUS (sin multicast, punto a punto, directo)

 @param libsbus es el enlace SBUS
 @param msgtag es un código identificativo del tipo de mensaje enviado
 @param peerid es el identificativo del receptor
 @param data son los datos del mensaje
 @param bytes es el tamaño de data

 @return el número de bytes enviados en caso de éxito o -1 en caso de error
*/
int sbus_send(SBusType sbus, int msgtag, int peerid, char *data, int bytes) {
  string _data(data,bytes);
  return sbus->sbus->send(msgtag,(SBusPeer)peerid,_data);    
}

/**
 Recepción Temporizada (en ms) de datos de SBUS a través de este enlace

 @param libsbus es el enlace SBUS
 @param msgtag contendrá el código identificativo del tipo de mensaje enviado
 @param peerid contendrá el identificativo del enviante del mensaje
 @param data son los datos del mensaje
 @param bytes es el tamaño máximo de data

 @return bytes recibidos, 0 en caso de timeout o -1 en caso de error
*/
int sbus_recv(SBusType sbus, int* pmsgtag, int* pch2peer, char *data, int bytes) {
  int res;
  string _data(data,bytes);
  res=sbus->sbus->recv(pmsgtag,(SBusPeer*)pch2peer,_data);
  if(res>0) {
    memcpy(data,_data.data(),res);
  }
  return res;
}

/**
 (Re)sets the initial secure SBus name
 */
void sbus_setDefaultSafeName(SBusType sbus) {
  sbus->sbus->setDefaultSafeName();
}

/**
  Transforms binary data messages to null terminated strings

  @param str is the destination string, it has to be at least size+1 bytes long
  @param bin is the binary data array
  @param size is the binary data array size
*/
char* sbus_bin2str(char* str,char* bin, int size) {
  memcpy(str,bin,size);
  str[size]='\0';
  return str;
}

/**
 Cierra y libera los recursos un enlace SBUS

 @param libsbus es el enlace SBUS

 @return 0 si fue bien y -1 en caso de error
 */
int sbus_dispose(SBusType sbus) {
  delete(sbus->sbus);
  delete(sbus);
  return 0;
}

