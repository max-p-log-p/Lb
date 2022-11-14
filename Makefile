CC = cc
INSTALL = /usr/bin/install -c
INSTALL_PROGRAM = ${INSTALL}
INSTALL_DATA = ${INSTALL} -m 644

CFLAGS = -O2 -Wall -Wextra -Werror -fanalyzer

DESTDIR = $(HOME)/bin
DATADIR = $(HOME)/.lb

install: config/.curlrc html config/lesskey.m4 urlencode/urlencode
	mkdir -p $(DESTDIR)
	mkdir -p $(DATADIR)
	mkdir -p $(DATADIR)/pages
	$(INSTALL_PROGRAM) urlencode/urlencode $(DESTDIR)/urlencode
	$(INSTALL_PROGRAM) ./html $(DESTDIR)/html
	$(INSTALL_DATA) config/.curlrc $(DATADIR)
	$(INSTALL_DATA) config/lesskey.m4 $(DATADIR)
	m4 config/lesskey.m4 | lesskey -o $(DATADIR)/less -

install_doc: lb.1
	$(INSTALL_DATA) ./lb.1 $(HOME)/.lb
	@echo "Set MANPATH to $(HOME)/.lb:\$$MANPATH"

urlencode/urlencode: urlencode/urlencode.c
	$(CC) $(CFLAGS) $^ -o $@

uninstall:
	rm -f $(DESTDIR)/{urlencode,html}
	rm -rf $(HOME)/.lb
	@echo "Check ~/bin"
	@echo "Check alias lb and PATH in ~/.*rc"
	@echo "Check ~/.lb/pages in /etc/fstab"

uninstall_doc:
	rm -f $(HOME)/.lb/lb.1
	@echo "Check MANPATH"

clean:
	rm -f urlencode/urlencode
