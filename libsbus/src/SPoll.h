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
/** @file SPoll.h

  Detección Simplificada de sucesos en descriptores de fichero (ficheros, sockets ...)

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <sys/poll.h>

namespace simple {

class SPoll {
  private:
        /// Lista de polls
	struct pollfd* fds;
	/// Tamaño de la lista util
	int size;
	/// Capacidad total de la lista
	int capacity;  
	/// Hace crecer el poller
	int grow();
	/// Borra una posición dada
	int removeit(int i);
  public:
	/// Constructor
	SPoll();
	/// Destructor
	~SPoll();
	/**
	Añade un descriptor de fichero (file descriptor) a la lista de descriptores 'bajo vigilancia'
	
	@param fd es el file descriptor o descriptor de fichero a vigilar
	@param events es la máscara que indica que sucesos se desean vigilar,
	compatible com poll() ('man poll')
	
	@return 0 si todo fue bien y -1 en caso de error
	*/
	int add(int fd, short events);
	/**
	Añade un file descriptor a la lista de descriptores 'bajo vigilancia'
	
	@param fd es el file descriptor o descriptor de fichero a vigilar
	@param events es la máscara que indica que sucesos se desean vigilar, 
	compatible com poll() ('man poll')
	
	@return 0 si todo fue bien y -1 en caso de error 
	(si el fd no se encuentra no se considera fallo)
	*/
	int remove(int fd);
	
	/**
	Realiza el sondeo de sucesos programados
	
	@param timeout (en milisegundos) es el tiempo máximo de espera al suceso, '<0 = para siempre'
	
	@return 0 en caso de timeout(o errno=EINTR), >0 si hubo eventos y -1 en caso de error
	*/
	int doPoll(int timeout);
	
	/**
	Devuelve la lista de sondeos programados
	
	@param pfds es el puntero que se dejara apuntado a la lista
	
	@return el tamaño de la lista o -1 en caso de error
	*/
	int getpolls(struct pollfd** pfds);
};

}
