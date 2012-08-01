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
/** @file spoll.h

  Detección Simplificada de sucesos en descriptores de fichero (ficheros, sockets ...)

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/

// Obsoleto a borrar

#include <sys/poll.h>

#ifdef __cplusplus
extern "C" {
#endif

/// Crea el objeto poller
void* spoll_create();

/**
  Añade un descriptor de fichero (file descriptor) a la lista de descriptores 'bajo vigilancia'

  @param spoll es el objeto poller (='this')
  @param fd es el file descriptor o descriptor de fichero a vigilar
  @param events es la máscara que indica que sucesos se desean vigilar,
    compatible com poll() ('man poll')

  @return 0 si todo fue bien y -1 en caso de error
  */
int spoll_add(void* spoll, int fd, short events);

/**
  Añade un file descriptor a la lista de descriptores 'bajo vigilancia'

  @param spoll es el objeto poller (='this')
  @param fd es el file descriptor o descriptor de fichero a vigilar
  @param events es la máscara que indica que sucesos se desean vigilar, 
    compatible com poll() ('man poll')

  @return 0 si todo fue bien y -1 en caso de error 
    (si el fd no se encuentra no se considera fallo)
  */
int spoll_remove(void* spoll, int fd);

/**
  Realiza el sondeo de sucesos programados

  @param poller es el objeto poller (='this')
  @param timeout (en milisegundos) es el tiempo máximo de espera al suceso, '<0 = para siempre'

  @return 0 en caso de timeout(o errno=EINTR), >0 si hubo eventos y -1 en caso de error
*/
int spoll_poll(void* spoll, int timeout);

/**
  Devuelve la lista de sondeos programados

  @param spoll es el objeto poller (='this')
  @param pfds es el puntero que se dejara apuntado a la lista

  @return el tamaño de la lista o -1 en caso de error
*/
int spoll_getpolls(void *spoll,struct pollfd** pfds);

/**
  Libera la memoria ocupada por el objeto poller

  @param spoll es el objeto poller (='this')
  
  @return 0 si fue bien o -1 en caso de error
*/
int spoll_dispose(void* spoll);

#ifdef __cplusplus
}
#endif
