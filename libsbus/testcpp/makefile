CPP := g++
CFLAGS := -fPIC -g2 -O2 -Wall -Wno-deprecated
LIBFLAGS =-shared -Wl,-soname
OUTPATH := ../bin

LIBSBUS := -lsbus -L../bin

INCLUDES=-I../src
LIBS=$(LIBGLIB) $(LIBSBUS) 

SRCS=sbustest.cpp

CLEANS=$(OUTPATH)/testcpp

$(OUTPATH)/testcpp: $(SRCS)
	$(CPP) $(CFLAGS) $(SRCS) $(INCLUDES) $(LIBS) -o $@
	
clean:
	$(RM) $(CLEANS)
	
