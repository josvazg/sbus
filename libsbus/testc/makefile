CFLAGS := -fPIC -g2 -O2 -Wall
LIBFLAGS =-shared -Wl,-soname
OUTPATH := ../bin

LIBSBUS := -lsbus -L../bin

INCLUDES=-I../src
LIBS=$(LIBGLIB) $(LIBSBUS) 

SRCS=sbustest.c

CLEANS=$(OUTPATH)/testc

$(OUTPATH)/testc: $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) $(INCLUDES) $(LIBS) -o $@
	
clean:
	$(RM) $(CLEANS)
	