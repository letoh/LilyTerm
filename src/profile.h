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

#ifndef PROFILE_H
#define PROFILE_H

#define _GNU_SOURCE

#include <gtk/gtk.h>
#include <vte/vte.h>

// for L10n
#include <locale.h>
#include <glib/gi18n.h>

// for strlen()
#include <string.h>

// for aoti() aotf() canonicalize_file_name()
#include <stdlib.h>
// for access()
#include <unistd.h>

#include "data.h"
#include "dialog.h"

#define MOD 6
struct ModKey
{
	gchar *name;
	guint mod;
};

void init_new_page(struct Window *win_data,
		   struct Page *page_data,
		   gint column,
		   gint row,
		   gint run_once);
void set_hyprelink(struct Window *win_data, struct Page *page_data);
void set_vte_color(struct Window *win_data, struct Page *page_data);
void set_page_width(struct Window *win_data, struct Page *page_data);
void pack_vte_and_scrollbar_to_hbox(struct Window *win_data, struct Page *page_data);
void hide_scrollbar(struct Window *win_data, struct Page *page_data);
void add_remove_page_timeout_id(struct Window *win_data, struct Page *page_data);
void add_remove_window_title_changed_signal(struct Page *page_data);
void init_window_parameters(struct Window *win_data);
void init_page_parameters(struct Window *win_data, struct Page *page_data);
void init_monitor_cmdline_datas(struct Window *win_data, struct Page *page_data);
void init_function_keys();
void init_page_color_data();
void init_mod_keys();
void get_user_settings(GtkWidget *window, struct Window *win_data, gchar *encoding);
gboolean check_boolean_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key, const gboolean default_vaule);
gchar *check_string_value(GKeyFile *keyfile, const gchar *group_name,
			 const gchar *key, gchar *default_value, gboolean enable_empty);
gint check_integer_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key,
			 const gint default_value, gboolean enable_empty, gboolean enable_zero,
			 gboolean check_min, gint min, gboolean check_max, gint max);
gdouble check_double_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key, const gdouble default_value);
gboolean accelerator_parse(const gchar *key_name, const gchar *key_value, guint *key, guint *mods);
void clean_accelerator_parse_data(gchar **values, gchar **functions);
void init_rgba(GtkWidget *window, struct Window *win_data);
gboolean set_background_saturation(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vte);
gboolean set_window_opacity (GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *window);
void window_resizable(GtkWidget *window, GtkWidget *vte, gint run_once, gint minsize);
GString *save_user_settings(GtkWidget *widget, GtkWidget *current_vte);
void free_user_settings_data(GError *error, gchar *profile, GString *contents, GKeyFile *keyfile, gboolean use_custom_profile);
void init_pagekeys();
gchar **get_supported_locales(gchar *default_locale, gchar *locales_list, gchar *LC_TYPE, gchar *other_locale);
void win_data_dup(struct Window *win_data_orig, struct Window *win_data);
void page_data_dup(struct Page *page_data_prev, struct Page *page_data);
gchar *get_current_pwd_by_pid(pid_t pid);
void init_user_color_data(struct Window *win_data);
void init_user_color(struct Window *win_data);
void check_color_value (const gchar *key_name, const gchar *color_name, GdkColor *color);
void adjust_ansi_color(GdkColor color[COLOR], GdkColor color_orig[COLOR], gdouble color_brightness);
void adjust_ansi_color_severally(GdkColor *color, GdkColor *color_orig, gdouble color_brightness);
struct Page *get_page_data_from_nth_page(struct Window *win_data, guint page_no);
void apply_new_win_data_to_page (struct Window *win_data_orig,
				 struct Window *win_data,
				 struct Page *page_data);
void apply_new_page_color (struct Window *win_data, struct Page *page_data);
gboolean compare_color(GdkColor a, GdkColor b);
#endif
