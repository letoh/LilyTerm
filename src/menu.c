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


#include "menu.h"

// We may use current_vtebox here safely. 
extern GtkWidget *current_vtebox;
extern gchar *err_str;

void create_menu(GtkWidget *window)
{
#ifdef DEBUG
	g_debug("! Launch create_menu() for window %p", window);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when creating menu!", win_data);

	GSList *group=NULL;
	GtkWidget *menu_item, *sub_menu=NULL;
	gint i=0;
	gboolean enable_vte_cjk_width = win_data->system_VTE_CJK_WIDTH ? 1 : 0;
	//g_debug("enable_vte_cjk_width = %d, and VTE_CJK_WIDTH = %s",
	//	enable_vte_cjk_width, g_getenv("VTE_CJK_WIDTH"));
	win_data->menu = gtk_menu_new();

	if ((win_data->supported_locales!=NULL) ||
	     (enable_vte_cjk_width &&
	     		(g_ascii_strcasecmp(vte_terminal_get_encoding(VTE_TERMINAL(current_vtebox)), "UTF-8")==0)))
	{
		// We will create 2 sub menu here:
		// [New Tab/Change text encoding] and [New tab with specified locale]
		
		// create a fake supported_locales for [enable_vte_cjk_width=TRUE but supported_locales=""]
		gchar **temp_locales = win_data->supported_locales;
		if (! temp_locales) temp_locales = g_strsplit("UTF-8.UTF-8", " ", -1);

		// enable_locale_menu: if the items in supported_locales is like 'zh_TW.Big5', not only 'Big5'
		// enable_locale_menu = TRUE will create [New tab with specified locale] sub menu.
		gboolean enable_locale_menu = FALSE;
		GtkWidget *locale_sub_menu = NULL, *locales_menu_item, *locale_menu_item;
		gchar **locales;
		GList *locale_list = NULL;

		// win_data->supported_locales=NULL: The supported_locales is empty but it is using UTF-8 locale.
		if (win_data->supported_locales!=NULL)
		{
			menu_item = gtk_image_menu_item_new_with_label(_("Change text encoding"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_DND, GTK_ICON_SIZE_MENU));
			sub_menu = gtk_menu_new ();
			gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), sub_menu);
			gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
		}

		// We create locales_menu_item first, And append it to menu if enable_locale_menu = TRUE
		if ((win_data->supported_locales))
			locales_menu_item = gtk_image_menu_item_new_with_label(_("New tab with specified locale"));
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
				// zh_TW.Big5 -> [0]zh_TW, [1]Big5
				locales = g_strsplit(temp_locales[i], ".", 0);
				menu_item = NULL;

				// Create the menuitem for [Change text encoding]
				if ((win_data->supported_locales))
				{
					// tmp_local can NOT free()!
					const gchar *tmp_local;

					// The system default.
					// Use vte_terminal_get_encoding here for LC_CTYPE=zh_TW (Not zh_TW.Big5)
					if (i==0)
						tmp_local = vte_terminal_get_encoding(VTE_TERMINAL(current_vtebox));
					// For "zh_TW.Big5 -> [0]zh_TW, [1]Big5"
					else if (locales[1])
					{
						tmp_local = locales[1];
						menu_item = check_encoding_in_menuitem(sub_menu, tmp_local);
					}
					// For "Big5"
					else
					{
						tmp_local = win_data->supported_locales[i];
						menu_item = check_encoding_in_menuitem(sub_menu,tmp_local);
					}

					// menu_item = NULL: Not find in sub_menu
					if (! menu_item)
					{
						menu_item = gtk_radio_menu_item_new_with_label(group, tmp_local);
						if (i==0)
							win_data->default_encoding = menu_item;
						if (win_data->supported_locales)
							// zh_TW.UTF-8
							menu_item->name = g_strdup(win_data->supported_locales[i]);
						else
							// UTF-8.UTF-8
							menu_item->name = g_strdup(tmp_local);
						// g_debug("Set the %d encoding to %s", i, tmp_local);
						gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
						group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menu_item));
						g_signal_connect(menu_item, "activate", G_CALLBACK(set_encoding), NULL);
						// g_debug("Init the menuitem %p with %s.", menu_item, tmp_local);
					}
				}
				
				// The menuitem for [New tab with specified locale]
				// menu_item = NULL: [enable_vte_cjk_width=TRUE but supported_locales=""]
				if ((locales[1] || (!menu_item)) &&
				    (! check_locale_in_menuitem(locale_list, temp_locales[i])))
				{
					// g_debug("Append %s to locale_list", temp_locales[i]);
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
							locale_menu_item->name = g_strdup_printf("%d", j+1);
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
							locale_menu_item->name = g_strdup_printf("0");
						}
						gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(locale_menu_item),
								gtk_image_new_from_stock(GTK_STOCK_DND_MULTIPLE,
											 GTK_ICON_SIZE_MENU));
						gtk_menu_shell_append(GTK_MENU_SHELL(locale_sub_menu),
												locale_menu_item);
						g_signal_connect(locale_menu_item, "activate",
									G_CALLBACK(new_tab_with_locale), menu_item);
					}
				g_strfreev(locales);
				}
			}
			i++;
		}
		g_list_free(locale_list);
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
	g_signal_connect(menu_item, "activate", G_CALLBACK(select_font), window);

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

		if (win_data->use_color_page)
		{
			// style 11: change the text color of cmdline
			menu_item = gtk_image_menu_item_new_with_label(_("Change the cmdline color on tab"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 11);
	
			// style 12: change the text color of current dir
			menu_item = gtk_image_menu_item_new_with_label(_("Change the dir color on tab"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 12);
	
			// style 13: change the text color of custom page name
			menu_item = gtk_image_menu_item_new_with_label(_("Change the custom color on tab"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 13);
	
			// style 14: change the text color of root privileges cmdline
			menu_item = gtk_image_menu_item_new_with_label(_("Change the root color on tab"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 14);
	
			// style 15: change the text color of normal text
			menu_item = gtk_image_menu_item_new_with_label(_("Change the normal color on tab"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_SELECT_COLOR, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(sub_menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *) 15);
		}

		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
	}
#endif
	if (win_data->show_transparent_menu)
	{
#ifdef ENABLE_RGBA
		if (win_data->use_rgba)
		{
			// Transparent Window
			win_data->menuitem_trans_win = gtk_check_menu_item_new_with_label (_("Transparent Window"));
			gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_trans_win);
			g_signal_connect(win_data->menuitem_trans_win, "activate", G_CALLBACK(set_trans_win), window);
		
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
		g_signal_connect(win_data->menuitem_trans_bg, "activate", G_CALLBACK(set_trans_bg), window);
	
		menu_item = gtk_image_menu_item_new_with_label(_("Background Saturation"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU));  
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)2);
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
	}

	if (win_data->scrollback_lines)
	{
		// use scrollback lines
		win_data->menuitem_scrollback_lines = gtk_check_menu_item_new_with_label (_("Scrollback lines"));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), win_data->menuitem_scrollback_lines);
		g_signal_connect(win_data->menuitem_scrollback_lines, "activate", G_CALLBACK(clean_scrollback_lines), FALSE);

		// clean scrollback lines
		menu_item = gtk_image_menu_item_new_with_label(_("Clean scrollback lines"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_CLEAR, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(clean_scrollback_lines), (gboolean *)TRUE);

		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
	}

	// Input Method
	if (win_data->show_input_method_menu)
	{
		menu_item = gtk_image_menu_item_new_with_label(_("Switch input methods"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_INDEX, GTK_ICON_SIZE_MENU));   
		sub_menu = gtk_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), sub_menu);
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);
		vte_terminal_im_append_menuitems (VTE_TERMINAL(current_vtebox), GTK_MENU_SHELL (sub_menu));
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
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)3);
		menu_item = gtk_image_menu_item_new_with_label(_("Decrease window size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_ZOOM_OUT, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)4);
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (win_data->menu), menu_item);

		// Reset font and window size
		menu_item = gtk_image_menu_item_new_with_label(_("Reset to default font/size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)5);
	
		menu_item = gtk_image_menu_item_new_with_label(_("Reset to system font/size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)6);
	}
	
	// Reset
	menu_item = gtk_image_menu_item_new_with_label(_("Reset the content"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(win_data->menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(reset_vtebox), NULL);

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
	g_signal_connect(menu_item, "activate", G_CALLBACK(save_user_settings), current_vtebox);

	gtk_widget_show_all(win_data->menu);
}

void clean_scrollback_lines(GtkWidget *widget, gboolean type)
{
#ifdef DEBUG
	g_debug("! Launch clean_scrollback_lines()!");
#endif
	// type = TRUE: clean scrollback lines
	// type = FALSE: use/nouse scrollback lines
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when clean scrollback lines!", win_data);

	if (!type)
		page_data->use_scrollback_lines = ! page_data->use_scrollback_lines;
	
	if ((type && page_data->use_scrollback_lines) || (! page_data->use_scrollback_lines))
		vte_terminal_set_scrollback_lines (VTE_TERMINAL(current_vtebox), 0);
	
	if ((type && page_data->use_scrollback_lines) || page_data->use_scrollback_lines)
		vte_terminal_set_scrollback_lines (VTE_TERMINAL(current_vtebox), win_data->scrollback_lines);
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

		gchar **check_names =  g_strsplit(menu_item->name, ".", 0);
		if (check_names[1])
		{
			// g_debug("Checking the encoding %s with %s", check_names[1], name);
			if (g_ascii_strcasecmp (check_names[1], name)==0)
				return_menuitem = menu_item;
		}
		else
		{
			// g_debug("Checking the encoding %s with %s", menu_item->name, name);
			if (g_ascii_strcasecmp (menu_item->name, name)==0)
				return_menuitem = menu_item;
		}
		g_strfreev(check_names);
		if (return_menuitem) return return_menuitem;
		widget_list = widget_list->next;
	}
	return NULL;
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

void reset_vtebox(GtkWidget *widget, gpointer user_data)
{
#ifdef DEBUG
	g_debug("! Launch reset_vtebox()!");
#endif
	vte_terminal_reset(VTE_TERMINAL(current_vtebox), TRUE, FALSE);
}

void set_trans_bg(GtkWidget *widget, GtkWidget *window)
{
#ifdef DEBUG
	g_debug("! Launch set_trans_bg() for window %p", window);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when set_trans_bg!", win_data);

	gint i;
	GtkWidget *vtebox;

	win_data->transparent_background = GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_bg)->active;
	
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT( gtk_notebook_get_tab_label(GTK_NOTEBOOK(
								    win_data->notebook),
									gtk_notebook_get_nth_page(
									    GTK_NOTEBOOK(win_data->notebook),i))),
								"VteBox");
		set_background_saturation (NULL, 0, win_data->background_saturation, vtebox);
	}
}
#ifdef ENABLE_RGBA
void set_trans_win(GtkWidget *widget, GtkWidget *window)
{
#ifdef DEBUG
	g_debug("! Launch set_trans_win() for window %p!", window);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when set_trans_win!", win_data);

	win_data->transparent_window = GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_win)->active;
	set_window_opacity (NULL, 0, win_data->window_opacity, window);
}
#endif

// it is OK to use either zh_TW.Big5 or Big5 here
void set_encoding(GtkWidget *menuitem, GtkWidget *vtebox)
{
#ifdef DEBUG
	g_debug("! Launch set_encoding() for vte %p", vtebox);
#endif
	const gchar *encoding;
	// g_debug("Got the encoding item: %p", menuitem);
	if (menuitem==NULL && vtebox==NULL)
	{
		err_str = g_strdup_printf("set_encoding(): menuitem = NULL and vtebox = NULL\n\n"
					  "Please report bug to %s, Thanks!",
					  PACKAGE_BUGREPORT);
		dialog(NULL, 17);
		g_free(err_str);
		return;
	}

	// vtebox==NULL: called by the "activate" signal of menuitem. vtebox = current_vtebox.
	if (vtebox==NULL)
	{
		// GTK_CHECK_MENU_ITEM(menuitem)->active = false: We only set the encoding with actived menuitem. 
		if (!GTK_CHECK_MENU_ITEM(menuitem)->active) return;
		vtebox = current_vtebox;
	}

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// No action when just right click the mouse
	if (win_data->query_encoding) return;

	if (menuitem)
	{
		encoding = menuitem->name;
		page_data->encoding=menuitem;
	}
	else
	{
		// g_debug("win_data->default_locale = %s", win_data->default_locale);
		gchar **locales = g_strsplit(win_data->default_locale, ".", -1);
		if (locales[1])
			encoding = win_data->default_locale;
		else
			encoding = vte_terminal_get_encoding(VTE_TERMINAL(vtebox));
		g_strfreev(locales);
	}
	// g_debug("Trying to set %d page (%p) to encoding %s...", page_data->page_no, vtebox, encoding);
	
	gchar **locales = g_strsplit(encoding, ".", -1);
	if (locales[1])
		vte_terminal_set_encoding(VTE_TERMINAL(vtebox), locales[1]);
	else
		vte_terminal_set_encoding(VTE_TERMINAL(vtebox), locales[0]);

	// page_data->label->name==NULL: add_page() will call set_encoding() before page_data->label is created.
	// And it will cause the following error: "Document ended unexpectedly while inside an attribute value"
	if (win_data->page_shows_encoding && page_data->label->name!=NULL)
		update_page_name (page_data->window, vtebox, page_data->label, page_data->page_no+1,
				  page_data->custom_page_name, page_data->tab_color,
				  page_data->is_root, page_data->is_bold,
				  page_data->encoding != win_data->default_encoding, page_data->encoding);

	g_strfreev(locales);
}

void new_tab_with_locale(GtkWidget *local_menuitem, GtkWidget *menuitem)
{
#ifdef DEBUG
	if (menuitem)
		g_debug("! Launch new_tab_with_locale() with locale = %s and VTE_CJK_WIDTH = %s", menuitem->name, local_menuitem->name);
	else
		g_debug("! Launch new_tab_with_locale() with VTE_CJK_WIDTH = %s", local_menuitem->name);
#endif
	// g_debug("Got the VTE_CJK_WIDTH = %s", local_menuitem->name);
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Page_Data");

	// menuitem == NULL: The locales_list in profile is empty. Only VTE_CJK_WIDTH is setted.
	// if (menuitem)
	//	g_debug("Add page by %p to locale %s", current_vtebox, menuitem->name);
	if (menuitem)
		add_page(page_data->window, page_data->notebook, menuitem,
			 menuitem->name, NULL, FALSE, atoi(local_menuitem->name));
	else
		add_page(page_data->window, page_data->notebook, NULL, NULL, NULL,
			 FALSE, atoi(local_menuitem->name));
}

void select_font(GtkWidget *widget, GtkWidget *window)
{
#ifdef DEBUG
	g_debug("! Launch select_font() for window %p", window);
#endif
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Page_Data");
	
	GtkWidget *dialog = gtk_font_selection_dialog_new(_("Font Selection"));
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
	// set the default font name in gtk_font_selection_dialog
	gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dialog), page_data->font_name);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
	{
		// g_debug("Trying to change font name!\n");
		g_free(page_data->font_name);
		page_data->font_name = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dialog));
		set_vtebox_font(NULL, 7);
	}
	gtk_widget_destroy(dialog);
}
