/*
      SBUS library: 
      Simple BUS communcations library for distributed software developments
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
/** @file SMsg.h
   @brief Simple Message Object Wrapper
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/

#include <string>
#include <iostream>

#include <stcp.h>

using namespace std;

namespace simple {

class SMsg {
	/// TAG
	short msgtag;
	/// Sender's current IP
	int ip;
	/// Sender's current port
	unsigned short port;
	/// Socket woth peer
	SocketType socket;
	/// Mensaje
	string msg;
	/// Error flag
	bool error;
  public:
  	/// Default Constructor 
	SMsg(short msgtag, int ip, unsigned short port, SocketType socket, string& msg);
	/// Error message
	SMsg(short errcode, int ip, unsigned short port, SocketType socket);
	/// Default Destructor
	~SMsg();
	/// Tells if this is a normal message or an error condition
	bool isError();
	/// Error code getter
	int getErrCode();
	/// Tag getter
	short getMsgTag();
	/// IP getter
	int getIP();
	/// Port getter
	int getPort();
	/// Socket getter
	SocketType getSocket();
	/// Msg getter
	string& getMsg();
};

}

using namespace simple;
