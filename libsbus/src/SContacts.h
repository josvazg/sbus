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
/** @file SContacts.h
   @brief SBUS's simple contacts list, keeps track of known peers
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <string>
#include <iostream>

#include <hashdefs.h>
#include <SPeerInfo.h>

using namespace std;

typedef hash_map<SBusPeer,SPeerInfo*> PeerIndexHash;

typedef hash_map<string,SPeerInfo*> StringIndexHash;

typedef hash_map<SocketType,SPeerInfo*> SocketIndexHash;


namespace simple {

class SContacts {
  private:
        /// Last asigned peer sequence number
	SBusPeer lastPeerId;
	/// Contacts table on a queue
	PeerIndexHash peers;
	/// Indexed contacts by address (ip+port)
	StringIndexHash addr2peer;
	/// Indexed by name
	StringIndexHash name2peer;
	/// Indexed by socket
	SocketIndexHash socket2peer;
	/// Last clean up made
	int lastCleanUp;
	/// Does some old unused contacts cleanup
	void doCleanUp();
	/// Erases an entry completely from all references
	void erase(SPeerInfo* peerInfo);
	/// Shows internal state of contacts
	void show();
  public:
	/// Creates a SContacts object
	SContacts();
	/// Destroys the SContacts object
	~SContacts();
	/**
	  Adds a new peer to contacts
	  @param socket to asociate with this peer
	  @param ip to asociate with the peer ip
	  @param port to asociate with the peer
	  @param name to asociate with the peer
	  @return the peer reference or NULL on error
	*/
	SPeerInfo* add(SocketType socket,int ip,unsigned short port, string name);
	/**
	  Finds a peer by location
	  @param ip is the IP of the peer to find
	  @param port is the port of the peer to find
	  @return the peer reference or NULL if not found
	*/
	SPeerInfo* find(int ip, unsigned short port);
	/**
	  Finds a peer by name
	  @param name is the name of the peer to find
	  @return the peer reference or NULL if not found
	*/
	SPeerInfo* find(string name);
	/**
	  Finds a peer by socket
	  @param ip is the IP of the peer to find
	  @param port is the port of the peer to find
	  @return the peer reference or NULL if not found
	*/
	SPeerInfo* findFromSocket(SocketType socket);
	/**
	  Finds a peer by peerId
	  @param peer is the peer's id to find
	  @return the peer reference or NULL if not found
	*/
	SPeerInfo* find(SBusPeer peer);
	/**
	  Updates a peer info's socket
	  @param peerInfo is the peer's information reference
	  @param socket is the new socket to associate with this peer Info
	*/
	SPeerInfo* updateSocket(SPeerInfo*  peerInfo, SocketType socket);
	/**
	  Updates a peer info's name
	  @param peerInfo is the peer's information reference
	  @param name is the new name to associate with this peer Info
	*/
	SPeerInfo* updateName(SPeerInfo* peerInfo, string name);
};

}

using namespace simple;
