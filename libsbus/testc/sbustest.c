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
/** sbustest.c

  Prueba del Simple-BUS Bus Simple para interconectar componentes en la misma o distinta máquina

*/
#include <unistd.h>
#include <stdlib.h>
#include <string.h>

#include <errdefs.h>
#include <sbus.h>
#include <log.h>

#define MAX_LINE_SIZE 1024

#define TUOT 50

#define PING_PERIOD_MS 200

#define PING_MSGCODE 0

#define FALSE 0

#define TRUE !(FALSE)

int active=1;

void sbustest_gotMaNameIs(SBusType sbus, char* msg, int size) {
  char* str=alloca(size+1);
  sbus_bin2str(str,msg,size);
  if(strncmp(msg,sbus_getName(sbus),size)==0) {
    INFO("Someone else wanted to be called %s, but I was quicker!!",str);
  } else {
    INFO("Someone is called %s",str);
  }
}

void sbustest_gotNameTaken(SBusType sbus, char* msg, int size) {
  INFO("%s was already taken, I have to abort now!",sbus_getName(sbus));
  active=0;
}

void sbustest_sendHi(SBusType sbus) {
  char hi[100];
  sprintf(hi,"hi this is %s speaking!",sbus_getName(sbus));
  sbus_mcsend(sbus,1,hi,strlen(hi));
}

int sbustest_start_pinger(const char *name, char *device) {
  SBusType sbus;
  //log_setLevel(Debug);
  // App presentation message
  if(device!=NULL) {
    INFO("SBus %s at device %s",name,device);
  } else {
    INFO("SBus %s with no device setting",name);
  }
  sbus=sbus_atDevice(device);
  RET_ON_FALSE(sbus!=0);
  INFO("SBUS pinger started (default name is %s)",sbus_getName(sbus));
  // SBus unique name registration
  if(sbus_setName(sbus,(char*)name)!=0) {
    INFO("Could not register SBUS with name %s",name);
    return -1;
  }
  INFO("%s registered",sbus_getName(sbus));
  // Message process loop
  {
    int counter=0;
    while(active) {
      int recvbytes;
      int msgtag;
      SBusChType ch2peer;
      char msg[MAX_LINE_SIZE];
      // get responses
      if((recvbytes=sbus_recv(sbus,&msgtag,&ch2peer,msg,MAX_LINE_SIZE))<0) {
        ERROR("Error receiving");
        return -1;
      } else if(recvbytes>0) { // and process them
        if(msgtag==SBUS_MANAMEIS) {
          sbustest_gotMaNameIs(sbus,msg,recvbytes);
        } else if(msgtag==SBUS_NAMETAKEN) {
          sbustest_gotNameTaken(sbus,msg,recvbytes);
        } else {
          char str[MAX_LINE_SIZE+1];
          sbus_bin2str(str,msg,recvbytes);
          INFO("%d |<- %d - %s",counter,msgtag,str);
        }
        if(counter==0) {
          sbustest_sendHi(sbus);
        }
        counter++;
      }
    }
    return 0;
  }
  sbus_dispose(sbus);
  return 0;
}

int main(int argc, char* argv[]) {
  const char *name=(argc>1)?argv[1]:argv[0];
  char *device=(argc>2)?argv[2]:NULL;
  if(argc<=1) {
    INFO("Usage: %s name [device]",argv[0]);
  }
  sbustest_start_pinger(name,device);
  return 0;
}
