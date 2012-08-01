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
/** @file SMessenger.h
    @brief SMessenger allows raw message passing over TCP and UDP-multicast sockets
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <string>
#include <iostream>

using namespace std;

#include <stcp.h>
#include <SMsg.h>
#include <SPoll.h>

/// Maximun message body length in bytes (1MB)
#define MAX_DATALEN 1*1024*1024

/// Message header
typedef struct smsg_header {
  /// Message code
  unsigned short code;
  /// Sender location port (TCP listener's port)
  unsigned short port;
  /// Message length
  int length;
} smsg_header;

#define ERRCODE_PEER_DISCONNECTED -1

namespace simple {

class SMessenger {
  private:
	/// Packs a message for sending
 static int packhdr(smsg_header* hdr, short msgtag, unsigned short port, int bytes);
	/// Unpacks a received message
 static int unpackhdr(smsg_header* hdr, short* pmsgtag, unsigned short* pport);
	/// Local service access point
	struct sockaddr sap;
	/// Multicast IP
	const char* mcip;
    /// Device, binded net interface
    const char* device;
	/// Multicast port
	unsigned short mcport;
	/// TCP server port
	unsigned short port;
	/// TCP server socket
	SocketType servsock;
	/// Conexión Multicast del SBUS
	SocketType mcsock;
	/// IP local de escucha
	int ip;
	/// Vigilancia de conexiones entrantes y salientes
	SPoll spoll;
	/// Inits the TCP server socket for unicast messaging
	int initServer();
	/// Inits the UDP Multicast server socket for multicast messaging
	int initMCast();
	/// Accepts a TCP connection
	int acceptConn(SocketType servsock);
	/// Listen incomming connections
	int listenConn();
	/// Tries to get the next message in full from a socket
	int nextMsg(int fd, short* pmsgtag, char* data, int *len, struct sockaddr_in* from);
	/// On socket error, drop it and, if it is the multicast one, reget it
	int socketErrorHandling(SocketType fd);
  public:
	/**
	  Creates a SMessenger over custom device, port and multicast IP settings
	  @param device is the device of the network interface to bind this SMessenger
	  @param mcip is the Multicast IP to bind to
	  @param port for multicast binding
	*/
	SMessenger(const char* device, const char* mcip, int port);
	/// Cierra y libera los recursos un enlace SBUS
	~SMessenger();
	/// Returns multicast socket being used
	SocketType getMulticastSocket();
	/// Returns TCP Server socket being used
	SocketType getServerSocket();
	/// Returns TCP Server IP
	int getServerIP();
	/// Returns TCP Server Port
	int getServerPort();
	/**
	  Creates a new point to point connection (TCP)
	  @param ip is the remote ip to connect to
	  @param port is the remote TCP port to connect to 
	  @return the connected socket, or -1 on error
	*/
	SocketType connect(int ip, unsigned short port);
	/**
	  Sends a messange header with no data over multicast
	  @param msgtag is the message tag/code to be sent within the header
	  @return 0 on success or -1 in case of an error (message was not sent)
	*/
	int send(int msgtag);
	/**
	  Sends a message over multicast
	  @param msgtag is the message tag/code to be sent within the header
	  @param msg is the data or body of the message
	  @return 0 on success or -1 in case of an error (message was not sent)
	*/
	int send(int msgtag, string& msg);
	/**
	  Sends an unicast messange header with no data to a single peer
	  @param msgtag is the message tag/code to be sent within the header
	  @return 0 on success or -1 in case of an error (message was not sent)
	*/
	int send(int msgtag, SocketType socket2peer);
	/**
	  Sends an unicast message to a peer
	  @param msgtag is the message tag/code to be sent within the header
	  @param msg is the data or body of the message
	  @return 0 on success or -1 in case of an error (message was not sent)
	*/
	int send(int msgtag, SocketType socket2peer, string& msg);
	/**
	  Receives a message, blocking for it the specified time
	  @param timeout is the maximum time to wait for a message
	  @return the message recived with header information included or NULL on timeout
	*/
	SMsg* recv(int timeout);
};

}

using namespace simple;
