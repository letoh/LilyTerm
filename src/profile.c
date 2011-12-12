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

#include "profile.h"

extern gint total_window;
extern GtkWidget *menu_active_window;

struct ModKey modkeys[MOD] = {{0}};
struct KeyValue system_keys[KEYS] = {{0}};
struct Command command[COMMAND] = {{0}};
struct Color color[COLOR] = {{0}};
struct Page_Color page_color[PAGE_COLOR] = {{0}};

void init_command()
{
#ifdef DETAIL
	g_debug("! Launch init_command()!");
#endif

	#define USER "[A-Za-z][-A-Za-z0-9.]*"
	#define PASS "(:[^ \t\r\n]+)?"
	#define LOGIN "(" USER PASS "@)?"
	#define HOST "[A-Za-z0-9][-A-Za-z0-9.]+\\.[A-Za-z0-9]+[-A-Za-z0-9.]*[-A-Za-z0-9]*"
	#define SPECIAL "([^.< \t\r\n,;\\\"]*([,.;][^< \t\r\n\\\"])?[^.< \t\r\n,;\\\"]*)*"
	#define END "[^< \t\r\n,;\\\"]*[^.< \t\r\n,;\\\"]"

	// WWW
	command[0].name = "web_browser";
	command[0].match = "[Hh][Tt][Tt][Pp][Ss]?://" LOGIN HOST "(/" SPECIAL END ")?/*";
	command[0].comment = "# The web browser using for http(s)://";
	command[0].method_name = "web_method";
	command[0].environ_name = "web_environ";
	command[0].VTE_CJK_WIDTH_name = "web_VTE_CJK_WIDTH";

	// FTP
	command[1].name = "ftp_client";
	command[1].match = "[Ff][Tt][Pp][Ss]?://" LOGIN HOST "(/" SPECIAL END ")?/*";
	command[1].comment = "# The ftp client using for ftp(s)://";
	command[1].method_name = "ftp_method";
	command[1].environ_name = "ftp_environ";
	command[1].VTE_CJK_WIDTH_name = "ftp_VTE_CJK_WIDTH";

	// FILE
	command[2].name = "file_manager";
	command[2].match = "[Ff][Ii][Ll][Ee]:///" SPECIAL END;
	command[2].comment = "# The file manager using for file://";
	command[2].method_name = "file_method";
	command[2].environ_name = "file_environ";
	command[2].VTE_CJK_WIDTH_name = "file_VTE_CJK_WIDTH";

	// MAIL
	command[3].name = "email_client";
	command[3].match = "([Mm][Aa][Ii][Ll][Tt][Oo]:)?" USER "@" HOST;
	command[3].comment = "# The email client using for user@host";
	command[3].method_name = "email_method";
	command[3].environ_name = "email_environ";
	command[3].VTE_CJK_WIDTH_name = "email_VTE_CJK_WIDTH";
}

void init_user_command(struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch init_user_command() for win_data %p", win_data);
#endif

	win_data->user_command[0].command = g_strdup("firefox");
	win_data->user_command[1].command = g_strdup("firefox");
	win_data->user_command[2].command = g_strdup("firefox");
	win_data->user_command[3].command = g_strdup("thunderbird");

	gint i;
	for (i=0;i<COMMAND;i++)
	{
		win_data->user_command[i].method = 1;
		win_data->user_command[i].environ = g_strdup("");
		win_data->user_command[i].VTE_CJK_WIDTH = 0;
	}
}

void init_window_parameters(struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch init_window_parameters() with win_data = %p", win_data);
#endif
	// win_data->environment;
	// win_data->default_locale;
	win_data->locales_list = g_strdup("ja_JP.EUC-JP zh_CN.GB2312 zh_TW.Big5");
	// win_data->supported_locales;
	win_data->emulate_term = g_strdup("xterm");
	win_data->VTE_CJK_WIDTH = 1;
	// win_data->VTE_CJK_WIDTH_STR;
	// win_data->argc;
	// win_data->argv;
	// win_data->command;
	// win_data->init_tab_number;
	// win_data->init_dir;
	// win_data->use_custom_profile;
	// win_data->profile;
	// win_data->fullscreen;
	// win_data->true_fullscreen;
	// win_data->fullscreen_show_tab_bar;
	// win_data->fullscreen_show_scroll_bar;
	// win_data->unfullscreen;
	// win_data->window;
	// win_data->notebook;
	// win_data->current_vte;
	win_data->window_shows_current_page = TRUE;
	// win_data->custom_window_title;
	// win_data->window_title_tpgid;
	// 0: do NOT use rgba
	// 1: force to use rgba
	// 2: decide by gdk_screen_is_composited()
	win_data->use_rgba = 2;
	// win_data->use_rgba_orig = 2;
#ifdef	ENABLE_RGBA
	win_data->transparent_window = 2;
	win_data->window_opacity = 0.05;
#endif
	win_data->enable_function_key = TRUE;
	// win_data->User_KeyValue user_keys[KEYS];
	// win_data->update_hints;
	win_data->lost_focus = FALSE;
	// win_data->keep_vte_size;
	// win_data->menu;
	// win_data->menu_actived;
	// win_data->encoding_sub_menu;
	// win_data->encoding_group;
	// win_data->default_encoding;
#ifdef	ENABLE_RGBA
	// win_data->menuitem_trans_win;
#endif
	// win_data->menuitem_trans_bg;
	win_data->show_color_selection_menu = TRUE;
	win_data->show_resize_menu = TRUE;
	win_data->font_resize_ratio = 0;
	win_data->window_resize_ratio = 1.12;
	win_data->show_transparent_menu = TRUE;
	// win_data->show_input_method_menu = FALSE;
	win_data->show_get_function_key_menu = TRUE;
	// win_data->show_change_page_name_menu = FALSE;
	win_data->enable_hyperlink = TRUE;
	// win_data->User_Command user_command[COMMAND];
	// win_data->menuitem_copy_url;
	// win_data->menuitem_dim_text;
	// win_data->menuitem_cursor_blinks;
	// win_data->menuitem_audible_bell;
	// win_data->menuitem_show_tabs_bar;
	// win_data->menuitem_hide_tabs_bar;
	// win_data->menuitem_hide_scroll_bar;
	win_data->show_copy_paste_menu = TRUE;
	// win_data->menuitem_copy;
	// win_data->menuitem_paste;
	win_data->foreground_color = g_strdup("white");
	win_data->background_color = g_strdup("black");
	// win_data->fg_color;
	// win_data->fg_color_inactive;
	// win_data->bg_color;
	// win_data->color_theme;
	// win_data->color_theme_orig;
	// win_data->color_value[COLOR];
	// win_data->using_custom_color;
	// win_data->using_custom_color_orig;
	// win_data->color[COLOR];
	// win_data->color_inactive[COLOR];
	// win_data->color_orig[COLOR];
	win_data->color_brightness = 0.2;
	// win_data->color_brightness_orig;
	win_data->color_brightness_inactive = -0.3;
	win_data->page_width = 16;
	win_data->fixed_page_width = TRUE;
	// win_data->fill_tab_bar;
	// win_data->tabbar_position;
	win_data->page_name = g_strdup("Terminal");
	win_data->reuse_page_names = TRUE;
	win_data->page_names = g_strdup("Terminal");
	// win_data->splited_page_names;
	// win_data->page_names_no;
	// win_data->max_page_names_no;
	win_data->page_shows_number = TRUE;
	win_data->page_shows_encoding = TRUE;
	win_data->page_shows_current_cmdline = TRUE;
	win_data->page_shows_window_title = TRUE;
	win_data->page_shows_current_dir = TRUE;
	win_data->use_color_page = TRUE;
	win_data->check_root_privileges = TRUE;
	win_data->bold_current_page_name = TRUE;
	win_data->bold_action_page_name = TRUE;
	win_data->user_page_color[0] = g_strdup("#9A6401");
	win_data->user_page_color[1] = g_strdup("#1C1CDC");
	win_data->user_page_color[2] = g_strdup("#215E3E");
	win_data->user_page_color[3] = g_strdup("#9C0A81");
	win_data->user_page_color[4] = g_strdup("#BE0020");
	win_data->user_page_color[5] = g_strdup("#333333");
	win_data->font_anti_alias = VTE_ANTI_ALIAS_USE_DEFAULT;
	win_data->default_font_name = g_strdup("Monospace 12");
	win_data->system_font_name = g_strdup("Monospace 12");
	// win_data->restore_font_name;
	win_data->default_column = 80;
	win_data->default_row = 24;
	win_data->system_column = 80;
	win_data->system_row = 24;
	win_data->word_chars = g_strdup("-A-Za-z0-9_$.+!*(),;:@&=?/~#%[]<>");
	win_data->show_scrollbar = 1;
	// 0: left
	// 1: right
	win_data->scrollbar_position = 1;
	win_data->transparent_background = 2;
	win_data->background_saturation = 0.15;
	win_data->confirm_to_close_multi_tabs = FALSE;
	win_data->scrollback_lines = 1024;
	win_data->cursor_blinks = TRUE;
	win_data->dim_text = TRUE;
	win_data->audible_bell = TRUE;
	// win_data->visible_bell = FALSE;
	win_data->erase_binding = VTE_ERASE_ASCII_DELETE;
	// win_data->query_encoding;
	// win_data->dialog_actived;
	// win_data->kill_color_demo_vte;
	// win_data->temp_str;
}

void init_page_parameters(struct Window *win_data, struct Page *page_data)
{
	page_data->window = win_data->window;
	page_data->notebook = win_data->notebook;

//	page_data->page_no = 0;
//	page_data->label = NULL;
//	page_data->hbox = NULL;
//	page_data->vte = NULL;
//	page_data->scrollbar = NULL;

	page_data->encoding_str = get_encoding_from_menu_item_name(win_data->default_encoding);
	page_data->locale = g_strdup(win_data->default_locale);
	page_data->environ = g_strdup("");
	// g_debug("g_strdupv(page_data_prev->environments)!");
	// page_data->environments = g_strdupv(win_data->environments);
	page_data->VTE_CJK_WIDTH_STR = g_strdup(win_data->VTE_CJK_WIDTH_STR);

//	page_data->page_name = NULL;
//	page_data->_t pid = 0;
//	page_data->_t new_tpgid = 0;
//	page_data->_t displayed_tpgid = 0;
//	page_data->custom_page_name = NULL;
//	page_data->tab_color = NULL;
	page_data->pwd = g_strdup(win_data->init_dir);
//	page_data->is_root = 0;
//	page_data->is_bold = 0;
//	page_data->window_title_updated = 0;
//	page_data->page_update_method = 0;
//	page_data->page_update_method_orig = 0;
//	page_data->window_title_signal = 0;

//	page_data->timeout_id = 0;

	page_data->font_name = g_strdup(win_data->default_font_name);
//	page_data->font_size = 0;

	// some data came from window. for the performance of monitor_cmdline
	init_monitor_cmdline_datas(win_data, page_data);

//	page_data->use_scrollback_lines = win_data->use_scrollback_lines;

//	page_data->window_title = NULL;

//	page_data->tag[COMMAND] = {{0}};
}

void init_monitor_cmdline_datas(struct Window *win_data, struct Page *page_data)
{
	page_data->window_title_updated = -1;

	page_data->check_root_privileges = win_data->check_root_privileges;
	page_data->page_shows_window_title = win_data->page_shows_window_title;
	page_data->page_shows_current_dir = win_data->page_shows_current_dir;
	page_data->page_shows_current_cmdline = win_data->page_shows_current_cmdline;
	page_data->bold_action_page_name = win_data->bold_action_page_name;

	page_data->window_title_tpgid = &(win_data->window_title_tpgid);
	page_data->lost_focus = &(win_data->lost_focus);
	page_data->keep_vte_size = &(win_data->keep_vte_size);
	page_data->current_vte = &(win_data->current_vte);
	// page_data->update_window_title_only = &(win_data->update_window_title_only);
	page_data->custom_window_title = (win_data->custom_window_title != NULL);
}

void init_user_keys(struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch init_user_keys() with win_data = %p", win_data);
#endif

	// for disable/enable the function keys
	win_data->user_keys[0].value = g_strdup("Ctrl grave");
	// New Page
	win_data->user_keys[1].value = g_strdup("Ctrl T");
	// Close Page
	win_data->user_keys[2].value = g_strdup("Ctrl Q");
	// Edit Page Name
	win_data->user_keys[3].value = g_strdup("Ctrl E");
	// Pre Page
	win_data->user_keys[4].value = g_strdup("Ctrl Page_Up");
	// Next Page
	win_data->user_keys[5].value = g_strdup("Ctrl Page_Down");
	// First Page
	win_data->user_keys[6].value = g_strdup("Ctrl Home");
	// Last Page
	win_data->user_keys[7].value = g_strdup("Ctrl End");
	// Move the label forward
	win_data->user_keys[8].value = g_strdup("Ctrl Left");
	// Move the label backward
	win_data->user_keys[9].value = g_strdup("Ctrl Right");
	// Move the label to first
	win_data->user_keys[10].value = g_strdup("Ctrl Up");
	// Move the label to last
	win_data->user_keys[11].value = g_strdup("Ctrl Down");
	// switch to #%d page
	win_data->user_keys[12].value = g_strdup("Ctrl F1");
	win_data->user_keys[13].value = g_strdup("Ctrl F2");
	win_data->user_keys[14].value = g_strdup("Ctrl F3");
	win_data->user_keys[15].value = g_strdup("Ctrl F4");
	win_data->user_keys[16].value = g_strdup("Ctrl F5");
	win_data->user_keys[17].value = g_strdup("Ctrl F6");
	win_data->user_keys[18].value = g_strdup("Ctrl F7");
	win_data->user_keys[19].value = g_strdup("Ctrl F8");
	win_data->user_keys[20].value = g_strdup("Ctrl F9");
	win_data->user_keys[21].value = g_strdup("Ctrl F10");
	win_data->user_keys[22].value = g_strdup("Ctrl F11");
	win_data->user_keys[23].value = g_strdup("Ctrl F12");
	// select all
	win_data->user_keys[24].value = g_strdup("Ctrl O");
	// copy the text to clipboard
	win_data->user_keys[25].value = g_strdup("Ctrl X");
	// paste the text in clipboard
	win_data->user_keys[26].value = g_strdup("Ctrl V");
	// Increase the font size
	win_data->user_keys[27].value = g_strdup("Ctrl equal");
	// decrease the font size
	win_data->user_keys[28].value = g_strdup("Ctrl minus");
	// reset the font size
	win_data->user_keys[29].value = g_strdup("Ctrl Return");
	// max window
	win_data->user_keys[30].value = g_strdup("Alt F11");
	// full screen
	win_data->user_keys[31].value = g_strdup("Alt Return");
#ifdef DEBUG
	// dump_data
	win_data->user_keys[32].value = g_strdup("Ctrl Print");
#endif
}

void init_function_keys()
{
#ifdef DETAIL
	g_debug("! Launch init_function_keys()!");
#endif

	// for disable/enable the function keys
	system_keys[0].name = "disable_function_key";
	system_keys[0].comment = "# Disable/Enable function keys and hyperlinks for temporary.";
	// New Page
	system_keys[1].name = "new_tab_key";
	system_keys[1].comment = "# Add a new tab.";
	// Close Page
	system_keys[2].name = "close_tab_key";
	system_keys[2].comment = "# Close current tab.";
	// Edit Page Name
	system_keys[3].name = "edit_label_key";
	system_keys[3].comment = "# Rename the page name of current tab.";
	// Pre Page
	system_keys[4].name = "prev_tab_key";
	system_keys[4].comment = "# Switch to prev tab.";
	// Next Page
	system_keys[5].name = "next_tab_key";
	system_keys[5].comment = "# Switch to next tab.";
	// First Page
	system_keys[6].name = "first_tab_key";
	system_keys[6].comment = "# Switch to first tab.";
	// Last Page
	system_keys[7].name = "last_tab_key";
	system_keys[7].comment = "# Switch to last tab.";
	// Move the label forward
	system_keys[8].name = "move_tab_forward";
	system_keys[8].comment = "# Move current page forward.";
	// Move the label backward
	system_keys[9].name = "move_tab_backward";
	system_keys[9].comment = "# Move current page backward.";
	// Move the label to first
	system_keys[10].name = "move_tab_first";
	system_keys[10].comment = "# Move current page to first.";
	// Move the label to last
	system_keys[11].name = "move_tab_last";
	system_keys[11].comment = "# Move current page to last.";
	// switch to #%d page
	system_keys[12].name = "switch_to_tab_1";
	system_keys[12].comment = "# Switch to 1st tab directly.";
	system_keys[13].name = "switch_to_tab_2";
	system_keys[13].comment = "# Switch to 2nd tab directly.";
	system_keys[14].name = "switch_to_tab_3";
	system_keys[14].comment = "# Switch to 3rd tab directly.";
	system_keys[15].name = "switch_to_tab_4";
	system_keys[15].comment = "# Switch to 4th tab directly.";
	system_keys[16].name = "switch_to_tab_5";
	system_keys[16].comment = "# Switch to 5th tab directly.";
	system_keys[17].name = "switch_to_tab_6";
	system_keys[17].comment = "# Switch to 6th tab directly.";
	system_keys[18].name = "switch_to_tab_7";
	system_keys[18].comment = "# Switch to 7th tab directly.";
	system_keys[19].name = "switch_to_tab_8";
	system_keys[19].comment = "# Switch to 8th tab directly.";
	system_keys[20].name = "switch_to_tab_9";
	system_keys[20].comment = "# Switch to 9th tab directly.";
	system_keys[21].name = "switch_to_tab_10";
	system_keys[21].comment = "# Switch to 10th tab directly.";
	system_keys[22].name = "switch_to_tab_11";
	system_keys[22].comment = "# Switch to 11th tab directly.";
	system_keys[23].name = "switch_to_tab_12";
	system_keys[23].comment = "# Switch to 12th tab directly.";
	// select all
	system_keys[24].name = "select_all";
	system_keys[24].comment = "# Select all the text in the Vte Terminal box.";
	// copy the text to clipboard
	system_keys[25].name = "copy_clipboard";
	system_keys[25].comment = "# Copy the text to clipboard.";
	// paste the text in clipboard
	system_keys[26].name = "paste_clipboard";
	system_keys[26].comment = "# Paste the text in clipboard.";
	// Increase the font size
	system_keys[27].name = "increase_font_size";
	system_keys[27].comment = "# Increase the font size of current tab.";
	// decrease the font size
	system_keys[28].name = "decrease_font_size";
	system_keys[28].comment = "# Decrease the font size of current tab.";
	// reset the font size
	system_keys[29].name = "reset_font_size";
	system_keys[29].comment = "# Reset the font of current tab to original size.";
	// max window
	system_keys[30].name = "max_window";
	system_keys[30].comment = "# Try to max the window to use all avaible space on your display.";
	// full screen
	system_keys[31].name = "full_screen";
	system_keys[31].comment = "# Asks to place window in the fullscreen/unfullscreen state.";
#ifdef DEBUG
	system_keys[32].name = "dump_data";
	system_keys[32].comment = "# dump the runtime data of lilyterm for debug.";
#endif
}

void init_page_color_data()
{
	page_color[0].name = "page_win_title_color";
	page_color[0].comment = _("Window Title");
	page_color[0].comment_eng = "Window Title";
	page_color[1].name = "page_cmdline_color";
	page_color[1].comment = _("Running Command");
	page_color[1].comment_eng = "Running Command";
	page_color[2].name = "page_dir_color";
	page_color[2].comment = _("Current Dir");
	page_color[2].comment_eng = "Current Dir";
	page_color[3].name = "page_custom_color";
	page_color[3].comment = _("Custom Tab Name");
	page_color[3].comment_eng = "Custom Tab Name";
	page_color[4].name = "page_root_color";
	page_color[4].comment = _("Root Privileges");
	page_color[4].comment_eng = "Root Privileges";
	page_color[5].name = "page_normal_color";
	page_color[5].comment = _("Normal Text");
	page_color[5].comment_eng = "Normal Text";
}

void init_mod_keys()
{
#ifdef DETAIL
	g_debug("! Launch init_mod_keys()!");
#endif
	modkeys[0].name = "Shift";
	modkeys[0].mod = GDK_SHIFT_MASK;
	// modkeys[1].name = "NumLock";
	// modkeys[1].mod = GDK_LOCK_MASK ;
	modkeys[1].name = "Ctrl";
	modkeys[1].mod = GDK_CONTROL_MASK;
	modkeys[2].name = "Alt";
	modkeys[2].mod = GDK_MOD1_MASK ;
	// Mod2 = Alt
	// modkeys[2].name = "Mod1";
	// modkeys[2].mod = GDK_MOD1_MASK;
	// Mod2 = NumLock. We won't check it.
	// modkeys[3].name = "Mod2";
	// modkeys[3].mod = GDK_MOD2_MASK ;
	modkeys[3].name = "Mod3";
	modkeys[3].mod = GDK_MOD3_MASK;
	modkeys[4].name = "Mod4";
	modkeys[4].mod = GDK_MOD4_MASK ;
	modkeys[5].name = "Mod5";
	modkeys[5].mod = GDK_MOD5_MASK ;
	// modkeys[6].name = "Button1";
	// modkeys[6].mod = GDK_BUTTON1_MASK;
	// modkeys[7].name = "Button2";
	// modkeys[7].mod = GDK_BUTTON2_MASK;
	// modkeys[8].name = "Button3";
	// modkeys[8].mod = GDK_BUTTON3_MASK;
	// modkeys[9].name = "Button4";
	// modkeys[9].mod = GDK_BUTTON4_MASK;
	// modkeys[10].name = "Button5";
	// modkeys[10].mod = GDK_BUTTON5_MASK;
	// modkeys[11].name = "Super";
	// modkeys[11].mod = GDK_SUPER_MASK;
	// modkeys[12].name = "Hyper";
	// modkeys[12].mod = GDK_HYPER_MASK;
	// modkeys[13].name = "Meta";
	// modkeys[13].mod = GDK_META_MASK ;
}

void init_colors()
{
	color[0].comment = "# The ANSI color code for Normal Black";
	color[1].comment = "# The ANSI color code for Normal Red";
	color[2].comment = "# The ANSI color code for Normal Green";
	color[3].comment = "# The ANSI color code for Normal Yellow";
	color[4].comment = "# The ANSI color code for Normal Blue";
	color[5].comment = "# The ANSI color code for Normal Magenta";
	color[6].comment = "# The ANSI color code for Normal Cyan";
	color[7].comment = "# The ANSI color code for Normal White";
	color[8].comment = "# The ANSI color code for Bright Black";
	color[9].comment = "# The ANSI color code for Bright Red";
	color[10].comment = "# The ANSI color code for Bright Green";
	color[11].comment = "# The ANSI color code for Bright Yellow";
	color[12].comment = "# The ANSI color code for Bright Blue";
	color[13].comment = "# The ANSI color code for Bright Magenta";
	color[14].comment = "# The ANSI color code for Bright Cyan";
	color[15].comment = "# The ANSI color code for Bright White";

	gint i;
	for (i=0; i<16; i++)
		color[i].name = g_strdup_printf("Color%d", i);
}

void init_user_color_data(struct Window *win_data)
{
	// FIXME: should we use win_data->color_theme = NULL here?
	win_data->color_theme = g_strdup("vte_fixed");
	int i;
	for (i=0; i<COLOR; i++)
		win_data->color_value[i] = g_strdup("");
}

void init_user_color(struct Window *win_data)
{
	const GdkColor *palette;

	win_data->using_custom_color = TRUE;
	// g_debug("Got win_data->color_theme = %s", win_data->color_theme);

	if (!strcmp(win_data->color_theme, "tango"))
		palette = color_theme_xterm;
	else if (!strcmp(win_data->color_theme, "linux"))
		palette = color_theme_linux;
	else if (!strcmp(win_data->color_theme, "xterm"))
		palette = color_theme_xterm;
	else if (!strcmp(win_data->color_theme, "rxvt"))
		palette = color_theme_rxvt;
	else if (!strcmp(win_data->color_theme, "vte_fixed"))
		palette = color_theme_vte_fixed;
	else
	{
		win_data->using_custom_color = FALSE;
		g_free(win_data->color_theme);
		win_data->color_theme = g_strdup("");
		palette = color_theme_vte_fixed;
	}
	
	gint i;
	for (i=0; i<COLOR; i++)
	{
		win_data->color_orig[i] = *(palette++);
		//g_debug("win_data->color_orig[%d] = %x, %x, %x, %x",
		//	i,
		//	win_data->color_orig[i].pixel,
		//	win_data->color_orig[i].red,
		//	win_data->color_orig[i].green,
		//	win_data->color_orig[i].blue);
	}
}

void adjust_ansi_color(GdkColor color[COLOR], GdkColor color_orig[COLOR], gdouble color_brightness)
{
#ifdef DETAIL
	g_debug("! Launch adjust_ansi_color() with color_brightness = %f",  color_brightness);
#endif
	// g_debug("Get win_data = %d when set background saturation!", win_data);
	if (color_brightness > 1) color_brightness = 1;
	if (color_brightness < -1) color_brightness = -1;

	gint i;
	for (i=0; i<COLOR; i++)
	{
		adjust_ansi_color_severally(&color[i], &color_orig[i], color_brightness);
		//g_debug("color_orig[%d] = %x, %x, %x",
		//	i,
		//	color_orig[i].red,
		//	color_orig[i].green,
		//	color_orig[i].blue);

		//g_debug("color[%d] = %x, %x, %x",
		//	i,
		//	color[i].red,
		//	color[i].green,
		//	color[i].blue);
	}
}

void adjust_ansi_color_severally(GdkColor *color, GdkColor *color_orig, gdouble color_brightness)
{
	if (color_brightness>=0)
	{
		(*color).red = (0xffff - (*color_orig).red) * color_brightness + (*color_orig).red;
		(*color).green = (0xffff - (*color_orig).green) * color_brightness + (*color_orig).green;
		(*color).blue = (0xffff - (*color_orig).blue) * color_brightness + (*color_orig).blue;
	}
	else
	{
		(*color).red = (1 + color_brightness) * (*color_orig).red;
		(*color).green = (1 + color_brightness) * (*color_orig).green;
		(*color).blue = (1 + color_brightness) * (*color_orig).blue;
	}
}

// get user settings from profile.
void get_user_settings(GtkWidget *window, struct Window *win_data, gchar *encoding)
{
#ifdef DETAIL
	g_debug("! Launch get_user_settings() with window = %p, and win_data = %p", window, win_data);
#endif

	// g_debug("Get win_data = %d when get user settings!", win_data);

	init_window_parameters(win_data);
	init_user_keys(win_data);
	init_user_command(win_data);
	init_user_color_data(win_data);
	if (total_window==1)
	{
		init_function_keys();
		init_page_color_data();
		init_mod_keys();
		init_command();
		init_colors();
	}

	gint i;
	GError *error = NULL;

	// got the rc file
	GKeyFile *keyfile = g_key_file_new();
	// gchar *profile = g_strdup_printf("%s/%s", g_get_user_config_dir(), RCFILE);
	// g_debug ("Using the profile: %s \n", profile);

	if (win_data->profile != NULL)
	{
		if (g_key_file_load_from_file(keyfile, win_data->profile, G_KEY_FILE_NONE, &error))
		{
			// Check the profile version
			gchar *profile_version = check_string_value(keyfile, "main", "version", g_strdup(""), FALSE);
			// g_debug("RCFILE_FORMAT_VERSION = %s, and profile_version = %s",
			//	RCFILE_FORMAT_VERSION,
			//	profile_version);

			if ((*profile_version != 0) &&  (strcmp(RCFILE_FORMAT_VERSION, profile_version)))
			{
				GtkWidget *menu_active_window_orig = menu_active_window;
				menu_active_window = win_data->window;
				// g_debug("set menu_active_window = %p", menu_active_window);
				dialog(NULL, 20);
				menu_active_window = menu_active_window_orig;
				// g_debug("set menu_active_window = %p", menu_active_window);
			}
#ifdef DETAIL
			g_debug("* free profile_version %p (%s) in get_user_settings()", profile_version, profile_version);
#endif
			g_free(profile_version);

			win_data->default_font_name = check_string_value(keyfile, "main", "font_name",
									 win_data->default_font_name, FALSE);

			PangoFontDescription *font_desc = pango_font_description_from_string(win_data->default_font_name);
			if ((pango_font_description_get_size(font_desc)/PANGO_SCALE)==0)
			{
				g_warning("Invalid font name: \"%s\"", win_data->default_font_name);
#ifdef DETAIL
				g_debug("* free win_data->default_font_name %p (%s) in get_user_settings()",
					win_data->default_font_name, win_data->default_font_name);
#endif
				g_free(win_data->default_font_name);
				win_data->default_font_name = g_strdup(win_data->system_font_name);
			}

			win_data->default_column = check_integer_value( keyfile, "main", "column", win_data->default_column,
									FALSE, FALSE, TRUE, 1, FALSE, 0);

			win_data->default_row = check_integer_value(keyfile, "main", "row", win_data->default_row,
								    FALSE, FALSE, TRUE, 1, FALSE, 0);


#ifdef ENABLE_RGBA
			win_data->use_rgba = check_integer_value(keyfile, "main", "use_rgba", win_data->use_rgba,
								 FALSE, TRUE, TRUE, 0, TRUE, 2);
			win_data->use_rgba_orig = win_data->use_rgba;

			win_data->transparent_window = check_integer_value(keyfile, "main", "transparent_window",
							 win_data->transparent_window, FALSE, TRUE, TRUE, 0, TRUE, 2);
			win_data->window_opacity = check_double_value ( keyfile, "main", "window_opacity",
									win_data->window_opacity);
#endif

			win_data->transparent_background = check_integer_value(keyfile, "main", "transparent_background",
							win_data->transparent_background, FALSE, TRUE, TRUE, 0, TRUE, 2);

			win_data->background_saturation = check_double_value(keyfile, "main", "background_saturation",
									     win_data->background_saturation);

			win_data->confirm_to_close_multi_tabs = check_boolean_value(keyfile, "main",
										    "confirm_to_close_multi_tabs",
										    win_data->confirm_to_close_multi_tabs);
			win_data->show_transparent_menu = check_boolean_value(keyfile, "main", "show_transparent_menu",
								    win_data->show_transparent_menu);

			win_data->show_color_selection_menu = check_boolean_value(keyfile, "main",
						"show_color_selection_menu", win_data->show_color_selection_menu);

			win_data->foreground_color = check_string_value(keyfile, "main", "foreground_color",
									win_data->foreground_color, FALSE);

			win_data->background_color = check_string_value(keyfile, "main", "background_color",
									win_data->background_color, FALSE);

			win_data->show_resize_menu = check_boolean_value(keyfile, "main", "show_resize_menu",
									 win_data->show_resize_menu);


			win_data->font_anti_alias = check_integer_value( keyfile, "main", "font_anti_alias",
					 win_data->font_anti_alias, FALSE, TRUE, TRUE, 0, TRUE, 2);

			win_data->font_resize_ratio = check_double_value(keyfile, "main", "font_resize_ratio",
									 win_data->font_resize_ratio);

			win_data->window_resize_ratio = check_double_value(keyfile, "main", "window_resize_ratio",
									   win_data->window_resize_ratio);

			win_data->word_chars = check_string_value(keyfile, "main", "word_chars", win_data->word_chars,
								  TRUE);

			win_data->scrollback_lines = check_integer_value(keyfile,
									  "main",
									  "scrollback_lines",
									  win_data->scrollback_lines,
									  FALSE,
									  TRUE,
									  TRUE,
									  0,
									  FALSE,
									  0);
			win_data->show_scrollbar = check_boolean_value(keyfile, "main",
							 "show_scrollbar", win_data->show_scrollbar);

			win_data->scrollbar_position = check_boolean_value(keyfile, "main",
							 "scrollbar_position", win_data->scrollbar_position);

			win_data->show_input_method_menu = check_boolean_value(keyfile, "main", "show_input_method_menu",
								     win_data->show_input_method_menu);

			win_data->show_get_function_key_menu = check_boolean_value(keyfile, "main",
					"show_get_function_key_menu", win_data->show_get_function_key_menu);

			win_data->show_change_page_name_menu = check_boolean_value(keyfile, "main",
					"show_change_page_name_menu", win_data->show_change_page_name_menu);

			win_data->enable_hyperlink = check_boolean_value(keyfile, "main",
							"enable_hyperlink", win_data->enable_hyperlink);

			win_data->cursor_blinks = check_boolean_value(keyfile, "main", "cursor_blinks",
									  win_data->cursor_blinks);

			win_data->show_copy_paste_menu =
				check_boolean_value(keyfile,
						    "main",
						    "show_copy_paste_menu",
						    win_data->show_copy_paste_menu);

			win_data->audible_bell = check_boolean_value(keyfile,
								     "main",
								     "audible_bell",
								     win_data->audible_bell);

			win_data->visible_bell = check_boolean_value(keyfile,
								     "main",
								     "visible_bell",
								      win_data->visible_bell);

			// g_debug("VTE_ERASE_AUTO = %d, VTE_ERASE_ASCII_BACKSPACE = %d, "
			//      "VTE_ERASE_ASCII_DELETE = %d, VTE_ERASE_DELETE_SEQUENCE %d",
			//      VTE_ERASE_AUTO, VTE_ERASE_ASCII_BACKSPACE,
			//      VTE_ERASE_ASCII_DELETE, VTE_ERASE_DELETE_SEQUENCE);
			win_data->erase_binding = check_integer_value(keyfile,
								      "main",
								      "erase_binding",
								      win_data->erase_binding,
								      FALSE,
								      TRUE,
								      TRUE,
								      0,
								      TRUE,
								      3);

			win_data->locales_list = check_string_value( keyfile, "main", "locales_list",
								     win_data->locales_list, TRUE);
			// g_debug("Got locales_list = %s from user's profile!\n", value);

			win_data->emulate_term = check_string_value( keyfile, "main", "emulate_term",
							     win_data->emulate_term, FALSE);

			win_data->VTE_CJK_WIDTH = check_integer_value( keyfile, "main", "VTE_CJK_WIDTH",
							win_data->VTE_CJK_WIDTH, FALSE, TRUE, TRUE, 0, TRUE, 2);

			win_data->page_width = check_integer_value( keyfile, "page", "page_width",
								    win_data->page_width, FALSE, FALSE, TRUE, 1, FALSE, 0);

			win_data->fixed_page_width = check_boolean_value(keyfile, "page", "fixed_page_width",
									 win_data->fixed_page_width);

			win_data->tabbar_position = check_boolean_value(keyfile, "page", "tabbar_position", 
								     win_data->tabbar_position);

			win_data->fill_tab_bar = check_boolean_value(keyfile, "page", "fill_tab_bar",
								     win_data->fill_tab_bar);


			win_data->page_name = check_string_value(keyfile, "page", "page_name", win_data->page_name, FALSE);

			win_data->page_names = check_string_value(keyfile, "page", "page_names",
								  win_data->page_names, TRUE);
			win_data->splited_page_names = g_strsplit_set(win_data->page_names, " ", 0);
			while (win_data->splited_page_names[win_data->max_page_names_no]!=NULL)
				win_data->max_page_names_no++;

			win_data->reuse_page_names = check_boolean_value(keyfile, "page", "reuse_page_names",
									 win_data->reuse_page_names);

			win_data->page_shows_number = check_boolean_value(keyfile, "page", "page_shows_number",
									  win_data->page_shows_number);

			win_data->page_shows_current_cmdline = check_boolean_value(keyfile, "page",
				 "page_shows_current_cmdline", win_data->page_shows_current_cmdline);

			win_data->page_shows_window_title = check_boolean_value(keyfile, "page", "page_shows_window_title",
									 win_data->page_shows_window_title);

			win_data->page_shows_current_dir = check_boolean_value(keyfile, "page", "page_shows_current_dir",
									 win_data->page_shows_current_dir);

			win_data->check_root_privileges = check_boolean_value(keyfile, "page", "check_root_privileges",
								    win_data->check_root_privileges);

			win_data->page_shows_encoding = check_boolean_value(keyfile, "page",
						"win_data->page_shows_encoding", win_data->page_shows_encoding);

			win_data->bold_current_page_name = check_boolean_value(keyfile, "page", "bold_current_page_name",
								     win_data->bold_current_page_name);

			win_data->bold_action_page_name = check_boolean_value(keyfile, "page", "bold_action_page_name",
								    win_data->bold_action_page_name);

			win_data->window_shows_current_page = check_boolean_value(keyfile, "page",
						"window_shows_current_page", win_data->window_shows_current_page);

			win_data->use_color_page = check_boolean_value( keyfile, "page", "use_color_page",
									win_data->use_color_page);

			for (i=0; i<PAGE_COLOR; i++)
				win_data->user_page_color[i] = check_string_value(keyfile,
										  "page",
										  page_color[i].name,
										  win_data->user_page_color[i],
										  FALSE);

			// g_debug("Key Value: Shift=%x, NumLock=%x, Control=%x, Mod1=%x,"
			//		"Mod2=%x, Mod3=%x, Mod4=%x, Mod5=%x\n",
			//		GDK_SHIFT_MASK, GDK_LOCK_MASK, GDK_CONTROL_MASK, GDK_MOD1_MASK, 
			//		GDK_MOD2_MASK, GDK_MOD3_MASK, GDK_MOD4_MASK, GDK_MOD5_MASK);
			gchar *value = NULL;
			for (i=0; i<KEYS; i++)
			{
				// g_debug("Checking %s key...", pagekeys[i].name);
				value = g_key_file_get_value(keyfile, "key", system_keys[i].name, NULL);
				if (value)
				{
					// g_debug("Got %s key = %s form profile.\n", pagekeys[i].name, value);
					if ( accelerator_parse(system_keys[i].name, value,
							  &(win_data->user_keys[i].key), &(win_data->user_keys[i].mods)))
					{
						g_free(win_data->user_keys[i].value);
						win_data->user_keys[i].value = value;
					}
					else
					{
						g_critical("\"%s = %s\" is not a valid key! Please check!",
							system_keys[i].name, value);
#ifdef DETAIL
						g_debug("* free value %p (%s) in get_user_settings()", value, value);
#endif
						g_free(value);
					}
					// if (pagekeys[i].key)
					//	g_debug("Got %s = key %x(%s), mods = %x.\n", pagekeys[i].name,
					//			pagekeys[i].key, gdk_keyval_name(pagekeys[i].key),
					//			pagekeys[i].mods);
				}
				// else
				//	g_debug("We can not find %s key in profile...", pagekeys[i].name);
			}

			for (i=0; i<COMMAND; i++)
			{
				win_data->user_command[i].command = check_string_value(keyfile,	"command",
						command[i].name, win_data->user_command[i].command, FALSE);
				win_data->user_command[i].method = check_integer_value(
						keyfile, "command", command[i].method_name,
						win_data->user_command[i].method, FALSE, TRUE, TRUE, 0, TRUE, 2);
				win_data->user_command[i].environ = check_string_value(keyfile, "command",
						command[i].environ_name, win_data->user_command[i].environ, FALSE);
				win_data->user_command[i].VTE_CJK_WIDTH = check_integer_value(
						keyfile, "command", command[i].VTE_CJK_WIDTH_name,
						win_data->user_command[i].VTE_CJK_WIDTH, FALSE, TRUE, TRUE, 0, TRUE, 2);
				//g_debug("command[i].name = %s (%d)",
				//	win_data->user_command[i].command, win_data->user_command[i].method);
			}

			win_data->color_theme = check_string_value(keyfile, "color", "theme",
								   win_data->color_theme, TRUE);
			init_user_color(win_data);

			for (i=0; i<COLOR; i++)
			{
				win_data->color_value[i] = check_string_value(keyfile, "color",
									      color[i].name,
									      win_data->color_value[i],
									      FALSE);
				// g_debug("win_data->color_value[%d] = %s", i, win_data->color_value[i]);
				if (strcmp(win_data->color_value[i], ""))
				{
					win_data->using_custom_color = TRUE;
					check_color_value(color[i].name, win_data->color_value[i],
							  &(win_data->color_orig[i]));
					//g_debug("win_data->color_value[%d] = %s, "
					//	"win_data->color[%d] = %x, %x, %x, %x",
					//	i, win_data->color_value[i], i, 
					//	win_data->color[i].pixel, win_data->color[i].red,
					//	win_data->color[i].green, win_data->color[i].blue );
				}
			}

			win_data->color_brightness = check_double_value(keyfile,
									"color",
									"brightness",
									win_data->color_brightness);
			if (win_data->color_brightness) win_data->using_custom_color = TRUE;

			win_data->color_brightness_inactive = check_double_value(keyfile,
										 "color",
										 "inactive_brightness",
										 win_data->color_brightness);
			if (win_data->color_brightness != win_data->color_brightness_inactive)
				win_data->using_custom_color = TRUE;
			else
				win_data->dim_text = FALSE;
			// g_debug("win_data->dim_text = %d", win_data->dim_text);
		}
		else
		{
			gchar *err_msg;
			gchar *local_profile = convert_str_to_utf8(win_data->profile, encoding);
			if (local_profile)
				err_msg = g_strdup_printf(_("The profile \"%s\" is invalid: %s"),
							    local_profile, error->message);
			else
				err_msg = g_strdup_printf(_("The profile \"%s\" is invalid: %s"),
							    win_data->profile, error->message);
			// g_warning("the profile \"%s\" is invalid: %s", win_data->profile, error->message);
			error_dialog(win_data, err_msg, NULL, 18);
			g_clear_error (&error);
			g_free(local_profile);
			g_free(err_msg);
			init_user_color(win_data);
		}
	}
	else
	{
		g_warning("can NOT find any profile. Use program defaults.");
		init_user_color(win_data);
	}
	
 	for (i=0; i<KEYS; i++)
 	{
		if ( ! win_data->user_keys[i].key)
		{
 			if ( ! accelerator_parse(system_keys[i].name, win_data->user_keys[i].value,
					      &(win_data->user_keys[i].key), &(win_data->user_keys[i].mods)))
				g_critical("%s = %s is not a valid key! Please report bug!\n",
					 system_keys[i].name, win_data->user_keys[i].value);
 			// g_debug("Use default key %s, %x(%s), mods = %x.\n", pagekeys[i].name,
			//		pagekeys[i].key, gdk_keyval_name(pagekeys[i].key), pagekeys[i].mods);
		}
		// g_debug("* We'll use the key for %s: %x(%s), mods = %x.\n", pagekeys[i].name,
		//		pagekeys[i].key, gdk_keyval_name(pagekeys[i].key), pagekeys[i].mods);
 	}

	for (i=0; i<COMMAND; i++)
	{
		// g_debug("Got the environ = %s", win_data->user_command[i].environ);
		win_data->user_command[i].environments = g_strsplit(win_data->user_command[i].environ, " ", -1);
	}

	// some defaults
	if (win_data->splited_page_names==NULL)
		win_data->splited_page_names = g_strsplit_set("", " ", 0);

	check_color_value ("foreground_color", win_data->foreground_color, &(win_data->fg_color));
	adjust_ansi_color_severally(&(win_data->fg_color_inactive),
				    &(win_data->fg_color),
				    win_data->color_brightness_inactive);
	check_color_value ("background_color", win_data->background_color, &(win_data->bg_color));
	adjust_ansi_color(win_data->color, win_data->color_orig, win_data->color_brightness);
	adjust_ansi_color(win_data->color_inactive, win_data->color_orig, win_data->color_brightness_inactive);

	if (win_data->VTE_CJK_WIDTH_STR == NULL || win_data->VTE_CJK_WIDTH_STR[0] == '\0')
	{
		g_free(win_data->VTE_CJK_WIDTH_STR);
		win_data->VTE_CJK_WIDTH_STR = g_strdup(get_VTE_CJK_WIDTH_str(win_data->VTE_CJK_WIDTH));
	}

#ifdef ENABLE_RGBA
	// If the system supports rgba, enable transparent background by default
	if (win_data->use_rgba)
		init_rgba(window, win_data);
	if (win_data->transparent_window==2)
		win_data->transparent_window = win_data->use_rgba;
	if (win_data->transparent_background==2)
		win_data->transparent_background = win_data->use_rgba;

	set_window_opacity (NULL, 0, win_data->window_opacity, window);
#endif

	// get the default locale from environment
	// win_data->default_locale = get_default_locale();
	win_data->default_locale = g_strdup((char*)g_getenv("LC_ALL"));
	
//	// g_debug("win_data->default_VTE_CJK_WIDTH = %d in get_user_settings()", win_data->default_VTE_CJK_WIDTH);
//	if (win_data->default_VTE_CJK_WIDTH)
//		win_data->system_VTE_CJK_WIDTH = win_data->default_VTE_CJK_WIDTH;
//	else
//		win_data->system_VTE_CJK_WIDTH = get_default_VTE_CJK_WIDTH();

	// g_debug("Got locales_list = '%s' ( %d bytes)",locales_list, strlen(locales_list));
	if (strlen(win_data->locales_list))
		win_data->supported_locales = get_supported_locales(win_data->default_locale,
								    win_data->locales_list,
								    get_default_LC_TYPE(),
								    NULL);

	g_key_file_free(keyfile);
	// the win_data->profile will be free when close window
	// g_free(win_data->profile);
}

gboolean check_boolean_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key, const gboolean default_value)
{
	gchar *value = g_key_file_get_value(keyfile, group_name, key, NULL);
	gboolean setting;

	if (value)
	{
		if (strlen(value))
			setting = g_key_file_get_boolean(keyfile, group_name, key, NULL);
		else
			setting = default_value;
#ifdef DETAIL
		g_debug("* free value %p (%s) in check_boolean_value()", value, value);
#endif
		g_free(value);
	}
	else
		setting = default_value;
	
	// g_debug("Got key value \"%s = %d\"", key, setting);
	return setting;
}

gdouble check_double_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key, const gdouble default_value)
{
	gchar *value = g_key_file_get_value(keyfile, group_name, key, NULL);
	gdouble setting;

	if (value)
	{
		if (strlen(value))
			setting = g_key_file_get_double(keyfile, group_name, key, NULL);
		else
			setting = default_value;
#ifdef DETAIL
		g_debug("* free value %p (%s) in check_double_value()", value, value);
#endif
		g_free(value);
	}
	else
		setting = default_value;

	// g_debug("Got key value \"%s = %d\"", key, setting);
	return setting;
}

// enable_empty -> True: 0, False: default
gint check_integer_value(GKeyFile *keyfile, const gchar *group_name, const gchar *key,
			 const gint default_value, gboolean enable_empty, gboolean enable_zero,
			 gboolean check_min, gint min, gboolean check_max, gint max)
{
	gchar *value = g_key_file_get_value(keyfile, group_name, key, NULL);
	gint setting;

	if (value)
	{
		if (strlen(value))
			setting = g_key_file_get_integer(keyfile, group_name, key, NULL);
		else
		{
			if (enable_empty)
				setting = 0;
			else
				setting = default_value;
		}
#ifdef DETAIL
		g_debug("* free value %p (%s) in check_integer_value()", value, value);
#endif
		g_free(value);
	}
	else
		setting = default_value;
	
	if (setting==0 && (! enable_zero))
		setting = default_value;

	if (check_min)
		if (setting < min)
			setting = default_value;
	
	if (check_max)
		if (setting > max)
			setting = default_value;

	// g_debug("Got key value \"%s = %d\"", key, setting);
	return setting;
}

// The returned string should be freed when no longer needed.
gchar *check_string_value(GKeyFile *keyfile, const gchar *group_name,
			const gchar *key, gchar *original_value, gboolean enable_empty)
{
	gchar *setting = g_key_file_get_value(keyfile, group_name, key, NULL);
	if (setting)
		if (! strlen(setting))
			if (! enable_empty)
			{
#ifdef DETAIL
				g_debug("* free setting %p (%s) in check_string_value()", setting, setting);
#endif
				g_free(setting);
				setting = NULL;
			}
	
	if (!setting)
		setting = g_strdup(original_value);
	
#ifdef DETAIL
	g_debug("* free default_value %p (%s) in check_string_value()", default_value, default_value);
#endif
	g_free(original_value);
	
	// g_debug("Got key value \"%s = %s\"", key, setting);
	return setting;
}

void check_color_value (const gchar *key_name, const gchar *color_name, GdkColor *color)
{
	if( ! gdk_color_parse(color_name, color))
	{
		g_warning("\"%s = %s\" is not a valid color value! Please check!",
			  key_name, color_name);
	}
}

// to init a new page
void init_new_page(struct Window *win_data,
		   struct Page *page_data,
		   gint column,
		   gint row,
		   gint run_once)
{
#ifdef DETAIL
	g_debug("! Launch init_new_page() with win_data = %p, page_data = %p, "
		" column = %d, row = %d, run_once = %d",
		win_data, page_data, column, row, run_once);
#endif

	// g_debug("Get win_data = %d when init new page!", win_data);

	// set font
	// g_debug("Set Font AA = %d", win_data->font_anti_alias);
	vte_terminal_set_font_from_string_full (VTE_TERMINAL(page_data->vte),
						page_data->font_name,
						win_data->font_anti_alias);
	//g_debug("Got font size from %s: %d\n", font_name, pango_font_description_get_size (
	//	  pango_font_description_from_string(font_name))/PANGO_SCALE);

	// set terminal size
	// g_debug("Set the vte size to: %dx%d", column, row);
	vte_terminal_set_size(VTE_TERMINAL(page_data->vte), column, row);

	set_vte_color(win_data, page_data);

	// set transparent
	set_background_saturation(NULL, 0, win_data->background_saturation, page_data->vte);

	// other settings
	vte_terminal_set_word_chars(VTE_TERMINAL(page_data->vte), win_data->word_chars);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(page_data->vte), win_data->scrollback_lines);

	// some fixed parameter
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(page_data->vte), FALSE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(page_data->vte), TRUE);
	// vte_terminal_set_backspace_binding (VTE_TERMINAL(page_data->vte), VTE_ERASE_ASCII_DELETE);
	// vte_terminal_set_delete_binding (VTE_TERMINAL(page_data->vte), VTE_ERASE_ASCII_DELETE);
	// vte_terminal_set_emulation (VTE_TERMINAL(page_data->vte), "xterm");

	set_hyprelink(win_data, page_data);

	vte_terminal_set_cursor_blinks (VTE_TERMINAL(page_data->vte), win_data->cursor_blinks);
	vte_terminal_set_audible_bell (VTE_TERMINAL(page_data->vte), win_data->audible_bell);
	vte_terminal_set_visible_bell (VTE_TERMINAL(page_data->vte), win_data->visible_bell);
	vte_terminal_set_backspace_binding (VTE_TERMINAL(page_data->vte), win_data->erase_binding);
	vte_terminal_set_emulation (VTE_TERMINAL(page_data->vte), win_data->emulate_term);
	
	if (run_once)
	{
		win_data->update_hints = 1;
		// update hints and make window unresizable
		window_resizable(win_data->window, page_data->vte, run_once, 1);
	}
}

void set_hyprelink(struct Window *win_data, struct Page *page_data)
{
	if (win_data->enable_hyperlink)
	{
		if (win_data->enable_function_key)
		{
			gint i;
			for (i=0; i<COMMAND; i++)
			{
				page_data->tag[i] = vte_terminal_match_add (VTE_TERMINAL(page_data->vte),
									    command[i].match);
				vte_terminal_match_set_cursor_type(VTE_TERMINAL(page_data->vte),
								   page_data->tag[i],
								   GDK_HAND2);
			}
		}
		else
			vte_terminal_match_clear_all(VTE_TERMINAL(page_data->vte));
	}
}

void set_vte_color(struct Window *win_data, struct Page *page_data)
{
	// set font/background colors
	vte_terminal_set_default_colors(VTE_TERMINAL(page_data->vte));
	if (win_data->using_custom_color)
	{
		// g_debug("win_data->using_custom_color = %d", win_data->using_custom_color);
		vte_terminal_set_colors(VTE_TERMINAL(page_data->vte),
					&(win_data->fg_color),
					&(win_data->bg_color),
					win_data->color, 16);
	}
	else
	{
		vte_terminal_set_color_foreground(VTE_TERMINAL(page_data->vte), &(win_data->fg_color));
		vte_terminal_set_color_background(VTE_TERMINAL(page_data->vte), &(win_data->bg_color));
	}
	vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color));
}

void set_page_width(struct Window *win_data, struct Page *page_data)
{
	if (win_data->fixed_page_width)
		gtk_label_set_width_chars(GTK_LABEL(page_data->label), win_data->page_width);
	else
		gtk_label_set_max_width_chars(GTK_LABEL(page_data->label), win_data->page_width);
}

void pack_vte_and_scrollbar_to_hbox(struct Window *win_data, struct Page *page_data)
{
	if (win_data->scrollbar_position)
	{
		gtk_box_pack_start(GTK_BOX(page_data->hbox), page_data->vte, TRUE, TRUE, 0);
		gtk_box_pack_start(GTK_BOX(page_data->hbox), page_data->scrollbar, FALSE, FALSE, 0);
	}
	else
	{
		gtk_box_pack_end(GTK_BOX(page_data->hbox), page_data->vte, TRUE, TRUE, 0);
		gtk_box_pack_end(GTK_BOX(page_data->hbox), page_data->scrollbar, FALSE, FALSE, 0);
	}
	if (! win_data->show_scrollbar)
		gtk_widget_set_no_show_all (page_data->scrollbar, TRUE);
}

void hide_scrollbar(struct Window *win_data, struct Page *page_data)
{
	if (win_data->show_scrollbar)
		gtk_widget_show (page_data->scrollbar);
	else
		gtk_widget_hide (page_data->scrollbar);
}

void add_remove_page_timeout_id(struct Window *win_data, struct Page *page_data)
{
	if (page_data->timeout_id)
	{
		g_source_remove (page_data->timeout_id);
		// get_and_update_page_name(page_data);
		// win_data->page_names_no++;
		//if ((page_data->custom_page_name == NULL) && (page_data->label != NULL))
		//	 page_data->custom_page_name = g_strdup(page_data->page_name);
		page_data->timeout_id = 0;
	}
	else
		// monitor_cmdline(page_data->monitor, page_data->pid);
		// monitor_cmdline(page_data->channel, page_data->pid);
#ifdef USE_TIMEOUT_SECONDS
		page_data->timeout_id = g_timeout_add_seconds (1,
							       (GSourceFunc)monitor_cmdline,
							       page_data);
#else
		page_data->timeout_id = g_timeout_add (1000,
						       (GSourceFunc)monitor_cmdline,
						       page_data);
#endif
}

void add_remove_window_title_changed_signal(struct Page *page_data)
{
	if (page_data->page_shows_window_title)
		page_data->window_title_signal = g_signal_connect(page_data->vte, "window-title-changed",
								  G_CALLBACK(update_page_window_title), page_data);
	else if (page_data->window_title_signal)
	{
		g_signal_handler_disconnect(page_data->vte, page_data->window_title_signal);
		page_data->window_title_signal = 0;
	}
		
}

gboolean set_background_saturation(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vte)
{
#ifdef DETAIL
	g_debug("! Launch set_background_saturation() with value = %f, vte = %p", value, vte);
#endif

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when set background saturation!", win_data);

	if (value > 1)
		value =1;
	if (value < 0)
		value =0;

#ifdef ENABLE_RGBA
	if (win_data->use_rgba)
	{
		if (win_data->transparent_background)
			vte_terminal_set_opacity(VTE_TERMINAL(vte), (1-value)*65535);
		else
			vte_terminal_set_opacity(VTE_TERMINAL(vte), 65535);
	}
	else
#endif
		vte_terminal_set_background_transparent(VTE_TERMINAL(vte), win_data->transparent_background);

	// g_debug("win_data->transparent_background = %d, value = %1.3f",
	//	win_data->transparent_background, value);
	if (win_data->transparent_background)
		vte_terminal_set_background_saturation( VTE_TERMINAL(vte), value);
	else
		vte_terminal_set_background_saturation( VTE_TERMINAL(vte), 0);

	return FALSE;
}

#ifdef ENABLE_RGBA
gboolean set_window_opacity(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *window)
{
#ifdef DETAIL
	g_debug("! Launch set_window_opacity() with value = %f, window = %p", value, window);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when set window opacity!", win_data);

	if (value > 1)
		value =1;
	if (value < 0)
		value =0;

	if (win_data->use_rgba)
	{
		if (win_data->transparent_window)
			gtk_window_set_opacity (GTK_WINDOW(window), 1-value);
		else
			gtk_window_set_opacity (GTK_WINDOW(window), 1);
	}
	return FALSE;
}
#endif

#ifdef ENABLE_RGBA
// init rgba to enable true transparent.
void init_rgba(GtkWidget *window, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch init_rgba() with window = %p, win_data = %p", window, win_data);
#endif

	// g_debug("Get win_data = %d when init rgba!", win_data);

	GdkScreen *screen;
	GdkColormap *colormap;

	screen = gtk_widget_get_screen(window);
	if (win_data->use_rgba==2)
		win_data->use_rgba = gdk_screen_is_composited(screen);
	
	//if (use_rgba)
	//	g_debug("Yes, This screen supports rgba!\n");
	//else
	//	g_debug("No, This screen don't supports rgba!\n");
	
	if (win_data->use_rgba)
	{
		colormap = gdk_screen_get_rgba_colormap(screen);
		if (colormap != NULL)
		{
			gtk_widget_set_colormap(window, colormap);
			gdk_screen_set_default_colormap(screen, colormap);
			gtk_widget_set_default_colormap(colormap);
		}
	}
}
#endif

// set the window hints information
void window_resizable(GtkWidget *window, GtkWidget *vte, gint run_once, gint minsize)
{
#ifdef DETAIL
	g_debug("! Launch window_resizable() with window = %p, vte = %p, run_once = %d, min_size = %d",
		window, vte, run_once, minsize);
#endif
	// vte=NULL when creating a new root window with drag & drop.
	// if (vte==NULL) return;

	GdkGeometry hints = {0};
	// g_debug("Trying to get padding...\n");
	vte_terminal_get_padding (VTE_TERMINAL(vte), &hints.base_width, &hints.base_height);
	
	if (run_once)
	{
		// Only run once when init window or fonts had been changed.
		if (run_once==1)
		{
			hints.width_inc = vte_terminal_get_char_width(VTE_TERMINAL(vte));
			hints.height_inc = vte_terminal_get_char_height(VTE_TERMINAL(vte));
		}
		if (run_once==2)
		{
			hints.width_inc = 1;
			hints.height_inc = 1;
		}
		// g_debug("hints.base_width = %d, hints.base_height = %d\n",
		//	hints.base_width, hints.base_height);
		// g_debug("hints.width_inc = %d, hints.height_inc = %d",
		//	hints.width_inc, hints.height_inc);
	}

	// minsize = -1: the size of vte can NOT be changed.
	if (minsize == -1)
	{
		hints.min_width = minsize;
		hints.min_height = minsize;
	}
	else
	{
		hints.min_width = hints.base_width + hints.width_inc;
		hints.min_height = hints.base_height + hints.height_inc;
	}

	// g_debug("Tring to set geometry on %p, and run_once = %d\n", vte, run_once);
	gtk_window_set_geometry_hints (GTK_WINDOW (window), GTK_WIDGET (vte), &hints,
					GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

	//g_debug("current the size of vte %p whith hinting = %ld x %ld\n",
	//			vte,
	//			vte_terminal_get_column_count(VTE_TERMINAL(vte)),
	//			vte_terminal_get_row_count(VTE_TERMINAL(vte)));
}

// return TRUE if 'key_name' is a valid key or NULL; or it will return FALSE.
gboolean accelerator_parse (const gchar *key_name, const gchar *key_value, guint *key, guint *mods)
{
	// key_value example: "Ctrl+Shift Home"

	gchar **values = NULL, **functions = NULL;
	gint i=0, j;
	gint tempmods=0;
	gint tempkey=0;
	
	// g_debug ("Ken Value = %s\n",key_value);
	// g_debug ("Checking '%s' is \"\" or NULL...%d, %d, %d\n",
	//	    key_value, key_value!=NULL, strlen(key_value)>0, g_ascii_strcasecmp(key_value, "NULL"));
	// got the Function key first
	if ( key_value !=NULL && (strlen(key_value)>0) && ( g_ascii_strcasecmp(key_value, "NULL")))
	{
		// g_debug ("Start to checking %s in accelerator_parse()...\n", key_value);
		values = g_strsplit_set(key_value, " ", 0);
		// split in to "Ctrl+Shift" and "Home" now...
		if (values)
		{
			functions = g_strsplit_set(values[0], "+", 0);
			// got the function key ("Ctrl+Shift" for example)
			while (functions[i]!=NULL)
			{
				for (j=0; j<MOD; j++)
				{
					// g_debug("dealing %s...\n", functions[i]);
					if ( ! g_ascii_strcasecmp(functions[i], modkeys[j].name))
						tempmods |= modkeys[j].mod;
				}
				i++;
			}
			// g_debug("masks = %x\n", tempmods);
			if (! tempmods)
			{
				g_warning("no Function Key found in [%s] (%s)!", key_name, key_value);
				clean_accelerator_parse_data(values, functions);
				return FALSE;
			}
		
			if (values[1])
			{
				// got the key ("Home" for example)
				tempkey = gdk_keyval_from_name(values[1]);
				// g_debug("key = %d\n", tempkey);
				if (tempkey == GDK_VoidSymbol || tempkey == 0)
				{
					// not a valid key
					g_warning("\"%s\" in [%s] (%s) is not a valid key!",
						  values[1], key_name, key_value);
					clean_accelerator_parse_data(values, functions);
					return FALSE;
				}
				else
				{
					// g_debug("%s (%s, value %x) is a valid key!\n",
					//		 values[1], gdk_keyval_name(tempkey), tempkey);
					// suppose that the key is always upper case
					if ((tempkey>=GDK_a) && (tempkey<=GDK_z))
						tempkey=tempkey-GDK_a+GDK_A;
					*key=tempkey;
					// suppose that CapsLock & NumLock always on
					// *mods=(tempmods|GDK_LOCK_MASK|GDK_MOD2_MASK)&GDK_MODIFIER_MASK;
					*mods = tempmods | DUD_MASK;
					clean_accelerator_parse_data(values, functions);
					return TRUE;
				}
			}
			else
			{
				g_warning("no Work Key found in [%s] (%s)!", key_name, key_value);
				clean_accelerator_parse_data(values, functions);
				return FALSE;
			}
		}
		else
		{
			// not a valid key, no space in key_name
			g_warning("%s in [%s] (%s) is not a valid key. There is no SPACE in it.",
					values[1], key_name, key_value);
			clean_accelerator_parse_data(values, functions);
			return FALSE;
		}
	}
	else
	{
		// NULL
		// g_warning("We Got a NULL Key (%s)!\n", key_value);
		*key=GDK_VoidSymbol;
		*mods=-1;
		clean_accelerator_parse_data(values, functions);
		return TRUE;
	}
}

void clean_accelerator_parse_data(gchar **values, gchar **functions)
{
#ifdef DETAIL
	g_debug("* free vales %p and functions %p in clean_accelerator_parse_data()", values, functions);
#endif
	g_strfreev(values);
	g_strfreev(functions);
}

GString *save_user_settings(GtkWidget *widget, GtkWidget *vte)
{
#ifdef DETAIL
	g_debug("! Launch save_user_settings with vte = %p", vte);
#endif

	struct Window *win_data = NULL;
	struct Page *page_data = NULL;
	GError *error = NULL;
	const gchar *profile_dir;
	gchar *profile = NULL;
	GString *contents = NULL;
	GKeyFile *keyfile = g_key_file_new();
	gint i;

	if (vte)
	{
		page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
		win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	}
	else
	{
		// For -p command line option
		win_data = g_new0(struct Window, 1);
		page_data = g_new0(struct Page, 1);

		init_window_parameters(win_data);
		init_user_keys(win_data);
		init_function_keys();
		init_page_color_data();
		init_command();
		init_user_command(win_data);
		init_colors();
		init_user_color_data(win_data);
	}


	if (win_data->use_custom_profile)
		profile = win_data->profile;
	else if (vte)
	{
		profile_dir = g_get_user_config_dir();
		profile = g_strdup_printf("%s/%s", profile_dir, RCFILE);
		if (g_mkdir_with_parents(profile_dir, 0700))
		{
			g_critical("can NOT create the directory: %s", profile_dir);
			free_user_settings_data(error, profile, contents, keyfile, FALSE);
			return NULL;
		}
	}
	
	contents = g_string_new("[main]\n\n");
	g_string_append_printf(contents,"# The version of this profile's format. DO NOT EDIT IT!\n"
					"version = %s\n\n", RCFILE_FORMAT_VERSION);
	if (vte)
	{
		g_string_append_printf(contents,"# The default font name of vte terminal.\n"
						"font_name = %s\n\n", page_data->font_name);
		g_string_append_printf(contents,"# The default column of vte terminal.\n"
						"column = %ld\n\n",
				       vte_terminal_get_column_count(VTE_TERMINAL(vte)));
		g_string_append_printf(contents,"# The default row of vte terminal.\n"
						"row = %ld\n\n", vte_terminal_get_row_count(VTE_TERMINAL(vte)));
		g_string_append_printf(contents,"# Use true opacity. Left it blank will enable it automatically\n"
						"# if the window manager were composited.\n");
		if (win_data->use_rgba_orig == 2)
			g_string_append_printf(contents, "use_rgba =\n\n");
		else
			g_string_append_printf(contents, "use_rgba = %d\n\n", win_data->use_rgba_orig);
	}
	else
	{
		g_string_append_printf(contents,"# The default font name of vte terminal.\n"
						 "font_name = %s\n\n", win_data->system_font_name);
		g_string_append_printf(contents,"# The default column of vte terminal.\n"
						 "column = %d\n\n", win_data->system_column);
		g_string_append_printf(contents,"# The default row of vte terminal.\n"
						 "row = %d\n\n", win_data->system_row);
		g_string_append_printf(contents,"# Use true opacity. Left it blank will enable it automatically\n"
						"# if the window manager were composited.\n"
						 "use_rgba =\n\n");
	}
#ifdef ENABLE_RGBA
	g_string_append_printf(contents,"# Transparent Window. Only enabled when the window manager were composited.\n"
					"transparent_window = %d\n\n", win_data->transparent_window>0);
	g_string_append_printf(contents,"# The opacity of transparent Window.\n"
					"window_opacity = %1.3f\n\n", win_data->window_opacity);
#endif
	g_string_append_printf(contents,"# Use Transparent Background.\n"
					"# It will use true transparent if the window manager were composited.\n"
					"transparent_background = %d\n\n", win_data->transparent_background>0);
	g_string_append_printf(contents,"# The saturation of transparent background.\n"
					"background_saturation = %1.3f\n\n", win_data->background_saturation);
       g_string_append_printf(contents,"# Comfirm to close multi tabs.\n"
					"confirm_to_close_multi_tabs = %d\n\n", win_data->confirm_to_close_multi_tabs);
	g_string_append_printf(contents,"# Shows [Transparent Background], [Background Saturation]\n"
					"# [Transparent Window] and [Window Opacity] on right click menu.\n"
					"show_transparent_menu = %d\n\n", win_data->show_transparent_menu);
	g_string_append_printf(contents,"# Shows [Change the foreground color]\n"
					"# and [Change the background color] on right click menu.\n"
					"show_color_selection_menu = %d\n\n", win_data->show_color_selection_menu);
	g_string_append_printf(contents,"# The normal text color used in vte terminal.\n"
					"# You may use black or #000000 here.\n"
					"foreground_color = %s\n\n", win_data->foreground_color);
	g_string_append_printf(contents,"# The background color used in vte terminal.\n"
					"# You may use black or #000000 here.\n"
					"background_color = %s\n\n", win_data->background_color);
	g_string_append_printf(contents,"# Shows [Increase window size], [Decrease window size],\n"
					"# [Reset to default font/size] and [Reset to system font/size]\n"
					"# on right click menu.\n"
					"show_resize_menu = %d\n\n", win_data->show_resize_menu);
	g_string_append_printf(contents,"# Using AntiAlias when showing fonts.\n"
					"# 0: default. 1: force enable. 2: force disable.\n"
					"font_anti_alias = %d\n\n", win_data->font_anti_alias);
	g_string_append_printf(contents,"# The ratio when resizing font via function key <Ctrl><+> and <Ctrl><->.\n"
					"# 0: the font size is +/- 1 when resizing.\n"
					"font_resize_ratio = %1.3f\n\n", win_data->font_resize_ratio);
	g_string_append_printf(contents,"# The ratio when resizing window via right click menu.\n"
					"# 0: the font size is +/- 1 when resizing window.\n"
					"window_resize_ratio = %1.3f\n\n", win_data->window_resize_ratio);
	g_string_append_printf(contents,"# When user double clicks on a text, which character will be selected.\n"
					"word_chars = %s\n\n", win_data->word_chars);
	g_string_append_printf(contents,"# The lines of scrollback history.\n"
					"scrollback_lines = %d\n\n", win_data->scrollback_lines);
	g_string_append_printf(contents,"# Shows scrollbar or not.\n"
					"show_scrollbar = %d\n\n", win_data->show_scrollbar);
	g_string_append_printf(contents,"# The position of scrollbar.\n"
					"# 0: scrollbar is on left; 1: scrollbar is on right.\n"
					"scrollbar_position = %d\n\n", win_data->scrollbar_position);
	g_string_append_printf(contents,"# Shows input method menu on right click menu.\n"
					"show_input_method_menu = %d\n\n", win_data->show_input_method_menu);
	g_string_append_printf(contents,"# Shows get function key menu on right click menu.\n"
					"show_get_function_key_menu = %d\n\n", win_data->show_get_function_key_menu);
	g_string_append_printf(contents,"# Shows change page name menu on right click menu.\n"
					"show_change_page_name_menu = %d\n\n", win_data->show_change_page_name_menu);
	g_string_append_printf(contents,"# Enable hyperlink in vte terminal.\n"
					"enable_hyperlink = %d\n\n", win_data->enable_hyperlink);
	g_string_append_printf(contents,"# Sets whether or not the cursor will blink in vte terminal.\n"
					"cursor_blinks = %d\n\n", win_data->cursor_blinks);

	g_string_append_printf(contents,"# Shows copy/paste menu on right click menu.\n"
					"show_copy_paste_menu = %d\n\n",
					win_data->show_copy_paste_menu);

	g_string_append_printf(contents,"# Sets whether or not the terminal will beep\n"
					"# when the child outputs the \"bl\" sequence.\n"
					"audible_bell = %d\n\n", win_data->audible_bell);

	g_string_append_printf(contents,"# Sets whether or not the terminal will flash\n"
					"# when the child outputs the \"bl\" sequence.\n"
					"visible_bell = %d\n\n", win_data->visible_bell);

	g_string_append_printf(contents,"# Which string the terminal should send to an application\n"
					"# when the user presses the Delete or Backspace keys.\n"
					"# 0: VTE_ERASE_AUTO\n"
					"# 1: VTE_ERASE_ASCII_BACKSPACE\n"
					"# 2: VTE_ERASE_ASCII_DELETE\n"
					"# 3: VTE_ERASE_DELETE_SEQUENCE\n"
					"erase_binding = %d\n\n", win_data->erase_binding);

	g_string_append_printf(contents,"# The locales list on right click menu.\n"
					"# You may use zh_TW or zh_TW.Big5 here.\n"
					"# Left it blank will disable locale select menu items.\n"
					"locales_list = %s\n\n", win_data->locales_list);
	g_string_append_printf(contents,"# Sets what type of terminal attempts to emulate.\n"
					"# It will also set the TERM environment.\n"
					"# Unless you are interested in this feature, always use \"xterm\".\n"
					"emulate_term = %s\n\n", win_data->emulate_term);

	g_string_append_printf(contents,"# The environment 'VTE_CJK_WIDTH' used when init a vte terminal.\n"
					"# 0: get via environment; 1: use narrow ideograph; 2: use wide ideograph\n"
					"VTE_CJK_WIDTH = %d\n\n", win_data->VTE_CJK_WIDTH);
	g_string_append_printf(contents,"\n");
	g_string_append_printf(contents,"[page]\n\n");
	g_string_append_printf(contents,"# The max character width of page name.\n"
					"page_width = %d\n\n", win_data->page_width);
	g_string_append_printf(contents,"# The page width will always use the max character width.\n"
					"fixed_page_width = %d\n\n", win_data->fixed_page_width);
	g_string_append_printf(contents,"# The position of tab bar.\n" 
					"# 0: Top, 1: bottom.\n"
					"tabbar_position = %d\n\n", win_data->tabbar_position);
	g_string_append_printf(contents,"# The label of tabs will fill the tab bar.\n"
					"fill_tab_bar = %d\n\n", win_data->fill_tab_bar);
	g_string_append_printf(contents,"# The page name used for a new page.\n"
					"page_name = %s\n\n", win_data->page_name);
	g_string_append_printf(contents,"# The page names list used for new pages, separate with <space>.\n"
					"page_names = %s\n\n", win_data->page_names);
	g_string_append_printf(contents,"# Reuse the page name in the page names list.\n"
					"reuse_page_names = %d\n\n", win_data->reuse_page_names);
	g_string_append_printf(contents,"# Shows a (number no) on the page name.\n"
					"page_shows_number = %d\n\n", win_data->page_shows_number);
	g_string_append_printf(contents,"# Shows the foreground running command on the page name.\n"
					"page_shows_current_cmdline = %d\n\n", win_data->page_shows_current_cmdline);
	g_string_append_printf(contents,"# Shows the terminal's idea of what the window's title should be.\n"
					"page_shows_window_title = %d\n\n", win_data->page_shows_window_title);
	g_string_append_printf(contents,"# Shows current directory on the page name.\n"
					"page_shows_current_dir = %d\n\n", win_data->page_shows_current_dir);
	g_string_append_printf(contents,"# Check if the running command is root privileges.\n"
					"check_root_privileges = %d\n\n", win_data->check_root_privileges);
	g_string_append_printf(contents,"# Shows current encoding on the page name.\n"
					"page_shows_encoding = %d\n\n", win_data->page_shows_encoding);
	g_string_append_printf(contents,"# Bold the text of current page name.\n"
					"bold_current_page_name = %d\n\n", win_data->bold_current_page_name);
	g_string_append_printf(contents,"# Bold the text of action page name.\n"
					"bold_action_page_name = %d\n\n", win_data->bold_action_page_name);
	g_string_append_printf(contents,"# Shows the page name of current page on window title.\n"
					"window_shows_current_page = %d\n\n", win_data->window_shows_current_page);       
	g_string_append_printf(contents,"# Use colorful text on page.\n"
					"use_color_page = %d\n\n", win_data->use_color_page);
	for (i=0; i<PAGE_COLOR; i++)
	{			       
		g_string_append_printf( contents,
					"# The color used for showing %s on page name.\n"
					"# You may use black or #000000 or #000000000000 here.\n"
					"%s = %s\n\n",
					page_color[i].comment_eng, page_color[i].name, win_data->user_page_color[i]);
	}
	g_string_append_printf(contents,"[key]\n\n");
	for (i=0; i<KEYS; i++)
		g_string_append_printf( contents,"%s\n%s = %s\n\n",
					system_keys[i].comment, system_keys[i].name, win_data->user_keys[i].value);
	g_string_append_printf(contents,"\n");
	g_string_append_printf(contents,"[color]\n\n");
	g_string_append_printf(contents,"# The main ansi color theme used in vte.\n"
					"# Possible values are linux, xterm, rxvt, tango, and vte_fixed.\n"
					"# or left it blank to use the default settings form libvte.\n"
					"theme = %s\n\n", win_data->color_theme);
	g_string_append_printf(contents,"# The brightness for ansi colors used in terminal.\n"
					"brightness = %1.3f\n\n", win_data->color_brightness);
	g_string_append_printf(contents,"# The brightness for ansi colors used in terminal when inactive.\n"
					"# Left it blank to disable this feature.\n"
					"inactive_brightness = %1.3f\n\n", win_data->color_brightness_inactive);
	for (i=0; i<COLOR; i++)
		g_string_append_printf( contents,"%s\n"
					"# You may use black or #000000 or #000000000000 here.\n"
					"%s = %s\n\n",
					color[i].comment, color[i].name, win_data->color_value[i]);
	g_string_append_printf(contents,"\n");

	g_string_append_printf(contents,"[command]\n\n");
	g_string_append_printf(contents,"# method={1,2,3}\n"
					"# 0: Open the hyperlink in new tab.\n"
					"#    Use it if the command were using CLI, like w3m.\n");
	g_string_append_printf(contents,"# 1: Open the hyperlink with gdk_spawn_on_screen_with_pipes().\n"
					"#    Use it if the command were using GUI, like firefox.\n");
	g_string_append_printf(contents,"# 2: Open the hyperlink in new window,\n"
					"#    Use it if you not sure.\n");
	g_string_append_printf(contents,"#\n"
					"# The environs will apply to the application, separated with <tab>.\n"
					"# The parameters of the application should be also separated with <tab>, if any.\n\n");
	for (i=0; i<COMMAND; i++)
	{
		g_string_append_printf( contents,"%s\n%s = %s\n",
					command[i].comment, command[i].name, win_data->user_command[i].command);
		g_string_append_printf( contents,"%s = %d\n",
					command[i].method_name, win_data->user_command[i].method);
		g_string_append_printf( contents,"%s = %d\n",
					command[i].VTE_CJK_WIDTH_name, win_data->user_command[i].VTE_CJK_WIDTH);
		g_string_append_printf( contents,"%s = %s\n\n",
					command[i].environ_name, win_data->user_command[i].environ);
	}

	if (!vte)
		// For -p command line option
		// LilyTerm will exit immediately, so we don't need to free the datas used by LilyTerm. XD
		return contents;
	
	// g_debug("\n%s\n", contents->str);
	gchar *real_file_name = canonicalize_file_name((const gchar *)profile);
	if (! real_file_name)
	{
		//if (access(profile, F_OK))
		if (g_file_test(profile, G_FILE_TEST_IS_SYMLINK))
		{
			error_dialog(win_data, _("The target file is NOT exists"), NULL, 6);
			return NULL;
		}
		else
			real_file_name = g_strdup(profile);
	}

	// g_debug("real_file_name = %s", real_file_name);
	if (g_file_test(real_file_name, G_FILE_TEST_IS_DIR))
	{       
		// g_debug("The target file is a directory!!");
		error_dialog(win_data, _("The target file is a directory"), NULL, 6);
	}
	else
	{
		// g_debug("G_FILE_TEST_IS_REGULAR = TRUE");
		if ( ! g_file_set_contents(real_file_name, contents->str, -1, &error))
		{
			// g_debug("Error while writing profile '%s': %s", real_file_name, error->message);
			error_dialog(win_data, error->message, NULL, 6);
		}
	}
	g_free(real_file_name);
	free_user_settings_data(error, profile, contents, keyfile, win_data->use_custom_profile);
	return NULL;
}

void free_user_settings_data(GError *error, gchar *profile, GString* contents, GKeyFile *keyfile, gboolean use_custom_profile)
{
#ifdef DETAIL
	g_debug("! Launch free_user_settings_data");
#endif

	if (error!=NULL)
		g_clear_error(&error);
	if (!use_custom_profile)
	{
#ifdef DETAIL
		g_debug("* free profile %p (%s) in free_user_settings_data()", profile, profile);
#endif
		g_free(profile);
	}
#ifdef DETAIL
	g_debug("* free contents %p (%s) in free_user_settings_data()", contents, contents->str);
#endif
	g_string_free(contents, TRUE);
#ifdef DETAIL
	g_debug("* free keyfile %p in free_user_settings_data()", keyfile);
#endif
	g_key_file_free(keyfile);
}

gchar **get_supported_locales(gchar *default_locale, gchar *locales_list, gchar *LC_TYPE, gchar *other_locale)
{
#ifdef DETAIL
	g_debug("! Launch get_supported_locale with *default_locale = %s,"
		" locales_list = %s, other_locale = %s",
		default_locale, locales_list, other_locale);
#endif

	GString *full_locales_list = g_string_new(NULL);
	if (default_locale)
		g_string_append( full_locales_list, default_locale);
	
	if (locales_list)
	{
		if (full_locales_list->len)
			g_string_append( full_locales_list, " ");
		g_string_append( full_locales_list, locales_list);
	}

	if (LC_TYPE)
	{
		if (full_locales_list->len)
			g_string_append( full_locales_list, " ");
		g_string_append( full_locales_list, LC_TYPE);
	}

	if (other_locale)
	{
		if (full_locales_list->len)
			g_string_append( full_locales_list, " ");
		g_string_append( full_locales_list, other_locale);
	}

	// g_debug("Got the full_locales_list = %s", full_locales_list);
	gchar **supported_locales = g_strsplit_set(full_locales_list->str, " ,", 0);
#ifdef DETAIL
	g_debug("* free full_locales_list %p (%s) in get_supported_locales",
		full_locales_list, full_locales_list->str);
#endif
	g_string_free(full_locales_list, TRUE);
	return supported_locales;
}

void win_data_dup(struct Window *win_data_orig, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch win_data_dup with win_data_orig = %p, win_data = %p", win_data_orig, win_data);
#endif

	// The win_data->window is created already before win_data_dup() is launched.
	GtkWidget *window_orig = win_data->window;
	memcpy( win_data, win_data_orig, sizeof(* win_data_orig));
	// g_debug("Copyed size = %d", sizeof(* win_data_orig));


// ---- environ and command line option ---- //

	// win_data->environment = g_strdup (win_data_orig->environment);
	win_data->default_locale = g_strdup(win_data_orig->default_locale);
	win_data->locales_list = g_strdup(win_data_orig->locales_list);
	//g_debug("win_data->default_locale = %s", win_data->default_locale);
	//g_debug("win_data->locales_list = %s", win_data->locales_list);
	//g_debug("get_default_LC_TYPE() = %s", get_default_LC_TYPE());
	//g_debug("win_data_orig->supported_locales[0] = %s", win_data_orig->supported_locales[0]);
	gchar *supported_locale = NULL;
	if (win_data_orig->supported_locales)
		supported_locale = win_data_orig->supported_locales[0];
	win_data->supported_locales = get_supported_locales(win_data->default_locale,
							    win_data->locales_list,
							    get_default_LC_TYPE(),
							    supported_locale);
	win_data->emulate_term = g_strdup(win_data_orig->emulate_term);
	// win_data->VTE_CJK_WIDTH;
	win_data->VTE_CJK_WIDTH_STR = g_strdup(win_data_orig->VTE_CJK_WIDTH_STR);
	// win_data->argc;
	// win_data->argv;
	// win_data->command;
	win_data->init_tab_number = 1;
	win_data->init_dir = NULL;
	// win_data->use_custom_profile;
	win_data->profile = g_strdup(win_data_orig->profile);

	// g_debug("dup environ and command line option finish!");

// ---- the component of a single window ---- //

	// win_data->fullscreen;
	// win_data->true_fullscreen;
	// win_data->fullscreen_show_tab_bar;
	// win_data->fullscreen_show_scroll_bar;
	// win_data->unfullscreen;

	win_data->window = window_orig;
	win_data->notebook = NULL;
	win_data->current_vte = NULL;
	// win_data->window_shows_current_page;
	if (win_data->custom_window_title)		  
	{
		win_data->custom_window_title = g_strdup(win_data_orig->custom_window_title);
		update_window_title(win_data->window, win_data->custom_window_title);
	}
	win_data->window_title_tpgid=0;
	// win_data->use_rgba;
	// win_data->use_rgba_orig;
#ifdef ENABLE_RGBA
	// win_data->transparent_window;
	// win_data->window_opacity;
#endif
	// win_data->enable_function_key;
	gint i;
	for (i=0; i<KEYS; i++)
		win_data->user_keys[i].value = g_strdup(win_data_orig->user_keys[i].value);
	// win_data->update_hints = win_data_orig->update_hints;
	win_data->update_hints = 1;
	// win_data->lost_focus;
	// win_data->keep_vte_size;

// ---- the component of a single menu ---- //

	win_data->menu = NULL;
	// win_data->menu_actived;
	win_data->encoding_sub_menu = NULL;
	win_data->encoding_group = NULL;
	win_data->default_encoding = NULL;
#ifdef ENABLE_RGBA
	win_data->menuitem_trans_win = NULL;
#endif
	win_data->menuitem_trans_bg = NULL;
	// win_data->show_color_selection_menu;
	// win_data->show_resize_menu;
	// win_data->font_resize_ratio;
	// win_data->window_resize_ratio;
	// win_data->show_transparent_menu;
	// win_data->show_input_method_menu;
	// win_data->show_get_function_key_menu;
	// win_data->show_change_page_name_menu;
	// win_data->enable_hyperlink;
	for (i=0; i<COMMAND; i++)
	{
		win_data->user_command[i].command = g_strdup(win_data_orig->user_command[i].command);
		win_data->user_command[i].environ = g_strdup(win_data_orig->user_command[i].environ);
		win_data->user_command[i].environments = g_strsplit(
						win_data->user_command[i].environ, " ", -1);
	}
	win_data->menuitem_copy_url = NULL;
	win_data->menuitem_dim_text = NULL;
	win_data->menuitem_cursor_blinks = NULL;
	win_data->menuitem_audible_bell = NULL;
	win_data->menuitem_show_tabs_bar = NULL;
	win_data->menuitem_hide_tabs_bar = NULL;
	win_data->menuitem_hide_scroll_bar = NULL;
	// win_data->show_copy_paste_menu;
	win_data->menuitem_copy = NULL;
	win_data->menuitem_paste = NULL;

// ---- the color used in vte ---- //

	win_data->foreground_color = g_strdup(win_data_orig->foreground_color);
	win_data->background_color = g_strdup(win_data_orig->background_color);
	// win_data->fg_color;
	// win_data->fg_color_inactive;
	// win_data->bg_color;
	win_data->color_theme = g_strdup(win_data_orig->color_theme);
	win_data->color_theme_orig = g_strdup(win_data_orig->color_theme_orig);
	for (i=0; i<COLOR; i++)
	{
		win_data->color_value[i] = g_strdup(win_data_orig->color_value[i]);
		win_data->color[i] = win_data_orig->color[i];
		win_data->color_inactive[i] = win_data_orig->color_inactive[i];
		win_data->color_orig[i] = win_data_orig->color_orig[i];
	}
	// win_data->using_custom_color;
	// win_data->using_custom_color_orig;
	// win_data->color_brightness;
	// win_data->color_brightness_orig;
	// win_data->color_brightness_inactive;

// ---- tabs on notebook ---- //

	// win_data->page_width;
	// win_data->fixed_page_width;
	// win_data->fill_tab_bar;
	// win_data->tabbar_position;

	win_data->page_name = g_strdup(win_data_orig->page_name);
	// win_data->reuse_page_names;
	win_data->page_names = g_strdup(win_data_orig->page_names);
	win_data->splited_page_names = g_strsplit_set(win_data->page_names, " ", 0);
	// win_data->page_names_no;
	// win_data->max_page_names_no;

	// win_data->page_shows_number;
	// win_data->page_shows_encoding;
	// win_data->page_shows_current_cmdline;
	// win_data->page_shows_window_title;
	// win_data->page_shows_current_dir;
	// win_data->use_color_page;
	// win_data->check_root_privileges;
	// win_data->bold_current_page_name;
	// win_data->bold_action_page_name;

	for (i=0; i<PAGE_COLOR; i++)
		win_data->user_page_color[i] = g_strdup(win_data_orig->user_page_color[i]);

// ---- font ---- //
	
	// win_data->font_anti_alias;
	win_data->default_font_name = g_strdup(win_data_orig->default_font_name);
	win_data->system_font_name = g_strdup(win_data_orig->system_font_name);
	win_data->restore_font_name = NULL;

// ---- other settings for init a vte ---- //

	// win_data->default_column;
	// win_data->default_row;
	// win_data->system_column;
	// win_data->system_row;
	win_data->word_chars = g_strdup(win_data_orig->word_chars);
	// win_data->show_scrollbar;
	// win_data->scrollbar_position;
	// win_data->transparent_background;
	// win_data->background_saturation;
	// win_data->scrollback_lines;
	// win_data->dim_text;
	// win_data->cursor_blinks;
	// win_data->audible_bell;
	// win_data->visible_bell;
	// win_data->erase_binding;

// ---- other ---- //

	// win_data->query_encoding;
	// win_data->dialog_actived;
	// win_data->kill_color_demo_vte;
	win_data->err_str.string = NULL;
	win_data->err_str.encoding = NULL;
}

void page_data_dup(struct Page *page_data_prev, struct Page *page_data)
{
	memcpy( page_data, page_data_prev, sizeof(* page_data_prev));

	// page_data->window;
	// page_data->notebook;
	page_data->page_no = 0;

	page_data->label = NULL;
	page_data->hbox = NULL;
	page_data->vte = NULL;
	page_data->scrollbar = NULL;

	page_data->encoding_str = g_strdup(page_data_prev->encoding_str);
	page_data->locale = g_strdup(page_data_prev->locale);
	page_data->environ = g_strdup(page_data_prev->environ);
	// g_debug("g_strdupv(page_data_prev->environments)!");
	// page_data->environments = g_strdupv(page_data_prev->environments);
	page_data->VTE_CJK_WIDTH_STR = g_strdup(page_data_prev->VTE_CJK_WIDTH_STR);

	page_data->page_name = NULL;
	page_data->pid = -1;
	page_data->new_tpgid = 0;
	page_data->displayed_tpgid = 0;
	page_data->page_name = NULL;
	page_data->custom_page_name = NULL;
	page_data->tab_color = NULL;

	page_data->pwd = get_current_pwd_by_pid(page_data_prev->pid);
	page_data->is_root = 0;
	page_data->is_bold = 0;

	page_data->window_title_updated = -1;
	page_data->page_update_method = 0;
	page_data->page_update_method_orig = 0;
	page_data->window_title_signal = 0;

	page_data->timeout_id = 0;

	page_data->font_name = g_strdup(page_data_prev->font_name);
	page_data->font_size = 0;

	// page_data->check_root_privileges;
	// page_data->page_shows_window_title;
	// page_data->page_shows_current_dir;
	// page_data->page_shows_current_cmdline;
	// page_data->bold_action_page_name;
	// page_data->window_title_tpgid;
	// page_data->lost_focus;
	// page_data->keep_vte_size;
	// page_data->current_vte;
	page_data->window_title_pwd = NULL;
	// page_data->custom_window_title;

	gint i;
	for (i=0; i<COMMAND; i++)
		page_data->tag[i] = 0;
}

// The returned string should be freed when no longer needed.
gchar *get_current_pwd_by_pid(pid_t pid)
{
	gchar *priv_pwd = g_strdup_printf("/proc/%d/cwd", pid);
	gchar *pwd = g_file_read_link(priv_pwd, NULL);
	g_free(priv_pwd);
	// g_debug("use the directory %s (/proc/%d/cwd)\n", directory, prev_page->pid);
	return pwd;
}

struct Page *get_page_data_from_nth_page(struct Window *win_data, guint page_no)
{
	GtkWidget *vte = (GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
					GTK_NOTEBOOK(win_data->notebook),
						gtk_notebook_get_nth_page(
							GTK_NOTEBOOK(win_data->notebook),
									 page_no))),
							"VteBox");
	return (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");

}

void apply_new_win_data_to_page (struct Window *win_data_orig,
				 struct Window *win_data,
				 struct Page *page_data)
{
//	// 0: do NOT use rgba
//	// 1: force to use rgba
//	// 2: decide by gdk_screen_is_composited()
//	if (win_data_orig->use_rgba != win_data->use_rgba)
//	{
//	}
//#ifdef ENABLE_RGBA
//	if (win_data_orig->transparent_window != win_data->transparent_window)
//	{
//	}
//	if (win_data_orig->window_opacity != win_data->window_opacity)
//	{
//	}
//#endif
	if (win_data_orig->enable_hyperlink != win_data->enable_hyperlink)
		set_hyprelink(win_data, page_data);

// ---- the color used in vte ---- //
	gboolean update_color = FALSE;

	if (compare_color(win_data_orig->fg_color, win_data->fg_color) ||
	    compare_color(win_data_orig->bg_color, win_data->bg_color) ||
	    (win_data_orig->using_custom_color != win_data->using_custom_color) ||
	    (win_data_orig->color_brightness != win_data->color_brightness))
	    	update_color = TRUE;
	
	gint i;
	if ( ! update_color)
	{
		for (i=0; i<COLOR; i++)
			if (compare_color(win_data_orig->color[i], win_data->color[i]))
				update_color = TRUE;
	}
	if (update_color)
		set_vte_color(win_data, page_data);

// ---- tabs on notebook ---- //

	if ((win_data_orig->page_width != win_data->page_width) ||
	    (win_data_orig->fixed_page_width != win_data->fixed_page_width))
		set_page_width(win_data, page_data);

	if ((win_data_orig->page_shows_current_cmdline || win_data_orig->page_shows_current_dir || win_data_orig->page_shows_window_title) !=
	    (win_data->page_shows_current_cmdline || win_data->page_shows_current_dir || win_data->page_shows_window_title))
		add_remove_page_timeout_id(win_data, page_data);
	
	if (win_data->page_shows_window_title != win_data_orig->page_shows_window_title)
		add_remove_window_title_changed_signal(page_data);
	
	// If the new page don't use page_shows_current_cmdline & page_shows_current_dir
	switch (page_data->page_update_method)
	{
		case 0:
			apply_new_page_color (win_data, page_data);
			break;
		case 1:
			if (win_data->page_shows_window_title != win_data_orig->page_shows_window_title)
				apply_new_page_color (win_data, page_data);
			break;
		case 2:
			if (win_data->page_shows_current_cmdline != win_data_orig->page_shows_current_cmdline)
				apply_new_page_color (win_data, page_data);
			break;
		case 3:
			if (win_data->page_shows_current_dir != win_data_orig->page_shows_current_dir)
				apply_new_page_color (win_data, page_data);
			break;
		case 4:
			if (win_data->page_shows_window_title ||
			    win_data->page_shows_current_cmdline ||
			    win_data->page_shows_current_dir)
				apply_new_page_color (win_data, page_data);
			break;
	}

// ---- font ---- //

	if (win_data_orig->font_anti_alias != win_data->font_anti_alias)
		vte_terminal_set_font_from_string_full (VTE_TERMINAL(page_data->vte),
							page_data->font_name,
							win_data->font_anti_alias);

// ---- other settings for init a vte ---- //

	if (strcmp(win_data_orig->word_chars, win_data->word_chars))
		vte_terminal_set_word_chars(VTE_TERMINAL(page_data->vte), win_data->word_chars);

	if (win_data_orig->show_scrollbar != win_data->show_scrollbar)
		hide_scrollbar(win_data, page_data);

	if (win_data_orig->scrollbar_position != win_data->scrollbar_position)
	{
		g_object_ref(page_data->vte);
		gtk_container_remove (GTK_CONTAINER(page_data->hbox), page_data->vte);
		g_object_ref(page_data->scrollbar);
		gtk_container_remove (GTK_CONTAINER(page_data->hbox), page_data->scrollbar);

		pack_vte_and_scrollbar_to_hbox(win_data, page_data);

		g_object_unref(page_data->vte);
		g_object_unref(page_data->scrollbar);
	}

	if ((win_data_orig->transparent_background != win_data->transparent_background) ||
	    (win_data_orig->background_saturation != win_data->background_saturation))
		set_background_saturation (NULL, 0, win_data->background_saturation, page_data->vte);

	// FIXME: The menu_active_window = win_data_orig or win_data here?
	// if ((win_data_orig->use_scrollback_lines != win_data->use_scrollback_lines) ||
	//     (win_data_orig->scrollback_lines != win_data->scrollback_lines))
	// {
	//	win_data->use_scrollback_lines = ! win_data->use_scrollback_lines;
	//	clean_scrollback_lines(NULL, FALSE);
	//}

	if (win_data_orig->cursor_blinks != win_data->cursor_blinks)
		vte_terminal_set_cursor_blinks (VTE_TERMINAL(page_data->vte), win_data->cursor_blinks);

	if (win_data_orig->audible_bell != win_data->audible_bell)
		vte_terminal_set_audible_bell (VTE_TERMINAL(page_data->vte), win_data->audible_bell);

	if (win_data_orig->visible_bell != win_data->visible_bell)
		vte_terminal_set_visible_bell (VTE_TERMINAL(page_data->vte), win_data->visible_bell);
	
	if (win_data_orig->erase_binding != win_data->erase_binding)
		vte_terminal_set_backspace_binding (VTE_TERMINAL(page_data->vte), win_data->erase_binding);
}

void apply_new_page_color (struct Window *win_data, struct Page *page_data)
{
	if (page_data->custom_page_name == NULL)
		page_data->custom_page_name = g_strdup(page_data->page_name);

	if (page_data->is_root)
		page_data->tab_color = win_data->user_page_color[4];
	else    
		page_data->tab_color = win_data->user_page_color[3];
}

// Will return TRUE if a and b are NOT the same.
gboolean compare_color(GdkColor a, GdkColor b)
{
	if ((a.pixel != b.pixel) ||
	    (a.red != b.red) ||
	    (a.green != b.green ) ||
	    (a.blue != b.blue))
		return TRUE;
	else
		return FALSE;
}
