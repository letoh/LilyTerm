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


#include "profile.h"

extern gint total_window;
struct ModKey modkeys[MOD] = {{0}};
struct KeyValue system_keys[KEYS] = {{0}};

void init_window_option(struct Window *win_data)
{
	// g_debug("Get win_data = %d when init window option!", win_data);

	// init_window_parameter(win_data);
	// The first command in -e option
//	win_data->command_line = NULL;
	// the argc after -e option
//	win_data->parameter = 0;
	// The argv[] after -e option
//	win_data->parameters = NULL;
	// The profile that specify by user
//	win_data->profile = NULL;
	// The init tab number after -t option
	win_data->init_tab_number = 1;
}

void init_window_parameter(struct Window *win_data)
{
	// g_debug("Get win_data = %d when init window parameter!", win_data);
	
	// the component of a single window
//	win_data->menu = NULL;
#ifdef ENABLE_RGBA
//	win_data->menuitem_trans_win = NULL;
#endif
//	win_data->menuitem_trans_bg = NULL;
//	win_data->default_encoding = NULL;
//	win_data->menuitem_scrollback_lines = NULL;

//	win_data->notebook = NULL;
//	win_data->current_vtebox = NULL;
	// default settings
	win_data->page_name = g_strdup("");
	win_data->page_names = g_strdup("");
//	win_data->splited_page_names = NULL;
//	win_data->page_names_no = 0;
	win_data->reuse_page_names = TRUE;
	win_data->page_shows_number = TRUE;
	win_data->page_shows_current_cmdline = TRUE;
	win_data->page_shows_current_dir = TRUE;
	win_data->page_shows_encoding = TRUE;
	win_data->use_color_page = TRUE;
	win_data->check_root_privileges = TRUE;
	win_data->bold_current_page_name = TRUE;
	win_data->bold_action_page_name = TRUE;
	win_data->page_cmdline_color = g_strdup("#1C1CDC");
	win_data->page_dir_color = g_strdup("#215E3E");
	win_data->page_custom_color = g_strdup("#9C0A81");
	win_data->page_root_color = g_strdup("#BE0020");
	win_data->page_normal_color = g_strdup("#333333");
	win_data->window_shows_current_page = TRUE;
	win_data->page_width = 16;
	win_data->fixed_page_width = TRUE;
	win_data->show_color_selection_menu = TRUE;
	win_data->foreground_color = g_strdup("white");
	win_data->background_color = g_strdup("black");
	//win_data->fg_color = {0};
	//win_data->bg_color = {0};
	win_data->show_resize_menu = TRUE;
	win_data->default_font_name = g_strdup("Monospace 12");
	win_data->system_font_name = g_strdup("Monospace 12");
	win_data->default_column = 80;
	win_data->default_row = 24;
	win_data->system_column = 80;
	win_data->system_row = 24;
	win_data->show_transparent_menu = TRUE;
	// 0: do NOT use rgba
	// 1: force to use rgba
	// 2: decide by gdk_screen_is_composited()
	win_data->use_rgba = 2;
	win_data->original_use_rgba = 2;
#ifdef ENABLE_RGBA
	win_data->transparent_window = 2;
	win_data->window_opacity = 0.05;
#endif
	win_data->transparent_background = 2;
	win_data->background_saturation = 0.15;
	win_data->word_chars = g_strdup("-A-Za-z0-9_$.+!*(),;:@&=?/~#%[]<>");
	win_data->scrollback_lines = 1024;
	win_data->show_input_method_menu = FALSE;
	win_data->show_get_function_key_menu = TRUE;
//	win_data->default_locale = NULL;
	win_data->locales_list = g_strdup("ja_JP.EUC-JP zh_CN.GB2312 zh_TW.Big5");
	// supported_locales CAN NOT be free!
//	win_data->supported_locales = NULL;
	// the default key value. the default value is initialed in get_user_settings().
	// system_keys[KEYS] = {{0}};
//	win_data->update_hints = 0;
	win_data->enable_function_key = TRUE;

	// Trying to keep the vtebox size:
	// 1, When the page bar was hidden.
	// 2, When the page bar was shown.
	// 3, When the font was changed by right click menu.
	// 4, Increase/decrease window size.
	// 5, Resotre to system/default font.
	// 6, Theme has been changed.
	// 7, Using Dir/Cmdline on pagename.
	win_data->lost_focuse = FALSE;

	//  1    : Updating Page Name.
	//  2,  4: Showing/Hiding tab bar, Only run window_size_request() once. 
	//  8    : Changing Themes.
	// 16, 32: Resing Window, Only run window_size_request() once.
//	win_data->keep_vtebox_size = 0;
//	win_data->query_coding = 0;

//	win_data->restore_font_name = NULL;
//	win_data->temp_str = NULL;
}

void init_user_keys(struct Window *win_data)
{
	// g_debug("Get win_data = %d when init user keys!", win_data);

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
	// past the text in clipboard
	win_data->user_keys[26].value = g_strdup("Ctrl V");
	// Increase the font size
	win_data->user_keys[27].value = g_strdup("Ctrl equal");
	// decrease the font size
	win_data->user_keys[28].value = g_strdup("Ctrl minus");
	// reset the font size
	win_data->user_keys[29].value = g_strdup("Ctrl Return");
}

void init_function_keys()
{

	// for disable/enable the function keys
	system_keys[0].name = "disable_function_key";
	system_keys[0].comment = "# Disable/Enable function keys for temporary.";
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
	system_keys[12].name = "swith_to_tab_1";
	system_keys[12].comment = "# Switch to 1st tab directly.";
	system_keys[13].name = "swith_to_tab_2";
	system_keys[13].comment = "# Switch to 2nd tab directly.";
	system_keys[14].name = "swith_to_tab_3";
	system_keys[14].comment = "# Switch to 3rd tab directly.";
	system_keys[15].name = "swith_to_tab_4";
	system_keys[15].comment = "# Switch to 4th tab directly.";
	system_keys[16].name = "swith_to_tab_5";
	system_keys[16].comment = "# Switch to 5th tab directly.";
	system_keys[17].name = "swith_to_tab_6";
	system_keys[17].comment = "# Switch to 6th tab directly.";
	system_keys[18].name = "swith_to_tab_7";
	system_keys[18].comment = "# Switch to 7th tab directly.";
	system_keys[19].name = "swith_to_tab_8";
	system_keys[19].comment = "# Switch to 8th tab directly.";
	system_keys[20].name = "swith_to_tab_9";
	system_keys[20].comment = "# Switch to 9th tab directly.";
	system_keys[21].name = "swith_to_tab_10";
	system_keys[21].comment = "# Switch to 10th tab directly.";
	system_keys[22].name = "swith_to_tab_11";
	system_keys[22].comment = "# Switch to 11th tab directly.";
	system_keys[23].name = "swith_to_tab_12";
	system_keys[23].comment = "# Switch to 12th tab directly.";
	// select all
	system_keys[24].name = "select_all";
	system_keys[24].comment = "# Select all the text in the Vte Terminal box.";
	// copy the text to clipboard
	system_keys[25].name = "copy_clipboard";
	system_keys[25].comment = "# Copy the text to clipboard.";
	// past the text in clipboard
	system_keys[26].name = "past_clipboard";
	system_keys[26].comment = "# Past the text in clipboard.";
	// Increase the font size
	system_keys[27].name = "increase_font_size";
	system_keys[27].comment = "# Increase the font size of current tab.";
	// decrease the font size
	system_keys[28].name = "decrease_font_size";
	system_keys[28].comment = "# Decrease the font size of current tab.";
	// reset the font size
	system_keys[29].name = "reset_font_size";
	system_keys[29].comment = "# Reset the font of current tab to original size.";
}

void init_mod_keys()
{
	modkeys[0].name = "Ctrl";
	modkeys[0].mod = GDK_CONTROL_MASK;
	modkeys[1].name = "Shift";
	modkeys[1].mod = GDK_SHIFT_MASK;
	modkeys[2].name = "Alt";
	modkeys[2].mod = GDK_MOD1_MASK;
	modkeys[3].name = "Mod1";
	modkeys[3].mod = GDK_MOD1_MASK;
	// Mod2 = NumLock. We won't check it.
	modkeys[4].name = "Mod3";
	modkeys[4].mod = GDK_MOD3_MASK;
	modkeys[5].name = "Mod4";
	modkeys[5].mod = GDK_MOD4_MASK;
	modkeys[6].name = "Mod5";
	modkeys[6].mod = GDK_MOD5_MASK;
}

// get user settings from profile.
void get_user_settings(GtkWidget *window, struct Window *win_data)
{
	// g_debug("Get win_data = %d when get user settings!", win_data);

	init_window_parameter(win_data);
	init_user_keys(win_data);
	if (total_window==1)
	{
		init_function_keys(win_data);
		init_mod_keys();
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
			win_data->page_name = check_string_value(keyfile, "main", "page_name", win_data->page_name, FALSE);
			
			win_data->page_names = check_string_value(keyfile, "main", "page_names",
								  win_data->page_names, TRUE);
			win_data->splited_page_names = g_strsplit_set(win_data->page_names, " ", 0);
			
			win_data->reuse_page_names = check_boolean_value(keyfile, "main", "reuse_page_names",
									 win_data->reuse_page_names);
			
			win_data->page_shows_current_cmdline = check_boolean_value(keyfile, "main",
				 "page_shows_current_cmdline", win_data->page_shows_current_cmdline);
			win_data->page_shows_current_dir = check_boolean_value(keyfile, "main", "page_shows_current_dir",
									 win_data->page_shows_current_dir);
			
			win_data->page_shows_encoding = check_boolean_value(keyfile, "main",
						"win_data->page_shows_encoding", win_data->page_shows_encoding);

			win_data->window_shows_current_page = check_boolean_value(keyfile, "main",
						"window_shows_current_page", win_data->window_shows_current_page);

			win_data->use_color_page = check_boolean_value( keyfile, "main", "use_color_page",
									win_data->use_color_page);

			win_data->check_root_privileges = check_boolean_value(keyfile, "main", "check_root_privileges",
								    win_data->check_root_privileges);

			win_data->bold_current_page_name = check_boolean_value(keyfile, "main", "bold_current_page_name",
								     win_data->bold_current_page_name);

			win_data->bold_action_page_name = check_boolean_value(keyfile, "main", "bold_action_page_name",
								    win_data->bold_action_page_name);

			win_data->page_cmdline_color = check_string_value(keyfile, "main", "page_cmdline_color",
								win_data->page_cmdline_color, FALSE);

			win_data->page_dir_color = check_string_value(keyfile, "main", "page_dir_color",
								      win_data->page_dir_color, FALSE);

			win_data->page_custom_color = check_string_value( keyfile, "main", "page_custom_color",
								win_data->page_custom_color, FALSE);

			win_data->page_normal_color = check_string_value( keyfile, "main", "page_normal_color",
								win_data->page_normal_color, FALSE);

			win_data->page_root_color = check_string_value( keyfile, "main", "page_root_color",
							      win_data->page_root_color, FALSE);

			win_data->page_shows_number = check_boolean_value(keyfile, "main", "page_shows_number",
									  win_data->page_shows_number);

			win_data->page_width = check_integer_value( keyfile, "main", "page_width",
								    win_data->page_width, FALSE, FALSE);
			win_data->fixed_page_width = check_boolean_value(keyfile, "main", "fixed_page_width",
									 win_data->fixed_page_width);

			win_data->show_color_selection_menu = check_boolean_value(keyfile, "main",
						"show_color_selection_menu", win_data->show_color_selection_menu);

			win_data->foreground_color = check_string_value(keyfile, "main", "foreground_color",
									win_data->foreground_color, FALSE);

			win_data->background_color = check_string_value(keyfile, "main", "background_color",
									win_data->background_color, FALSE);

			win_data->default_font_name = check_string_value(keyfile, "main", "font_name",
									 win_data->default_font_name, FALSE);

			PangoFontDescription *font_desc = pango_font_description_from_string(win_data->default_font_name);
			if ((pango_font_description_get_size(font_desc)/PANGO_SCALE)==0)
			{
				g_warning("Invalid font name: %s", win_data->default_font_name);
				g_free(win_data->default_font_name);
				win_data->default_font_name = g_strdup(win_data->system_font_name);
			}

			win_data->show_resize_menu = check_boolean_value(keyfile, "main", "show_resize_menu",
									 win_data->show_resize_menu);

			win_data->default_column = check_integer_value(keyfile, "main", "column",
									win_data->default_column, FALSE, FALSE);

			win_data->default_row = check_integer_value(keyfile, "main", "row", win_data->default_row,
								    FALSE, FALSE);

			win_data->show_transparent_menu = check_boolean_value(keyfile, "main", "show_transparent_menu", 
								    win_data->show_transparent_menu);

#ifdef ENABLE_RGBA
			win_data->use_rgba = check_integer_value(keyfile, "main", "use_rgba", win_data->use_rgba,
								 FALSE, TRUE);
			win_data->original_use_rgba = win_data->use_rgba;

			win_data->transparent_window = check_integer_value(keyfile, "main", "transparent_window",
								 win_data->transparent_window, FALSE, TRUE);

			win_data->window_opacity = g_key_file_get_double(keyfile, "main", "window_opacity", NULL);
#endif

			win_data->transparent_background = check_integer_value(keyfile, "main", "transparent_background", 
								     win_data->transparent_background, FALSE, TRUE);

			win_data->background_saturation = g_key_file_get_double(keyfile, "main", "background_saturation",
										NULL);

			win_data->word_chars = check_string_value(keyfile, "main", "word_chars", win_data->word_chars,
								  TRUE);
			
			win_data->scrollback_lines = check_integer_value( keyfile, "main", "scrollback_lines",
									  win_data->scrollback_lines, FALSE, TRUE);

			win_data->show_input_method_menu = check_boolean_value(keyfile, "main", "show_input_method_menu", 
								     win_data->show_input_method_menu);

			win_data->show_get_function_key_menu = check_boolean_value(keyfile, "main",
							"show_get_function_key_menu", win_data->show_get_function_key_menu);

			win_data->locales_list = check_string_value( keyfile, "main", "locales_list",
								     win_data->locales_list, TRUE);
			// g_debug("Got locales_list = %s from user's profile!\n", value);

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
					// g_debug("Got %s key = %s form config file.\n", pagekeys[i].name, value);
					if ( accelerator_parse(system_keys[i].name, value,
							  &(win_data->user_keys[i].key), &(win_data->user_keys[i].mods)))
						win_data->user_keys[i].value = value;
					else
					{
						g_warning("%s = %s is not a valid key! Please check!\n",
							system_keys[i].name, value);
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
		}
		else
		{
			g_warning("the config file %s is invalid: %s\n", win_data->profile, error->message);
			g_clear_error (&error);
		}
	}
	else
		g_warning("can NOT find any profile. Use program defaults.\n");
	
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
	// some defaults
	// g_debug("Got locales_list = '%s' ( %d bytes)",locales_list, strlen(locales_list));
	if (strlen(win_data->locales_list))
	{
		gchar *full_locales_list = NULL;
		full_locales_list = g_strconcat("  ", win_data->locales_list, NULL);
		win_data->supported_locales = g_strsplit_set(full_locales_list, " ,", 0);
		g_free(win_data->supported_locales[0]);
		// win_data->supported_locales[0] will never be freed. so we can assign a const value here.
		win_data->supported_locales[0] = _("System Default");
		g_free(full_locales_list);
	}

	if (win_data->splited_page_names==NULL)
		win_data->splited_page_names = g_strsplit_set("", " ", 0);

	gdk_color_parse(win_data->foreground_color, &(win_data->fg_color));
	gdk_color_parse(win_data->background_color, &(win_data->bg_color));

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
	win_data->default_locale = get_default_locale();

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
		g_free(value);
	}
	else
		setting = default_value;
	
	// g_debug("Got key value \"%s = %d\"", key, setting);
	return setting;
}

gint check_integer_value(GKeyFile *keyfile, const gchar *group_name,
			 const gchar *key, const gint default_value, gboolean enable_empty, gboolean enable_zero)
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
		g_free(value);
	}
	else
		setting = default_value;
	
	if ( setting==0 && (! enable_zero))
		setting = default_value;

	// g_debug("Got key value \"%s = %d\"", key, setting);
	return setting;
}

// The returned string should be freed when no longer needed.
gchar *check_string_value(GKeyFile *keyfile, const gchar *group_name,
			const gchar *key, gchar *default_value, gboolean enable_empty)
{
	gchar *setting = g_key_file_get_value(keyfile, group_name, key, NULL);
	if (setting)
		if (! strlen(setting))
			if (! enable_empty)
			{
				g_free(setting);
				setting = NULL;
			}
	
	if (!setting)
		setting = g_strdup(default_value);
	
	g_free(default_value);
	
	// g_debug("Got key value \"%s = %s\"", key, setting);
	return setting;
}

// get default locale from environ
gchar *get_default_locale()
{
	// the idea of these codes is came from GCIN. Pay homage to Edward Liu!
	char *LC_CTYPE = (char*)g_getenv("LC_CTYPE");
	char *LC_ALL = (char*)g_getenv("LC_ALL");
	char *LANG = (char*)g_getenv("LANG");

	if ( (!LC_CTYPE) && LANG)
		LC_CTYPE = LANG;

	if (LC_ALL)
		LC_CTYPE = LC_ALL;

	if (!LC_CTYPE)
		LC_CTYPE = "UTF-8";

	return g_strdup(LC_CTYPE);
}

// to init a new page
void init_new_page(GtkWidget *window, struct Window *win_data, GtkWidget *vtebox, char* font_name, gint column, gint row, gint run_once)
{
	// g_debug("Get win_data = %d when init new page!", win_data);

	// set font
	vte_terminal_set_font_from_string(VTE_TERMINAL(vtebox), font_name);
	//g_debug("Got font size from %s: %d\n", font_name, pango_font_description_get_size (
	//	  pango_font_description_from_string(font_name))/PANGO_SCALE);

	// set terminal size
	// g_debug("Set the vtebox size to: %dx%d", column, row);
	vte_terminal_set_size(VTE_TERMINAL(vtebox), column, row);

	// set font/background colors
	vte_terminal_set_default_colors(VTE_TERMINAL(vtebox));
	vte_terminal_set_color_foreground(VTE_TERMINAL(vtebox), &(win_data->fg_color));
	vte_terminal_set_color_background(VTE_TERMINAL(vtebox), &(win_data->bg_color));

	// set transparent
	set_background_saturation(NULL, 0, win_data->background_saturation, vtebox);

	// other settings
	vte_terminal_set_word_chars(VTE_TERMINAL(vtebox), win_data->word_chars);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(vtebox), win_data->scrollback_lines);

	// some fixed parameter
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(vtebox), FALSE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(vtebox), TRUE);
	vte_terminal_set_backspace_binding (VTE_TERMINAL(vtebox), VTE_ERASE_ASCII_DELETE);
	// vte_terminal_set_delete_binding (VTE_TERMINAL(vtebox), VTE_ERASE_ASCII_DELETE);

	if (run_once)
	{
		win_data->update_hints = 1;
		// update hints and make window unresizable
		window_resizable(window, vtebox, run_once, 1);
	}
}

gboolean set_background_saturation(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vtebox)
{
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
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
			vte_terminal_set_opacity(VTE_TERMINAL(vtebox), (1-value)*65535);
		else
			vte_terminal_set_opacity(VTE_TERMINAL(vtebox), 65535);
	}
	else
#endif
		vte_terminal_set_background_transparent(VTE_TERMINAL(vtebox), win_data->transparent_background);

	if (win_data->transparent_background)
		vte_terminal_set_background_saturation( VTE_TERMINAL(vtebox), value);
	else
		vte_terminal_set_background_saturation( VTE_TERMINAL(vtebox), 0);

	return FALSE;
}

#ifdef ENABLE_RGBA
gboolean set_window_opacity (GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *window)
{
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
void window_resizable(GtkWidget *window, GtkWidget *vtebox, gint run_once, gint minsize)
{
	if (vtebox==NULL) return;

	GdkGeometry hints = {0};
	// g_debug("Trying to get padding...\n");
	vte_terminal_get_padding (VTE_TERMINAL(vtebox), &hints.base_width, &hints.base_height);
	
	if (run_once)
	{
		// Only run once when init window or fonts had been changed.
		if (run_once==1)
		{
			hints.width_inc = vte_terminal_get_char_width(VTE_TERMINAL(vtebox));
			hints.height_inc = vte_terminal_get_char_height(VTE_TERMINAL(vtebox));
		}
		if (run_once==2)
		{
			hints.width_inc = 1;
			hints.height_inc = 1;
		}
		// g_debug("hints.base_width = %d, hints.base_height = %d\n",
		//	    hints.base_width, hints.base_height);
	}

	// minsize = -1: the size of vtebox can NOT be changed.
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

	// g_debug("Tring to set geometry to %d, and run_once = %d\n", vtebox, run_once);
	gtk_window_set_geometry_hints (GTK_WINDOW (window), GTK_WIDGET (vtebox), &hints,
					GDK_HINT_RESIZE_INC | GDK_HINT_MIN_SIZE | GDK_HINT_BASE_SIZE);

	// g_debug("current # %d vtebox size when hinting = %d x %d, KeepSize = %d\n",
	//			gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)),
	//			vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//			vte_terminal_get_row_count(VTE_TERMINAL(vtebox)),
	//			keepsize);
}

// return TRUE if 'key_name' is a valid key or NULL; or it will return FALSE.
gboolean accelerator_parse (const gchar *key_name, const gchar *key_value, guint *key, guint *mods)
{
	// key_value example: "Ctrl+Shift Home"

	gchar **values;
	gchar **functions;
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
				g_critical("no Function Key found in [%s] (%s)!\n", key_name, key_value);
				return FALSE;
			}
		
			if (values[1])
			{
				// got the key ("Home" for example)
				tempkey = gdk_keyval_from_name(values[1]);
				// g_debug("key = %d\n", tempkey);
				if (tempkey == GDK_VoidSymbol)
				{
					// not a valid key
					g_critical("%s in [%s] (%s) is not a valid key!\n",
						values[1], key_name, key_value);
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
					*mods=(tempmods|GDK_LOCK_MASK|GDK_MOD2_MASK);
					return TRUE;
				}
			}
			else
			{
				g_critical("no Work Key found in [%s] (%s)!\n", key_name, key_value);
				return FALSE;
			}
		}
		else
		{
			// not a valid key, no space in key_name
			g_critical("%s in [%s] (%s) is not a valid key. There is no SPACE in it.\n",
					values[1], key_name, key_value);
			return FALSE;
		}
	}
	else
	{
		// NULL
		// g_warning("We Got a NULL Key (%s)!\n", key_value);
		*key=GDK_VoidSymbol;
		*mods=-1;
		return TRUE;
	}
}

GString *save_user_settings(GtkWidget *widget, GtkWidget *vtebox)
{
	struct Page *page_data = NULL;
	struct Window *win_data = NULL;
	GError *error = NULL;
	const gchar *profile_dir;
	gchar *profile = NULL;
	GString *contents = NULL;
	GKeyFile *keyfile = g_key_file_new();
	gint i;

	if (vtebox)
	{
		page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
		win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	}
	else
	{
		win_data = g_new0(struct Window, 1);

		init_window_parameter(win_data);
		init_user_keys(win_data);
		init_function_keys(win_data);
	}


	if (win_data->use_custom_profile)
		profile = win_data->profile;
	else if (vtebox)
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
	if (vtebox)
	{
		g_string_append_printf(contents,"# The default font name of vte terminal.\n"
						"font_name = %s\n\n", page_data->font_name);
		g_string_append_printf(contents,"# The default column of vte terminal.\n"
						"column = %ld\n\n",
				       vte_terminal_get_column_count(VTE_TERMINAL(vtebox)));
		g_string_append_printf(contents,"# The default row of vte terminal.\n"
						"row = %ld\n\n", vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));
		g_string_append_printf(contents,"# Use true opacity. Left it blank will enable it automatically\n"
						"# if the window manager were composited.\n");
		if (win_data->original_use_rgba == 2)
			g_string_append_printf(contents, "use_rgba =\n\n");
		else
			g_string_append_printf(contents, "use_rgba = %d\n\n", win_data->original_use_rgba);
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
	g_string_append_printf(contents,"# Shows [Transparent Background], [Background Saturation]\n"
					"# [Transparent Window] and [Window Opacity] on right click menu.\n"
					"show_transparent_menu = %d\n\n", win_data->show_transparent_menu);
	g_string_append_printf(contents,"# The max character width of page name.\n"
					"page_width = %d\n\n", win_data->page_width);
	g_string_append_printf(contents,"# The page width will always use the max character width.\n"
					"fixed_page_width = %d\n\n", win_data->fixed_page_width);
	g_string_append_printf(contents,"# The page name used for a new page.\n"
					"page_name = %s\n\n", win_data->page_name);
	g_string_append_printf(contents,"# The page names list used for new pages, separate with space.\n"
					"page_names = %s\n\n", win_data->page_names);
	g_string_append_printf(contents,"# Reuse the page name in the page names list.\n"
					"reuse_page_names = %d\n\n", win_data->reuse_page_names);
	g_string_append_printf(contents,"# Shows a (number no) on the page name.\n"
					"page_shows_number = %d\n\n", win_data->page_shows_number);
	g_string_append_printf(contents,"# Shows the foreground running command on the page name.\n"
					"page_shows_current_cmdline = %d\n\n", win_data->page_shows_current_cmdline);
	g_string_append_printf(contents,"# Shows current directory on the page name.\n"
					"page_shows_current_dir = %d\n\n", win_data->page_shows_current_dir);
	g_string_append_printf(contents,"# Shows current encoding on the page name.\n"
					"page_shows_encoding = %d\n\n", win_data->page_shows_encoding);
	g_string_append_printf(contents,"# Shows the page name of current page on window title.\n"
					"window_shows_current_page = %d\n\n", win_data->window_shows_current_page);
	g_string_append_printf(contents,"# Use colorful text on page.\n"
					"use_color_page = %d\n\n", win_data->use_color_page);
	g_string_append_printf(contents,"# The color used for showing cmdline on page name.\n"
					"# You may use black or #000000 here.\n"
					"page_cmdline_color = %s\n\n", win_data->page_cmdline_color);
	g_string_append_printf(contents,"# The color used for showing current directory on page name.\n"
					"# You may use black or #000000 here.\n"
					"page_dir_color = %s\n\n", win_data->page_dir_color);
	g_string_append_printf(contents,"# The color used for showing custom name on page name.\n"
					"# You may use black or #000000 here.\n"
					"page_custom_color = %s\n\n", win_data->page_custom_color);
	g_string_append_printf(contents,"# The color used for showing normal name on page name.\n"
					"# You may use black or #000000 here.\n"
					"page_normal_color = %s\n\n", win_data->page_normal_color);
	g_string_append_printf(contents,"# The color used for showing root privileges cmdline on page name.\n"
					"# You may use black or #000000 here.\n"
					"page_root_color = %s\n\n", win_data->page_root_color);
	g_string_append_printf(contents,"# Shows [Change the foreground color]\n"
					"# and [Change the background color] on right click menu.\n"
					"show_color_selection_menu = %d\n\n", win_data->show_color_selection_menu);
	g_string_append_printf(contents,"# The normal text color used in vte terminal.\n"
					"# You may use black or #000000 here.\n"
					"foreground_color = %s\n\n", win_data->foreground_color);
	g_string_append_printf(contents,"# The background color used in vte terminal.\n"
					"# You may use black or #000000 here.\n"
					"background_color = %s\n\n", win_data->background_color);
	g_string_append_printf(contents,"# Check if the running command is root privileges.\n"
					"check_root_privileges = %d\n\n", win_data->check_root_privileges);
	g_string_append_printf(contents,"# Bold the text of current page name.\n"
					"bold_current_page_name = %d\n\n", win_data->bold_current_page_name);
	g_string_append_printf(contents,"# Bold the text of action page name.\n"
					"bold_action_page_name = %d\n\n", win_data->bold_action_page_name);
	g_string_append_printf(contents,"# Shows [Increase window size], [Decrease window size],\n"
					"# [Reset to default font/size] and [Reset to system font/size]\n"
					"# on right click menu.\n"
					"show_resize_menu = %d\n\n", win_data->show_resize_menu);
	g_string_append_printf(contents,"# When user double clicks on a text, which character will be selected.\n"
					"word_chars = %s\n\n", win_data->word_chars);
	g_string_append_printf(contents,"# The lines of scrollback history.\n"
					"scrollback_lines = %d\n\n", win_data->scrollback_lines);
	g_string_append_printf(contents,"# Shows input method menu on right cilck menu.\n"
					"show_input_method_menu = %d\n\n", win_data->show_input_method_menu);
	g_string_append_printf(contents,"# Shows get function key menu on right cilck menu.\n"
					"show_get_function_key_menu = %d\n\n", win_data->show_get_function_key_menu);
	g_string_append_printf(contents,"# The locales list on right click menu.\n"
					"# You may use zh_TW or zh_TW.Big5 here.\n"
					"# Left it blank will disable locale select menu items.\n"
					"locales_list = %s\n\n", win_data->locales_list);
	g_string_append_printf(contents,"\n");
	g_string_append_printf(contents,"[key]\n\n");
	
	for (i=0; i<KEYS; i++)
		g_string_append_printf( contents,"%s\n%s = %s\n\n",
					system_keys[i].comment, system_keys[i].name, win_data->user_keys[i].value);

	if (!vtebox)
		return contents;
	
	// g_debug("\n%s\n", contents->str);

	if (!g_file_set_contents(profile, contents->str, -1, &error))
	{
		win_data->temp_str = error->message;
		dialog (NULL, 6);
	}
	//	g_debug("Error while writing profile '%s': %s", profile, error->message);
	
	free_user_settings_data(error, profile, contents, keyfile, win_data->use_custom_profile);
	return NULL;
}

void free_user_settings_data(GError *error, gchar *profile, GString* contents, GKeyFile *keyfile, gboolean use_custom_profile)
{
	if (error!=NULL)
		g_clear_error(&error);
	if (!use_custom_profile)
		g_free(profile);
	g_string_free(contents, TRUE);
	g_key_file_free(keyfile);
}
