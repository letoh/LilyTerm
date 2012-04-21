/*
 * Copyright (c) 2008-2010 Lu, Chao-Ming (Tetralet).  All rights reserved.
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

#include "lilyterm.h"

gchar **get_argv(struct Window *win_data, gboolean *argv_need_be_free);
void create_utf8_child_process_failed_dialog (struct Window *win_data, gchar *message, gchar *encoding);
void create_child_process_failed_dialog(struct Window *win_data, gchar *message, gchar *encoding);
void clear_arg(struct Window *win_data);
void label_size_request (GtkWidget *label, GtkRequisition *requisition, struct Page *page_data);
gboolean vte_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
void vte_grab_focus(GtkWidget *vte, gpointer user_data);
gboolean compare_win_page_encoding(GtkWidget *menu_item_encoding, gchar *encoding_str);
// void vte_style_set (GtkWidget *vte, GtkStyle *previous_style, gpointer user_data);
// void vte_size_request (GtkWidget *vte, GtkRequisition *requisition, gpointer user_data);
// void vte_size_allocate (GtkWidget *vte, GtkAllocation *allocation, gpointer user_data);
gchar *get_url (GdkEventButton *event, struct Page *page_data, gint *tag);
void page_data_dup(struct Page *page_data_prev, struct Page *page_data);
// void vte_paste_clipboard (VteTerminal *vte, gpointer user_data);
