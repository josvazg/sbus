/*
      SBUS library: Simple BUS communications library for distributed software developments
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
/** SPoll.cpp

 Detección Simplificada de sucesos en descriptores de fichero (ficheros, sockets ...)

  José Luis Váquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdlib.h>

#include <errdefs.h>
#include <timing.h>
#include <string.h>
#include <strings.h>

#include <string>
#include <iostream>

#include <stcp.h>
#include <SPoll.h>

using namespace std;
using namespace simple;

/// Tamaño inicial del poll
#define INITIAL_CAPACITY 10
/// Crecimiento del poll
#define GROW_SIZE  5

/// Hace crecer el poller
int SPoll::grow() {  
  int newcapacity;
  ASSERT(fds!=NULL);
  ASSERT(size>=0);
  ASSERT(capacity>=size);
  newcapacity=GROW_SIZE+(capacity);
  /// Reserva de memoria para los polls 
  RET_ON_FALSE((fds=(struct pollfd*)realloc(fds,sizeof(struct pollfd)*newcapacity))!=0);
  /// Limpia los fds sobrantes
  bzero(&fds[capacity],sizeof(struct pollfd)*GROW_SIZE);
  capacity=newcapacity;
  DEBUG("spoll: capacity=%d bytes",capacity);
  return 0;
}

/// Borra una posición dada
int SPoll::removeit(int i) {
  int fd;
  ASSERT(fds!=NULL);
  ASSERT(size>=0);
  ASSERT(capacity>=size);
  ASSERT((i>=0)&&(i<=capacity));
  fd=fds[i].fd;  
  // si quedan sockets en la lista detras...
  if((i+1)<size) { // ... compactación
    memmove(&fds[i],&fds[i+1],sizeof(struct pollfd)*(size-i));
    DEBUG("spoll: fd=%d pos=%d freed (moved positions %d-%d to %d-%d)",fd,i,i+1,size+1,i,size);
  } else { // sino, borrar este ultimo
    bzero(&fds[i],sizeof(struct pollfd));  
    DEBUG("spoll: fd=%d pos=%d freed",fd,i);
  }
  size--;
  return 0;
}

/**
  Crea el objeto poller
 */
SPoll::SPoll() {
  /// Reserva de memoria para los polls 
  if((fds=NEWARRAY(struct pollfd,INITIAL_CAPACITY))==NULL) {
    throw new string("Could not create poll array for SPoll object\n");
  }
  /// Inicio de campos
  size=0;
  capacity=INITIAL_CAPACITY;
}


/**
  Libera la memoria ocupada por el objeto poller
*/
SPoll::~SPoll() {
  int i;
  ASSERT(size>=0);
  ASSERT(capacity>=size);     
  // Limpieza de los suscritos
  for(i=0;i<capacity;i++) {
    if(fds[i].fd!=0) {
      // clean this up
      removeit(i);
    }
  }
  // Limpieza del objeto
  if(fds!=NULL) {
    DELETE(fds);
    fds=NULL;
  }
}

/**
  Añade un descriptor de fichero (file descriptor) a la lista de descriptores 'bajo vigilancia'

  @param fd es el file descriptor o descriptor de fichero a vigilar
  @param events es la máscara que indica que sucesos se desean vigilar, 
    compatible com poll() ('man poll')

  @return 0 si todo fue bien y -1 en caso de error
  */
int SPoll::add(int fd, short events) {
  int i;
  ASSERT(fd>0);
  ASSERT(events!=0);
  ASSERT(fds!=NULL);
  ASSERT(size>=0);
  ASSERT(capacity>=size);  
  if(size>=capacity) {
    RET_ON_ERROR(grow());
  }
  for(i=size;i<capacity;i++) {
    if(fds[i].fd==0) {  // Encontrado hueco
      fds[i].fd=fd;
      fds[i].events=events;
      size++;      
      DEBUG("spoll: fd=%d events=0x%04X pos=%d (capacity=%d)",fd,events,i,capacity);
      return 0;
    }
  }
  ERROR("No space found to add fd=%d",fd);
  return -1;
}

/**
  Añade un file descriptor a la lista de descriptores 'bajo vigilancia'

  @param fd es el file descriptor o descriptor de fichero a vigilar
  @param events es la máscara que indica que sucesos se desean vigilar, 
    compatible com poll() ('man poll')

  @return 0 si todo fue bien y 
          -1 en caso de error (si el fd no se encuentra no se considera fallo)
  */
int SPoll::remove(int fd) {
  int i;
  ASSERT(fds!=NULL);
  ASSERT(size>=0);
  ASSERT(capacity>=size);  
  ASSERT(fd>0);  
  for(i=0;i<capacity;i++) {
    if(fds[i].fd==fd) {  // Encontrado afectado
      removeit(i);
      return 0;
    }
  }
  return 0;
}

/**
  Realiza el sondeo de sucesos programados

  @param timeout (en milisegundos) es el tiempo máximo de espera al suceso, '<0 = para siempre'

  @return 0 en caso de timeout(o errno=EINTR), >0 si hubo eventos y -1 en caso de error
*/
int SPoll::doPoll(int timeout) {
  int pollres;
  ASSERT(size>=0);
  ASSERT(capacity>=size);    
  ASSERT(timeout>=0);
  pollres=poll(fds,size,timeout);
  if(pollres<0) {
    if(errno==EINTR) { // EINTR=Timeout
      return 0;
    }
    PERROR("Error poll()");
    return -1;
  }
  return pollres;
}

/**
  Devuelve la lista de sondeos programados

  @param pfds es el puntero que se dejara apuntado a la lista

  @return el tamaño de la lista o -1 en caso de error
*/
int SPoll::getpolls(struct pollfd** pfds) {
  ASSERT(size>=0);
  ASSERT(capacity>=size);      
  (*pfds)=fds;
  ERROR(const_cast<char*>("hola"));
  log_error(const_cast<char*>("hola"));
  return size;
}


