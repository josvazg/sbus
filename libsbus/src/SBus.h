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
/** @file SBus.h
   @brief SBUS's Message Passing Interface

   Interfaz de paso de mensajes sobre SBUS<p>

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <pthread.h>
#include <deque>
#include <string>
#include <iostream>

using namespace std;
#include <sbusdefs.h>
#include <SMessenger.h>
#include <SContacts.h>

/// Default Multicast Port
#define DEFAULT_MCPORT 10001
/// Default Multicast IP
#define DEFAULT_MCIP "224.0.23.130"

namespace simple {

class SBus {
  private:
	/// This SBus name
	string name;
	/// The messager to send and receive messages
	SMessenger* smessenger;
	/// The contacts manager, with the list of know SBus peers
	SContacts* scontacts;
	/// Incoming message's queue asociated mutex
	pthread_mutex_t inMutex;
	/// Incoming message's queue
	deque<SMsg*> inq;
	/// Incoming message's queue thread
	pthread_t inThread;
	/// Incoming message's queue thread life's flag
	bool alive;
	/// Gets the socket of a peer, connecting to it if needed
	SocketType peer2Socket(int peer);
	/// Gets a peer location
	SBusPeer processSMsg(SMsg* smsg);
	/// Initializes the SBus
	void init(const char* device, const char* mcip, int mcport);
  public:
	/**
	  Creates a SBus binding with default settings
	  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
	*/
	SBus();
	/**
	  Creates a SBus binding on a specified multicast port
	  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
	  @param port is the multicast port to bind to
	*/
	SBus(int port);
	/**
	  Creates a SBus binding on a specified multicast net interface
	  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
	  @param device is the net interface to bind to
	*/
	SBus(const char* device);
	/**
	  Creates a SBus binding on specified multicast net interface and port
	  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
	  @param device is the net interface to bind to
	  @param port is the multicast port to bind to
	*/
	SBus(const char* device, int port);
	/**
	  Creates a SBus binding on a specified multicast port
	  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
	  @param device is the net interface to bind to
	  @param mcip is the multicast address to bind to
	  @param port is the multicast port to bind to
	*/
	SBus(const char* device, const char* mcip, int mcport);
	/// Closes and frees the SBus resources
	~SBus();
	/**
	  Sends a multicast message with a msgtag and empty data
	  @param msgtag is the message code to send
	  @return is 0 if the message was sent succesfully or -1 on error
	*/
	int send(int msgtag);
	/**
	  Sends a multicast message 
	  @param msgtag is the message code to send
	  @param msg is the message to send (Note: a string C++ can contain binary or test data)
	  @return is 0 if the message was sent succesfully or -1 on error
	*/
	int send(int msgtag, string& msg);
	/**
	  Sends an unicast message to a peer with a msgtag and empty data
	  @param msgtag is the message code to send
	  @param peer is a local id for the SBus peer to send the message to
	  @return is 0 if the message was sent succesfully or -1 on error
	*/
	int send(int msgtag, SBusPeer peer);
	/**
	  Sends an unicast message to a peer with a msgtag and empty data
	  @param msgtag is the message code to send
	  @param peer is a local id for the SBus peer to send the message to
	  @param msg is the message to send (Note: a string C++ can contain binary or test data)
	  @return is 0 if the message was sent succesfully or -1 on error
	*/
	int send(int msgtag, SBusPeer peer, string& msg);
	/**
	  Gives the number of pending message to be received
	  @return the number of pending messages in the queue
	*/
	int getPending();
	/**
	  Blocks and receives the next pending message (use getPending() to avoid blocking)
	  @param pmsgtag is filled with the message code received
	  @param ppeer is filled with the sender peer's local id
	  @param msg is the message received (Note: a string C++ can contain binary or test data)
	  @return is 0 if there was no message, the message received length, or -1 on error
	*/
	int recv(int* pmsgtag, SBusPeer* ppeer, string& msg);
	/**
	  Get this SBus registered name
	  @return a copy of the name string
	*/
	string& getName();
	/**
	  Registers this SBus with a name
	  (Note that there may be name conflicts, received as messages)  
	  @param name name to register this SBus with
	  @return 0 on success, -1 on error
        */
	int setName(string& name);
	/**
	 (Re)sets the initial secure SBus name
	 */
	void setDefaultSafeName();
	/**
	  Get some peer SBus name
	  @param peer es el canal con el SBus
	  @return a copy of the name string
	*/
	string& getNameFor(SBusPeer peer);
	/**
	  Get some peer SBus name
	  It returns inmediatelly, if the name is not known already 
	  a find is sent for that name, and the reply will get as a message
	  @param peer es el canal con el SBus
	  @return the channel found or -1 if the name was not know yet
	*/
	int find(string& name);
	// Main reception thread loop
	void inLoop();
};

}

using namespace simple;
