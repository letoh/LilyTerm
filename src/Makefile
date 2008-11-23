bindir = /usr/bin
etcdir = /etc/xdg
datadir = /usr/share
CC = gcc
CFLAGS = $$CFLAGS -Wall -Os
LDFLAGS = -L/usr/local/lib -lvte -lgtk-x11-2.0

INCLUDES = -DENABLE_DEVELOP \
	   -DENABLE_RGBA \
	   -DUSE_TIMEOUT_SECONDS \
	   -DENABLE_TAB_REORDER \
	   -DENABLE_SELECT_ALL \
	   -DENABLE_GDKCOLOR_TO_STRING \
	   -DPACKAGE_NAME=\"LilyTerm\" \
	   -DPACKAGE=\"lilyterm\" \
	   -DVERSION=\"0.9.5\" \
	   -DPACKAGE_BUGREPORT=\"tetrlet@gmail.com\" \
	   -DLOCALEDIR=\"$(datadir)/locale\" \
	   -DICONDIR=\"$(datadir)/pixmaps\"

.PHONY: all
all: compile
	strip --remove-section=.comment --remove-section=.note lilyterm_dev

.PHONY: debug
debug: CFLAGS := $(CFLAGS) -g
debug: INCLUDES := $(INCLUDES) -DDEBUG
debug: compile

vtefont.o: data.h profile.h profile.h notebook.h menu.h dialog.h pagename.h vtefont.h vtefont.h vtefont.c
	$(CC) $(CFLAGS) $(INCLUDES) -c vtefont.c `pkg-config --cflags gtk+-2.0 vte`

menu.o: data.h profile.h dialog.h vtefont.h notebook.h menu.h menu.c
	$(CC) $(CFLAGS) $(INCLUDES) -c menu.c `pkg-config --cflags gtk+-2.0 vte`

dialog.o: data.h profile.h window.h notebook.h pagename.h dialog.h dialog.c
	$(CC) $(CFLAGS) $(INCLUDES) -c dialog.c `pkg-config --cflags gtk+-2.0 vte`

profile.o: data.h dialog.h profile.h profile.c
	$(CC) $(CFLAGS) $(INCLUDES) -c profile.c `pkg-config --cflags gtk+-2.0 vte`

pagename.o: data.h profile.h notebook.h pagename.h pagename.c
	$(CC) $(CFLAGS) $(INCLUDES) -c pagename.c `pkg-config --cflags gtk+-2.0 vte`

notebook.o: data.h profile.h dialog.h pagename.h menu.h main.h notebook.h notebook.c
	$(CC) $(CFLAGS) $(INCLUDES) -c notebook.c `pkg-config --cflags gtk+-2.0 vte`

window.o: data.h profile.h notebook.h menu.h dialog.h pagename.h vtefont.h window.h window.c
	$(CC) $(CFLAGS) $(INCLUDES) -c window.c `pkg-config --cflags gtk+-2.0 vte`

main.o: data.h window.h main.h main.c
	$(CC) $(CFLAGS) $(INCLUDES) -c main.c `pkg-config --cflags gtk+-2.0 vte`

.PHONY: clean
clean:
	-rm lilyterm_dev
	-rm *.o
	-rm ../po/*.mo

.PHONY: data
data: compile
	xgettext --from-code=UTF-8 -k_ -o ../po/lilyterm.pot *.c
	msgmerge ../po/zh_TW.po ../po/lilyterm.pot -o ../po/zh_TW.po
	msgfmt --check --statistics ../po/zh_TW.po -o /dev/null
	msgmerge ../po/de.po ../po/lilyterm.pot -o ../po/de.po
	msgfmt --check --statistics ../po/de.po -o /dev/null
	./lilyterm_dev -p > ../data/lilyterm.rc

.PHONY: compile
compile: menu.o profile.o dialog.o pagename.o notebook.o vtefont.o window.o main.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o lilyterm_dev \
		menu.o profile.o dialog.o pagename.o notebook.o vtefont.o window.o main.o \
		`pkg-config --cflags --libs gtk+-2.0 vte`
	msgfmt -o ../po/zh_TW.mo ../po/zh_TW.po
	msgfmt -o ../po/de.mo ../po/de.po
