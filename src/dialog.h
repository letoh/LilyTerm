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
// for struct Page
#include "data.h"

gboolean dialog(GtkWidget *widget, gint style);
extern gboolean set_background_saturation(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vtebox);
extern gboolean set_window_opacity(GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data);

gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *key_value_label);
gchar *dialog_key_press_join_string(gchar *value, gchar *separator, gchar *mask);
void set_vtebox_color (GtkColorSelection *colorselection, GtkWidget *vtebox);
void recover_page_colors();

extern gboolean close_page(GtkWidget *vtebox, gboolean need_safe_close);
extern GString *got_help_message();
extern gchar *got_profile_sample();
extern void add_page(gboolean run_once);
extern void update_tab_name(GtkWidget *vtebox, GtkWidget *label, pid_t pid, pid_t tpgid,
		     gint page_no, gchar *custom_page_name, const gchar *pwd, gboolean is_root, gboolean is_bold);
extern void update_page_name(GtkWidget *vtebox, GtkWidget *label, gint page_no, gchar *custom_page_name,
			     const gchar* tab_color, gboolean is_root, gboolean bold);
extern void change_notebook_color(gboolean is_root);
extern gint get_tpgid(pid_t pid);
extern gchar *get_cmdline(pid_t tpgid);

#endif
