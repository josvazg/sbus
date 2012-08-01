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
/** SMessenger.cpp
    @brief SMessenger allows raw message passing over TCP and UDP-multicast sockets
  José Luis Vázquez González <josvazg@terra.es><br>
  LGPL
*/
#include <sys/types.h>
#include <sys/socket.h>
#include <string.h>
#include <strings.h>
#include <string>
#include <iostream>

#include <SMessenger.h>

#include <stcp.h>
#include <sudp.h>

using namespace std;
using namespace simple;

/// Maximum connection wait time
#define MAX_CONN_TIMEOUT_MS 350
/// Maximum buffer size (1 page)
#define MAX_BUF 4096
/// Header lenght
#define HDRLEN sizeof(smsg_header)

/// Packs a message for sending
int SMessenger::packhdr(smsg_header* hdr, short msgtag, unsigned short port, int bytes) {
  hdr->code=msgtag;
  hdr->port=port;
  hdr->length=bytes;
  hdr->code=htons(hdr->code);
  hdr->port=htons(hdr->port);
  hdr->length=htonl(hdr->length);
  return HDRLEN;
}

/// Unpacks a received message
int SMessenger::unpackhdr(smsg_header* hdr, short* pmsgtag, unsigned short* pport) {
  hdr->code=ntohs(hdr->code);
  hdr->port=ntohs(hdr->port);
  hdr->length=ntohl(hdr->length);
  (*pmsgtag)=hdr->code;
  (*pport)=hdr->port;
  return hdr->length;
}

/// Inits the TCP server socket for unicast messaging
int SMessenger::initServer() {
  RET_ON_ERROR((servsock=stcp_server(ANY_IP_TXT,0)));
  stcp_setNonBlocking(servsock);
  char txt[IP_ADDR_STR_LENGTH];
  DEBUG("TCP server ServerSocket=%d - %s:%d",
      servsock,
      sockaddr_int2ip(txt,stcp_getLocalIP(servsock)),
      stcp_getLocalPort(servsock));
  return 0;
}

/// Inits the UDP Multicast server socket for multicast messaging
int SMessenger::initMCast() {
  RET_ON_ERROR((mcsock=sudp_mcast(device, mcip, mcport)));
  RET_ON_ERROR(spoll.add(mcsock, POLLIN | POLLHUP | POLLERR | POLLNVAL));
  stcp_setNonBlocking(mcsock);
  DEBUG("MCast MulticastSocket=%d - %s:%d",mcsock,mcip,mcport);
  return 0;
}

/**
  Creates a SMessenger over custom device, port and multicast IP settings
  @param device is the device of the network interface to bind this SMessenger
  @param mcip is the Multicast IP to bind to
  @param port for multicast binding
*/
SMessenger::SMessenger(const char* device, const char* mcip, int mcport) {
  // Servidor TCP
  if(initServer()<0) {
    throw new string("Could not init TCP server");
  }
  // Multicast
  this->device=device;
  this->mcip=mcip;
  this->mcport=mcport;
  if(initMCast()<0) {
    throw new string("Could not init MultiCast");
  }
  // Ids del SBUS
  this->port=stcp_getLocalPort(servsock);
  //DEBUG("port=%d",this->port);
  if(this->device!=NULL) { // we get this net interface's IP
    ip=sockaddr_device2ip(this->device);
  } else { // or from the the last valid net interface
    int ninterfaces=sockaddr_interfaceCount();
    //DEBUG("%d interfaces",ninterfaces);
    unsigned int* ips=(unsigned int*)alloca(ninterfaces*sizeof(unsigned int));
    sockaddr_ips(ips);
    ip=ips[ninterfaces-1];
  }
  bzero(&this->sap,sizeof(struct sockaddr_in));
  char ipstr[MAX_IP_ADDR_STR];
  sockaddr_int2ip(ipstr,ip);
  DEBUG("IP:PORT = %s:%d",ipstr,this->port);
  sockaddr_set((struct sockaddr_in*)&this->sap,ipstr,this->port);
}

/// Cierra y libera los recursos un enlace SBUS
SMessenger::~SMessenger() {
  sudp_mclose(mcsock,device,mcip);
  close(servsock);
}

/// Returns multicast socket being used
SocketType SMessenger::getMulticastSocket() {
  return mcsock;
}

/// Returns TCP Server socket being used
SocketType SMessenger::getServerSocket() {
  return servsock;
}

/// Returns TCP Server IP
int SMessenger::getServerIP() {
  return ip;
}

/// Returns TCP Server Port
int SMessenger::getServerPort() {
  return port;
}

/**
  Creates a new point to point connection (TCP)
  @param ip is the remote ip to connect to
  @param port is the remote TCP port to connect to 
  @return the connected socket, or -1 on error
*/
SocketType SMessenger::connect(int ip, unsigned short port) {
  int fd;
  char ipstr[IP_ADDR_STR_LENGTH];
  sockaddr_int2ip(ipstr,ip);
  //DEBUG("Connecting to %s:%d...\n",ipstr,port);
  RET_ON_ERROR((fd=stcp_client(ipstr,port,0)));
  RET_ON_ERROR(stcp_waitConnection(fd,MAX_CONN_TIMEOUT_MS));
  //DEBUG("Connected to %s:%d via %d\n",ipstr,port,fd);
  char ip1str[MAX_IP_ADDR_STR],ip2str[MAX_IP_ADDR_STR];
  DEBUG("Connected socket %d <L %s:%d-R %s:%d>"
        ,fd
	,sockaddr_int2ip(ip1str,stcp_getLocalIP(fd)),stcp_getLocalPort(fd)
	,sockaddr_int2ip(ip2str,stcp_getIP(fd)),stcp_getPort(fd));
  RET_ON_ERROR(spoll.add(fd,POLLIN | POLLHUP | POLLERR | POLLNVAL));
  return fd;
}

/**
  Sends a messange header with no data over multicast
  @param msgtag is the message tag/code to be sent within the header
  @return 0 on success or -1 in case of an error (message was not sent)
*/
int SMessenger::send(int msgtag) {
  string nil="";
  return this->send(msgtag,nil);
}

/**
  Sends a message over multicast
  @param msgtag is the message tag/code to be sent within the header
  @param msg is the data or body of the message
  @return 0 on success or -1 in case of an error (message was not sent)
*/
int SMessenger::send(int msgtag, string& msg) {
  int sent=0;
  int bytes=msg.size();
  if(bytes>MAX_DATALEN) {
    ERROR("Message too big (%d bytes>%d bytes)",bytes,MAX_DATALEN);
    return -1;
  }
  char* buf=NULL;
  int total2send=HDRLEN+bytes;
  RET_ON_ERROR((buf=(char*)alloca(total2send)));
  packhdr((smsg_header*)buf, msgtag, port, bytes);
  memcpy(&buf[HDRLEN],msg.data(),bytes);
  if((sent=sudp_mcsend(mcsock,buf,total2send,mcip,mcport))!=(int)total2send) {
    PERROR("Error send()");
    ERROR("Could not send message (sent=%d of %d)",sent,total2send);
    return -1;
  }
  DEBUG("mcsendit sent MSGTAG=%d and %d bytes via multicast socket %d",msgtag,sent,mcsock); 
  return 0;
}

/**
  Sends an unicast messange header with no data to a single peer
  @param msgtag is the message tag/code to be sent within the header
  @return 0 on success or -1 in case of an error (message was not sent)
*/
int SMessenger::send(int msgtag, SocketType socket2peer) {
  string nil=string("");
  return send(msgtag,socket2peer,nil);
}

/**
  Sends an unicast message to a peer
  @param msgtag is the message tag/code to be sent within the header
  @param msg is the data or body of the message
  @return 0 on success or -1 in case of an error (message was not sent)
*/
int SMessenger::send(int msgtag, SocketType socket2peer, string& msg) {
  int sent=0;
  char* buf;
  int bytes=msg.size();
  if(bytes>MAX_DATALEN) {
    ERROR("Message too big (%d bytes>%d bytes)",bytes,MAX_DATALEN);
    return -1;
  }  
  int total2send=HDRLEN+bytes;
  RET_ON_ERROR((buf=(char*)alloca(total2send)));
  /* Port in TCP (point to point messages) is the TCP sender port, 
    NOT the TCP server port sent along for multicast messages
  */
  int tcpPort=stcp_getLocalPort(socket2peer);
  packhdr((smsg_header*)buf, msgtag, tcpPort, bytes);
  memcpy(&buf[HDRLEN],msg.data(),bytes);
  if((sent=stcp_send(socket2peer,buf,total2send))!=(int)total2send) {
    if(sent<0) {
      PERROR("Error send()");
    } else {
      ERROR("Sending message (sent=%d of %d)",sent,total2send);
    }
    return -1;
  }
  DEBUG("send sent MSGTAG=%d and %d bytes to %d",msgtag,sent,socket2peer);
  return 0;
}

/// Accepts a TCP connection
int SMessenger::acceptConn(SocketType servsock) {
  int fd;
  //DEBUG("Incoming connection...");
  RET_ON_ERROR((fd = accept(servsock,NULL,NULL)));
  DEBUG("Connection %d accepted",fd);
  // Añade la nueva conexión como fuente de sucesos
  spoll.add(fd, POLLIN | POLLHUP | POLLERR | POLLNVAL);
  return 0;
}

/// Listen incomming connections
int SMessenger::listenConn() {
  int res;
  struct pollfd fds;
  fds.fd=servsock;
  fds.events=POLLIN|POLLHUP|POLLERR|POLLNVAL;
  res=poll(&fds,1,0);
  if(res>0) {
    // Problems?
    if((fds.revents&POLLHUP)||(fds.revents&POLLERR)||(fds.revents&POLLNVAL)) {
      int fd=fds.fd;
      ERROR("Server socket error detected");
      close(fd);
      return initServer();
    }
    // Data = Incoming connection
    if(fds.revents&POLLIN) {
      return acceptConn(fds.fd);
    }
  }
  return 0;
}

/// Tries to get the next message in full from a socket
int SMessenger::nextMsg(int fd, short* pmsgtag, char* data, int *len, struct sockaddr_in* from) {
  int ready=0;
  int datasize;
  smsg_header head;
  int addrlen;
  char *buf;
  unsigned short portFrom;
  if(*len<=0) {
    ERROR("Buffer too small (%d bytes)",*len);
    return -1;
  }
  addrlen=sizeof(struct sockaddr_in);
  bzero(from,sizeof(struct sockaddr_in));
  if((ready=recvfrom(fd,&head,HDRLEN,MSG_PEEK,
      (struct sockaddr*)from,(socklen_t*)&addrlen))<0) {
    if(errno==EAGAIN) {
      DEBUG("fd=%d receiving EAGAIN",fd);
      *len=0;
      usleep(1); // Evita expera activa
      return 0;
    }
    ERROR("Could not test header reception from %d",fd);
    return -1;
  }
  if((ready>0)&&(ready<(int)HDRLEN)) {
    //LOG("header not ready (ready=%d bytes of %d)",ready,HDRLEN);  
    *len=0;
    usleep(1); // Evita expera activa
    return 0;
  }
  if(ready<=0) { // Error o desconexión
    if(ready<0) {
      ERROR("Could not receive from %d",fd);
    } else {
      DEBUG("Connection %d dropped",fd);
    }
    return -1;
  }
  datasize=unpackhdr(&head,pmsgtag,&portFrom);
  if(datasize<0) {
    WARN("Message dropped!");
    return -1;
  }
  if(datasize>(*len)) {
    WARN("Received message too big for buffer %d > %d",datasize,*len);
    return -1;
  }
  if((buf=(char*)alloca(HDRLEN+datasize))<0) {
    ERROR("Could not prepare receiving buffer");
    return -1;
  }
  if((ready=recvfrom(fd,buf,HDRLEN+datasize,MSG_PEEK,
      (struct sockaddr*)from,(socklen_t*)&addrlen))<0) {
    if(errno==EAGAIN) {
      *len=0;
      return 0;
    }
    ERROR("Could not test frame reception from %d",fd);
    return -1;
  }
  if(ready<(int)(datasize+HDRLEN)) {
//    INFO("data not ready (ready=%d bytes)",ready);
    *len=0;
    usleep(1); // Evita expera activa
    return 0;
  }
  // Llegados aqui extraemos todo del buffer porque ya que esta listo
  if((ready=recvfrom(fd,buf,HDRLEN+datasize,0,
      (struct sockaddr*)from,(socklen_t*)&addrlen))<(int)(HDRLEN+datasize)) {
    if(ready<0) {
      PERROR("Error recvfrom()");
    } else {
      ERROR("Could not extract msg from socket %d buffer\n",fd);
    }
    return -1;
  }
  memcpy(data,&buf[HDRLEN],datasize);
  *len=datasize;  
  from->sin_family=AF_INET;
  from->sin_port=htons(head.port);
  // TCP conn. addr info is empty, we have to refill it
  if(from->sin_addr.s_addr==0) { 
    from->sin_addr.s_addr=htonl(stcp_getIP(fd));
  }
  return datasize+HDRLEN;
}

/// On socket error, drop it and, if it is the multicast one, reget it
int SMessenger::socketErrorHandling(SocketType fd) {
  if(fd==mcsock) {
    close(fd);
    initMCast();
  } else {
    spoll.remove(fd);
    close(fd);
  }
  return 0;
}

/**
  Receives a message, blocking for it the specified time
  @param timeout is the maximum time to wait for a message
  @return the message received with header information included, an Error SMsg on error or NULL on timeout
*/
SMsg* SMessenger::recv(int timeout) {
  RET_NULL_ON_ERROR(this->listenConn());
  long long int limit=timing_current_millis()+timeout;
  while((spoll.doPoll(timeout)>0)&&(timing_current_millis()<limit)) {
    int i;
    int size;
    struct pollfd* fds;
    RET_NULL_ON_ERROR((size=spoll.getpolls(&fds)));
    for(i=0;i<size;i++) {
      if((fds[i].fd!=0)&&(fds[i].revents!=0)) {
        // Problems?
	if((fds[i].revents&POLLHUP)||(fds[i].revents&POLLERR)||(fds[i].revents&POLLNVAL)) {
	  int sock=fds[i].fd;
          ERROR("Socket %d error %d (HUP=%d ERR=%d NVAL=%d) detected"
	   ,fds[i].fd,fds[i].revents,POLLHUP,POLLERR,POLLNVAL);
	  socketErrorHandling(fds[i].fd);
	  return new SMsg(ERRCODE_PEER_DISCONNECTED, 0, 0, sock);
        }
	// Data?
	if(fds[i].revents&POLLIN) {
	  int res;
	  struct sockaddr_in from;
	  short msgtag=0;
	  int len=MAX_DATALEN;
	  char *data=(char*)alloca(MAX_DATALEN);
	  if((res=nextMsg(fds[i].fd,&msgtag,data,&len,&from))<0) {
            int sock=fds[i].fd;
            DEBUG("Data socket %d error: removing from spoll",fds[i].fd);
            socketErrorHandling(fds[i].fd);
	    return new SMsg(ERRCODE_PEER_DISCONNECTED, 0, 0, sock);
          } else if(res>0) {
	    string msg="";
	    msg.assign(data,len);
	    SocketType socket2peer=fds[i].fd;
	    if(socket2peer>0) {
              int ip=sockaddr_getIP(&from);
	      unsigned short port=sockaddr_getPort(&from);
              DEBUG("Receive MSGTAG=%d from socket %d with %dbytes",msgtag, socket2peer, msg.size());
	      return new SMsg(msgtag, ip, port, socket2peer, msg);
	    }
	  }
        }
      }
    }
  }
  return NULL;
}

