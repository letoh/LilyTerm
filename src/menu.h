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

#ifndef MENU_H
#define MENU_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib/gi18n.h>
#include <string.h>

#include "data.h"
#include "environ.h"
#include "profile.h"
#include "dialog.h"
#include "vtefont.h"
#include "notebook.h"

void create_menu(struct Window *win_data);
void set_encoding(GtkWidget *menuitem, GtkWidget *vte);
void new_tab_with_locale(GtkWidget *local_menuitem, gboolean VTE_CJK_WIDTH);
GtkWidget *check_encoding_in_menuitem(GtkWidget *sub_menu, const gchar *name);
gboolean check_locale_in_menuitem(GList *widget_list, const gchar *name);
GtkWidget *add_menuitem_to_encoding_sub_menu(struct Window *win_data,
				       gint no,
				       const gchar *name);
void set_trans_bg(GtkWidget *menuitem_trans_bg, struct Window *win_data);
void set_trans_win(GtkWidget *widget, GtkWidget *window);
void hide_scrollback_lines(GtkWidget *widget, gpointer user_data);
void clean_scrollback_lines(GtkWidget *widget, gpointer user_data);
void reset_vte(GtkWidget *widget, gpointer user_data);
void select_font(GtkWidget *widget, GtkWidget *window);
void reset_vte_font_size(GtkWidget *widget, gboolean reset_all);
void set_cursor_blinks(GtkWidget *widget, struct Window *win_data);
void set_audible_bell(GtkWidget *widget, struct Window *win_data);
void launch_hide_and_show_tabs_bar(GtkWidget *widget, gboolean show_tabs_bar);
void hide_and_show_tabs_bar(struct Window *win_data , gboolean show_tabs_bar);
void copy_url_clipboard(GtkWidget *widget, gpointer user_data);
void copy_clipboard(GtkWidget *widget, gpointer user_data);
void paste_clipboard(GtkWidget *widget, gpointer user_data);

#endif
