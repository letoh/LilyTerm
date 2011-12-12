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

#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib/gi18n.h>
#include <glib.h>
// for kill
#include <signal.h>
// for exit() and env()
#include <stdlib.h>
// for sockaddr_un
#include <sys/un.h>

#include <langinfo.h> 

#include "data.h"
#include "profile.h"
#include "dialog.h"
#include "pagename.h"
#include "menu.h"
#include "main.h"

struct Page *add_page(struct Window *win_data,
		      struct Page *page_data_prev,
		      GtkWidget *menuitem_encoding,
		      gchar *encoding,
		      gchar *locale,
		      gchar *user_environ,
		      gboolean run_once,
		      gchar *VTE_CJK_WIDTH);
void label_size_request (GtkWidget *label, GtkRequisition *requisition, struct Page *page_data);
gboolean close_page (GtkWidget *vte, gboolean need_safe_close);
gboolean vte_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void vte_grab_focus(GtkWidget *vte, gpointer user_data);
void vte_style_set (GtkWidget *vte, GtkStyle *previous_style, gpointer user_data);
void vte_size_request (GtkWidget *vte, GtkRequisition *requisition, gpointer user_data);
void vte_size_allocate (GtkWidget *vte, GtkAllocation *allocation, gpointer user_data);
gboolean compare_win_page_encoding(GtkWidget *encoding, gchar *encoding_str);
gchar *get_encoding_from_menu_item_name(GtkWidget *menuitem);
gchar *get_url (GdkEventButton *event, struct Page *page_data, gint *tag);

#endif
