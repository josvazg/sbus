TAR_OPTS=zcvf 
TAR_EXTRAS= --exclude=*~ --exclude=CVS
PROJNAME=libsbus
LIBNAME=libsbus.so*
SOURCE_FILENAME=libsbus.src.tgz
SRC_FILES := makefile *.txt 
SRC_FILES := $(SRC_FILES) src/makefile src/*.c* src/*.h
SRC_FILES := $(SRC_FILES) testcpp/makefile testcpp/*.cpp
SRC_FILES := $(SRC_FILES) testc/makefile testc/*.c

SRC_FILES_FULLPATH=$(foreach file,$(SRC_FILES),../../$(PROJNAME)/$(file))

BIN_FILENAME=libsbus.bin.tgz

INST_LIBDIR = /usr/lib
INST_DEVDIR = /usr/include
LIB_EXPORTED_HDRS = src/sbus.h src/SBus.h src/sbusdefs.h

all: bin bin/libsbus.so bin/testcpp bin/testc bin/$(SOURCE_FILENAME) bin/$(BIN_FILENAME)

rebuild: clean all

bin/testcpp: testcpp/*.cpp
	cd testcpp && make

bin/testc: testc/*.c 
	cd testc && make
	
bin/libsbus.so: src/*.c* src/*.h*
	cd src && make

bin/$(SOURCE_FILENAME): $(SRC_FILES)
	cd bin && tar $(TAR_OPTS) $(SOURCE_FILENAME) $(TAR_EXTRAS) $(SRC_FILES_FULLPATH) 

bin:
	mkdir bin

bin/$(BIN_FILENAME): bin/$(PROJNAME).so
	mkdir -p temp/$(INST_LIBDIR)
	cp -p -r -d bin/$(LIBNAME) temp/$(INST_LIBDIR)
	cd temp && tar $(TAR_OPTS) ../bin/$(BIN_FILENAME) $(TAR_EXTRAS) * 
	rm -rf temp
	
install: clean all
	cp -p -r -d bin/$(LIBNAME) $(INST_LIBDIR)
	cp $(LIB_EXPORTED_HDRS) $(INST_DEVDIR)

clean:
	cd src && make clean
	cd testcpp && make clean
	cd testc && make clean
	
