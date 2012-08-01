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
/** SContacts.cpp
   @brief SBUS's simple contacts list, keeps track of known peers
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <string>
#include <iostream>

#include <timing.h>
#include <SContacts.h>

using namespace std;
using namespace simple;

/// A PeerInfo not being used is keep for at least 2h
#define MAX_ALLOWED_PEERINFO_IDLE_S 7200

/// A Cleanup is repeated at maximun every 2 mins
#define MIN_CLEANUP_PERIOD_S 120

/// Creates a SContacts object
SContacts::SContacts() {
  lastCleanUp=-1;
  lastPeerId=0;
}

/// Destroys the SContacts object
SContacts::~SContacts() {
  addr2peer.clear();
  name2peer.clear();
  socket2peer.clear();
  PeerIndexHash::iterator it=peers.begin();
  for(;it!=peers.end();it++) {
    delete(it->second);
  }
  peers.clear();
}

/// Does some old unused contacts cleanup
void SContacts::doCleanUp() {
  int now=timing_current_seconds();
  if((now-lastCleanUp)>MIN_CLEANUP_PERIOD_S) {
    lastCleanUp=now;
    PeerIndexHash::iterator it=peers.begin();
    for(;it!=peers.end();it++) {
      SPeerInfo* peerInfo=(SPeerInfo*)it->second;
      if(now-peerInfo->getLastActivity()>MAX_ALLOWED_PEERINFO_IDLE_S) {
        addr2peer.erase(addr2peer.find(peerInfo->getAddr()));
        name2peer.erase(name2peer.find(peerInfo->getName()));
        socket2peer.erase(socket2peer.find(peerInfo->getSocket()));
        peers.erase(it);
      }
    }
  }
}

/// Erases an entry completely from all references
void SContacts::erase(SPeerInfo* peerInfo) {
  peers.erase(peers.find(peerInfo->getPeer()));
  addr2peer.erase(addr2peer.find(peerInfo->getAddr()));
  name2peer.erase(name2peer.find(peerInfo->getName()));
  socket2peer.erase(socket2peer.find(peerInfo->getSocket()));
  delete(peerInfo);
}

/// Shows internal state of contacts
void SContacts::show() {
  PeerIndexHash::iterator it=peers.begin();
  for(;it!=peers.end();it++) {
    SPeerInfo* peerInfo=(SPeerInfo*)it->second;
    DEBUG("%d: socket=%d addr=%s %s",
      peerInfo->getPeer(),peerInfo->getSocket(),
      peerInfo->getAddr().c_str(),peerInfo->getName().c_str());
  }
}

/**
  Adds a new peer to contacts
  @param socket to asociate with this peer
  @param ip to asociate with the peer ip
  @param port to asociate with the peer
  @param name to asociate with the peer
  @return the peer reference or -1 on error 
*/
SPeerInfo* SContacts::add(SocketType socket,int ip,unsigned short port, string name) {
  SPeerInfo* peerInfo=find(ip,port);
  if(peerInfo!=NULL) {
    return peerInfo; // no need to add it was already there
  }
  if((peerInfo=find(name))!=NULL) {
    return peerInfo; // no need to add it was already there
  }
  // socket is not checked as it might be unreliable
  // Might do some cleanup before...
  doCleanUp();
  // add new entry
  lastPeerId++;
  peerInfo=new SPeerInfo(lastPeerId,socket,ip,port,name);
  peers[peerInfo->getPeer()]=peerInfo;
  addr2peer[peerInfo->getAddr()]=peerInfo;
  name2peer[peerInfo->getName()]=peerInfo;
  socket2peer[peerInfo->getSocket()]=peerInfo;
  DEBUG("Added new entry");
  show();
  return peerInfo;
}

/**
  Finds a peer by location
  @param ip is the IP of the peer to find
  @param port is the port of the peer to find
  @return the peer reference or NULL if not found
*/
SPeerInfo* SContacts::find(int ip, unsigned short port) {
  string addr=SPeerInfo::ipnport2addr(ip,port);
  StringIndexHash::iterator it=addr2peer.find(addr);
  if(it==addr2peer.end()) {
    return NULL;
  }
  return (SPeerInfo*)(it->second);
}

/**
  Finds a peer by name
  @param name is the name of the peer to find
  @return the peer reference or NULL if not found
*/
SPeerInfo* SContacts::find(string name) {
  StringIndexHash::iterator it=name2peer.find(name);
  if(it==name2peer.end()) {
    return NULL;
  }
  return (SPeerInfo*)(it->second);
}

/**
  Finds a peer by socket
  @param ip is the IP of the peer to find
  @param port is the port of the peer to find
  @return the peer reference or NULL if not found
*/
SPeerInfo* SContacts::findFromSocket(SocketType socket) {
  SocketIndexHash::iterator it=socket2peer.find(socket);
  if(it==socket2peer.end()) {
    return NULL;
  }
  return (SPeerInfo*)(it->second);
}

/**
  Finds a peer by peerId
  @param peer is the peer's id to find
  @return the peer reference or NULL if not found
*/
SPeerInfo* SContacts::find(SBusPeer peer) {
  SocketIndexHash::iterator it=peers.find(peer);
  if(it==peers.end()) {
    return NULL;
  }
  return (SPeerInfo*)(it->second);
}

/**
  Updates a peer info's socket
  @param peerInfo is the peer's information reference
  @param socket is the new socket to associate with this peer Info
*/
SPeerInfo* SContacts::updateSocket(SPeerInfo* peerInfo, SocketType socket) {
  SBusPeer oldPeer=peerInfo->getPeer();
  int oldIp=peerInfo->getIP();
  unsigned short oldPort=peerInfo->getPort();
  string& oldName=peerInfo->getName();
  erase(peerInfo);
  SPeerInfo*  newPeerInfo=new SPeerInfo(oldPeer,socket,oldIp,oldPort,oldName);
  peers[newPeerInfo->getPeer()]=newPeerInfo;
  addr2peer[newPeerInfo->getAddr()]=newPeerInfo;
  name2peer[newPeerInfo->getName()]=newPeerInfo;
  socket2peer[newPeerInfo->getSocket()]=newPeerInfo;
  DEBUG("Updated Socket");
  show();
  return newPeerInfo;
}

/**
  Updates a peer info's name
  @param peerInfo is the peer's information reference
  @param name is the new name to associate with this peer Info
*/
SPeerInfo* SContacts::updateName(SPeerInfo* peerInfo, string name) {
  SBusPeer oldPeer=peerInfo->getPeer();
  SocketType oldSocket=peerInfo->getSocket();
  int oldIp=peerInfo->getIP();
  unsigned short oldPort=peerInfo->getPort();
  erase(peerInfo);
  SPeerInfo*  newPeerInfo=new SPeerInfo(oldPeer,oldSocket,oldIp,oldPort,name);
  peers[newPeerInfo->getPeer()]=newPeerInfo;
  addr2peer[newPeerInfo->getAddr()]=newPeerInfo;
  name2peer[newPeerInfo->getName()]=newPeerInfo;
  socket2peer[newPeerInfo->getSocket()]=newPeerInfo;
  DEBUG("Updated Name");
  show();
  return newPeerInfo;
}
