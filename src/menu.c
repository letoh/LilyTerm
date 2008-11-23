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

// user default
extern gchar **supported_locales;
extern gchar *default_locale;
extern gboolean show_input_method_menu;

extern GtkWidget *window;
extern GtkWidget *notebook;
extern GtkWidget *current_vtebox;
extern gboolean show_transparent_menu;
#ifdef ENABLE_RGBA
extern gboolean use_rgba;
extern gint transparent_window;
extern gdouble window_opacity;
GtkWidget *menuitem_trans_win;
#endif

extern gchar *default_font_name;
extern gboolean show_resize_menu;

GtkWidget *menu;

extern gboolean transparent_background;
extern gdouble background_saturation;
GtkWidget *menuitem_trans_bg;
GtkWidget *default_encoding;

void create_menu()
{
	GSList *group=NULL;
	GtkWidget *menu_item, *sub_menu;
	gint i=0;
	
	menu = gtk_menu_new();

	if (*supported_locales!=NULL)
	{
		// 1st item: System Default
		default_encoding = gtk_radio_menu_item_new_with_label (group, _("System Default"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), default_encoding);
		group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (default_encoding));
		g_signal_connect(default_encoding, "activate", G_CALLBACK(set_locale), "");

		// other Locales
		while (supported_locales[i]!=NULL)
		{
			if (*(supported_locales[i]))
			{
				menu_item = gtk_radio_menu_item_new_with_label (group, supported_locales[i]);
				gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
				group = gtk_radio_menu_item_get_group (GTK_RADIO_MENU_ITEM (menu_item));
				g_signal_connect(menu_item, "activate", G_CALLBACK(set_locale), supported_locales[i]);
			}
			i++;
		}
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	}
	if (show_transparent_menu)
	{
#ifdef ENABLE_RGBA
		if (use_rgba)
		{
			// Transparent Window
			menuitem_trans_win = gtk_check_menu_item_new_with_label (_("Transparent Window"));
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_trans_win);
			g_signal_connect(menuitem_trans_win, "activate", G_CALLBACK(set_trans_win), NULL);
		
			menu_item = gtk_image_menu_item_new_with_label(_("Window Opacity"));
			gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU));
			gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
			g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)8);
		
			// ----------------------------------------
			menu_item = gtk_separator_menu_item_new ();
			gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		}
#endif
		// Transparent Background
		menuitem_trans_bg = gtk_check_menu_item_new_with_label (_("Transparent Background"));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menuitem_trans_bg);
		g_signal_connect(menuitem_trans_bg, "activate", G_CALLBACK(set_trans_bg), NULL);
	
		menu_item = gtk_image_menu_item_new_with_label(_("Background Saturation"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_EXECUTE, GTK_ICON_SIZE_MENU));  
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)2);
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	}
	if (show_resize_menu)
	{
		// Window Size
		menu_item = gtk_image_menu_item_new_with_label(_("Increase window size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_ZOOM_IN, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)3);
		menu_item = gtk_image_menu_item_new_with_label(_("Decrease window size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_ZOOM_OUT, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)4);
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);

	}

	// Input Method
	if (show_input_method_menu)
	{
		menu_item = gtk_image_menu_item_new_with_label(_("Switch input methods"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
					      gtk_image_new_from_stock(GTK_STOCK_INDEX, GTK_ICON_SIZE_MENU));   
		sub_menu = gtk_menu_new ();
		gtk_menu_item_set_submenu (GTK_MENU_ITEM (menu_item), sub_menu);
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
		vte_terminal_im_append_menuitems (VTE_TERMINAL(current_vtebox), GTK_MENU_SHELL (sub_menu));
	}

	// Change the font for every tab
	menu_item = gtk_image_menu_item_new_with_label(_("Change the font"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_SELECT_FONT, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(select_font), NULL);

	// Edit tab name
	menu_item = gtk_image_menu_item_new_with_label(_("Rename this tab"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_EDIT, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)1);

	if (show_resize_menu)
	{
		// ----------------------------------------
		menu_item = gtk_separator_menu_item_new ();
		gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);
	
		// Reset font and window size
		menu_item = gtk_image_menu_item_new_with_label(_("Reset to default font/size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)5);
	
		menu_item = gtk_image_menu_item_new_with_label(_("Reset to system font/size"));
		gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
						      gtk_image_new_from_stock(GTK_STOCK_ZOOM_100, GTK_ICON_SIZE_MENU));
		gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
		g_signal_connect(menu_item, "activate", G_CALLBACK(set_vtebox_font), (gint *)6);
	}
	
	// Reset
	menu_item = gtk_image_menu_item_new_with_label(_("Reset the content of this tab"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_REFRESH, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(reset_vtebox), NULL);

	// ----------------------------------------
	menu_item = gtk_separator_menu_item_new ();
	gtk_menu_shell_append (GTK_MENU_SHELL (menu), menu_item);

	// Usage
	menu_item = gtk_image_menu_item_new_with_label(_("Usage"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_HELP, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)5);

	// Get the key value for using in profile
	menu_item = gtk_image_menu_item_new_with_label(_("Get function key value"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_CONVERT, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)4);

	// Profile
	menu_item = gtk_image_menu_item_new_with_label(_("Profile sample"));
	gtk_image_menu_item_set_image(GTK_IMAGE_MENU_ITEM(menu_item),
				      gtk_image_new_from_stock(GTK_STOCK_PREFERENCES, GTK_ICON_SIZE_MENU));
	gtk_menu_shell_append(GTK_MENU_SHELL(menu), menu_item);
	g_signal_connect(menu_item, "activate", G_CALLBACK(dialog), (gint *)6);

	gtk_widget_show_all(menu);
}

void reset_vtebox(GtkWidget *widget, gpointer user_data)
{
	vte_terminal_reset(VTE_TERMINAL(current_vtebox), TRUE, FALSE);
}

void set_trans_bg(GtkWidget *widget, gpointer user_data)
{
	gint i;
	GtkWidget *vtebox;

	transparent_background = GTK_CHECK_MENU_ITEM(menuitem_trans_bg)->active;
	
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));i++)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT( gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),
									gtk_notebook_get_nth_page(
										GTK_NOTEBOOK(notebook), i))),
								"VteBox");
		set_background_saturation (NULL, 0, background_saturation, vtebox);
	}
}
#ifdef ENABLE_RGBA
void set_trans_win(GtkWidget *widget, gpointer user_data)
{
	transparent_window = GTK_CHECK_MENU_ITEM(menuitem_trans_win)->active;
	set_window_opacity (NULL, 0, window_opacity, NULL);
}
#endif

// it is OK to use either zh_TW.Big5 or Big5
void set_locale(GtkWidget *widget, gchar *locale)
{
	gchar **locales;

	// Using system default
	if ( ! (*locale))
		locale = default_locale;

	locales = g_strsplit_set(locale, ".", 0);
	if (locales[1])
	{
		vte_terminal_set_encoding(VTE_TERMINAL(current_vtebox), locales[1]);
		// vte_terminal_feed_child(VTE_TERMINAL(current_vtebox),"export LC_ALL=zh_TW.Big5\n",-1);
	}
	else
		vte_terminal_set_encoding(VTE_TERMINAL(current_vtebox), locales[0]);

	//
	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
	current_data->encoding=widget;

	g_strfreev(locales);
}

void select_font(GtkWidget *widget, gpointer user_data)
{
	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
	
	GtkWidget *dialog = gtk_font_selection_dialog_new(_("Font Selection"));
	gtk_window_set_transient_for(GTK_WINDOW(dialog), GTK_WINDOW(window));
	// set the default font name in gtk_font_selection_dialog
	gtk_font_selection_dialog_set_font_name(GTK_FONT_SELECTION_DIALOG(dialog), current_data->font_name);
	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
	{
		// g_debug("Trying to change font name!\n");
		g_free(current_data->font_name);
		current_data->font_name = gtk_font_selection_dialog_get_font_name(GTK_FONT_SELECTION_DIALOG(dialog));
		set_vtebox_font(NULL, 7);
	}
	gtk_widget_destroy(dialog);
}

