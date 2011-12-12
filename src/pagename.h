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

#ifndef PAGENAME_H
#define PAGENAME_H

#include <gtk/gtk.h>
#include <vte/vte.h>
// for aoti()
#include <stdlib.h>
// for usleep()
#include <unistd.h>
// for strcmp()
#include <string.h>

#include "data.h"
#include "profile.h"
#include "notebook.h"

void reorder_page_number (GtkNotebook *notebook, GtkWidget *child, guint page_num, GtkWidget *window);
gboolean monitor_cmdline(struct Page *page_data);
gboolean check_cmdline (struct Page *page_data, pid_t check_tpgid);
gboolean check_update_cmdline(struct Page *page_data, pid_t check_tpgid);
gboolean check_window_title (struct Page *page_data, gboolean lost_focus);
gboolean check_pwd(struct Page *page_data, gchar *pwd, gchar *new_pwd, gint page_update_method);
gboolean get_and_update_page_name(struct Page *page_data, gboolean lost_focus);
void update_window_title(GtkWidget *window, gchar *name);
void update_page_name_wintitle(gchar **page_name,
			      gchar **page_color,
			      struct Window *win_data,
			      struct Page *page_data);
void update_page_name_cmdline(gchar **page_name,
			     gchar **page_color,
			     struct Window *win_data,
			     struct Page *page_data);
void update_page_name_pwd(gchar **page_name,
			 gchar **page_color,
			 struct Window *win_data,
			 struct Page *page_data,
			 gboolean lost_focus);
gboolean update_page_name(GtkWidget *window, GtkWidget *vte, gchar *page_name, GtkWidget *label,
			  gint page_no, gchar *custom_page_name, const gchar *tab_color, gboolean is_root,
			  gboolean is_bold, gboolean show_encoding, gchar *encoding_str,
			  gboolean custom_window_title, gboolean lost_focus);
void update_page_name_normal(gchar **page_name,
			     gchar **page_color,
			     struct Window *win_data,
			     struct Page *page_data);
gchar *get_page_name_with_number(gchar *label_name, gint page_no);
gchar *get_tab_name_with_page_names(struct Window *win_data);
gchar *get_tab_name_with_cmdline(pid_t tpgid);
gchar *get_tab_name_with_current_dir(pid_t pid);
gint get_tpgid(pid_t pid);
gchar *get_cmdline(pid_t tpgid);
gboolean check_is_root(pid_t tpgid);
gboolean check_status_data(gchar **status_data);
void change_notebook_color(gboolean is_root);
void update_page_window_title (VteTerminal *vte, struct Page *page_data);

#endif
