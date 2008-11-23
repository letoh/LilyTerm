bindir = /usr/bin
etcdir = /etc/xdg
datadir = /usr/share
CC = gcc
CFLAGS = $$CFLAGS -Wall -g -Os
LDFLAGS = -L/usr/local/lib -lvte -lgtk-x11-2.0

INCLUDES = -DENABLE_DEVELOP \
	   -DENABLE_RGBA \
	   -DUSE_TIMEOUT_SECONDS \
	   -DENABLE_TAB_REORDER \
	   -DENABLE_SELECT_ALL \
	   -DENABLE_GDKCOLOR_TO_STRING \
	   -DPACKAGE_NAME=\"LilyTerm\" \
	   -DPACKAGE=\"lilyterm\" \
	   -DVERSION=\"0.9.3\" \
	   -DPACKAGE_BUGREPORT=\"tetrlet@gmail.com\" \
	   -DLOCALEDIR=\"$(datadir)/locale\" \
	   -DICONDIR=\"$(datadir)/pixmaps\"

.PHONY: all
all: debug
	strip --remove-section=.comment --remove-section=.note lilyterm

vtefont.o: data.h profile.h profile.h notebook.h menu.h dialog.h pagename.h vtefont.h vtefont.h vtefont.c
	$(CC) $(CFLAGS) $(INCLUDES) -c vtefont.c `pkg-config --cflags gtk+-2.0 vte`

menu.o: data.h profile.h dialog.h vtefont.h notebook.h menu.h menu.c
	$(CC) $(CFLAGS) $(INCLUDES) -c menu.c `pkg-config --cflags gtk+-2.0 vte`

dialog.o: data.h profile.h window.h notebook.h pagename.h dialog.h dialog.c
	$(CC) $(CFLAGS) $(INCLUDES) -c dialog.c `pkg-config --cflags gtk+-2.0 vte`

profile.o: data.h dialog.h profile.h profile.c
	$(CC) $(CFLAGS) $(INCLUDES) -c profile.c `pkg-config --cflags gtk+-2.0 vte`

pagename.o: data.h profile.h notebook.h pagename.h pagename.c
	# $(CC) $(CFLAGS) $(INCLUDES) -c pagename.c `pkg-config --cflags gtk+-2.0 libgtop-2.0`
	$(CC) $(CFLAGS) $(INCLUDES) -c pagename.c `pkg-config --cflags gtk+-2.0 vte`

notebook.o: data.h profile.h dialog.h pagename.h menu.h main.h notebook.h notebook.c
	$(CC) $(CFLAGS) $(INCLUDES) -c notebook.c `pkg-config --cflags gtk+-2.0 vte`

window.o: data.h profile.h notebook.h menu.h dialog.h pagename.h vtefont.h window.h window.c
	$(CC) $(CFLAGS) $(INCLUDES) -c window.c `pkg-config --cflags gtk+-2.0 vte`

main.o: data.h window.h main.h main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c `pkg-config --cflags gtk+-2.0 vte`

.PHONY: clean
clean:
	-rm lilyterm
	-rm *.o
	-rm ../po/*.mo

.PHONY: install
install:
	install lilyterm $(bindir)
	mkdir -p $(etcdir)
	install ../data/$(PACKAGE).rc $(etcdir)
	mkdir -p $(datadir)/pixmaps
	install ../data/lilyterm.png $(datadir)/pixmaps
	mkdir -p $(datadir)/doc/$(PACKAGE)/examples
	install ../data/$(PACKAGE).rc $(datadir)/doc/$(PACKAGE)/examples
	mkdir -p $(datadir)/locale/zh_TW/LC_MESSAGES/
	install ../po/zh_TW.mo $(datadir)/locale/zh_TW/LC_MESSAGES/lilyterm.mo
	mkdir -p $(datadir)/locale/de/LC_MESSAGES/
	install ../po/de.mo $(datadir)/locale/de/LC_MESSAGES/lilyterm.mo

.PHONY: uninstall
uninstall:
	-rm -f $(bindir)/lilyterm
	-rm -f $(etcdir)/$(PACKAGE).rc
	-rmdir $(etcdir)
	-rm -f $(datadir)/pixmaps/lilyterm.png
	-rmdir $(datadir)/pixmaps
	-rm -f $(datadir)/doc/$(PACKAGE)/examples/$(PACKAGE).rc
	-rmdir -p $(datadir)/doc/$(PACKAGE)/examples
	-rm -f $(datadir)/locale/zh_TW/LC_MESSAGES/lilyterm.mo
	-rmdir -p $(datadir)/locale/zh_TW/LC_MESSAGES
	-rm -f $(datadir)/locale/de/LC_MESSAGES/lilyterm.mo
	-rmdir -p $(datadir)/locale/de/LC_MESSAGES

.PHONY: data
data: debug
	xgettext --from-code=UTF-8 -k_ -o ../po/lilyterm.pot *.c
	msgmerge ../po/zh_TW.po ../po/lilyterm.pot -o ../po/zh_TW.po
	msgfmt --check --statistics ../po/zh_TW.po -o /dev/null
	msgmerge ../po/de.po ../po/lilyterm.pot -o ../po/de.po
	msgfmt --check --statistics ../po/de.po -o /dev/null
	./lilyterm -p > ../data/lilyterm.rc

.PHONY: debug
debug: menu.o profile.o dialog.o pagename.o notebook.o vtefont.o window.o main.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o lilyterm \
		menu.o profile.o dialog.o pagename.o notebook.o vtefont.o window.o main.o \
		`pkg-config --cflags --libs gtk+-2.0 vte`
	msgfmt -o ../po/zh_TW.mo ../po/zh_TW.po
	msgfmt -o ../po/de.mo ../po/de.po
