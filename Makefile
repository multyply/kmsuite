# CHTML make-file

#DATABASE := postgresql
DATA=/data/chtml
ARCH := 64
#OPT := -ggdb -O0
OPT := -O3
CFG :=
CC := g++
INC := -I src/lib
#LIB=-ldl

ifeq ($(DATABASE), postgresql)
INC += -I$(shell pg_config --includedir-server)
INC += -I$(shell pg_config --includedir)
LIB += -L$(shell pg_config --libdir)
LIB += -lpq
CFG += -D CONFIG_DATABASE=$(DATABASE)
endif

CFLAGS = $(OPT) $(CFG) -m$(ARCH) -c -Wall $(INC) $(LIB)
LDFLAGS = $(OPT) $(CFG) -m$(ARCH) -Wall $(INC) $(LIB)
AR = ar
ARFLAGS = rcv

all: clean init lib utl mod

init:
	mkdir -p bin
	mkdir -p bin/lib
	mkdir -p bin/mod
	mkdir -p bin/utl

install:
	mkdir -p $(DATA)
	mkdir -p $(DATA)/bin
	mkdir -p $(DATA)/cfg
	mkdir -p $(DATA)/log
	chmod 777 $(DATA)/log
	mkdir -p $(DATA)/xdb
	cp -R bin/mod/* $(DATA)/bin
	cp -R cfg $(DATA)
	cp -R web $(DATA)

package:
	tar czvf ../CHTML.tgz ../CHTML

clean:
	rm -rf bin/lib
	rm -rf bin/mod
	#rm -rf bin/utl


lib: init
	$(CC) $(CFLAGS) -o bin/lib/common.o src/lib/common.cc
	$(CC) $(CFLAGS) -o bin/lib/parameter.o src/lib/parameter.cc
	$(CC) $(CFLAGS) -o bin/lib/config.o src/lib/config.cc
ifeq ($(DATABASE), postgresql)
	$(CC) $(CFLAGS) -o bin/lib/database.o src/lib/database-$(DATABASE).cc
	$(CC) $(CFLAGS) -o bin/lib/session.o src/lib/session.cc
endif
	$(CC) $(CFLAGS) -o bin/lib/log.o src/lib/log.cc
	$(CC) $(CFLAGS) -o bin/lib/request.o src/lib/request.cc
	$(CC) $(CFLAGS) -o bin/lib/response.o src/lib/response.cc
	$(CC) $(CFLAGS) -o bin/lib/prototype.o src/lib/prototype.cc
	$(AR) $(ARFLAGS) bin/lib/chtml.a bin/lib/*.o

utl: init
	$(CC) $(LDFLAGS) -c -o bin/lib/utl.o src/utl/utl.cc
	$(CC) $(LDFLAGS) -o bin/utl/create src/utl/create.cc bin/lib/utl.o bin/lib/chtml.a
	$(CC) $(LDFLAGS) -o bin/utl/compile src/utl/compile.cc bin/lib/utl.o bin/lib/chtml.a
	$(CC) $(LDFLAGS) -o bin/utl/drop src/utl/drop.cc bin/lib/utl.o bin/lib/chtml.a

mod: init mod_lib

mod_lib:
	$(CC) $(CFLAGS) -o bin/lib/chtml.o src/mod/chtml.lib.cc

