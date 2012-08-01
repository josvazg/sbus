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
/** @file PeerInfo.h
   @brief SBus's peer information on SContacts
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <string>
#include <iostream>

#include <stcp.h>

typedef int SBusPeer;

using namespace std;

namespace simple {

class SPeerInfo {
  private:
        /// Unique ID for this PeerInfo object
	SBusPeer peer;
	/// IP
	int ip;
	/// port
	unsigned short port;
	/// Socket
	SocketType socket;
	/// Name
	string name;
	/// Last Activity
	int lastActivity;
  public:
	/// Default Constructor
	SPeerInfo(SBusPeer peer, SocketType socket, int ip, unsigned short port, string& name);
	/// Default Destructor
	~SPeerInfo();
        /// IP & port to addr name
static string& ipnport2addr(int ip, unsigned short port);
	/// ID getter
	SBusPeer getPeer();
	/// IP getter
	int getIP();
	/// Port getter
	unsigned short getPort();
	/// Socket getter
	SocketType getSocket();
	/// Name getter
	string& getName();
	/// Addr string getter
        string& getAddr();
	/// Last Activity getter
	int getLastActivity();
};

}

using namespace simple;
