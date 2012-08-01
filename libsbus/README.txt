LIB SBUS
________


Simple-Bus Library is a very thin wrapper to TCP and UDP multicast sockets.
It allows easy message passing with the following features:

  1) No single point of failure, all peers are equal and none stores a special 
     state that others would require. (Thanks to the use multicast sockets)
     
  2) Peers can be in your same, or different machine, even in the same process.
  
  3) Peers can be referenced by a local ID or name. The name can be registered 
     as unique by checking first it not used by others. The user application 
     decides whether it wants a unique name or not.
     
  4) Each message received includes:
      - The local ID for the sender.
      - A message code, than can be used to distinguish message types.
        IMPORTANT NOTE: 0 and negative message codes are reserved for 
        system events/messages 
      - The message body itself, that could be void.
     
  5) Messages can be sent to a single peer (by ID) or to all (multicast). Point
     to point messages are sent via TCP connections, while multicast ones are
     sent by UDP multicast, but this is transparent for the user application.
  
  6) Messages can be received by multicast or tcp, but the user application is not
     aware of that.
  
  7) SBus can be used "thread safely"; just use different sbus instances 
     (peerids) on different threads. SBUS can be used even to intercommuncate 
     your process/threads.
     
     We recommend you do NOT share SBUS instances (peer IDs) between threads 
     or you will probably have to lock on sbus calls, as these are not thread-safe.
     
  8) SBus (from version 0.2.0 on) comes with a built-in reception queue. The user
     application has to poll the reception queue often anyway, to avoid it to get too
     full or/and start reading too outdated messages. 
  
It's LGPL, which basically means you can use it as part of commercial or open source 
projects (just need to provide the sources of sbus, not yours). So...

...Try and enjoy it!


Compile:
________
(Needs make, gcc and g++)
Just do:

# make


Install:
________

# make install


Implementation Details
______________________

- SBus minimizes automatic traffic overhead, that is system messages sent 
automatically by the SBus without the user application asking for them.

- Everytime an SBus sends an ADVERTISING multicast message with its name, receivers
write it down in its contact's table, and DOES NOT REPLY automatically UNLESS the name
of the receiver is the same as the one received in the message. This automatic reply
is a NAMETAKEN notification, it allows user applications to know their name was already 
taken by some other.
The recommended behaviour when an user application receives an NAMETAKEN message is to
failback to the default unique safe name and, probably stop providing the application
service, as it is already being done by some other application.

- If the user application wants to reply to a particular SBus peer, then its SBus 
sends a unicast message (TCP), the connection TCP is (re)established, if needed within
the SBus's send call. The contact's table is used to query or set more info. on the 
peer.

- User applications providing a service will probably want to set a unique name within 
the group it first does a setName() call. This call will send an ADVERTISING multicast 
message. 

- SBus user applications not using the setName() are typically service costumers. 
They use find() and getNameFor() calls to find service providers to talk to them. 

- getNameFor() anf find() are first tried to be answered locally, by looking at the 
contacts table of the SBus. If the name could not be found there, a FIND multicast 
message is sent. Only the receiving SBus peer receiving the message AND helding
that SAME NAME, will reply with a unicast ADVERTISING message. 

Future
______

Basically:
- Add security option by ciphering sent messages.


Windows?
________

Only Linux version is available at the moment, but the Windows should not be had to 
do. Drop me a line if you are interested at josvazg@gmail.com       
    
_______________________________________________________________________________________
Jose Luis Vázquez Gonzaléz
josvazg@gmail.com

    

