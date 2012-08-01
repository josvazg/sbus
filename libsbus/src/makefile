CPP := g++
CFLAGS := -fPIC -g2 -O2 -Wall -Wno-deprecated
LIBFLAGS =-shared -Wl,-soname
OUTPATH := ../bin

LIBSBUS := -lsbus -I../libsbus -L../libsbus

LIBPTHREAD := -lpthread

LIBS=$(LIBPTHREAD)

INCLUDES=-I.

LIB_VERSION=0.2.1-git
LIB_NAME=libsbus.so
TARGET=$(OUTPATH)/$(LIB_NAME).$(LIB_VERSION)
LIB_EXPORTED_HDRS=sbus.h SBus.h QBus.h qbus.h SBusMsg.h

SRCS=*.c *.cpp
HDRS=*.h

CLEANS=$(OUTPATH)/$(LIB_NAME)*

$(TARGET): $(SRCS) $(HDRS)
	$(CPP) $(CFLAGS) $(SRCS) $(INCLUDES) $(LIBS) $(LIBFLAGS),$(@F) -o $@
	ln -sf $(@F) $(basename $(basename $(TARGET)))
	ln -sf $(@F) $(basename $(basename $(basename $(TARGET))))
	
clean:
	$(RM) $(CLEANS)
