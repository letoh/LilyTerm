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

#ifndef WINDOW_H
#define WINDOW_H

#include <gtk/gtk.h>
#include <vte/vte.h>
// for L10n
#include <locale.h>
#include <glib/gi18n.h>
// for exit()
#include <stdlib.h>
// for strcmp()
#include <string.h>

#include "data.h"
#include "profile.h"
#include "notebook.h"
#include "menu.h"
#include "dialog.h"
#include "pagename.h"
#include "vtefont.h"

#define ALL_ACCELS_MASK (GDK_CONTROL_MASK | GDK_SHIFT_MASK | GDK_MOD1_MASK | GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK)
#define SHIFT_ONLY_MASK (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK)
#define DUD_MASK (GDK_LOCK_MASK | GDK_MOD2_MASK | ( ~GDK_MODIFIER_MASK))
GtkNotebook *new_window(int argc,
			char *argv[],
			gchar *environment,
			gchar *PWD,
			gchar *VTE_CJK_WIDTH_STR,
			gchar *user_environ,
			gchar *encoding,
			struct Window *win_data_orig,
			struct Page *page_data_orig);
void set_window_icon(GtkWidget *window);
gboolean window_option(struct Window *win_data, gchar *encoding, int argc, char *argv[]);
gboolean window_quit(GtkWidget *window, GdkEvent *event, struct Window *win_data);
gchar *got_profile_sample();
gboolean window_key_press(GtkWidget *widget, GdkEventKey *event, struct Window *win_data);
void deal_key_press(GtkWidget *window, gint type, struct Window *win_data);
void window_style_set (GtkWidget *window, GtkStyle *previous_style, struct Window *win_data);
void window_size_request (GtkWidget *window, GtkRequisition *requisition, struct Window *win_data);
void window_size_allocate (GtkWidget *window, GtkAllocation *allocation, struct Window *win_data);
gboolean window_get_focus(GtkWidget *window, GdkEventFocus *event, struct Window *win_data);
gboolean window_lost_focus (GtkWidget *window, GdkEventFocus *event, struct Window *win_data);
void notebook_page_added (GtkNotebook *notebook, GtkWidget *child, guint page_num, struct Window *win_data);
void remove_notebook_page (GtkNotebook *notebook, GtkWidget *child, guint page_num, struct Window *win_data);
// void notebook_page_removed (GtkNotebook *notebook, GtkWidget *child, guint page_num, struct Window *win_data);
void reorder_page_after_added_removed_page(struct Window *win_data, guint page_num);
void destroy_window(struct Window *win_data);
void update_window_hint(struct Window *win_data,
			struct Page *page_data);
GtkNotebook* create_window (GtkNotebook *notebook, GtkWidget *page, gint x, gint y,
			    struct Window *win_data);
gboolean window_state_event (GtkWidget *widget, GdkEventWindowState *event, struct Window *win_data);
void keep_window_size (struct Window *win_data, GtkWidget *vte, guint keep_vte_size);
#ifdef DEBUG
void dump_data (struct Window *win_data, struct Page *page_data);
void print_array(gchar *name, gchar **data);
void print_color(gchar *name, GdkColor color);
#endif

#endif
