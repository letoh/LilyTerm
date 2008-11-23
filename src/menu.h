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


#ifndef MENU_H
#define MENU_H

#include <gtk/gtk.h>
#include <vte/vte.h>
#include <glib/gi18n.h>
#include <string.h>

#include "data.h"
#include "profile.h"
#include "dialog.h"
#include "vtefont.h"
#include "notebook.h"

void create_menu(GtkWidget *window);
void set_encoding(GtkWidget *menuitem, GtkWidget *vtebox);
void new_tab_with_locale(GtkWidget *local_menu_item, GtkWidget *menuitem);
void set_trans_bg(GtkWidget *widget, GtkWidget *window);
void set_trans_win(GtkWidget *widget, GtkWidget *window);
void clean_scrollback_lines(GtkWidget *widget, gboolean type);
void reset_vtebox(GtkWidget *widget, gpointer user_data);
void select_font(GtkWidget *widget, GtkWidget *window);
void reset_vtebox_font_size(GtkWidget *widget, gboolean reset_all);

#endif
