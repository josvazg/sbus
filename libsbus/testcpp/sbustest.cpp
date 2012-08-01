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
/** sbustest.cpp

  Simple-BUS Test to interconnect different processes in the same or different machine or process

*/
#include <unistd.h>
#include <stdlib.h>

#include <log.h>
#include <errdefs.h>
#include <SBus.h>

using namespace std;
using namespace simple;

#define PING_PERIOD_MS 2000

#define PING_MSGCODE 0

int active=1;

void sbustest_gotMaNameIs(SBus &sbus, string msg) {
  if(msg==sbus.getName()) {
    INFO("Someone else wanted to be called %s, but I was quicker!!",msg.c_str());
  } else {
    INFO("Someone is called %s",msg.c_str());
  }
}

void sbustest_gotNameTaken(SBus &sbus, string msg) {
  INFO("%s was already taken, I have to abort now!",sbus.getName().c_str());
  active=0;
  throw new string("Name taken\n");
}

void sbustest_sendHi(SBus &sbus) {
  string hi="hi this is "+sbus.getName()+" speaking!";
  sbus.send(1,hi);
}

/// Main test app loop
int sbustest_start_pinger(string& name, char* device) {
  //log_setLevel(Debug);
  try {
    // App presentation message
    if(device!=NULL) {
      INFO("SBus %s at device %s",name.c_str(),device);
    } else {
      INFO("SBus %s with no device setting",name.c_str());
    }
    // SBus cretion / binding
    SBus sbus(device);
    INFO("SBUS pinger started (default name is %s)",sbus.getName().c_str());
    if(sbus.setName(name)!=0) {
      INFO("Could not register SBUS with name %s",name.c_str());
      return -1;
    }
    INFO("%s registered",sbus.getName().c_str());
    // Message process loop
    int counter=0;
    while(active) {
      int recvbytes;
      int msgtag;
      SBusPeer peer;
      string msg;
      // get responses
      msg=string("-");
      if((recvbytes=sbus.recv(&msgtag,&peer,msg))<0) {
        ERROR("Error receiving");
        return -1;
      } else if(recvbytes>0) { // and process them
        if(msgtag==SBUS_MANAMEIS) {
          sbustest_gotMaNameIs(sbus,msg);
        } else if(msgtag==SBUS_NAMETAKEN) {
          sbustest_gotNameTaken(sbus,msg);
        } else {
          INFO("%d  |<- %d - %s",counter,msgtag,msg.c_str());
        }
        if(counter==0) {
          sbustest_sendHi(sbus);
        }
        counter++;
      }
    }
    return 0;
  // C++ exception catching code
  } catch(string* s) {
    ERROR("Exception %s",s->c_str());
    return -1;
  }
  return 0;
}

// Main: args parsing
int main(int argc, char* argv[]) {
  string name=(argc>1)?argv[1]:argv[0];
  char* device=(argc>2)?argv[2]:NULL;
  if(argc<=1) {
    INFO("Usage: %s name [device]",argv[0]);
  }
  sbustest_start_pinger(name,device);
}
