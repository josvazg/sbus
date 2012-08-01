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
/** PeerInfo.cpp
   @brief SBus's peer information on SContacts
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <timing.h>
#include <sockaddr.h>
#include <SPeerInfo.h>

using namespace std;
using namespace simple;

/// Default Constructor 
SPeerInfo::SPeerInfo(SBusPeer peer, SocketType socket, int ip, unsigned short port, string& name) {
  this->peer=peer;
  this->ip=ip;
  this->port=port;
  this->socket=socket;
  this->name=name;
  lastActivity=timing_current_seconds();
}

/// Default Destructor
SPeerInfo::~SPeerInfo() {
}

/// IP & port to addr name
string& SPeerInfo::ipnport2addr(int ip, unsigned short port) {
  char ipStr[MAX_IP_ADDR_STR];
  char portStr[7];
  sockaddr_int2ip(ipStr,ip);
  sprintf(portStr,"%d",port);
  string* addr=new string(string(ipStr)+":"+string(portStr));
  return *addr;
}

/// ID getter
int SPeerInfo::getPeer() {
  lastActivity=timing_current_seconds();
  return peer;
}

/// IP getter
int SPeerInfo::getIP() {
  lastActivity=timing_current_seconds();
  return ip;
}

/// Port getter
unsigned short SPeerInfo::getPort() {
  lastActivity=timing_current_seconds();
  return port;
}

/// Socket getter
SocketType SPeerInfo::getSocket() {
  lastActivity=timing_current_seconds();
  return socket;
}

/// Name getter
string& SPeerInfo::getName() {
  lastActivity=timing_current_seconds();
  return name;
}

/// Addr string getter
string& SPeerInfo::getAddr() {
  lastActivity=timing_current_seconds();
  return SPeerInfo::ipnport2addr(ip,port);
}

/// Last Activity getter
int SPeerInfo::getLastActivity() {
  return lastActivity;
}

