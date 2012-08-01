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
/** SMsg.cpp
   @brief Simple Message Object Wrapper
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <SMsg.h>

using namespace simple;

/// Default Constructor 
SMsg::SMsg(short msgtag, int ip, unsigned short port, SocketType socket, string& msg) {
  this->msgtag=msgtag;
  this->ip=ip;
  this->port=port;
  this->socket=socket;
  this->msg=msg;
  this->error=false;
}

/// Error message
SMsg::SMsg(short errcode, int ip, unsigned short port, SocketType socket) {
  this->msgtag=errcode;
  this->ip=ip;
  this->port=port;
  this->socket=socket;
  this->error=true;
}

/// Default Destructor
SMsg::~SMsg() {
}

/// Tells if this is a normal message or an error condition
bool SMsg::isError() {
  return error;
}

/// Error code getter
int SMsg::getErrCode() {
  return msgtag;
}

/// Tag getter
short SMsg::getMsgTag() {
  return msgtag;
}

/// IP getter
int SMsg::getIP() {
  return ip;
}

/// Port getter
int SMsg::getPort() {
  return port;
}

/// Socket getter
SocketType SMsg::getSocket() {
  return socket;
}

/// Msg getter
string& SMsg::getMsg() {
  return msg;
}


