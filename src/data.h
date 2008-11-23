/*
 * Copyright (c) 2008 Lu, Chao-Ming (Tetralet).  All rights reserved.
 *
 * Redistribution and use in source and binary forms, with or without
 * modification, are permitted provided that the following conditions
 * are met:
 * 1. Redistributions of source code must retain the above copyright
 *    notice, this list of conditions and the following disclaimer.
 * 2. Redistributions in binary form must reproduce the above copyright
 *    notice, this list of conditions and the following disclaimer in the
 *    documentation and/or other materials provided with the distribution.
 *
 * THIS SOFTWARE IS PROVIDED BY THE AUTHOR ``AS IS'' AND ANY EXPRESS OR
 * IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES
 * OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
 * IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR ANY DIRECT, INDIRECT,
 * INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT
 * NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 * DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 * THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF
 * THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
 */


#ifndef DATA_H
#define DATA_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

// for key value
#include <gdk/gdkkeysyms.h>
// #include <gio/gio.h>

#define RCFILE PACKAGE ".rc"
#define KEYS 30
#define LINUX_TPGID 7

struct Page
{
	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *vtebox;
	GtkWidget *scrollbar;

	pid_t pid;
	// the pid of foreground program
	pid_t tpgid;
	// the path to /proc/pid/stat. *for performance*
	gchar *stat_path;
	// the id of g_timeout_add_seconds()
	guint timeout_id;

	// for resize font
	gchar *font_name;
	gint font_size;

	// for restore the vtebox size
	gint column;
	gint row;

	gchar *custom_page_name;
	GtkWidget *encoding;
	// current page no on notebook. *for performance*
	gint current_page_no;
};

struct KeyValue
{
	gchar *name;
	gchar *value;
	guint key;
	guint mods;
};

#endif
