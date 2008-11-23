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


#ifndef DIALOG_H
#define DIALOG_H

#include <gtk/gtk.h>
#include <vte/vte.h>
// for L10n
#include <glib/gi18n.h>
// for strlen()
#include <string.h>

#include "data.h"
#include "profile.h"
#include "window.h"
#include "notebook.h"
#include "pagename.h"

struct Dialog
{
#ifdef ENABLE_RGBA
	gboolean original_transparent_window;
	gdouble original_window_opacity;
#endif
	gboolean original_transparent_background;
	
	// For restore to original count of tabs when change the color of tab names.
	gint total_page;
	gint current_page_no;

	GtkWidget *adjustment;
	gboolean tab_1_is_bold;

	// The data come from win_data
	gboolean *kill_color_demo_vte;
};

struct ColorSelect
{
	// the function type.
	gint type;
	gboolean recover;

	gchar *demo_text;
	GtkWidget *demo_vtebox;

	gchar *original_page_color;
	GdkColor original_color;
	GdkColor fg_color;
	GdkColor bg_color;
};

gboolean dialog(GtkWidget *widget, gint style);
gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *key_value_label);
gchar *dialog_key_press_join_string(gchar *value, gchar *separator, gchar *mask);
void set_vtebox_color (GtkColorSelection *colorselection, GtkWidget *vtebox);
void recover_page_colors(GtkWidget *dialog, GtkWidget *window, GtkWidget *notebook);

#endif
