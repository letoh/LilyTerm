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
void update_tab_name(struct Page *page_data);
void update_window_title(GtkWidget *window, gchar *name);
void update_page_name(GtkWidget *window, GtkWidget *vtebox, GtkWidget *label, gint page_no, 
		      gchar *custom_page_name, const gchar *tab_color, gboolean is_root, gboolean is_bold,
		      gboolean show_encoding, GtkWidget *encoding);
gchar *get_page_name_with_number(gchar *label_name, gint page_no);
gchar *get_tab_name_with_page_names(struct Window *win_data);
gchar *get_tab_name_with_cmdline(pid_t tpgid);
gchar *get_tab_name_with_current_dir(pid_t pid);
gint get_tpgid(pid_t pid);
gchar *get_cmdline(pid_t tpgid);
gboolean check_is_root(pid_t tpgid);
gboolean check_status_data(gchar **status_data);
void change_notebook_color(gboolean is_root);

#endif
