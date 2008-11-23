bindir = /usr/bin
etcdir = /etc/xdg
datadir = /usr/share
CC = gcc
CFLAGS = $$CFLAGS -Wall -g

INCLUDES = -DENABLE_RGBA \
	   -DUSE_TIMEOUT_SECONDS \
	   -DENABLE_TAB_REORDER \
	   -DENABLE_SELECT_ALL \
	   -DENABLE_GDKCOLOR_TO_STRING \
	   -DPACKAGE_NAME=\"LilyTerm\" \
	   -DPACKAGE=\"lilyterm\" \
	   -DVERSION=\"0.9.1\" \
	   -DPACKAGE_BUGREPORT=\"tetrlet@gmail.com\" \
	   -DLOCALEDIR=\"$(datadir)/locale\" \
	   -DICONDIR=\"$(datadir)/pixmaps\"

.PHONY: all
all: debug
	strip lilyterm

menu.o: data.h menu.h menu.c
	$(CC) $(CFLAGS) $(INCLUDES) -c menu.c `pkg-config --cflags gtk+-2.0`

profile.o: data.h profile.h profile.c
	$(CC) $(CFLAGS) $(INCLUDES) -c profile.c `pkg-config --cflags gtk+-2.0`

dialog.o: data.h dialog.h dialog.c
	$(CC) $(CFLAGS) $(INCLUDES) -c dialog.c `pkg-config --cflags gtk+-2.0`

pagename.o: data.h pagename.h pagename.c
	# $(CC) $(CFLAGS) $(INCLUDES) -c pagename.c `pkg-config --cflags gtk+-2.0 libgtop-2.0`
	$(CC) $(CFLAGS) $(INCLUDES) -c pagename.c `pkg-config --cflags gtk+-2.0`

notebook.o: data.h notebook.h notebook.c
	$(CC) $(CFLAGS) $(INCLUDES) -c notebook.c `pkg-config --cflags gtk+-2.0`

vtefont.o: data.h vtefont.h vtefont.c
	$(CC) $(CFLAGS) $(INCLUDES) -c vtefont.c `pkg-config --cflags gtk+-2.0`

main.o: data.h main.h main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c `pkg-config --cflags gtk+-2.0`

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
debug: menu.o profile.o dialog.o pagename.o notebook.o vtefont.o main.o
	# $(CC) $(CFLAGS) -o lilyterm menu.o profile.o dialog.o pagename.o notebook.o vtefont.o main.o `pkg-config --cflags --libs gtk+-2.0 vte libgtop-2.0`
	$(CC) $(CFLAGS) -o lilyterm menu.o profile.o dialog.o pagename.o notebook.o vtefont.o main.o `pkg-config --cflags --libs gtk+-2.0 vte`
	msgfmt -o ../po/zh_TW.mo ../po/zh_TW.po
	msgfmt -o ../po/de.mo ../po/de.po
