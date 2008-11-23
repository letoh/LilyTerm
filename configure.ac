#                                               -*- Autoconf -*-
# Process this file with autoconf to produce a configure script.

AC_PREREQ(2.61)
AC_INIT(LilyTerm, 0.9.6, tetralet@gmail.com)
AM_INIT_AUTOMAKE
AC_CONFIG_SRCDIR([src/main.c])
AC_CONFIG_HEADERS([config.h])

# Checks for programs.
AC_PROG_CC
AM_PROG_CC_C_O

# Checks for libraries.
AM_PATH_GTK_2_0(2.8.20,,AC_MSG_ERROR([You need Gtk+ >= 2.8.20 to build $PACKAGE]))
AM_PATH_GLIB_2_0(2.12.4,,AC_MSG_ERROR([You need Glib >= 2.12.4 to build $PACKAGE]))
PKG_CHECK_MODULES(vte, [vte >= 0.12.2],, AC_MSG_ERROR([You need libvte >= 0.12.2 to build $PACKAGE]))

AC_ARG_ENABLE(debug, AC_HELP_STRING([--enable-debug], [turn on debug]), CFLAGS="$CFLAGS -g")
PKG_CHECK_EXISTS(gtk+-2.0 >= 2.10,
		 AC_DEFINE(ENABLE_TAB_REORDER,, [Define only if gtk+-2.0 >= 2.10.0.])
		 AC_DEFINE(ENABLE_RGBA,, [Define only if gtk+-2.0 >= 2.10.0.])
		 AC_DEFINE(ENABLE_GDKCOLOR_TO_STRING,, [Define only if gtk+-2.0 >= 2.10.0.]))
PKG_CHECK_EXISTS(vte >= 0.13, AC_DEFINE(ENABLE_SELECT_ALL,, [Define only if vte >= 0.13.]))
PKG_CHECK_EXISTS(glib-2.0 >= 2.14, AC_DEFINE(USE_TIMEOUT_SECONDS,, [Define only if glib-2.0 >= 2.14.]))

AC_PROG_INTLTOOL
ALL_LINGUAS="zh_TW de"
GETTEXT_PACKAGE="$PACKAGE"
AC_SUBST(GETTEXT_PACKAGE)
AM_GLIB_GNU_GETTEXT
AM_GLIB_DEFINE_LOCALEDIR(LOCALEDIR)

# Checks for header files.
AC_HEADER_STDC
AC_CHECK_HEADERS([locale.h stdlib.h string.h])

# Checks for typedefs, structures, and compiler characteristics.
AC_C_CONST
AC_TYPE_PID_T

# Checks for library functions.
AC_CHECK_FUNCS([setlocale])

AC_CONFIG_FILES([
Makefile
src/Makefile
po/Makefile.in
data/Makefile
])
AC_OUTPUT
