CC = cc
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

CFLAGS = -O2 -Wall -Werror -Wpedantic

DESTDIR = $(HOME)/bin

install: html lb lesskey.m4 urlencode
	mkdir -p $(DESTDIR)
	$(INSTALL_PROGRAM) ./urlencode $(DESTDIR)/urlencode
	$(INSTALL_PROGRAM) ./lb $(DESTDIR)/lb
	$(INSTALL_PROGRAM) ./html $(DESTDIR)/html
	mkdir -p $(HOME)/.lb/pages
	m4 ./lesskey.m4 | lesskey -o $(HOME)/.lb/less -

urlencode: urlencode_wrapper.c urlencode.c
	$(CC) $(CFLAGS) $^ -o $@

html: html.c urlencode.c
	$(CC) $(CFLAGS) $^ -o $@ -lxml2 -I/usr/include/libxml2

uninstall:
	rm -f $(DESTDIR)/{urlencode,lb,html}
	rm -f $(HOME)/.lesskey
	rm -rf $(HOME)/.lb

clean:
	rm -f urlencode html
