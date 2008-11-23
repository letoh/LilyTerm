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


#ifndef PROFILE_H
#define PROFILE_H

#include <gtk/gtk.h>
#include <vte/vte.h>
// for L10n
#include <locale.h>
#include <glib/gi18n.h>
// for strlen()
#include <string.h>
// for aoti() aotf()
#include <stdlib.h>

#include "data.h"
#include "dialog.h"

#define MOD 7
struct ModKey
{
	gchar *name;
	guint mod;
};

gchar *get_default_locale();
void init_new_page(GtkWidget *window, struct Window *win_data, GtkWidget *vtebox, char* font_name, gint column, gint row, gint run_once);
void init_window_option(struct Window *win_data);
void init_window_parameter(struct Window *win_data);
void init_function_keys();
void init_mod_keys();
void get_user_settings(GtkWidget *window, struct Window *win_data);
gboolean check_boolean_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key, const gboolean default_vaule);
gchar *check_string_value(GKeyFile *keyfile, const gchar *group_name,
			const gchar *key, gchar *default_value, gboolean enable_empty);
gint check_integer_value(GKeyFile *keyfile, const gchar *group_name,
			 const gchar *key, const gint default_value, gboolean enable_empty, gboolean enable_zero);

gboolean accelerator_parse (const gchar *key_name, const gchar *key_value, guint *key, guint *mods);
void init_rgba(GtkWidget *window, struct Window *win_data);
gboolean set_background_saturation(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vtebox);
gboolean set_window_opacity (GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *window);
void window_resizable(GtkWidget *window, GtkWidget *vtebox, gint run_once, gint minsize);
GString *save_user_settings(GtkWidget *widget, GtkWidget *current_vtebox);
void free_user_settings_data(GError *error, gchar *profile, GString *contents, GKeyFile *keyfile, gboolean use_custom_profile);
void init_pagekeys();

#endif
