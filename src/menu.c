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

#include "menu.h"

// Sometimes we may popup a menu on a non-active (not focus) window...
extern GtkWidget *menu_active_window;
extern GtkClipboard *pclipboard;
extern struct Page_Color page_color[PAGE_COLOR];

void create_menu(struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch create_menu() for win_data %p", win_data);
#endif
	GtkWidget *menu_item, *sub_menu=NULL;
	gint i=0;
	gboolean enable_vte_cjk_width = win_data->VTE_CJK_WIDTH ? 1 : 0;
	//g_debug("enable_vte_cjk_width = %d, and VTE_CJK_WIDTH (environ) = %s",
	//	enable_vte_cjk_width, g_getenv("VTE_CJK_WIDTH"));
	win_data->menu = gtk_menu_new();

	if ((win_data->supported_locales!=NULL) ||
	     (enable_vte_cjk_width &&
	     		(g_ascii_strcasecmp(vte_terminal_get_encoding(VTE_TERMINAL(
									win_data->current_vte)),
									"UTF-8")==0)))
	{
		// We will create 2 sub menu here:
		// [New Tab/Change text encoding] and [New tab with specified locale]
		
		gchar **temp_locales;
		if (win_data->supported_locales)
		{
			// if the encoding of current_vte is not listed in
			// win_data->default_locale and win_data->locales_list.
			// g_debug("win_data->default_locale = %s, win_data->locales_list = %s, "
			//	"vte_terminal_get_encoding = %s",
			//	win_data->default_locale,
			//	win_data->locales_list,
			//	(gchar *)vte_terminal_get_encoding(
			//		VTE_TERMINAL(win_data->current_vte)));
			if ((win_data->default_locale != NULL) && (strchr(win_data->default_locale, '.')))
				temp_locales = get_supported_locales((gchar *)vte_terminal_get_encoding(
									 VTE_TERMINAL(win_data->current_vte)),
								     win_data->locales_list,
								     get_default_LC_TYPE(),
								     win_data->default_locale);
			else
			{
				gchar *default_encoding = (gchar *)get_encoding_from_locale();
				temp_locales = get_supported_locales((gchar *)vte_terminal_get_encoding(
									VTE_TERMINAL(win_data->current_vte)),
								      win_data->locales_list,
								      get_default_LC_TYPE(),
								      default_encoding);
			}
		}
		else
			// create a fake supported_locales for
			// [enable_vte_cjk_width=TRUE but supported_locales=""]
			temp_locales = g_strsplit("UTF-8.UTF-8", " ", -1);

		// enable_locale_menu: if the items in supported_locales is
		//		       like 'zh_TW.Big5', not only 'Big5'
		// enable_locale_menu = TRUE will create [New tab with specified locale] sub menu.
		gboolean enable_locale_menu = FALSE;
		GtkWidget *locale_sub_menu = NULL, *locales_menu_item, *locale_menu_item;
		GList *locale_list = NULL;

		// win_data->supported_locales=NULL:
		// The supported_locales is empty but it is using UTF-8 locale.
		if (win_data->supported_locales!=NULL)
		{
			menu_item = gtk_image_menu_item_new_with_label(_("Change text encoding"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_DND, GTK_ICON_SIZE_MENU));
			win_data->encoding_sub_menu = gtk_menu_new ();
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), win_data->encoding_sub_menu);
			gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
		}

		// We create locales_menu_item first,
		// And append it to menu if enable_locale_menu = TRUE
		if (win_data->supported_locales)
			locales_menu_item = gtk_image_menu_item_new_with_label(
						_("New tab with specified locale"));
		else
			locales_menu_item = gtk_image_menu_item_new_with_label(_("New tab"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(locales_menu_item),
					      gtk_image_new_from_stock( GTK_STOCK_DND_MULTIPLE,
									GTK_ICON_SIZE_MENU));
		locale_sub_menu = gtk_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (locales_menu_item), locale_sub_menu);
		
		while (temp_locales[i]!=NULL)
		{
			if (*(temp_locales[i]))
			{
				gchar **locales;
				// g_debug("Checking temp_locales[%d] = %s...", i, temp_locales[i]);
				if (strcmp(temp_locales[i], "ANSI_X3.4-1968"))
					// zh_TW.Big5 -> [0]zh_TW, [1]Big5
					locales = g_strsplit(temp_locales[i], ".", 0);
				else
					// the encoding of "C" is "ANSI_X3.4-1968"
					locales = g_strsplit(temp_locales[i], "+", 0);
				menu_item = NULL;

				// Create the menuitem for [Change text encoding]
				if (win_data->supported_locales)
				{
					 // tmp_local can NOT free()!
					const gchar *tmp_local;

					// The system default.
					// Use vte_terminal_get_encoding here for LC_CTYPE=zh_TW (Not zh_TW.Big5)
					if (locales[1])
						// For "zh_TW.Big5 -> [0]zh_TW, [1]Big5"
						tmp_local = locales[1];
					else
						// For "Big5"
						tmp_local = locales[0];
					menu_item = check_encoding_in_menuitem(win_data->encoding_sub_menu,
									       tmp_local);
					
					// g_debug("Got tmp_local = %s", tmp_local);
					// menu_item = NULL: Not find in encoding_sub_menu
					if (! menu_item)
						menu_item = add_menuitem_to_encoding_sub_menu(win_data,
											      i,
											      tmp_local);
				}
				
				// The menuitem for [New tab with specified locale]
				// menu_item = NULL: [enable_vte_cjk_width=TRUE but supported_locales=""]
				if ((locales[1] || (!menu_item)) &&
				    (! check_locale_in_menuitem(locale_list, temp_locales[i])))
				{
					// g_debug("Append %s to locale_list, and locales[1] = %s",
					//	temp_locales[i],
					//	locales[1]);
					locale_list = g_list_append(locale_list, temp_locales[i]);
					if (i || (!menu_item)) enable_locale_menu = TRUE;

					gint j = 0, k;
					// g_debug("locales[1] = %s", locales[1]);
					if (enable_vte_cjk_width && ((g_ascii_strcasecmp(locales[1], "UTF-8")==0) ||
								     (!menu_item)))
						j=1;

					for (k=0; k<=j; k++)
					{
						
						if (k)
						{
							gchar *temp_label;
							if (win_data->supported_locales)
								temp_label = g_strdup_printf("%s%s",
											temp_locales[i], _(" (Wide)"));
							else
								temp_label = g_strdup_printf("%s%s",
											locales[0], _(" (Wide)"));
							locale_menu_item = gtk_image_menu_item_new_with_label(
													temp_label);
#ifdef DETAIL
							g_debug("* free temp_label %p (%s) in create_menu()", temp_label, temp_label);
#endif
							g_free(temp_label);
						}
						else
						{
							if (win_data->supported_locales)
								locale_menu_item = gtk_image_menu_item_new_with_label(
													temp_locales[i]);
							else
								locale_menu_item = gtk_image_menu_item_new_with_label(
													locales[0]);
						}
						
						if (win_data->supported_locales)
							gtk_widget_set_name(locale_menu_item, temp_locales[i]);
						else
							gtk_widget_set_name(locale_menu_item, locales[0]);

						gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(locale_menu_item),
								gtk_image_new_from_stock(GTK_STOCK_DND_MULTIPLE,
											 GTK_ICON_SIZE_MENU));
						gtk_menu_shell_append(GTK_MENU_SHELL(locale_sub_menu),
								      locale_menu_item);
						g_signal_connect(locale_menu_item,
								 "activate",
								 G_CALLBACK(new_tab_with_locale),
								 (gint *)k);
					}
				}
#ifdef DETAIL
				g_debug("* free locales %p in create_menu()", locales);
#endif
				g_strfreev(locales);
			}
			i++;
		}
#ifdef DETAIL
		g_debug("* free locale_list %p in create_menu()", locale_list);
#endif
		g_list_free(locale_list);
		g_strfreev(temp_locales);
		if (enable_locale_menu)
		{
			gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), locales_menu_item);
			// ----------------------------------------
			menu_item = gtk_separator_menu_item_new ();
			gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
		}

	}

//	// ----------------------------------------
//	menu_item = gtk_separator_menu_item_new ();
//	gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

	// Change the font for every tab
	menu_item = gtk_image_menu_item_new_with_label(_("Change the font"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_SELECT_FONT, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(select_font), win_data->window);

#ifdef ENABLE_GDKCOLOR_TO_STRING
	if (win_data->show_color_selection_menu)
	{
		// The submenu of Change color
		menu_item = gtk_image_menu_item_new_with_label(_("Change colors"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
		sub_menu = gtk_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), sub_menu);
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

		// Change the foreground color for every tab
		menu_item = gtk_image_menu_item_new_with_label(_("Change the foreground color"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 9);

		// Change the background color for every tab
		menu_item = gtk_image_menu_item_new_with_label(_("Change the background color"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 10);

		// Change the background color for every tab
		menu_item = gtk_image_menu_item_new_with_label(_("Adjust the brightness of ANSI colors"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 19);

		if (win_data->use_color_page)
		{
			for (i=0; i<PAGE_COLOR; i++)
			{
				gchar *label_name = g_strdup_printf(_("Change the %s color on tab"),
								    page_color[i].comment);
				menu_item = gtk_image_menu_item_new_with_label(label_name);
				gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
				gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
				g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) (11+i));
				g_free(label_name);
			}
		}
	}
	// ----------------------------------------
	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

#endif
	if (win_data->show_transparent_menu)
	{
#ifdef ENABLE_RGBA
		if (win_data->use_rgba)
		{
			// Transparent Window
			win_data->menuitem_trans_win = gtk_check_menu_item_new_with_label (_("Transparent Window"));
			gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_trans_win);
			g_signal_connect(win_data->menuitem_trans_win,
					 "activate",
					 G_CALLBACK(set_trans_win),
					 win_data->window);
		
			menu_item = gtk_image_menu_item_new_with_label(_("Window Opacity"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)8);
		
			// ----------------------------------------
			menu_item = gtk_separator_menu_item_new ();
			gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
		}
#endif
		// Transparent Background
		win_data->menuitem_trans_bg = gtk_check_menu_item_new_with_label (_("Transparent Background"));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_trans_bg);
		g_signal_connect(win_data->menuitem_trans_bg,
				 "activate",
				 G_CALLBACK(set_trans_bg),
				 win_data);
	
		menu_item = gtk_image_menu_item_new_with_label(_("Background Saturation"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU));  
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)2);
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
	}

	if (win_data->show_copy_paste_menu)
	{
		if (win_data->enable_hyperlink)
		{
			// copy_url
			win_data->menuitem_copy_url = gtk_image_menu_item_new_with_label(_("Copy URL"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(win_data->menuitem_copy_url),
						      gtk_image_new_from_stock(GTK_STOCK_COPY,
									       GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu),
					      win_data->menuitem_copy_url);
			g_signal_connect(win_data->menuitem_copy_url,
					 "activate",
					 G_CALLBACK(copy_url_clipboard),
					 NULL);
		}

		// copy
		win_data->menuitem_copy = gtk_image_menu_item_new_with_label(_("Copy"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(win_data->menuitem_copy),
					      gtk_image_new_from_stock(GTK_STOCK_COPY,
								       GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_copy);
		g_signal_connect(win_data->menuitem_copy, "activate", G_CALLBACK(copy_clipboard), NULL);

		// paste
		win_data->menuitem_paste = gtk_image_menu_item_new_with_label(_("Paste"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(win_data->menuitem_paste),
					      gtk_image_new_from_stock(GTK_STOCK_PASTE,
								       GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_paste);
		g_signal_connect(win_data->menuitem_paste, "activate", G_CALLBACK(paste_clipboard), NULL);

		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
	}

	// use scrollback lines
	win_data->menuitem_hide_scroll_bar = gtk_check_menu_item_new_with_label (_("Scrollback lines"));
	// g_debug("menuitem_hide_scroll_bar)->active = %d", win_data->scrollback_lines ? 1 : 0);
	// GTK_CHECK_MENU_ITEM(menuitem_hide_scroll_bar)->active = win_data->scrollback_lines ? 1 : 0;
	GTK_CHECK_MENU_ITEM(win_data->menuitem_hide_scroll_bar)->active = win_data->show_scrollbar;
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_hide_scroll_bar);
	g_signal_connect(win_data->menuitem_hide_scroll_bar, "activate", G_CALLBACK(hide_scrollback_lines), NULL);

	// clean scrollback lines
	menu_item = gtk_image_menu_item_new_with_label(_("Clean scrollback lines"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(clean_scrollback_lines), NULL);

	// ----------------------------------------
	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

	// Cursor Blinks
	win_data->menuitem_cursor_blinks = gtk_check_menu_item_new_with_label (_("Cursor Blinks"));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_cursor_blinks);
	g_signal_connect(win_data->menuitem_cursor_blinks,
			 "activate",
			 G_CALLBACK(set_cursor_blinks),
			 win_data);

	win_data->menuitem_audible_bell = gtk_check_menu_item_new_with_label (_("Audible Bell"));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_audible_bell);
	g_signal_connect(win_data->menuitem_audible_bell, "activate", G_CALLBACK(set_audible_bell), win_data);

	// Show the tabs bar
	win_data->menuitem_show_tabs_bar = gtk_image_menu_item_new_with_label(_("Show the tabs bar"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(win_data->menuitem_show_tabs_bar),
				      gtk_image_new_from_stock(GTK_STOCK_SORT_DESCENDING,
							       GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_show_tabs_bar);
	g_signal_connect(win_data->menuitem_show_tabs_bar,
			 "activate",
			 G_CALLBACK(launch_hide_and_show_tabs_bar), (gint *)TRUE);

	// Hide the tabs bar
	win_data->menuitem_hide_tabs_bar = gtk_image_menu_item_new_with_label(_("Hide the tabs bar"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(win_data->menuitem_hide_tabs_bar),
				      gtk_image_new_from_stock(GTK_STOCK_SORT_ASCENDING,
							       GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_hide_tabs_bar);
	g_signal_connect(win_data->menuitem_hide_tabs_bar,
			 "activate",
			 G_CALLBACK(launch_hide_and_show_tabs_bar), FALSE);

	// ----------------------------------------
	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);


	// Input Method
	if (win_data->show_input_method_menu)
	{
		menu_item = gtk_image_menu_item_new_with_label(_("Switch input methods"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_INDEX, GTK_ICON_SIZE_MENU));   
		sub_menu = gtk_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), sub_menu);
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
		vte_terminal_im_append_menuitems (VTE_TERMINAL(win_data->current_vte), GTK_MENU_SHELL (sub_menu));
	}

	// Edit tab name
	if (win_data->show_change_page_name_menu)
	{
		menu_item = gtk_image_menu_item_new_with_label(_("Rename this tab"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)1);
	}

	if (win_data->show_input_method_menu || win_data->show_change_page_name_menu)
	{
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
	}

	if (win_data->show_resize_menu)
	{
		// Window Size
		menu_item = gtk_image_menu_item_new_with_label(_("Increase window size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_ZOOM_IN, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vte_font), (gint *)3);
		menu_item = gtk_image_menu_item_new_with_label(_("Decrease window size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_ZOOM_OUT, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vte_font), (gint *)4);
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

		// Reset font and window size
		menu_item = gtk_image_menu_item_new_with_label(_("Reset to user custom font/size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vte_font), (gint *)5);
	
		menu_item = gtk_image_menu_item_new_with_label(_("Reset to system font/size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vte_font), (gint *)6);
	}
	
	// Reset
	menu_item = gtk_image_menu_item_new_with_label(_("Reset the content"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(reset_vte), NULL);

	// ----------------------------------------
	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

	// Usage
	menu_item = gtk_image_menu_item_new_with_label(_("Usage"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)5);

	// Get the key value for using in profile
	menu_item = gtk_image_menu_item_new_with_label(_("Get function key value"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_CONVERT, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)4);

	// Profile
	//menu_item = gtk_image_menu_item_new_with_label(_("Profile sample"));
	menu_item = gtk_image_menu_item_new_with_label(_("Save settings"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_SAVE, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	//g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)6);
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_user_settings), win_data->current_vte);

	gtk_widget_show_all(win_data->menu);
}

void hide_scrollback_lines(GtkWidget *menu_item, gpointer user_data)
{
	gint i;
	GtkWidget *vte;
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	win_data->show_scrollbar = GTK_CHECK_MENU_ITEM(menu_item)->active;

	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(GTK_NOTEBOOK(
								    win_data->notebook),
									gtk_notebook_get_nth_page(
									    GTK_NOTEBOOK(win_data->notebook),
									    i))),
						   "VteBox");
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
		if (win_data->show_scrollbar)
			gtk_widget_show (page_data->scrollbar);
		else
			gtk_widget_hide (page_data->scrollbar);
	}

	// Trying to keep vte size
	if (! (win_data->fullscreen || win_data->unfullscreen))
		// auto_resize_window(NULL, NULL, win_data);
		keep_window_size (win_data, win_data->current_vte, 0xc00);
}

void clean_scrollback_lines(GtkWidget *widget, gpointer user_data)
{
#ifdef DETAIL
	g_debug("! Launch clean_scrollback_lines()!");
#endif
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	vte_terminal_set_scrollback_lines (VTE_TERMINAL(win_data->current_vte), 0);
	vte_terminal_set_scrollback_lines (VTE_TERMINAL(win_data->current_vte), win_data->scrollback_lines);
}

void copy_url_clipboard(GtkWidget *widget, gpointer user_data)
{
	if (gtk_widget_get_name(widget))
		gtk_clipboard_set_text(pclipboard, gtk_widget_get_name(widget), -1);
}

void copy_clipboard(GtkWidget *widget, gpointer user_data)
{
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window),
								     "Win_Data");
	vte_terminal_copy_clipboard(VTE_TERMINAL(win_data->current_vte));
}

void paste_clipboard(GtkWidget *widget, gpointer user_data)
{
#ifdef DETAIL
	g_debug("! Launch clean_scrollback_lines()!");
#endif
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window),
								     "Win_Data");
	vte_terminal_paste_clipboard(VTE_TERMINAL(win_data->current_vte));
}

void set_cursor_blinks(GtkWidget *menuitem_cursor_blinks, struct Window *win_data)
{
	win_data->cursor_blinks = GTK_CHECK_MENU_ITEM(menuitem_cursor_blinks)->active;

	gint i;
	GtkWidget *vte;

	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(GTK_NOTEBOOK(
								    win_data->notebook),
									gtk_notebook_get_nth_page(
									    GTK_NOTEBOOK(win_data->notebook),
									    i))),
						   "VteBox");
		vte_terminal_set_cursor_blinks (VTE_TERMINAL(vte), win_data->cursor_blinks);
	}
}

void set_audible_bell(GtkWidget *menuitem_audible_bell, struct Window *win_data)
{
	win_data->audible_bell = GTK_CHECK_MENU_ITEM(menuitem_audible_bell)->active;
	gint i;
	GtkWidget *vte;

	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(GTK_NOTEBOOK(
								    win_data->notebook),
									gtk_notebook_get_nth_page(
									    GTK_NOTEBOOK(win_data->notebook),
									    i))),
						   "VteBox");
		vte_terminal_set_audible_bell (VTE_TERMINAL(vte), win_data->audible_bell);
	}
}

void launch_hide_and_show_tabs_bar(GtkWidget *widget, gboolean show_tabs_bar)
{
	// g_debug("menu_active_window = %p", menu_active_window);

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	hide_and_show_tabs_bar(win_data , show_tabs_bar);
}

void hide_and_show_tabs_bar(struct Window *win_data , gboolean show_tabs_bar)
{
	// win_data->keep_vte_size |= 6;
	// window_resizable(win_data->window, win_data->current_vte, 2, 1);
	if (! (win_data->fullscreen || win_data->unfullscreen))
		keep_window_size (win_data, win_data->current_vte, 0x1c);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(win_data->notebook), show_tabs_bar);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(win_data->notebook), show_tabs_bar);
	if (show_tabs_bar) GTK_WIDGET_UNSET_FLAGS(win_data->notebook, GTK_CAN_FOCUS);

	if (win_data->fullscreen_show_tab_bar)
	{
		gboolean tab_bar_is_shown = gtk_notebook_get_show_tabs GTK_NOTEBOOK(win_data->notebook);
		switch (win_data->fullscreen_show_tab_bar)
		{
			case 1:
			case 3:
				if (tab_bar_is_shown)
					win_data->fullscreen_show_tab_bar = 2;
				break;
		}
	}
}

GtkWidget *check_encoding_in_menuitem(GtkWidget *sub_menu, const gchar *name)
{
	GList *widget_list = GTK_MENU_SHELL(sub_menu)->children;
	GtkWidget *return_menuitem = NULL;
	GtkWidget *menu_item = NULL;

	while (widget_list!=NULL)
	{
		menu_item = widget_list->data;
		// g_debug("Got the menu_item = %p", menu_item);
		if (menu_item==NULL) break;

//		gchar **check_names =  g_strsplit(gtk_widget_get_name(menu_item), ".", 0);
//		if (check_names[1])
//		{
//			// g_debug("Checking the encoding %s with %s", check_names[1], name);
//			if (g_ascii_strcasecmp (check_names[1], name)==0)
//				return_menuitem = menu_item;
//		}
//		else
//		{
//			// g_debug("Checking the encoding %s with %s", gtk_widget_get_name(menu_item), name);
//			if (g_ascii_strcasecmp (gtk_widget_get_name(menu_item), name)==0)
//				return_menuitem = menu_item;
//		}
//#ifdef DETAIL
//		g_debug("* free check_names %p in check_encoding_in_menuitem()", check_names);
//#endif
//		g_strfreev(check_names);
		// g_debug("Checking '%s' and '%s' in check_encoding_in_menuitem...", gtk_widget_get_name(menu_item), name);

		if (g_ascii_strcasecmp (gtk_widget_get_name(menu_item), name)==0)
			return_menuitem = menu_item;

		if (return_menuitem) return return_menuitem;
		widget_list = widget_list->next;
	}
	return NULL;
}

GtkWidget *add_menuitem_to_encoding_sub_menu(struct Window *win_data,
					     gint no,
					     const gchar *name)
{
	GtkWidget *menu_item = gtk_radio_menu_item_new_with_label(win_data->encoding_group, name);
	if (no==0)
		win_data->default_encoding = menu_item;

	gtk_widget_set_name(menu_item, name);
	// g_debug("Set the %d encoding to %s", no, name);
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->encoding_sub_menu), menu_item);
	// g_debug("win_data->encoding_group = %p", win_data->encoding_group);
	win_data->encoding_group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menu_item));
	g_signal_connect(menu_item, "activate", G_CALLBACK(set_encoding), NULL);
	// g_debug("Init the menuitem %p with %s.", menu_item, tmp_local);

	return menu_item;
}

gboolean check_locale_in_menuitem(GList *locale_list, const gchar *name)
{
	//g_debug("Calling check_locale_in_menuitem with locale_list = %p, name =%s, total = %d",
	//	locale_list, name, g_list_length(locale_list));
	GList *widget_list = NULL;
	int i;

	for (i=0; i< g_list_length(locale_list); i++)
	{
		widget_list = g_list_nth(locale_list, i);
		// g_debug("Checking the locale %s with %s", (gchar *)widget_list->data, name);
		if (g_ascii_strcasecmp (widget_list->data, name)==0)
			return TRUE;
	}
	return FALSE;
}

void reset_vte(GtkWidget *widget, gpointer user_data)
{
#ifdef DETAIL
	g_debug("! Launch reset_vte()!");
#endif
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	vte_terminal_reset(VTE_TERMINAL(win_data->current_vte), TRUE, FALSE);
}

void set_trans_bg(GtkWidget *menuitem_trans_bg, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch set_trans_bg() for win_data %p", win_data);
#endif
	win_data->transparent_background = GTK_CHECK_MENU_ITEM(menuitem_trans_bg)->active;

	gint i;
	GtkWidget *vte;
	
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(GTK_NOTEBOOK(
								    win_data->notebook),
									gtk_notebook_get_nth_page(
									    GTK_NOTEBOOK(win_data->notebook),
									    i))),
						   "VteBox");
		set_background_saturation (NULL, 0, win_data->background_saturation, vte);
	}
}
#ifdef ENABLE_RGBA
void set_trans_win(GtkWidget *widget, GtkWidget *window)
{
#ifdef DETAIL
	g_debug("! Launch set_trans_win() for window %p!", window);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when set_trans_win!", win_data);

	win_data->transparent_window = GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_win)->active;
	set_window_opacity (NULL, 0, win_data->window_opacity, window);
}
#endif

// it is OK to use either zh_TW.Big5 or Big5 here
void set_encoding(GtkWidget *menuitem, GtkWidget *vte)
{
#ifdef DETAIL
	g_debug("! Launch set_encoding() for vte %p", vte);
#endif
	// g_debug("menu_active_window = %p", menu_active_window);
	if (menu_active_window==NULL) return;

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	// No action when just right click the mouse
	if (win_data->query_encoding) return;

	const gchar *encoding;
	// g_debug("Got the encoding item: %p", menuitem);
	if (menuitem==NULL && vte==NULL)
	{
		gchar *err_msg = g_strdup_printf("set_encoding(): menuitem = NULL and vte = NULL\n\n"
						 "Please report bug to %s, Thanks!",
						 PACKAGE_BUGREPORT);
		error_dialog(win_data, err_msg, NULL, 18);
#ifdef DETAIL
		g_debug("* free err_msg %p (%s) in set_encoding()", err_msg, err_msg);
#endif
		g_free(err_msg);
		return;
	}

	// vte==NULL: called by the "activate" signal of menuitem. vte = win_data->current_vte.
	if (vte==NULL)
	{
		// GTK_CHECK_MENU_ITEM(menuitem)->active = false: We only set the encoding with actived menuitem. 
		if (!GTK_CHECK_MENU_ITEM(menuitem)->active) return;
		vte = win_data->current_vte;
	}

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");

	if (menuitem)
		encoding = gtk_widget_get_name(menuitem);
	else
	{
		// g_debug("win_data->default_locale = %s", win_data->default_locale);
		gchar **locales = g_strsplit(win_data->default_locale, ".", -1);
		if (locales[1])
			encoding = win_data->default_locale;
		else
			encoding = vte_terminal_get_encoding(VTE_TERMINAL(vte));
#ifdef DETAIL
		g_debug("* free locales %p in set_encoding() #1", locales);
#endif
		g_strfreev(locales);
	}
	g_free (page_data->encoding_str);
	page_data->encoding_str = g_strdup(encoding);
	// g_debug("Trying to set %d page (%p) to encoding %s...", page_data->page_no, vte, encoding);
	
	gchar **locales = g_strsplit(encoding, ".", -1);
	if (locales[1])
	{
		// g_debug("Trying to set %d page (%p) to encoding %s...", page_data->page_no, vte, locales[1]);
		vte_terminal_set_encoding(VTE_TERMINAL(vte), locales[1]);
	}
	else
	{
		// g_debug("Trying to set %d page (%p) to encoding %s...", page_data->page_no, vte, locales[0]);
		vte_terminal_set_encoding(VTE_TERMINAL(vte), locales[0]);
	}

	// page_data->page_name==NULL: add_page() will call set_encoding() before page_data->label is created.
	// And it will cause the following error: "Document ended unexpectedly while inside an attribute value"
	if (win_data->page_shows_encoding && page_data->page_name!=NULL)
		update_page_name (page_data->window, vte, page_data->page_name, page_data->label, page_data->page_no+1,
				  page_data->custom_page_name, page_data->tab_color,
				  page_data->is_root, page_data->is_bold,
				  compare_win_page_encoding(win_data->default_encoding,
				  			    page_data->encoding_str),
				  page_data->encoding_str, page_data->custom_window_title,
				  FALSE);

#ifdef DETAIL
	g_debug("* free locales %p in set_encoding() #2", locales);
#endif
	g_strfreev(locales);
}

void new_tab_with_locale(GtkWidget *local_menuitem, gboolean VTE_CJK_WIDTH)
{
#ifdef DETAIL
	g_debug("! Launch new_tab_with_locale() with locale = %s and VTE_CJK_WIDTH = %d",
		gtk_widget_get_name(local_menuitem), VTE_CJK_WIDTH);
#endif
	// g_debug("local_menuitem->name = %s", gtk_widget_get_name(local_menuitem);
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	// g_debug("Got the VTE_CJK_WIDTH = %s", gtk_widget_get_name(local_menuitem);
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data");
	gchar *encoding = get_encoding_from_menu_item_name(local_menuitem);

	// menuitem == NULL: The locales_list in profile is empty. Only VTE_CJK_WIDTH is setted.
	// if (menuitem)
	//	g_debug("Add page by %p to locale %s", win_data->current_vte, gtk_widget_get_name(menuitem);

	// locale should not be free()
	const gchar *locale = NULL;
	if (g_ascii_strcasecmp(gtk_widget_get_name(local_menuitem), "UTF-8"))
		locale = gtk_widget_get_name(local_menuitem);
	
	add_page(win_data,
		 page_data,
		 NULL,
		 encoding,
		 (gchar *) locale,
		 NULL,
		 FALSE,
		 get_VTE_CJK_WIDTH_str(VTE_CJK_WIDTH+1));
	
	g_free(encoding);
}

void select_font(GtkWidget *widget, GtkWidget *window)
{
#ifdef DETAIL
	g_debug("! Launch select_font() for window %p", window);
#endif
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data");
	
	GtkWidget *dialog = gtk_font_selection_dialog_new(_("Font Selection"));
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
	// set the default font name in gtk_font_selection_dialog
	gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dialog), page_data->font_name);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
	{
		// g_debug("Trying to change font name!\n");
#ifdef DETAIL
		g_debug("* free page_data->font_name %p (%s) in select_font()", page_data->font_name, page_data->font_name);
#endif
		g_free(page_data->font_name);
		page_data->font_name = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dialog));
		set_vte_font(NULL, 7);
	}
	gtk_widget_destroy(dialog);
}
