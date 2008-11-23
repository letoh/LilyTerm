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
// for exit() and env()
#include <stdlib.h>
// for sockaddr_un
#include <sys/un.h>

#include "data.h"
#include "profile.h"
#include "dialog.h"
#include "pagename.h"
#include "menu.h"
#include "main.h"

GtkWidget *add_page(GtkWidget *window, GtkWidget *notebook, GtkWidget *menuitem_encoding,
		    gchar *locale, gchar *environ, gboolean run_once, gint VTE_CJK_WIDTH);
gboolean close_page (GtkWidget *vtebox, gboolean need_safe_close);
gboolean vtebox_button_press(GtkWidget *widget, GdkEventButton *event, GtkWidget *window);
void vtebox_grab_focus(GtkWidget *vtebox,GtkWidget *window);
void vtebox_style_set (GtkWidget *vtebox, GtkStyle *previous_style, gpointer user_data);
void vtebox_size_request (GtkWidget *vtebox, GtkRequisition *requisition, gpointer user_data);
void vtebox_size_allocate (GtkWidget *vtebox, GtkAllocation *allocation, gpointer user_data);
void set_VTE_CJK_WIDTH_environ(gint VTE_CJK_WIDTH);

#endif
