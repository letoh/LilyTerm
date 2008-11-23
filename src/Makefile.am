INCLUDES = -DICONDIR=\"$(datadir)/pixmaps\"

bin_PROGRAMS = lilyterm
lilyterm_CFLAGS  = $(GTK_CFLAGS) $(vte_CFLAGS) $(libgtop_CFLAGS)
lilyterm_LDADD   = $(GTK_LIBS) $(vte_LIBS) $(libgtop_LIBS) $(INTLLIBS)
lilyterm_SOURCES = page.h \
		   menu.h menu.c \
		   profile.h profile.c \
		   dialog.h dialog.c \
		   pagename.h pagename.c \
		   notebook.h notebook.c \
		   vtefont.h vtefont.c \
		   main.h main.c
