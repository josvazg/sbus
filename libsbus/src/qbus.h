/*
      SBUS library: 
      Simple BUS communcations library for distributed software developments
      Copyright (C) 2004  Jos� Luis V�zquez Gonz�lez <josvazg@terra.es>
      
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
/** @file qbus.h
   @brief Envoltorio C de colas de envio y recepci�n para SBUS
   Incluye una capa con una hebra y una cola para envio y 
   otra cola y hebra para recepci�n

  Jos� Luis V�zquez Gonz�lez <josvazg@terra.es><br>
  LGPL
*/

#ifdef __cplusplus
extern "C" {
#endif

#include <sbus.h>


/// Definici�n de tipo opaca
typedef struct QBusTypedef *QBusType;

/**
 Envuelve un SBus con acceso Autom�tico

 @param sbus es el enlace sbus a envolver

 @return el ASBUS o NULL en caso de no poder crearlo adecuadamente
 */
QBusType qbus_create(SBusType sbus);

/**
 Obtiene el tama�o actual de la cola de recepci�n en mensajes
 
 @param qbus es el ASBUS 
 @param recvQueue es el tama�o de la cola de recepci�n
 	0 significa que se tiran todos los mensajes
	>0 significa que se tiran los mensajes m�s viejos de tama�o mayor a queueSize
	<0 significa que no se tira ningun mensaje por mucho que crezca la cola
 
 @return el tama�o de la cola o -1 en caso de error
 */
int qbus_getRecvQueue(QBusType qbus);

/**
  Da el nombre registrado de ESTE SBUS
  
 @param qbus es el ASBUS   
 @param name es el buffer donde copiar el nombre (evita que lo sobrescriba el cliente)
 
 @return el puntero name con el nombre encontrado
*/
char* qbus_getName(QBusType qbus, char *name);

/**
 Envia datos por SBUS Multicast

 @param qbus es el ASBUS 
 @param msgtag es un c�digo identificativo del tipo de mensaje enviado
 @param data son los datos del mensaje
 @param bytes es el tama�o de data

 @return el n�mero de bytes enviados en caso de �xito o -1 en caso de error
*/
int qbus_mcsend(QBusType qbus, int msgtag, char *data, int bytes);

/**
 Envia datos directamente a otro SBUS (sin multicast, punto a punto, directo)

 @param qbus es el ASBUS 
 @param msgtag es un c�digo identificativo del tipo de mensaje enviado
 @param peerid es el identificativo del receptor
 @param data son los datos del mensaje
 @param bytes es el tama�o de data

 @return el n�mero de bytes enviados en caso de �xito o -1 en caso de error
*/
int qbus_send(QBusType qbus, int msgtag, long long int peerid, char *data, int bytes);

/**
 Recepci�n de datos de SBUS a trav�s de este enlace

 @param qbus es el ASBUS 
 @param msgtag contendr� el c�digo identificativo del tipo de mensaje enviado
 @param peerid contendr� el identificativo del enviante del mensaje
 @param data son los datos del mensaje
 @param bytes es el tama�o m�ximo de data

 @return 0 en caso de timeout, el n�mero de bytes recibidos o -1 en caso de error
*/
int qbus_recv(QBusType qbus, int* pmsgtag, long long int* ppeerid, char *data, int bytes);

/**
 Cierra y libera los recursos un enlace SBUS

 @param qbus es el ASBUS 
 
 @return 0 si fue bien y -1 en caso de error
 */
int qbus_dispose(QBusType qbus);

#ifdef __cplusplus
}
#endif
