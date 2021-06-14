OPTIMFLAGS= -g -O
PACKAGES= libsystemd
PKGCONFIG= pkg-config
ENAME= inrestart
SERVICENAME=teamcity.service
FPATH=/etc/alternatives
FFILE=jre
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

serviceinstall: 
	cp inotifyrestart.service /tmp/
	sed -i 's_INPATH_$(FPATH)_' /tmp/inotifyrestart.service
	sed -i 's_INFILE_$(FFILE)_' /tmp/inotifyrestart.service
	sed -i 's_INSERVICE_$(SERVICENAME)_' /tmp/inotifyrestart.service
	mv /tmp/inotifyrestart.service /etc/systemd/system
	systemctl daemon-reload
	systemctl enable inotifyrestart.service

serviceremove:
	systemctl disable inotifyrestart.service
	rm -yf /etc/systemd/system/inotifyrestart.service
	systemctl daemon-reload

.PHONY : clean
clean:
	-rm -f $(ENAME) *.o *~ core 