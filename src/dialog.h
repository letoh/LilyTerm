/*
 * Copyright (c) 2008-2009 Lu, Chao-Ming (Tetralet).  All rights reserved.
 * 
 * This file is part of LilyTerm.
 *
 * LilyTerm is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * LilyTerm is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with LilyTerm.  If not, see <http://www.gnu.org/licenses/>.
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
};

struct ColorSelect
{
	// the function type.
	gint type;
	gboolean recover;

	gchar *demo_text;
	GtkWidget *demo_vte;

	gchar *original_page_color;
	GdkColor original_color;
	gchar *original_custom_page_name;
	GdkColor fg_color;
	GdkColor bg_color;
};

gboolean dialog(GtkWidget *widget, gint style);
gchar *get_colorful_profile(struct Window *win_data);
gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *key_value_label);
gchar *dialog_key_press_join_string(gchar *value, gchar *separator, gchar *mask);
void recover_page_colors(GtkWidget *dialog, GtkWidget *window, GtkWidget *notebook);
void error_dialog(struct Window *win_data, gchar *error_message, gchar *encoding, gint style);
gboolean set_ansi_color(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vte);
void adjest_vte_color (GtkColorSelection *colorselection, GtkWidget *vte);
#endif
