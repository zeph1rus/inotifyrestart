OPTIMFLAGS= -g -O
PACKAGES= libsystemd
PKGCONFIG= pkg-config
ENAME= inrestart
CC= gcc
DEPS= sdbus-restart.h
OBJ = sdbus-restart.o inotifyrestart.o 
LLIBS = $(shell $(PKGCONFIG) --libs $(PACKAGES) --cflags)

ifeq ($(PREFIX),)
	PREFIX := /usr/local
endif


%.o: %.c $(DEPS)
		$(CC) -c $< $(CFLAGS) $(LLIBS)

inrestart: $(OBJ)
		$(CC) -o $@ $(OBJ) $(CFLAGS) $(LLIBS)


install: inrestart
	install -d $(PREFIX)/bin
	install -m 755 inrestart $(PREFIX)/bin

.PHONY : clean
clean:
	-rm -f $(ENAME) *.o *~ core 