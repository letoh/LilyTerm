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

#ifndef DATA_H
#define DATA_H

#ifdef HAVE_CONFIG_H
#	include "config.h"
#endif

// for key value
#include <gdk/gdkkeysyms.h>

#define RCFILE PACKAGE ".conf"
#define SOCKET_DATA_VERSION PACKAGE_NAME "-0.9.7~pre18"
#define RCFILE_FORMAT_VERSION "0.9.7~pre20"
#define NOTEBOOK_GROUP 8

#ifdef DEBUG
#	define KEYS 33
#else
#	define KEYS 32
#endif
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

struct ErrorData
{
	gchar *string;
	gchar *encoding;
};

#define COMMAND 4
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

#define COLOR 16
struct Color
{
	gchar *name;
	gchar *comment;
};

// 1: WinTitle
// 2: CmdLine
// 3: PWD
// 4: Custom
// 5: Root
// 6: Normal
#define PAGE_COLOR 6
struct Page_Color
{
	gchar *name;
	gchar *comment;
	gchar *comment_eng;
};

static const GdkColor color_theme_tango[COLOR] =
{
	{ 0, 0x2e2e, 0x3434, 0x3636 },
	{ 0, 0xcccc, 0x0000, 0x0000 },
	{ 0, 0x4e4e, 0x9a9a, 0x0606 },
	{ 0, 0xc4c4, 0xa0a0, 0x0000 },
	{ 0, 0x3434, 0x6565, 0xa4a4 },
	{ 0, 0x7575, 0x5050, 0x7b7b },
	{ 0, 0x0606, 0x9820, 0x9a9a },
	{ 0, 0xd3d3, 0xd7d7, 0xcfcf },
	{ 0, 0x5555, 0x5757, 0x5353 },
	{ 0, 0xefef, 0x2929, 0x2929 },
	{ 0, 0x8a8a, 0xe2e2, 0x3434 },
	{ 0, 0xfcfc, 0xe9e9, 0x4f4f },
	{ 0, 0x7272, 0x9f9f, 0xcfcf },
	{ 0, 0xadad, 0x7f7f, 0xa8a8 },
	{ 0, 0x3434, 0xe2e2, 0xe2e2 },
	{ 0, 0xeeee, 0xeeee, 0xecec }
};
static const GdkColor color_theme_linux[COLOR] =
{
	{ 0, 0x0000, 0x0000, 0x0000 },
	{ 0, 0xaaaa, 0x0000, 0x0000 },
	{ 0, 0x0000, 0xaaaa, 0x0000 },
	{ 0, 0xaaaa, 0x5555, 0x0000 },
	{ 0, 0x0000, 0x0000, 0xaaaa },
	{ 0, 0xaaaa, 0x0000, 0xaaaa },
	{ 0, 0x0000, 0xaaaa, 0xaaaa },
	{ 0, 0xaaaa, 0xaaaa, 0xaaaa },
	{ 0, 0x5555, 0x5555, 0x5555 },
	{ 0, 0xffff, 0x5555, 0x5555 },
	{ 0, 0x5555, 0xffff, 0x5555 },
	{ 0, 0xffff, 0xffff, 0x5555 },
	{ 0, 0x5555, 0x5555, 0xffff },
	{ 0, 0xffff, 0x5555, 0xffff },
	{ 0, 0x5555, 0xffff, 0xffff },
	{ 0, 0xffff, 0xffff, 0xffff }
};
static const GdkColor color_theme_xterm[COLOR] =
{
	{ 0, 0x0000, 0x0000, 0x0000 },
	{ 0, 0xcdcb, 0x0000, 0x0000 },
	{ 0, 0x0000, 0xcdcb, 0x0000 },
	{ 0, 0xcdcb, 0xcdcb, 0x0000 },
	{ 0, 0x1e1a, 0x908f, 0xffff },
	{ 0, 0xcdcb, 0x0000, 0xcdcb },
	{ 0, 0x0000, 0xcdcb, 0xcdcb },
	{ 0, 0xe5e2, 0xe5e2, 0xe5e2 },
	{ 0, 0x4ccc, 0x4ccc, 0x4ccc },
	{ 0, 0xffff, 0x0000, 0x0000 },
	{ 0, 0x0000, 0xffff, 0x0000 },
	{ 0, 0xffff, 0xffff, 0x0000 },
	{ 0, 0x4645, 0x8281, 0xb4ae },
	{ 0, 0xffff, 0x0000, 0xffff },
	{ 0, 0x0000, 0xffff, 0xffff },
	{ 0, 0xffff, 0xffff, 0xffff }
};
static const GdkColor color_theme_rxvt[COLOR] =
{
	{ 0, 0x0000, 0x0000, 0x0000 },
	{ 0, 0xcdcd, 0x0000, 0x0000 },
	{ 0, 0x0000, 0xcdcd, 0x0000 },
	{ 0, 0xcdcd, 0xcdcd, 0x0000 },
	{ 0, 0x0000, 0x0000, 0xcdcd },
	{ 0, 0xcdcd, 0x0000, 0xcdcd },
	{ 0, 0x0000, 0xcdcd, 0xcdcd },
	{ 0, 0xfafa, 0xebeb, 0xd7d7 },
	{ 0, 0x4040, 0x4040, 0x4040 },
	{ 0, 0xffff, 0x0000, 0x0000 },
	{ 0, 0x0000, 0xffff, 0x0000 },
	{ 0, 0xffff, 0xffff, 0x0000 },
	{ 0, 0x0000, 0x0000, 0xffff },
	{ 0, 0xffff, 0x0000, 0xffff },
	{ 0, 0x0000, 0xffff, 0xffff },
	{ 0, 0xffff, 0xffff, 0xffff }
};
static const GdkColor color_theme_vte_fixed[COLOR] =
{
	{ 0, 0x0000, 0x0000, 0x0000 },
	{ 0, 0xc0c0, 0x0000, 0x0000 },
	{ 0, 0x0000, 0xc0c0, 0x0000 },
	{ 0, 0xc0c0, 0xc0c0, 0x0000 },
	{ 0, 0x0000, 0x0000, 0xc0c0 },
	{ 0, 0xc0c0, 0x0000, 0xc0c0 },
	{ 0, 0x0000, 0xc0c0, 0xc0c0 },
	{ 0, 0xc0c0, 0xc0c0, 0xc0c0 },
	{ 0, 0x3f3f, 0x3f3f, 0x3f3f },
	{ 0, 0xffff, 0x3f3f, 0x3f3f },
	{ 0, 0x3f3f, 0xffff, 0x3f3f },
	{ 0, 0xffff, 0xffff, 0x3f3f },
	{ 0, 0x3f3f, 0x3f3f, 0xffff },
	{ 0, 0xffff, 0x3f3f, 0xffff },
	{ 0, 0x3f3f, 0xffff, 0xffff },
	{ 0, 0xffff, 0xffff, 0xffff }
};

struct Window
{

// ---- environ and command line option ---- //

	// environments: the environ for every lilyterm window, separated with <\t>
	gchar *environment;
	// the default_locale is got from environment
	gchar *default_locale;
	// the locales_list is got from profile
	gchar *locales_list;
	// supported_locales = g_strsplit_set(default_locale + locales_list + custom_locale + LC_CTYPE)
	gchar **supported_locales;
	// emulate_term = xterm, don't change it
	gchar *emulate_term;

	// for env VTE_CJK_WIDTH.
	// VTE_CJK_WIDTH is got from profile
	gint VTE_CJK_WIDTH;
	// VTE_CJK_WIDTH_STR is got from environment
	gchar *VTE_CJK_WIDTH_STR;

	// example: lilyterm -e vim /tmp/foo.txt
	// the argc after -e option
	// Will set to 0 after used. 
	gint argc;
	// The argv[] after -e option, like '/tmp/foo.txt' in the example
	// Will set to NULL after used.
	gchar **argv;
	// The first command in -e option, like 'vim' in the example
	// Will set to NULL after used.
	// * Do NOT free it *
	gchar *command;
	// For -t option
	gint init_tab_number;
	// For -d option
	gchar *init_dir;
	// For -l option
	gboolean login_shell;

	// If the profile is specify with -u option.
	gboolean use_custom_profile;
	gchar *profile;

// ---- the component of a single window ---- //

	gboolean fullscreen;
	gboolean true_fullscreen;
	// 0: No work
	// 1: Normal
	// 2: Force ON
	// 3: Force OFF
	gint fullscreen_show_tab_bar;
	gboolean fullscreen_show_scroll_bar;
	//  0: Normal
	// >0: unfullscreening
	// <0: fullscreening
	gint unfullscreen;

	// the component of a single window
	GtkWidget *window;
	GtkWidget *notebook;
	GtkWidget *current_vte;

	// When The window is lost-focus, update the window title only.
	// Or, The geometry of vte will be incorrect when add a new teb with [Menu]
	gboolean window_shows_current_page;
	// The title specified by -T option.
	// Please see http://www.debian.org/doc/debian-policy/ch-customized-programs.html#s11.8.3 for more details
	gchar *custom_window_title;
	// Only used to change the color of tab when launch the adjesting color dialog
	// gboolean update_window_title_only;
	// the pid of showing on the Window title
	pid_t window_title_tpgid;

	// 0: do NOT use rgba
	// 1: force to use rgba
	// 2: decide by gdk_screen_is_composited()
	gint use_rgba;
	gint use_rgba_orig;
#ifdef ENABLE_RGBA
	gint transparent_window;
	gdouble window_opacity;
#endif

	gboolean enable_function_key;
	struct User_KeyValue user_keys[KEYS];

	// 0: Do nothing
	// 1: Update the hints with base size = font char size
	// 2: Update the hints with base size = 1
	gint update_hints;

	gboolean lost_focus;

	// Trying to keep the vte size:
	//  1, When the page bar was hidden.
	//  2, When the page bar was shown.
	//  3, When the font was changed by right click menu.
	//  4, Increase/decrease window size.
	//  5, Resotre to system/default font.
	//  6, Theme has been changed.
	//  7, Using Dir/Cmdline on pagename.
	//  8, When clicking and dragging a tab
	//  9, Hide/Show scrollbar
	// 10, Fullscreen/Unfullscreen

	// x: Trying to resize 'twice'. setted to 0 after resized.
	// y: Trying to resize 'once'. setted to 0 after resized.
	// 1: Means that the Window is resizing.
	//
	// 0000 0000 0000 00x1 (0x0003): Updating Page Name.
	// 0000 0000 000y x100 (0x001C): Showing/Hiding tabs bar, It should only run window_size_request() once. 
	// 0000 0000 0x10 0000 (0x0060): Changing Themes.
	// 0000 00yx 1000 0000 (0x0380): Change the vte font, It should only run window_size_request() once.
	// 0000 x100 0000 0000 (0x0C00): Hide/Show scrollbar
	// 00x1 0000 0000 0000 (0x3000): Changing the "Window Title" of foreground program. <- useless
	// x100 0000 0000 0000 (0xC000): Fullscreen/Unfullscreen. <- useless

	// 1010 1001 0100 1010 (0xA94A): Check for x, Make y only run window_size_request() once.
	// 0101 0100 1010 0101 (0x54A5): Clean x and y, Means that it is resized.
	
	guint keep_vte_size;

// ---- the component of a single menu ---- //

	GtkWidget *menu;
	gboolean menu_actived;
	GtkWidget *encoding_sub_menu;
	GSList *encoding_group;
	// 1st item, System Default.
	// If the encoding of a vte is different with the default_encoding,
	// The encoding will be shown on the tab name.
	GtkWidget *default_encoding;

#ifdef ENABLE_RGBA
	GtkWidget *menuitem_trans_win;
#endif
	GtkWidget *menuitem_trans_bg;
//	GtkWidget *menuitem_scrollback_lines;

	gboolean show_color_selection_menu;

	gboolean show_resize_menu;
	gdouble font_resize_ratio;
	gdouble window_resize_ratio;

	gboolean show_transparent_menu;
	gboolean show_input_method_menu;
	gboolean show_get_function_key_menu;
	gboolean show_change_page_name_menu;

	// FIXME: May cause segfault
	// enable_hyperlink will enable [Copy URL] menuitem.
	// It can't copy to page_data
	gboolean enable_hyperlink;
	struct User_Command user_command[COMMAND];
	GtkWidget *menuitem_copy_url;

	GtkWidget *menuitem_dim_text;
	GtkWidget *menuitem_cursor_blinks;
	GtkWidget *menuitem_audible_bell;
	GtkWidget *menuitem_show_tabs_bar;
	GtkWidget *menuitem_hide_tabs_bar;
	GtkWidget *menuitem_hide_scroll_bar;

	gboolean show_copy_paste_menu;
	GtkWidget *menuitem_copy;
	GtkWidget *menuitem_paste;

// ---- the color used in vte ---- //

	gchar *foreground_color;
	gchar *background_color;
	GdkColor fg_color;				/* May move to page_data */
	GdkColor fg_color_inactive;			/* May move to page_data */
	GdkColor bg_color;				/* May move to page_data */

	// color datas
	gchar *color_theme;
	gchar *color_theme_orig;
	gchar *color_value[COLOR];

	// using_custom_color = TRUE: use vte_terminal_set_colors() to set the color of vte.
	// using_custom_color = FALSE: use vte_terminal_set_color_foreground/background().
	gboolean using_custom_color;			/* May move to page_data */
	gboolean using_custom_color_orig;		/* May move to page_data */
	// color[] and color_orig[] will always be initd when creating a window.
	GdkColor color[COLOR];				/* May move to page_data */
	GdkColor color_inactive[COLOR];			/* May move to page_data */
	GdkColor color_orig[COLOR];			/* May move to page_data */
	gdouble color_brightness;			/* May move to page_data */
	gdouble color_brightness_orig;			/* May move to page_data */
	gdouble color_brightness_inactive;		/* May move to page_data */

// ---- tabs on notebook ---- //

	gint page_width;				/* Should be take care when drag to another window */
	gboolean fixed_page_width;			/* Should be take care when drag to another window */
	gboolean fill_tab_bar;
	gboolean tabbar_position;

	// page name
	gchar *page_name;				/* Should be take care when drag to another window */
	gboolean reuse_page_names;
	gchar *page_names;
	gchar **splited_page_names;
	gint page_names_no;
	gint max_page_names_no;

	gboolean page_shows_number;			/* Should be take care when drag to another window */
	gboolean page_shows_encoding;			/* Should be take care when drag to another window */

	gboolean page_shows_current_cmdline;		/* May move to page_data */
	gboolean page_shows_window_title;		/* May move to page_data */
	gboolean page_shows_current_dir;		/* May move to page_data */

	// page color and bold
	gboolean use_color_page;			/* May move to page_data */
	gboolean check_root_privileges;			/* May move to page_data */
	gboolean bold_current_page_name;		/* May move to page_data */
	gboolean bold_action_page_name;			/* May move to page_data */
	// We will NOT copy the following data to page_data for performance
	// but trying to keep them to sync with win_data
	gchar *user_page_color[PAGE_COLOR];		/* Should be take care when drag to another window */

// ---- font ---- //

	gboolean font_anti_alias;			/* Should be take care when drag to another window */
	gchar *default_font_name;			/* Should be take care when drag to another window */
	gchar *system_font_name;
	// Only using in <Ctrl><Enter>
	gchar *restore_font_name;

// ---- other settings for init a vte ---- //

	gint default_column;
	gint default_row;
	gint system_column;
	gint system_row;
	gchar *word_chars;				/* Should be take care when drag to another window */
	gboolean show_scrollbar;			/* Should be take care when drag to another window */
	// 0: left
	// 1: right
	gboolean scrollbar_position;			/* Should be take care when drag to another window */

	gint transparent_background;			/* May move to page_data */
	gdouble background_saturation;			/* May move to page_data */

//	gboolean use_scrollback_lines;			/* May move to page_data */
	gint scrollback_lines;				/* May move to page_data */
	gboolean dim_text;
	gboolean cursor_blinks;				/* May move to page_data */
	gboolean audible_bell;				/* May move to page_data */
	gboolean visible_bell;				/* May move to page_data */
	gint erase_binding;				/* May move to page_data */

// ---- other ---- //

	gboolean confirm_to_close_multi_tabs;

	gboolean query_encoding;
	gboolean dialog_actived;
	gboolean kill_color_demo_vte;
	gboolean adding_page;

	// for the error messege shown in dialog. Do NOT to free it!
	struct ErrorData err_str;
};

struct Page
{

// ---- Some data from win_data ---- //

	GtkWidget *window;
	GtkWidget *notebook;
	// current page no on notebook. *for performance*
	guint page_no;

// ---- the component of a single vte ---- //

	GtkWidget *label;
	GtkWidget *hbox;
	GtkWidget *vte;
	GtkWidget *scrollbar;

// ---- environ and encoding ---- //

	// for drag&drop, or it will shows unnecessary "(UTF-8)" on the tab
	gchar *encoding_str;
	// locale: a string like "zh_TW.Big5".
	gchar *locale;
	// environ: The environ that user specify in profile
	gchar *environ;
	// for env VTE_CJK_WIDTH
	gchar *VTE_CJK_WIDTH_STR;

// ---- for the text shown on the tab of notebook ---- //

	// the page_name of this page.
	gchar *page_name;
	// the pid of vte
	pid_t pid;
	// the pid of foreground program
	pid_t new_tpgid;
	// the pid of showing on the tab name
	pid_t displayed_tpgid;
	// The custom page name which inputed by user
	gchar *custom_page_name;
	// [!] The color for this tab. *DON'T FREE IT*
	// It is a point for win_data->colors.
	gchar *tab_color;
	// Current Directory
	gchar *pwd;
	// The running command is root privileges or not
	gboolean is_root;
	// The text of current page is bold or not
	gboolean is_bold;
	// The text of current vte is 'inactived'
	gboolean vte_is_inactived;
	gboolean dim_text_expect;

	// 'sleep': a program that is not using window-title.
	// 'vim': a program that is using window-title.
	//
	// 'dash': a shell that is not using window-title.
	// 'bash'a; a shell that is using window-title.
	// 
	// record "window-title-changed" signal
	// situation: (*: the pid and tpgid is NOT the same)
	//
	//   1: use 'dash' as 'shell'
	//	set shell_use_win_title = NO, USE PWD
	//  *2: run 'sleep' on 'dash'
	//	-> if win_data->window_title and new_window_title are both NULL
	//	use CMDLINE
	//   3: exit from 'sleep' on 'dash'
	//	-> if win_data->window_title and new_window_title are both NULL, and shell_use_win_title = NO
	//	use PWD
	//  *4: run 'vim' on 'dash'
	//	-> if win_data->window_title=NULL and new_window_title!=NULL
	//	use WINTITLE
	//   5: exit from 'vim' on 'dash'
	//	-> if win_data->window_title != new_window_title
	//	clean win_data->window_title, use PWD
	//   6: exit from 'vim' on 'dash', but it don't update the win-title when exiting
	//	-> if win_data->window_title = new_window_title, and shell_use_win_title = NO
	//	clean win_data->window_title, use PWD
	//   7: use 'bash' as 'shell'
	//	set shell_use_win_title = YES, USE WINTITLE
	//  *8: run 'sleep' on 'bash'
	//	-> if win_data->window_title and new_window_title are the same
	//	use CMDLINE
	//   9: exit from 'sleep' on 'bash'
	//	-> if win_data->window_title and new_window_title are the same, and shell_use_win_title = YES
	//	use WINTITLE
	// *10: run 'vim' on 'bash'
	//	-> if win_data->window_title != new_window_title
	//	use WINTITLE
	//  11: exit from 'vim' on 'bash'
	//	-> if win_data->window_title != new_window_title
	//	use WINTITLE
	//  12: exit from 'vim' on 'bash', but it don't update the win-title when exiting
	//	-> if win_data->window_title != new_window_title
	//	use WINTITLE
	//  13: the 'shell' is starting to use window-title.
	//	-> if win_data->window_title != new_window_title
	//	use WINTITLE, shell_use_win_title = YES
	//  14: the 'shell' stop to use window-title.
	//	-> if PWD is changed and window-title is not changed
	//	clean win_data->window_title, set shell_use_win_title = NO, use PWD
	//  15: the running commsnd cleaned the winw-title <- impossiable?
	//  16: the shell cleaned the winw-title <- impossiable?

	//  1: updated
	//  0: nothing happened
	// -1: "window-title-changed" stop working
	gboolean window_title_updated;
	// 0: auto (default)
	// 1: window title
	// 2: command line
	// 3: pwd
	// 4: common
	gint page_update_method;
	gint page_update_method_orig;
	gulong window_title_signal;

	// the id of g_timeout_add_seconds()
	guint timeout_id;

// ---- font ---- //

	// for resize font
	gchar *font_name;
	// font_size = 0: use the data in font_name.
	gint font_size;

// ---- some data came from window---- //

	gboolean check_root_privileges;
	gboolean page_shows_window_title;
	gboolean page_shows_current_dir;
	gboolean page_shows_current_cmdline;
	gboolean bold_action_page_name;

//	gboolean page_shows_current_cmdline;
//	gboolean page_shows_current_dir;
//	gboolean use_color_page;
//	gboolean check_root_privileges;
//	gboolean bold_current_page_name;
//	gboolean bold_action_page_name;

//	GdkColor fg_color;
//	GdkColor bg_color;
//	// using_custom_color = TRUE: use vte_terminal_set_colors() to set the color of vte.
//	// using_custom_color = FALSE: use vte_terminal_set_color_foreground/background().
//	gboolean using_custom_color;
//	gboolean using_custom_color_orig;
//	// color[] and color_orig[] will always be initd when creating a window.
//	GdkColor color[COLOR];
//	GdkColor color_orig[COLOR];
//	gdouble color_brightness_orig;
//	gdouble color_brightness;
//
//	gint transparent_background;
//	gdouble background_saturation;
//
//	gboolean use_scrollback_lines;
//	gint scrollback_lines;
//	gboolean cursor_blinks;
//	gboolean audible_bell;
//	gboolean visible_bell;

	// some data came from window. for the performance of monitor_cmdline
	pid_t *window_title_tpgid;
	gboolean *lost_focus;
	guint *keep_vte_size;
	GtkWidget **current_vte;
	// gboolean *update_window_title_only;
	// a "path" string shown on window_title
	gchar *window_title_pwd;
	gboolean custom_window_title;

// ---- other---- //

	gint tag[COMMAND];
};

#endif
