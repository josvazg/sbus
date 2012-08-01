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
/** spoll.c

 Detección Simplificada de sucesos en descriptores de fichero (ficheros, sockets ...)

  José Luis Váquez González <josvazg@terra.es><br>
  LGPL
*/

// Obsoleto a borrar

#include <string.h>
#include <stdlib.h>

#include <memdefs.h>
#include <errdefs.h>
#include <timing.h>

#include <stcp.h>
#include <spoll.h>

#define INITIAL_CAPACITY 10

#define GROW_SIZE  5

/// Estructura de un objeto poller
typedef struct poller_datatype {
  struct pollfd* fds;
  int size;
  int capacity;
}poller_datatype;

/// Hace crecer el poller
static int spoll_grow(poller_datatype* opoller) {  
  int newcapacity;
  ASSERT(opoller!=NULL);
  ASSERT(opoller->fds!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);
  newcapacity=GROW_SIZE+(opoller->capacity);
  /// Reserva de memoria para los polls 
  RET_ON_FALSE((opoller->fds=(struct pollfd*)realloc(opoller->fds,sizeof(struct pollfd)*newcapacity))!=0);
  /// Limpia los fds sobrantes
  bzero(&opoller->fds[opoller->capacity],sizeof(struct pollfd)*GROW_SIZE);
  opoller->capacity=newcapacity;
  DEBUG("spoll: capacity=%d bytes",opoller->capacity);
  return 0;
}

/// Borra un aposición dada
static void spoll_removeit(poller_datatype* opoller, int i) {
  int fd;
  ASSERT(opoller!=NULL);
  ASSERT(opoller->fds!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);
  ASSERT((i>=0)&&(i<=opoller->capacity));
  fd=opoller->fds[i].fd;  
  // si quedan sockets en la lista detras...
  if((i+1)<opoller->size) { // ... compactación
    memmove(&opoller->fds[i],&opoller->fds[i+1],sizeof(struct pollfd)*(opoller->size-i));
    DEBUG("spoll: fd=%d pos=%d freed (moved positions %d-%d to %d-%d)"
      ,fd,i,i+1,opoller->size+1,i,opoller->size);
  } else { // sino, borrar este ultimo
    bzero(&opoller->fds[i],sizeof(struct pollfd));  
    DEBUG("spoll: fd=%d pos=%d freed",fd,i);
  }
  opoller->size--;
}

/**
  Crea el objeto poller
 */
void* spoll_create() {
  /// Creación del objeto
  poller_datatype* poller;
  RET_NULL_ON_FALSE((poller=NEWTYPE(poller_datatype)));
  /// Reserva de memoria para los polls 
  RET_NULL_ON_FALSE((poller->fds=NEWARRAY(struct pollfd,INITIAL_CAPACITY)));
  if(poller->fds==NULL) {
    free(poller); 
    return NULL; 
  }
  /// Inicio de campos
  poller->size=0;
  poller->capacity=INITIAL_CAPACITY;
  return poller;
}

/**
  Añade un descriptor de fichero (file descriptor) a la lista de descriptores 'bajo vigilancia'

  @param spoll es el objeto poller (='this')
  @param fd es el file descriptor o descriptor de fichero a vigilar
  @param events es la máscara que indica que sucesos se desean vigilar, 
    compatible com poll() ('man poll')

  @return 0 si todo fue bien y -1 en caso de error
  */
int spoll_add(void* spoll, int fd, short events) {
  poller_datatype* opoller=(poller_datatype*)spoll;
  int i;
  ASSERT(spoll!=NULL);
  ASSERT(fd>0);
  ASSERT(events!=0);
  RET_ON_FALSE(opoller!=0);
  ASSERT(opoller->fds!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);  
  if(opoller->size>=opoller->capacity) {
    RET_ON_ERROR(spoll_grow(opoller));
  }
  for(i=opoller->size;i<opoller->capacity;i++) {
    if(opoller->fds[i].fd==0) {  // Encontrado hueco
      opoller->fds[i].fd=fd;
      opoller->fds[i].events=events;
      opoller->size++;      
      DEBUG("spoll: fd=%d events=0x%04X pos=%d (capacity=%d)",fd,events,i,opoller->capacity);
      return 0;
    }
  }
  ERROR("No space found to add fd=%d",fd);
  return -1;
}

/**
  Añade un file descriptor a la lista de descriptores 'bajo vigilancia'

  @param spoll es el objeto poller (='this')
  @param fd es el file descriptor o descriptor de fichero a vigilar
  @param events es la máscara que indica que sucesos se desean vigilar, 
    compatible com poll() ('man poll')

  @return 0 si todo fue bien y 
          -1 en caso de error (si el fd no se encuentra no se considera fallo)
  */
int spoll_remove(void* spoll, int fd) {
  poller_datatype* opoller=(poller_datatype*)spoll;
  int i;
  ASSERT(opoller!=NULL);
  ASSERT(opoller->fds!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);  
  ASSERT(fd>0);  
  RET_ON_FALSE(opoller!=0);
  for(i=0;i<opoller->capacity;i++) {
    if(opoller->fds[i].fd==fd) {  // Encontrado afectado
      spoll_removeit(opoller,i);
      return 0;
    }
  }
  return 0;
}

/**
  Realiza el sondeo de sucesos programados

  @param poller es el objeto poller (='this')
  @param timeout (en milisegundos) es el tiempo máximo de espera al suceso, '<0 = para siempre'

  @return 0 en caso de timeout(o errno=EINTR), >0 si hubo eventos y -1 en caso de error
*/
int spoll_poll(void* spoll, int timeout) {
  int pollres;
  poller_datatype* opoller=(poller_datatype*)spoll;
  ASSERT(opoller!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);    
  ASSERT(timeout>=0);
  RET_ON_FALSE(opoller!=0);
  pollres=poll(opoller->fds,opoller->size,timeout);
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

  @param spoll es el objeto poller (='this')
  @param pfds es el puntero que se dejara apuntado a la lista

  @return el tamaño de la lista o -1 en caso de error
*/
int spoll_getpolls(void *spoll,struct pollfd** pfds) {
  poller_datatype* opoller=(poller_datatype*)spoll;
  ASSERT(opoller!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);      
  RET_ON_FALSE(opoller!=0);
  (*pfds)=opoller->fds;
  return opoller->size;
}

/**
  Libera la memoria ocupada por el objeto poller

  @param spoll es el objeto poller (='this')
*/
int spoll_dispose(void* spoll) {
  int i;
  poller_datatype* opoller=(poller_datatype*)spoll;
  ASSERT(opoller!=NULL);
  ASSERT(opoller->size>=0);
  ASSERT(opoller->capacity>=opoller->size);   
  RET_ON_FALSE(opoller!=0);  
  // Limpieza de los suscritos
  for(i=0;i<opoller->capacity;i++) {
    if(opoller->fds[i].fd!=0) {
      // clean this up
      spoll_removeit(opoller,i);
    }
  }
  // Limpieza del objeto
  if(opoller!=NULL) {
    if(opoller->fds!=NULL) {
      DELETE(opoller->fds);
      opoller->fds=NULL;
    }
    DELETE(opoller);
  }
  return 0;
}

