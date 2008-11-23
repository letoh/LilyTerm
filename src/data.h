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


#ifndef DATA_H
#define DATA_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

// for key value
#include <gdk/gdkkeysyms.h>
// #include <gio/gio.h>

#define RCFILE PACKAGE ".rc"
#define KEYS 30
#define COMMAND 4

// KeyValue: only need to init once when LilyTerm starts.
// so that we don't need to free them.
struct KeyValue
{
	gchar *name;
	gchar *comment;
};

// User_KeyValue: can be custom by profile.
// Every LilyTerm window has it's own User_KeyValue
struct User_KeyValue
{
	gchar *value;
	guint key;
	guint mods;
};

struct Command
{
	gchar *name;
	gchar *match;
	gchar *comment;
	gchar *method_name;
	gchar *environ_name;
	gchar *VTE_CJK_WIDTH_name;
};

struct User_Command
{
	gchar *command;
	gint method;
	gchar *environ;
	gchar **environments;
	gint VTE_CJK_WIDTH;
};

struct Window
{
	gchar **argv;
	// The first command in -e option
	gchar *command_line;
	// the argc after -e option
	gint parameter;
	// The argv[] after -e option
	gchar **parameters;
	// For -t option
	gint init_tab_number;

	// the component of a single window
	GtkWidget *notebook;
	GtkWidget *current_vtebox;

	// the component of a single menu
	GtkWidget *menu;
	// 1st item, System Default. for showing menu for the first time
	GtkWidget *default_encoding;
#ifdef ENABLE_RGBA
	GtkWidget *menuitem_trans_win;
#endif
	GtkWidget *menuitem_trans_bg;
	GtkWidget *menuitem_scrollback_lines;
	gboolean use_custom_profile;
	gchar *profile;

	// default settings
	gboolean font_anti_alias;
	gchar *page_name;
	gchar *page_names;
	gchar **splited_page_names;
	gint page_names_no;
	gboolean reuse_page_names;
	gboolean page_shows_number;
	gboolean page_shows_current_cmdline;
	gboolean page_shows_current_dir;
	gboolean page_shows_encoding;
	gboolean use_color_page;
	gboolean check_root_privileges;
	gboolean bold_current_page_name;
	gboolean bold_action_page_name;
	gchar *page_cmdline_color;
	gchar *page_dir_color;
	gchar *page_custom_color;
	gchar *page_root_color;
	gchar *page_normal_color;
	gboolean window_shows_current_page;
	gint page_width;
	gboolean fixed_page_width;
	gboolean fill_tab_bar;
	gboolean tabbar_position;

	gchar *foreground_color;
	gchar *background_color;
	GdkColor fg_color;
	GdkColor bg_color;
	gchar *default_font_name;
	gchar *system_font_name;
	gint default_column;
	gint default_row;
	gint system_column;
	gint system_row;
	// 0: do NOT use rgba
	// 1: force to use rgba
	// 2: decide by gdk_screen_is_composited()
	gint use_rgba;
	gint original_use_rgba;
#ifdef ENABLE_RGBA
	gint transparent_window;
	gdouble window_opacity;
#endif
	gint transparent_background;
	gdouble background_saturation;

	gchar *word_chars;
	gint scrollback_lines;
	// 0: don't use scrollbar
	// 1: right
	// 2: left
	gint scrollbar_position;

	gboolean show_color_selection_menu;
	gboolean show_resize_menu;
	gdouble font_resize_ratio;
	gdouble window_resize_ratio;
	gboolean show_transparent_menu;
	gboolean show_input_method_menu;
	gboolean show_get_function_key_menu;
	gboolean show_change_page_name_menu;

	// environment: the environ for every lilyterm window. Do NOT to free it!
	gchar **environment;
	// the default_locale is got from environment
	gchar *default_locale;
	gchar *locales_list;
	gchar **supported_locales;
	// for env VTE_CJK_WIDTH
	// 0: by environment
	// 1: force to use narrow ideograph
	// 2: force to use wide ideograph
	// default_VTE_CJK_WIDTH: The data come from profile.
	// system_VTE_CJK_WIDTH: The data that is currently using.
	gint default_VTE_CJK_WIDTH;
	gint system_VTE_CJK_WIDTH;

	struct User_KeyValue user_keys[KEYS];
	gboolean enable_hyperlink;
	struct User_Command user_command[COMMAND];

	// 0: Do nothing
	// 1: Update the hints with base size = font char size
	// 2: Update the hints with base size = 1
	gint update_hints;
	gboolean enable_function_key;
	// Trying to keep the vtebox size:
	// 1, When the page bar was hidden.
	// 2, When the page bar was shown.
	// 3, When the font was changed by right click menu.
	// 4, Increase/decrease window size.
	// 5, Resotre to system/default font.
	// 6, Theme has been changed.
	// 7, Using Dir/Cmdline on pagename.
	gboolean lost_focus;
	//  1    : Updating Page Name.
	//  2,  4: Showing/Hiding tab bar, Only run window_size_request() once. 
	//  8    : Changing Themes.
	// 16, 32: Resing Window, Only run window_size_request() once.
	gint keep_vtebox_size;
	gboolean query_encoding;
	gboolean kill_color_demo_vte;
	// for the error messege shown in dialog. Do NOT to free it!
	gchar *temp_str;
	// Only using in <Ctrl><Enter>
	gchar *restore_font_name;
};

struct Page
{
	GtkWidget *window;
	GtkWidget *notebook;

	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *vtebox;
	GtkWidget *scrollbar;

	pid_t pid;
	// the pid of foreground program
	pid_t tpgid;
	// The custom page name which inputed by user
	gchar *custom_page_name;
	// The color for this tab. *DON'T FREE IT*
	gchar *tab_color;
	// Current Directory
	gchar *pwd;
	// The running command is root privileges or not
	gboolean is_root;
	// The text of current page is bold or not
	gboolean is_bold;

	// the id of g_timeout_add_seconds()
	guint timeout_id;

	// for resize font
	gchar *font_name;
	// font_size = 0: use the data in font_name.
	gint font_size;

	gboolean use_scrollback_lines;

	// the encoding menuitem which this page is using.
	GtkWidget *encoding;
	// locale: a string like "LC=ALL zh_TW.Big5".
	gchar *locale;
	// environ: The environ that user specify in profile
	gchar *environ;

	// for env VTE_CJK_WIDTH
	// 0: by environment
	// 1: force to use narrow ideograph
	// 2: force to use wide ideograph
	gint VTE_CJK_WIDTH;

	// current page no on notebook. *for performance*
	gint page_no;
	gint tag[COMMAND];

	// some data came from window. for the performance of monitor_cmdline
	gboolean *lost_focus;
	gint *keep_vtebox_size;
	gboolean *check_root_privileges;
	gboolean *page_shows_current_dir;
	gboolean *page_shows_current_cmdline;
	gboolean *bold_action_page_name;
};

#endif
