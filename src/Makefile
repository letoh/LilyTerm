prefix ?= /usr
bindir = $(prefix)/bin
etcdir = /etc/xdg
datadir = $(prefix)/share
CC = gcc
CFLAGS = $$CFLAGS -Wall -Os
LDFLAGS = -L$(prefix)/local/lib -lvte -lgtk-x11-2.0

INCLUDES = -DENABLE_DEVELOP \
	   -DENABLE_RGBA \
	   -DUSE_TIMEOUT_SECONDS \
	   -DENABLE_TAB_REORDER \
	   -DENABLE_SELECT_ALL \
	   -DENABLE_GDKCOLOR_TO_STRING \
	   -DPACKAGE_NAME=\"LilyTerm\" \
	   -DPACKAGE=\"lilyterm\" \
	   -DVERSION=\"0.9.7\" \
	   -DPACKAGE_BUGREPORT=\"tetrlet@gmail.com\" \
	   -DLOCALEDIR=\"$(datadir)/locale\" \
	   -DICONDIR=\"$(datadir)/pixmaps\" \
	   -DG_DISABLE_DEPRECATED \
	   -DG_DISABLE_SINGLE_INCLUDES \
	   -DGDK_DISABLE_DEPRECATED \
	   -DGDK_DISABLE_SINGLE_INCLUDES \
	   -DGTK_DISABLE_DEPRECATED \
	   -DGTK_DISABLE_SINGLE_INCLUDES

.PHONY: all
all: compile
	strip --remove-section=.comment --remove-section=.note lilyterm_dev

.PHONY: debug
debug: CFLAGS := $(CFLAGS) -g
debug: INCLUDES := $(INCLUDES) -DDEBUG
debug: compile

.PHONY: detail
detail: CFLAGS := $(CFLAGS) -g
detail: INCLUDES := $(INCLUDES) -DDEBUG -DDETAIL
detail: compile

.PHONY: full
full: CFLAGS := $(CFLAGS) -g
full: INCLUDES := $(INCLUDES) -DDEBUG -DDETAIL -DFULL
full: compile

environ.o: environ.c environ.h
	$(CC) $(CFLAGS) $(INCLUDES) -c environ.c `pkg-config --cflags gtk+-2.0 vte`

vtefont.o: data.h profile.h profile.h notebook.h menu.h dialog.h pagename.h window.h vtefont.h vtefont.c
	$(CC) $(CFLAGS) $(INCLUDES) -c vtefont.c `pkg-config --cflags gtk+-2.0 vte`

menu.o: data.h environ.h profile.h dialog.h vtefont.h notebook.h window.h menu.h menu.c
	$(CC) $(CFLAGS) $(INCLUDES) -c menu.c `pkg-config --cflags gtk+-2.0 vte`

dialog.o: data.h profile.h window.h notebook.h pagename.h dialog.h dialog.c
	$(CC) $(CFLAGS) $(INCLUDES) -c dialog.c `pkg-config --cflags gtk+-2.0 vte`

profile.o: data.h environ.h dialog.h pagename.h profile.h profile.c
	$(CC) $(CFLAGS) $(INCLUDES) -c profile.c `pkg-config --cflags gtk+-2.0 vte`

pagename.o: data.h profile.h notebook.h pagename.h window.h pagename.c
	$(CC) $(CFLAGS) $(INCLUDES) -c pagename.c `pkg-config --cflags gtk+-2.0 vte`

notebook.o: data.h environ.h profile.h dialog.h pagename.h menu.h main.h window.h notebook.h notebook.c
	$(CC) $(CFLAGS) $(INCLUDES) -c notebook.c `pkg-config --cflags gtk+-2.0 vte`

window.o: data.h environ.h profile.h notebook.h menu.h dialog.h pagename.h vtefont.h window.h window.c
	$(CC) $(CFLAGS) $(INCLUDES) -c window.c `pkg-config --cflags gtk+-2.0 vte`

main.o: data.h environ.h window.h main.h main.c
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
	msgmerge ../po/tr.po ../po/lilyterm.pot -o ../po/tr.po
	msgfmt --check --statistics ../po/tr.po -o /dev/null
	msgmerge ../po/zh_CN.po ../po/lilyterm.pot -o ../po/zh_CN.po
	msgfmt --check --statistics ../po/zh_CN.po -o /dev/null
	./lilyterm_dev -p > ../data/lilyterm.conf
	cd ../po; /usr/bin/intltool-update -m; cd -

.PHONY: compile
compile: environ.o menu.o profile.o dialog.o pagename.o notebook.o vtefont.o window.o main.o
	$(CC) $(LDFLAGS) $(CFLAGS) -o lilyterm_dev \
		environ.o menu.o profile.o dialog.o pagename.o notebook.o vtefont.o window.o main.o \
		`pkg-config --cflags --libs gtk+-2.0 vte`
	msgfmt -o ../po/zh_TW.mo ../po/zh_TW.po
	msgfmt -o ../po/de.mo ../po/de.po
	msgfmt -o ../po/tr.mo ../po/tr.po
	msgfmt -o ../po/zh_CN.mo ../po/zh_CN.po
