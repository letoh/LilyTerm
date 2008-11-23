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

// default settings
gchar *page_name = "Terminal";
gchar *page_names = "";
gchar **splited_page_names = NULL;
gint page_names_no = 0;
gboolean reuse_page_names = TRUE;
gboolean page_number = TRUE;
#if defined(__linux__)
gboolean page_shows_current_cmdline = TRUE;
#else
gboolean page_shows_current_cmdline = FALSE;
#endif
// gboolean page_shows_current_dir = FALSE;
gboolean window_shows_current_page = TRUE;
gboolean show_color_selection_menu = TRUE;
gchar *foreground_color = "white";
gchar *background_color = "black";
GdkColor fg_color;
GdkColor bg_color;
gboolean show_resize_menu = TRUE;
gchar *default_font_name = "Monospace 12";
gchar *system_font_name = "Monospace 12";
gint default_column = 80;
gint default_row = 24;
gint system_column = 80;
gint system_row = 24;
gboolean show_transparent_menu = TRUE;
// 0: do NOT use rgba
// 1: force to use rgba
// 2: decide by gdk_screen_is_composited()
gint use_rgba = 2;
gint original_use_rgba = 2;
#ifdef ENABLE_RGBA
gint transparent_window = 2;
gdouble window_opacity = 0.05;
#endif
gint transparent_background = 2;
gdouble background_saturation = 0.15;
gchar *word_chars = "-A-Za-z0-9_$.+!*(),;:@&=?/~#%[]";
gint scrollback_lines = 1024;

gboolean show_input_method_menu = FALSE;
gchar *default_locale = NULL;
gchar *locales_list = "EUC-JP GB2312 Big5";
gchar **supported_locales = NULL;

GdkGeometry hints = {0};

extern gchar *profile;
extern GtkWidget *window;

// the default key value. the default value is initialed in get_user_settings().
struct KeyValue pagekeys[KEYS] = {{0}};
struct ModKey modkeys[MOD] = {{0}};

GError *error = NULL;

void init_pagekeys()
{
	// for disable/enable the function keys
	pagekeys[0].name = "disable_function_key";
	pagekeys[0].value = "Ctrl grave";
	// New Page
	pagekeys[1].name = "new_tab_key";
	pagekeys[1].value = "Ctrl T";
	// Close Page
	pagekeys[2].name = "close_tab_key";
	pagekeys[2].value = "Ctrl W";
	// Edit Page Name
	pagekeys[3].name = "edit_label_key";
	pagekeys[3].value = "Ctrl E";
	// Pre Page
	pagekeys[4].name = "prev_tab_key";
	pagekeys[4].value = "Ctrl Page_Up";
	// Next Page
	pagekeys[5].name = "next_tab_key";
	pagekeys[5].value = "Ctrl Page_Down";
	// First Page
	pagekeys[6].name = "first_tab_key";
	pagekeys[6].value = "Ctrl Home";
	// Last Page
	pagekeys[7].name = "last_tab_key";
	pagekeys[7].value = "Ctrl End";
	// Move the label forward
	pagekeys[8].name = "move_tab_forward";
	pagekeys[8].value = "Ctrl Left";
	// Move the label backward
	pagekeys[9].name = "move_tab_backward";
	pagekeys[9].value = "Ctrl Right";
	// Move the label to first
	pagekeys[10].name = "move_tab_first";
	pagekeys[10].value = "Ctrl Up";
	// Move the label to last
	pagekeys[11].name = "move_tab_last";
	pagekeys[11].value = "Ctrl Down";
	// switch to #%d page
	pagekeys[12].name = "swith_to_tab_1";
	pagekeys[12].value = "Ctrl F1";
	pagekeys[13].name = "swith_to_tab_2";
	pagekeys[13].value = "Ctrl F2";
	pagekeys[14].name = "swith_to_tab_3";
	pagekeys[14].value = "Ctrl F3";
	pagekeys[15].name = "swith_to_tab_4";
	pagekeys[15].value = "Ctrl F4";
	pagekeys[16].name = "swith_to_tab_5";
	pagekeys[16].value = "Ctrl F5";
	pagekeys[17].name = "swith_to_tab_6";
	pagekeys[17].value = "Ctrl F6";
	pagekeys[18].name = "swith_to_tab_7";
	pagekeys[18].value = "Ctrl F7";
	pagekeys[19].name = "swith_to_tab_8";
	pagekeys[19].value = "Ctrl F8";
	pagekeys[20].name = "swith_to_tab_9";
	pagekeys[20].value = "Ctrl F9";
	pagekeys[21].name = "swith_to_tab_10";
	pagekeys[21].value = "Ctrl F10";
	pagekeys[22].name = "swith_to_tab_11";
	pagekeys[22].value = "Ctrl F11";
	pagekeys[23].name = "swith_to_tab_12";
	pagekeys[23].value = "Ctrl F12";
	// select all
	pagekeys[24].name = "select_all";
	pagekeys[24].value = "Ctrl O";
	// copy the text to clipboard
	pagekeys[25].name = "copy_clipboard";
	pagekeys[25].value = "Ctrl X";
	// past the text in clipboard
	pagekeys[26].name = "past_clipboard";
	pagekeys[26].value = "Ctrl V";
	// Increase the font size
	pagekeys[27].name = "increase_font_size";
	pagekeys[27].value = "Ctrl equal";
	// decrease the font size
	pagekeys[28].name = "decrease_font_size";
	pagekeys[28].value = "Ctrl minus";
	// reset the font size
	pagekeys[29].name = "reset_font_size";
	pagekeys[29].value = "Ctrl Return";
}

// get user settings from profile.
void get_user_settings()
{
	init_pagekeys();

	gint i;

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

	// got the rc file
	GKeyFile *keyfile = g_key_file_new();
	// gchar *profile = g_strdup_printf("%s/%s", g_get_user_config_dir(), RCFILE);
	// g_debug ("Using the profile: %s \n", profile);

	if (profile != NULL)
	{
		if (g_key_file_load_from_file(keyfile, profile, G_KEY_FILE_NONE, &error))
		{
			page_name = check_string_value(keyfile, "main", "page_name", page_name, FALSE);
			
			page_names = check_string_value(keyfile, "main", "page_names", page_names, TRUE);
			splited_page_names = g_strsplit_set(page_names, " ", 0);
			
			reuse_page_names = check_boolean_value(keyfile, "main", "reuse_page_names", reuse_page_names);
			
			page_shows_current_cmdline = check_boolean_value(keyfile, "main", "page_shows_current_cmdline",
									 page_shows_current_cmdline);
			
			window_shows_current_page = check_boolean_value(keyfile, "main", "window_shows_current_page",
									window_shows_current_page);
			
			page_number = check_boolean_value(keyfile, "main", "page_number", page_number);

			show_color_selection_menu = check_boolean_value(keyfile, "main", "show_color_selection_menu", 
									show_color_selection_menu);

			foreground_color = check_string_value(keyfile, "main", "foreground_color", foreground_color, FALSE);

			background_color = check_string_value(keyfile, "main", "background_color", background_color, FALSE);

			default_font_name = check_string_value(keyfile, "main", "font_name", default_font_name, FALSE);
			PangoFontDescription *font_desc = pango_font_description_from_string(default_font_name);
			if ((pango_font_description_get_size(font_desc)/PANGO_SCALE)==0)
			{
				g_warning("Invalid font name: %s", default_font_name);
				g_free(default_font_name);
				default_font_name = g_strdup(system_font_name);
			}

			show_resize_menu = check_boolean_value(keyfile, "main", "show_resize_menu", show_resize_menu);

			default_column = check_integer_value(keyfile, "main", "column", default_column, FALSE, FALSE);

			default_row = check_integer_value(keyfile, "main", "row", default_row, FALSE, FALSE);

			show_transparent_menu = check_boolean_value(keyfile, "main", "show_transparent_menu", 
								    show_transparent_menu);

#ifdef ENABLE_RGBA
			use_rgba = check_integer_value(keyfile, "main", "use_rgba", use_rgba, FALSE, TRUE);
			original_use_rgba = use_rgba;

			transparent_window = check_integer_value(keyfile, "main", "transparent_window",
								 transparent_window, FALSE, TRUE);

			window_opacity = g_key_file_get_double(keyfile, "main", "window_opacity", NULL);
#endif

			transparent_background = check_integer_value(keyfile, "main", "transparent_background", 
								     transparent_background, FALSE, TRUE);

			background_saturation = g_key_file_get_double(keyfile, "main", "background_saturation", NULL);

			word_chars = check_string_value(keyfile, "main", "word_chars", word_chars, TRUE);
			
			scrollback_lines = check_integer_value( keyfile, "main",
								"scrollback_lines", scrollback_lines, FALSE, TRUE);

			show_input_method_menu = check_boolean_value(keyfile, "main", "show_input_method_menu", 
								     show_input_method_menu);

			locales_list = check_string_value(keyfile, "main", "locales_list", locales_list, TRUE);
			// g_debug("Got locales_list = %s from user's profile!\n", value);

			// g_debug("Key Value: Shift=%x, NumLock=%x, Control=%x, Mod1=%x,"
			//		"Mod2=%x, Mod3=%x, Mod4=%x, Mod5=%x\n",
			//		GDK_SHIFT_MASK, GDK_LOCK_MASK, GDK_CONTROL_MASK, GDK_MOD1_MASK, 
			//		GDK_MOD2_MASK, GDK_MOD3_MASK, GDK_MOD4_MASK, GDK_MOD5_MASK);
			gchar *value = NULL;
			for (i=0; i<KEYS; i++)
			{
				// g_debug("Checking %s key...", pagekeys[i].name);
				value = g_key_file_get_value(keyfile, "key", pagekeys[i].name, NULL);
				if (value)
				{
					// g_debug("Got %s key = %s form config file.\n", pagekeys[i].name, value);
					if ( accelerator_parse(pagekeys[i].name, value,
							  &(pagekeys[i].key), &(pagekeys[i].mods)))
						pagekeys[i].value = value;
					else
					{
						g_warning("%s = %s is not a valid key! Please check!\n",
							pagekeys[i].name, value);
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
			g_warning("the config file %s is invalid: %s\n", profile, error->message);
			g_clear_error (&error);
			strdup_settings();
		}
	}
	else
	{
		g_warning("can NOT find any profile. Use program defaults.\n");
		strdup_settings();
	}
	
 	for (i=0; i<KEYS; i++)
 	{
		if ( ! pagekeys[i].key)
		{
 			if ( ! accelerator_parse(pagekeys[i].name, pagekeys[i].value,
					      &(pagekeys[i].key), &(pagekeys[i].mods)))
				g_critical("%s = %s is not a valid key! Please report bug!\n",
					 pagekeys[i].name, pagekeys[i].value);
 			// g_debug("Use default key %s, %x(%s), mods = %x.\n", pagekeys[i].name,
			//		pagekeys[i].key, gdk_keyval_name(pagekeys[i].key), pagekeys[i].mods);
		}
		// g_debug("* We'll use the key for %s: %x(%s), mods = %x.\n", pagekeys[i].name,
		//		pagekeys[i].key, gdk_keyval_name(pagekeys[i].key), pagekeys[i].mods);

 	}
	// some defaults
	// g_debug("Got locales_list = '%s' ( %d bytes)",locales_list, strlen(locales_list));
	supported_locales = g_strsplit_set(locales_list, " ,", 0);
	if (splited_page_names==NULL)
		splited_page_names = g_strsplit_set("", " ", 0);
	
	gdk_color_parse(foreground_color, &fg_color);
	gdk_color_parse(background_color, &bg_color);

#ifdef ENABLE_RGBA
	// If the system supports rgba, enable transparent background by default
	if (use_rgba)
		init_rgba();
	if (transparent_window==2)
		transparent_window = use_rgba;
	if (transparent_background==2)
		transparent_background = use_rgba;

	set_window_opacity (NULL, 0, window_opacity, NULL);
#endif

	// get the default locale from environment
	get_default_locale();

	g_key_file_free(keyfile);
	g_free(profile);
}

void strdup_settings()
{
	page_name = g_strdup(page_name);
	// page_names = g_strdup(page_names);
	foreground_color = g_strdup(foreground_color);
	background_color = g_strdup(background_color);
	// default_font_name = g_strdup(default_font_name);
	// word_chars = g_strdup(word_chars);
	// locales_list = g_strdup(locales_list);
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
			const gchar *key, const gchar *default_value, gboolean enable_empty)
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
	
	// g_debug("Got key value \"%s = %s\"", key, setting);
	return setting;
}

// get default locale from environ
void get_default_locale()
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

	default_locale = g_strdup(LC_CTYPE);
}

// to init a new page
void init_new_page(GtkWidget *vtebox, char* font_name, gint column, gint row, gint run_once)
{
	// set font
	vte_terminal_set_font_from_string(VTE_TERMINAL(vtebox), font_name);
	//g_debug("Got font size from %s: %d\n", font_name, pango_font_description_get_size (
	//		pango_font_description_from_string(font_name))/PANGO_SCALE);

	// set font/background colors
	vte_terminal_set_default_colors(VTE_TERMINAL(vtebox));
	vte_terminal_set_color_foreground(VTE_TERMINAL(vtebox), &fg_color);
	vte_terminal_set_color_background(VTE_TERMINAL(vtebox), &bg_color);

	if (run_once)
		// update hints and make window unresizable
		window_resizable(vtebox, run_once, -1);

	// set terminal size
	// g_debug("Set the vtebox size to: %dx%d", column, row);
	vte_terminal_set_size(VTE_TERMINAL(vtebox), column, row);

	// set transparent
	set_background_saturation (NULL, 0, background_saturation, vtebox);

	// other settings
	vte_terminal_set_word_chars(VTE_TERMINAL(vtebox), word_chars);
	vte_terminal_set_scrollback_lines(VTE_TERMINAL(vtebox), scrollback_lines);

	// some fixed parameter
	vte_terminal_set_scroll_on_output(VTE_TERMINAL(vtebox), FALSE);
	vte_terminal_set_scroll_on_keystroke(VTE_TERMINAL(vtebox), TRUE);
	vte_terminal_set_backspace_binding (VTE_TERMINAL(vtebox), VTE_ERASE_ASCII_DELETE);
	// vte_terminal_set_delete_binding (VTE_TERMINAL(vtebox), VTE_ERASE_ASCII_DELETE);
}

gboolean set_background_saturation (GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vtebox)
{
	if (value > 1)
		value =1;
	if (value < 0)
		value =0;

#ifdef ENABLE_RGBA
	if (use_rgba)
	{
		if (transparent_background)
			vte_terminal_set_opacity(VTE_TERMINAL(vtebox), (1-value)*65535);
		else
			vte_terminal_set_opacity(VTE_TERMINAL(vtebox), 65535);
	}
	else
#endif
		vte_terminal_set_background_transparent(VTE_TERMINAL(vtebox), transparent_background);

	if (transparent_background)
		vte_terminal_set_background_saturation( VTE_TERMINAL(vtebox), value);
	else
		vte_terminal_set_background_saturation( VTE_TERMINAL(vtebox), 0);

	return FALSE;
}

#ifdef ENABLE_RGBA
gboolean set_window_opacity (GtkRange *range, GtkScrollType scroll, gdouble value, gpointer user_data)
{
	if (value > 1)
		value =1;
	if (value < 0)
		value =0;

	if (use_rgba)
	{
		if (transparent_window)
			gtk_window_set_opacity (GTK_WINDOW(window), 1-value);
		else
			gtk_window_set_opacity (GTK_WINDOW(window), 1);
	}
	return FALSE;
}
#endif

#ifdef ENABLE_RGBA
// init rgba to enable true transparent.
void init_rgba()
{
	GdkScreen *screen;
	GdkColormap *colormap;

	screen = gtk_widget_get_screen(window);
	if (use_rgba==2)
		use_rgba = gdk_screen_is_composited(screen);
	
	//if (use_rgba)
	//	g_debug("Yes, This screen supports rgba!\n");
	//else
	//	g_debug("No, This screen don't supports rgba!\n");
	
	if (use_rgba)
	{
		colormap = gdk_screen_get_rgba_colormap(screen);
		if (colormap != NULL)
			gtk_widget_set_colormap(window, colormap);
	}
}
#endif

// set the window hints information
void window_resizable(GtkWidget *vtebox, gint run_once, gint minsize)
{
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

	// g_debug("Tring to set geometry...\n");
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

GString *save_user_settings(GtkWidget *widget, GtkWidget *current_vtebox)
{
	const gchar *profile_dir = g_get_user_config_dir();
	gchar *profile = g_strdup_printf("%s/%s", profile_dir, RCFILE);
	GString *contents = NULL;
	GKeyFile *keyfile = g_key_file_new();
	struct Page *current_data = NULL;
	gint i;
	
	if (current_vtebox)
	{
		if (g_mkdir_with_parents(profile_dir, 0700))
		{
			g_critical("can NOT create the directory: %s", profile_dir);
			free_user_settings_data(error, profile, contents, keyfile);
			return NULL;
		}
		current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
	}
	else
		init_pagekeys();
	
	contents = g_string_new("[main]\n");
	g_string_append_printf(contents,"page_name = %s\n", page_name);
	g_string_append_printf(contents,"page_names = %s\n", page_names);
	g_string_append_printf(contents,"reuse_page_names = %d\n", reuse_page_names);
	g_string_append_printf(contents,"page_number = %d\n", page_number);
	g_string_append_printf(contents,"page_shows_current_cmdline = %d\n", page_shows_current_cmdline);
	g_string_append_printf(contents,"window_shows_current_page = %d\n", window_shows_current_page);
	g_string_append_printf(contents,"show_color_selection_menu = %d\n", show_color_selection_menu);
	g_string_append_printf(contents,"foreground_color = %s\n", foreground_color);
	g_string_append_printf(contents,"background_color = %s\n", background_color);
	g_string_append_printf(contents,"show_resize_menu = %d\n", show_resize_menu);
	
	if (current_vtebox)
	{
		g_string_append_printf(contents, "column = %d\n", current_data->column);
		g_string_append_printf(contents, "row = %d\n", current_data->row);
		g_string_append_printf(contents, "font_name = %s\n", current_data->font_name);
		if (original_use_rgba == 2)
			g_string_append_printf(contents, "use_rgba =\n");
		else
			g_string_append_printf(contents, "use_rgba = %d\n", original_use_rgba);
	}
	else
	{
		g_string_append_printf(contents, "font_name = %s\n", system_font_name);
		g_string_append_printf(contents, "column = %d\n", system_column);
		g_string_append_printf(contents, "row = %d\n", system_row);
		g_string_append_printf(contents, "use_rgba =\n");
	}
	g_string_append_printf(contents,"show_transparent_menu = %d\n", show_transparent_menu);
#ifdef ENABLE_RGBA
	g_string_append_printf(contents,"transparent_window = %d\n", transparent_window>0);
	g_string_append_printf(contents,"window_opacity = %1.3f\n", window_opacity);
#endif
	g_string_append_printf(contents,"transparent_background = %d\n", transparent_background>0);
	g_string_append_printf(contents,"background_saturation = %1.3f\n", background_saturation);
	g_string_append_printf(contents,"word_chars = %s\n", word_chars);
	g_string_append_printf(contents,"scrollback_lines = %d\n", scrollback_lines);
	g_string_append_printf(contents,"show_input_method_menu = %d\n", show_input_method_menu);
	g_string_append_printf(contents,"locales_list = %s\n", locales_list);
	g_string_append_printf(contents,"\n");
	g_string_append_printf(contents,"[key]\n");
	
	for (i=0; i<KEYS; i++)
		g_string_append_printf(contents,"%s = %s\n", pagekeys[i].name, pagekeys[i].value);

	if (!current_vtebox)
		return contents;
	
	// g_debug("\n%s\n", contents->str);

	if (!g_file_set_contents(profile, contents->str, -1, &error))
		dialog (NULL, 6);
	//	g_debug("Error while writing profile '%s': %s", profile, error->message);
	
	free_user_settings_data(error, profile, contents, keyfile);
	return NULL;
}

void free_user_settings_data(GError *error, gchar *profile, GString* contents, GKeyFile *keyfile)
{
	// g_clear_error(&error);
	g_free(profile);
	g_string_free(contents, TRUE);
	g_key_file_free(keyfile);

}
