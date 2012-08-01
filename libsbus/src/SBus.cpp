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
/** SBus.cpp
   @brief Interfaz de paso de mensajes sobre SBUS

   Interfaz de paso de mensajes sobre SBUS<p>

  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>

#include <memdefs.h>
#include <errdefs.h>
#include <stcp.h>
#include <sudp.h>
#include <spoll.h>

#include <SBus.h>

#define SBUS_FIND       0

#define INVALID_SOCKET -1

#define WAIT_MS 50

#define SLEEP_uS 5000

using namespace std;
using namespace simple;

/// Starts the inLoop receiver queue feeder
static void* inLoopStarter(void* ptrThis) {
  (reinterpret_cast<SBus*>(ptrThis))->inLoop();
  pthread_exit(NULL);
}

/// Initializes the SBus
void SBus::init(const char* device, const char* mcip, int mcport) {
  smessenger=new SMessenger(device,mcip, mcport);
  scontacts=new SContacts();
  setDefaultSafeName();
  pthread_mutex_init(&inMutex, NULL);
  alive=true;
  if(pthread_create(&inThread, NULL, inLoopStarter, (void*)this)!=0) {
    throw new string("Cannot start receiving loop thread");
  }
  pthread_detach(inThread);
}

/**
  Creates a SBus binding with default settings
  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
*/
SBus::SBus() {
  init(NULL,DEFAULT_MCIP,DEFAULT_MCPORT);
}

/**
  Creates a SBus binding on a specified multicast port
  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
  @param port is the multicast port to bind to
*/
SBus::SBus(int port) {
  init(NULL,DEFAULT_MCIP,port);
}

/**
  Creates a SBus binding on a specified multicast net interface
  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
  @param device is the net interface to bind to
*/
SBus::SBus(const char* device) {
  init(device,DEFAULT_MCIP,DEFAULT_MCPORT);
}

/**
  Creates a SBus binding on specified multicast net interface and port
  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
  @param device is the net interface to bind to
  @param port is the multicast port to bind to
*/
SBus::SBus(const char* device, int port) {
  init(device,DEFAULT_MCIP,port);
}

/**
  Creates a SBus binding on a specified multicast port
  NOTA: IT IS NOT thread-safe, avoid using the same SBus object on different threads
  @param device is the net interface to bind to
  @param mcip is the multicast address to bind to
  @param port is the multicast port to bind to
*/
SBus::SBus(const char* device, const char* mcip, int mcport) {
  init(device,mcip,mcport);
}

/**
  Sends a multicast message with a msgtag and empty data
  @param msgtag is the message code to send
  @return is 0 if the message was sent succesfully or -1 on error
*/
int SBus::send(int msgtag) {
  return smessenger->send(msgtag);
}

/**
  Sends a multicast message 
  @param msgtag is the message code to send
  @param msg is the message to send (Note: a string C++ can contain binary or test data)
  @return is 0 if the message was sent succesfully or -1 on error
*/
int SBus::send(int msgtag, string& msg) {
  return smessenger->send(msgtag,msg);
}

/**
  Gets the socket of a peer, connecting to it if needed
  @param peer is a local id for the SBus peer 
  @return the socket found or -1 on error
*/
SocketType SBus::peer2Socket(int peer) {
  SPeerInfo* peerInfo=scontacts->find(peer);
  SocketType socket=-1;
  if(peerInfo!=NULL) {
    socket=peerInfo->getSocket();
  } else {
    return -1;
  }
  if(socket>=0) {
    return socket;
  } else {
    int ip=peerInfo->getIP();
    unsigned short port=peerInfo->getPort();
    // If we know the location...
    if(ip!=-1) {
      // ...connect
      socket=smessenger->connect(ip,port);
      if(socket>=0) {
        peerInfo=scontacts->updateSocket(peerInfo,socket);
      }
      return socket;
    // Otherwise
    } else {
      // ...error but find it!
      string name=peerInfo->getName();
      if(name.size()>0) {
         send(SBUS_FIND,name);
      }
    }
  }
  return -1;
}

/**
  Sends an unicast message to a peer with a msgtag and empty data
  @param msgtag is the message code to send
  @param peer is a local id for the SBus peer to send the message to
  @return is 0 if the message was sent succesfully or -1 on error
*/
int SBus::send(int msgtag, SBusPeer peer){
  SocketType socket=peer2Socket(peer);
  if(socket<0) {
    return -1;
  }
  return smessenger->send(msgtag,socket);
}

/**
  Sends an unicast message to a peer with a msgtag and empty data
  @param msgtag is the message code to send
  @param peer is a local id for the SBus peer to send the message to
  @param msg is the message to send (Note: a string C++ can contain binary or test data)
  @return is 0 if the message was sent succesfully or -1 on error
*/
int SBus::send(int msgtag, SBusPeer peer, string& msg) {
  SocketType socket=peer2Socket(peer);
  if(socket<0) {
    return -1;
  }
  return smessenger->send(msgtag,socket,msg);
}

/// Bucle principal de la hebra de recepción
void SBus::inLoop() {
  do {
    SMsg* msg=smessenger->recv(WAIT_MS);
    if(msg!=NULL) {
      pthread_mutex_lock(&inMutex);
      inq.push_back(msg);
      pthread_mutex_unlock(&inMutex);
      //DEBUG("New message of size %d in queue, total=%d",msg->getMsg().size(),inq.size());
    }
  } while(alive);
  DEBUG("Inner thread [inLoop()] ends");
}

/**
  Gives the number of pending messages to be received
*/
int SBus::getPending() {
  return inq.size();
}

/**
  Gets a peer location while processing the message
  @param peer is the reference to the peer
  @param ip will be filled with the peer ip
  @param port will be filled with the peer port
  @return 0 on success or -1 if not found, not valid or consumed
*/
SBusPeer SBus::processSMsg(SMsg* smsg) {
  SocketType socket=smsg->getSocket();
  int ip=smsg->getIP();
  unsigned short port=smsg->getPort();
  //DEBUG("smsg->isError()=%d",smsg->isError());
  // Data messages
  if(!smsg->isError()) {
    // Is self-message?
    //DEBUG("msg 0x%X:%d - sbus 0x%X:%d",
    //  smsg->getIP(),smsg->getPort(),smessenger->getServerIP(), smessenger->getServerPort());
    if((smsg->getIP()==smessenger->getServerIP())&&
       (smsg->getPort()==smessenger->getServerPort())) {
      DEBUG("Selfmessage is ignored");
      return -1;
    }
    // If not self message we continue...
    SPeerInfo* peerInfo=scontacts->findFromSocket(socket);
    if(peerInfo==NULL) {
      peerInfo=scontacts->find(ip,port);
      if(peerInfo==NULL) {
        char ipstr[MAX_IP_ADDR_STR];
        sockaddr_int2ip(ipstr,ip);
	char portstr[7];
	sprintf(portstr,"%d",port);
	string peerName=string("sbus://")+ipstr+string(":")+string(portstr);
        int sock=(socket!=smessenger->getMulticastSocket())?socket:INVALID_SOCKET;
        DEBUG("socket=%d mcsock=%d sock=%d",socket,smessenger->getMulticastSocket(),sock);
        peerInfo=scontacts->add(sock,ip,port,name);
      } else if(socket!=smessenger->getMulticastSocket()) {
        scontacts->updateSocket(peerInfo,socket);
      }
    }
    SBusPeer peer=peerInfo->getPeer();
    DEBUG("Message from peer %d (socket %d)",peer,peerInfo->getSocket());
    // If system message... do something
    int msgtag=smsg->getMsgTag();
    if(msgtag<=0) {
      switch(msgtag) {
        case SBUS_FIND:
          DEBUG("Got a FIND");
	  if(name==smsg->getMsg()) {
	    send(SBUS_MANAMEIS,peer,name);
	  }
	  return -1;
	case SBUS_MANAMEIS:
          DEBUG("Got a MANAMEIS");
          scontacts->updateName(peerInfo,smsg->getMsg());
	  if(name==smsg->getMsg()) {
	    send(SBUS_NAMETAKEN,peer,name);
            DEBUG("NAMETAKEN internally detected and notified");
	  }
	  break;
	case SBUS_NAMETAKEN:
          DEBUG("Got a NAMETAKEN");
          peerInfo=scontacts->updateName(peerInfo,smsg->getMsg());
	  // Nothing more done here, user application decides if this is a bad thing
	  break;
	default:
          WARN("Unsupported system message tag %d received",msgtag);
	  break;
      }
    } else {
      DEBUG("Got a user msgtag (%d)",msgtag);
    }
    return peer;
  // Error messages
  } else {
    SPeerInfo* peerInfo=scontacts->findFromSocket(socket);
    if(peerInfo!=NULL) {
      peerInfo=scontacts->updateSocket(peerInfo,INVALID_SOCKET);
      DEBUG("Peer's %d socket %d no longer valid (it's probably disconnected)",
        peerInfo->getPeer(),socket);
    } else {
      DEBUG("Peer for socket %d not found! (it's probably disconnected)",socket);
    }
  }
  return -1;
}

/**
  Blocks and receives the next pending message (use getPending() to avoid blocking)
  @param pmsgtag is filled with the message code received
  @param ppeer is filled with the sender peer's local id
  @param msg is the message received (Note: a string C++ can contain binary or test data)
  @return is 0 if there was no message, the message received length, or -1 on error
*/
int SBus::recv(int* pmsgtag, SBusPeer* ppeer, string& msg) {
  do {
    pthread_mutex_lock(&inMutex);
    if(inq.empty()) {
      pthread_mutex_unlock(&inMutex);
      usleep(SLEEP_uS);
      continue;
    }
    SMsg* pmsg=inq.front();
    inq.pop_front();
    pthread_mutex_unlock(&inMutex);
    if(pmsg!=NULL) {
      if((*ppeer=processSMsg(pmsg))>=0) {
        *pmsgtag=pmsg->getMsgTag();
        msg=pmsg->getMsg();
        delete(pmsg);
        return msg.size();
      } else {
        delete(pmsg);
      }
    }
  } while(1);
}

/**
  Da el nombre registrado de ESTE SBUS

  @return Copia del nombre de este SBus
*/
string& SBus::getName() {
  return name;
}

/**
  Sets this SBus local name

  @param name new name for the SBus

  @return 0 on success, -1 on error
*/
int SBus::setName(string& name) {
  this->name=name;
  return send(SBUS_MANAMEIS,name);
}

/// (Re)Inicia el nombre de socket seguro
void SBus::setDefaultSafeName() {
  int ip=smessenger->getServerIP();
  unsigned short port=smessenger->getServerPort();
  char ipstr[MAX_IP_ADDR_STR];
  sockaddr_int2ip(ipstr,ip);
  char portstr[7];
  sprintf(portstr,"%d",port);
  this->name=string("sbus://")+ipstr+string(":")+string(portstr);
}

/// Closes and frees the SBus resources
SBus::~SBus() {
  alive=false;
  scontacts->~SContacts();
  smessenger->~SMessenger();
}

