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

#ifndef NOTEBOOK_H
#define NOTEBOOK_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib/gi18n.h>
#include <glib.h>
// for kill
#include <signal.h>
// for KEYS, struct Page, struct KeyValue
#include "data.h"

// #define ALL_ACCELS_MASK (GDK_CONTROL_MASK | GDK_SHIFT_MASK | GDK_MOD1_MASK | GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK)
// #define SHIFT_ONLY_MASK (GDK_CONTROL_MASK | GDK_MOD1_MASK | GDK_MOD3_MASK | GDK_MOD4_MASK | GDK_MOD5_MASK)

void add_page(gboolean run_once);
gboolean close_page (GtkWidget *widget, gboolean need_safe_close);
gboolean vtebox_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data);
// gboolean vtebox_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data);
// void deal_key_press(gint type);

void vtebox_grab_focuse(GtkWidget *vtebox, gpointer user_data);
void vtebox_get_focuse(GtkWidget *vtebox, gpointer user_data);
void vtebox_lost_focuse(GtkWidget *vtebox, gpointer user_data);
void vtebox_style_set (GtkWidget *vtebox, GtkStyle *previous_style, gpointer user_data);
void vtebox_size_request (GtkWidget *vtebox, GtkRequisition *requisition, gpointer user_data);
void vtebox_size_allocate (GtkWidget *vtebox, GtkAllocation *allocation, gpointer user_data);

extern void init_new_page(GtkWidget *vtebox, char* font_name, gint column, gint row, gint run_once);
extern void window_resizable(GtkWidget *vtebox, gboolean run_once, gint minsize);
extern gboolean dialog (GtkWidget *widget, gint style);
extern void update_tab_name(gchar *stat_path, GtkWidget *label, pid_t pid, pid_t *tpgid,
			    gint page_no, gchar *custom_page_name);
extern void reorder_page_number(GtkWidget *widget, gpointer user_data);
extern void update_window_title(gchar *name);
extern gboolean monitor_cmdline(gpointer data);
// extern void set_vtebox_font(GtkWidget *widget, gint type);
extern gint get_tpgid(gchar *stat_path, pid_t pid);

#endif
