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

void new_window(int argc, char *argv[]);
void window_option(struct Window *win_data, int argc, char *argv[]);
gboolean window_quit(GtkWidget *window, GdkEvent *event, struct Window *win_data);
GString *got_help_message();
gchar *got_profile_sample();
gboolean window_key_press(GtkWidget *widget, GdkEventKey *event, struct Window *win_data);
void deal_key_press(GtkWidget *window, gint type, struct Window *win_data);
void window_style_set (GtkWidget *window, GtkStyle *previous_style, struct Window *win_data);
void window_size_request (GtkWidget *window, GtkRequisition *requisition, struct Window *win_data);
void window_size_allocate (GtkWidget *window, GtkAllocation *allocation, struct Window *win_data);
gboolean window_get_focuse(GtkWidget *window, GdkEventFocus *event, struct Window *win_data);
gboolean window_lost_focuse (GtkWidget *window, GdkEventFocus *event, struct Window *win_data);

#endif
