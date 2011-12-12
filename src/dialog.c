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

#include "dialog.h"
extern GtkWidget *menu_active_window;
extern struct ModKey modkeys[MOD];
extern struct Page_Color page_color[PAGE_COLOR];

struct ErrorData err_str;
gboolean dialog_actived;

gboolean dialog(GtkWidget *widget, gint style)
{
#ifdef DETAIL
	g_debug("! Launch dialog() with style = %d", style);
#endif
	dialog_actived = TRUE;

	struct Window *win_data = NULL;
	// A critical error message for current_vte = NULL.
	if (style != 18 && style != 21)
	{
		if (menu_active_window==NULL)
		{
			gchar *err_msg = g_strdup_printf("dialog(%d): menu_active_window = NULL\n\n"
						  "Please report bug to %s, Thanks!",
						  style, PACKAGE_BUGREPORT);
			error_dialog(NULL, err_msg, NULL, 18);
#ifdef DETAIL
			g_debug("* free err_msg %p (%s) in dialog()", err_msg, err_msg);
#endif
			g_free(err_msg);
			return FALSE;
		}
		else
			win_data = (struct Window *)g_object_get_data(
							G_OBJECT(menu_active_window), "Win_Data");
	}
	
	struct Dialog *dialog_data = g_new0(struct Dialog, 1);
	struct ColorSelect *color_data = g_new0(struct ColorSelect, 1);
	color_data->original_page_color = NULL;
	gboolean tab_bar_is_shown = TRUE;

	// g_debug("Running dialog mod%d\n", style);
	
	// style  1: change the tab's name
	// style  2: change the saturation of background
	// style  3: confirm close multi pages
	// style  4: get function key value
	// style  5: usage message
	// style  6: fail when saving settings
	// style  7: confirm close running application
	// style  8: change the opacity of window
	// style  9: change the foreground color
	// style 10: change the background color
	// style 11: change the text color of window title
	// style 12: change the text color of cmdline
	// style 13: change the text color of current dir
	// style 14: change the text color of custom page name
	// style 15: change the text color of root privileges cmdline
	// style 16: change the text color of normal text
	// style 17: fault when creating the child process
	// style 18: error messages
	// style 19: Adjust the brightness of ANSI colors used in terminal
	// style 20: The format of profile is out of date
	// style 21: The format of socket data is out of date
	// style 22: confirm to execute command
	
	// string: can NOT be free()
	// temp_str: SHOULD be free()
	gchar *string=NULL, *temp_str[2]={NULL};
	GtkWidget *dialog = NULL, *main_hbox, *icon_vbox, *icon = NULL, *main_right_vbox,
		  *state_vbox, *title_hbox, *title = NULL, *label, *entry = NULL,
		  *entry_hbox = NULL, *key_value_label = NULL, *state_bottom_hbox;
	gboolean response = TRUE;

	struct Page *page_data = NULL;
	if (win_data!=NULL)
	{
//		if (style==7)
//		{
//			dialog_data->total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(
//										win_data->notebook));
//			dialog_data->current_page_no = dialog_data->total_page-1;
//			page_data = get_page_data_from_nth_page(win_data, dialog_data->current_page_no);
//		}
//		else if (win_data->current_vte!=NULL)
		if (win_data->current_vte!=NULL)
		{
			page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data");

			// g_debug("Get win_data = %p when creating dialog!", win_data);
			// For change the text color of tab
			dialog_data->total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(
										page_data->notebook));
			dialog_data->current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(
										page_data->notebook));
		}
	}

	// gboolean lost_focus = win_data->lost_focus;

	// g_object_get (gtk_settings_get_default(), "gtk-alternative-button-order", &BOTTON_ORDER, NULL);
	gboolean BOTTON_ORDER = gtk_alternative_dialog_button_order(NULL);
	// g_debug("gtk_alternative_dialog_button_order = %d" ,gtk_alternative_dialog_button_order (NULL));

	/*
		/--------------main_hbox--------------\
		|/icon_vbox\/------state_vbox-----\/-\|
		||	   ||/----title_hbox-----\||r||
		||	   |||			 |||i||
		||	   ||\-------------------/||g||
		||	   ||/----entry_hbox-----\||h||
		||	   |||			 |||t||
		||	   |||			 ||| ||
		||	   ||\-------------------/||v||
		||	   ||/-state_bottom_hbox-\||b||
		||	   |||			  ||o||
		||	   ||\-------------------/||x||
		|\---------/\---------------------/\-/|
		|     /------------\ /------------\   |
		|     |	    OK	   | |	 CANCEL	  |   |
		|     \------------/ \------------/   |
		\-------------------------------------/
	*/

	// main title
	switch (style)
	{
		case 1:
			string=_("Rename this tab");
			break;
		case 2:
			string=_("Change the saturation of background");
			break;
		case 3:
			string=_("Confirm to close multi tabs");
			break;
		case 4:
			string=_("Get the key value for using in profile");
			break;
		case 5:
			string = g_strdup_printf(_("Usage for %s %s"),PACKAGE_NAME, VERSION);
			break;
		case 6:
			string=_("Error when writing profile");
			break;
		case 7:
			string=_("Confirm to close running foreground program");
			break;
		case 8:
			string=_("Change the opacity of window");
			break;
		case 9:
			string=_("Change the foreground color in terminal");
			break;
		case 10:
			string=_("Change the background color in terminal");
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			string = g_strdup_printf(_("Change the %s color on tab"),
							    page_color[style-11].comment);
			break;
		case 17:
			string=_("Error when creating child process");
			break;
		case 18:
			string=_("The following error is occurred:");
			break;
		case 19:
			string=_("Adjust the brightness of ANSI colors used in terminal");
			break;
		case 20:
			string=_("The format of your profile is out of date");
			break;
		case 21:
			string=_("The format of socket data is out of date");
			break;
		case 22:
			string=_("Confirm to execute -e/-x/--execute command");
			break;
	}

	// Create the dialog window
	// g_debug("Create the dialog window");
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 7:
		case 8:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 19:
			if (BOTTON_ORDER)
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(page_data->window),
								      GTK_DIALOG_NO_SEPARATOR | 
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_OK,
								      GTK_RESPONSE_OK,
								      GTK_STOCK_CANCEL,
								      GTK_RESPONSE_CANCEL,
								      NULL);
			else
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(page_data->window),
								      GTK_DIALOG_NO_SEPARATOR |
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_CANCEL,
								      GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OK,
								      GTK_RESPONSE_OK,
								      NULL);
			break;
		case 4:
			if (BOTTON_ORDER)
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(page_data->window),
								      GTK_DIALOG_NO_SEPARATOR |
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_COPY,
								      GTK_RESPONSE_OK,
								      GTK_STOCK_CLOSE,
								      GTK_RESPONSE_CANCEL,
								      NULL);
			else
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(page_data->window),
								      GTK_DIALOG_NO_SEPARATOR |
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_CLOSE,
								      GTK_RESPONSE_CANCEL,
								      GTK_STOCK_COPY,
								      GTK_RESPONSE_OK,
								      NULL);
			break;
		case 5:
		case 6:
		case 17:
		case 18:
		case 20:
		case 21:
			// g_debug("string = %s, menu_active_window = %p", string, menu_active_window);
			dialog = gtk_dialog_new_with_buttons (string,
							      GTK_WINDOW(menu_active_window),
							      GTK_DIALOG_NO_SEPARATOR | 
								GTK_DIALOG_DESTROY_WITH_PARENT,
							      GTK_STOCK_OK,
							      GTK_RESPONSE_OK,
							      NULL);
			break;
		case 22:
			if (BOTTON_ORDER)
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(menu_active_window),
								      GTK_DIALOG_NO_SEPARATOR |
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_OK,
								      GTK_RESPONSE_OK,
								      GTK_STOCK_CANCEL,
								      GTK_RESPONSE_CANCEL,
								      NULL);
			else
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(menu_active_window),
								      GTK_DIALOG_NO_SEPARATOR |
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_CANCEL,
								      GTK_RESPONSE_CANCEL,
								      GTK_STOCK_OK,
								      GTK_RESPONSE_OK,
								      NULL);
			break;

	}
	
	// save the dialog data first
	g_object_set_data(G_OBJECT(dialog), "Dialog_Data", dialog_data);

	// FIXME: It may be a bug in gtk+2?
	// if a dialog shown before main window is shown,
	// destory the dialog will destory the data of the icon, too.
	// and when showing main window with gtk_widget_show_all(win_data->window) later,
	// It will have no icon, and showing following error:
	// Gtk-CRITICAL **: gtk_window_realize_icon: assertion `info->icon_pixmap == NULL' failed
	// So, we need to set the icon for the dialog here.
	set_window_icon(dialog);

	// It is for "Error when creating child process" dialog only.
	if ((win_data==NULL) || style==20 || style==18 || style==22 || style==17 )
		gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

	// g_debug("Setting if the dialog is resizable");
	// resizable
	switch (style)
	{
		case 1:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 20:
		case 21:
		case 22:
			gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
			break;
		case 2:
		case 8:
		case 19:
			gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
			break;
	}
	
	// dialog layout
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
		case 8:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			gtk_container_set_border_width (GTK_CONTAINER (dialog), 10);
			break;
		case 5:
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			gtk_container_set_border_width (GTK_CONTAINER (dialog), 15);
			break;
	}

	// set default response when press enter in entry.
	if (style==22)
		gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_CANCEL);
	else
		gtk_dialog_set_default_response(GTK_DIALOG(dialog), GTK_RESPONSE_OK);

	main_hbox = gtk_hbox_new (FALSE, 5);
	gtk_container_add (GTK_CONTAINER (GTK_DIALOG(dialog)->vbox), main_hbox);
	main_right_vbox = gtk_vbox_new (FALSE, 0);
	gtk_box_pack_end (GTK_BOX(main_hbox), main_right_vbox, FALSE, FALSE, 0);

	// information icon, it should be place at the top of iconvbox
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
		case 8:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			icon_vbox = gtk_vbox_new (FALSE, 30);
			gtk_box_pack_start (GTK_BOX(main_hbox), icon_vbox, FALSE, FALSE, 10);
			switch (style)
			{
				case 1:
				case 2:
				case 4:
				case 8:
				case 19:
				case 20:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO,
									 GTK_ICON_SIZE_DIALOG);
					break;
				case 3:
				case 7:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION,
									 GTK_ICON_SIZE_DIALOG);
					break;
				case 6:
				case 17:
				case 18:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_ERROR,
									 GTK_ICON_SIZE_DIALOG);
					break;
				case 21:
				case 22:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_WARNING,
									 GTK_ICON_SIZE_DIALOG);
					break;
					
			}
			gtk_box_pack_start (GTK_BOX(icon_vbox), icon, FALSE, FALSE, 10);
			break;
	}

	state_vbox = gtk_vbox_new (FALSE, 15);
	gtk_box_pack_start (GTK_BOX(main_hbox), state_vbox, TRUE, TRUE, 0);

	title_hbox = gtk_hbox_new (FALSE, 0);
	gtk_box_pack_start (GTK_BOX(state_vbox), title_hbox, FALSE, FALSE, 0);

	// the explain of this dialog
	switch (style)
	{
		case 1:
			string = _("Please input the Label Name of this tab:");
			break;
		case 2:
			string = _("Change the Saturation of background:");
			break;
		case 3:
			string = _("You are about to close multi tabs. Continue anyway?");
			break;
		case 4:
			string = _("You may press any key here to get the key value for using in profile.\n\n"
				   "Press [Copy] to copy the key value to clipboard;\n"
				   "And press [Close] to exit.");
			break;
		case 5:
		{
			// make the help_msg to be fixed width
			GString *help_msg = got_help_message();
			temp_str[1] = g_markup_escape_text(help_msg->str, -1);
			temp_str[0] = g_strconcat("<tt>", temp_str[1], "</tt>", NULL);
#ifdef DETAIL
			g_debug("* free help_msg %p (%s) for \"the explain of this dialog\"",
				help_msg, help_msg->str);
#endif
			g_string_free(help_msg, TRUE);
			break;
		}
		case 6:
		{
			gchar *profile = get_colorful_profile(win_data);

			// For easier to translate.
			temp_str[1] = g_strdup_printf(_("Error while writing profile '%s':"), profile);
			temp_str[0] = g_strconcat(temp_str[1], "\n\n\t", win_data->err_str.string, "\n", NULL);

			g_free(profile);
			break;
		}
		case 7:
		{
			// get the command line of running foreground program
			gchar *command_line = get_cmdline(get_tpgid(page_data->pid));
			gchar *page_no = g_strdup_printf(_("There is still a running foreground program on #%d tab:"), page_data->page_no+1);
			temp_str[1] = g_markup_escape_text(command_line, -1);
			temp_str[0] = g_strconcat(page_no,
						  "\n\n\t<b><span foreground=\"blue\">",
						  temp_str[1],
						  "</span></b>\n\n",
						  _("Continue anyway?"),
						  NULL);
			g_free(command_line);
			g_free(page_no);
			break;
		}
		case 8:
			string = _("Change the Opacity of window:");
			break;
		case 9:
		case 10:
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
			dialog_data->adjustment = gtk_color_selection_new();
			// save the color data first
			g_object_set_data(G_OBJECT(dialog_data->adjustment), "Color_Data", color_data);
			gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(dialog_data->adjustment),
								    FALSE);
			gtk_color_selection_set_has_palette(GTK_COLOR_SELECTION(dialog_data->adjustment), FALSE);
			switch (style)
			{
				// store the current using color in (GdkColor)original_color and
				// (gchar *)original_page_color for restoring later.
				case 9:
					color_data->original_color = win_data->fg_color;
					break;
				case 10:
					color_data->original_color = win_data->bg_color;
					break;
				case 11:
				case 12:
				case 13:
				case 14:
				case 15:
				case 16:
					color_data->original_page_color = win_data->user_page_color[style-11];
					gdk_color_parse(win_data->user_page_color[style-11], &(color_data->original_color));
					break;
			}
			color_data->type = style;
			color_data->recover = FALSE;
			
			// set the previous/current color of gtk_color_selection dialog
			gtk_color_selection_set_previous_color (GTK_COLOR_SELECTION(dialog_data->adjustment),
								&(color_data->original_color));
			gtk_color_selection_set_current_color ( GTK_COLOR_SELECTION(dialog_data->adjustment),
								&(color_data->original_color));
			
			gtk_box_pack_start (GTK_BOX(title_hbox), dialog_data->adjustment, TRUE, TRUE, 0);
			break;
		case 17:
		{
			gchar *local_err_str = convert_str_to_utf8(win_data->err_str.string,
								   win_data->err_str.encoding);
			if (local_err_str)
				temp_str[1] = g_markup_escape_text(local_err_str, -1);
			else
				temp_str[1] = g_markup_escape_text(win_data->err_str.string, -1);
			g_free(local_err_str);
			temp_str[0] = g_strconcat(_("Error while creating the child process:"),
						  "\n\n\t<b><span foreground=\"",
						  "blue",
						  "\">",
						  temp_str[1],
						  "</span></b>",
						  NULL);
			break;
		}
		case 19:
			string = _("Adjust the brightness of ANSI colors used in terminal");
			break;
		case 20:
		{
			gchar *profile = get_colorful_profile(win_data);

			temp_str[1] = g_strdup(_("Some entry in profile is added, removed, or changed in this version.\n"
						 "Please use [Save settings] on the right click menu to save your settings,\n"
						 "and edit it manually if necessary.\n"));
			temp_str[0] = g_strdup_printf(_("%s\nYour profile is %s."),
								temp_str[1], profile);
			g_free(profile);
			break;
		}
		case 21:
			temp_str[0] = g_strdup_printf(_("The data got from socket seems incorrect.\n"
							"If you just updated %s recently,\n"
							"Please close all the windows of %s and try again."),
							PACKAGE_NAME, PACKAGE_NAME);
			break;
		case 22:
		{
			// g_debug("err_str.string = %s", err_str.string);
			// g_debug("err_str.encoding = %s", err_str.encoding);
			gchar *local_err_str = convert_str_to_utf8(err_str.string, err_str.encoding);
			if (local_err_str)
				temp_str[1] = g_markup_escape_text(local_err_str, -1);
			else
				temp_str[1] = g_markup_escape_text(err_str.string, -1);
			g_free(local_err_str);
			temp_str[0] = g_strconcat(PACKAGE_NAME, _("is trying to execute the following command:"),
						  "\n\n\t<b><span foreground=\"", "blue", "\">",temp_str[1] ,"</span></b>\n\n",
						  _("Continue anyway?"), NULL);
			// g_debug("temp_str[0] = %s", temp_str[0]);
			break;
		}
	}

	// title
	// g_debug("Setting the title");
	title = gtk_label_new (NULL);
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 8:
		case 19:
			gtk_label_set_markup (GTK_LABEL (title), string);
			break;
		case 5:
		case 6:
		case 7:
		case 17:
		case 20:
		case 21:
		case 22:
			// set the label to be markuped
			gtk_label_set_markup (GTK_LABEL (title), temp_str[0]);
			if (style == 5)
				gtk_label_set_selectable(GTK_LABEL(title), TRUE);
			if (style==7 || style==22)
			{
				gtk_label_set_max_width_chars (GTK_LABEL(title), 50);
				gtk_label_set_ellipsize(GTK_LABEL(title), PANGO_ELLIPSIZE_MIDDLE);
			}
			break;
		case 18:
			gtk_label_set_markup (GTK_LABEL (title), err_str.string);
			break;
	}
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 5:
		case 6:
		case 7:
		case 8:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			gtk_box_pack_start (GTK_BOX(title_hbox), title, FALSE, FALSE, 0);
	}

	// state_bottom_hbox
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 6:
		case 7:
		case 8:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
			state_bottom_hbox = gtk_hbox_new (FALSE, 3);
			gtk_box_pack_end (GTK_BOX(state_vbox), state_bottom_hbox, FALSE, FALSE, 0);
			break;
	}

	// state_vbox
	switch (style)
	{
		case 1:
		case 2:
		case 4:
		case 8:
		case 19:
			entry_hbox = gtk_hbox_new (FALSE, 5);
			gtk_box_pack_start (GTK_BOX(state_vbox), entry_hbox, TRUE, TRUE, 10);
			break;
	}

	// entry_hbox
	switch (style)
	{
		case 1:
			label = gtk_label_new (_("Label Name:"));
			gtk_box_pack_start (GTK_BOX(entry_hbox), label, FALSE, FALSE, 0);
			// entry area
			entry = gtk_entry_new ();
			if (page_data->custom_page_name == NULL)
				gtk_entry_set_text(GTK_ENTRY(entry), page_data->page_name);
			else
				gtk_entry_set_text(GTK_ENTRY(entry), page_data->custom_page_name);
			gtk_box_pack_start (GTK_BOX (entry_hbox), entry, TRUE, TRUE, 0);
			// set activate = TRUE when [enter] pressed in entry
			gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
			break;
		case 2:
		case 8:
		case 19:
		{
			GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
			gtk_box_pack_start (GTK_BOX(entry_hbox), hbox, FALSE, FALSE, 10);
			switch (style)
			{
				case 2:
				case 8:
					dialog_data->adjustment = gtk_hscale_new_with_range(0, 1, 0.001);
					break;
				case 19:
					dialog_data->adjustment = gtk_hscale_new_with_range(-1, 1, 0.001);
					break;
			}
			gtk_widget_set_size_request(dialog_data->adjustment, 210, -1);
			switch (style)
			{
				case 2:
					gtk_range_set_value(GTK_RANGE(dialog_data->adjustment),
							    win_data->background_saturation);
					g_signal_connect_after( dialog_data->adjustment,
								"change-value",
								G_CALLBACK(set_background_saturation),
								win_data->current_vte);
					break;
#ifdef ENABLE_RGBA
				case 8:
					gtk_range_set_value(GTK_RANGE(dialog_data->adjustment),
							    win_data->window_opacity);
					g_signal_connect_after( dialog_data->adjustment,
								"change-value",
								G_CALLBACK(set_window_opacity),
								page_data->window);
					break;
#endif
				case 19:
					gtk_range_set_value(GTK_RANGE(dialog_data->adjustment),
								      win_data->color_brightness);
					g_signal_connect_after( dialog_data->adjustment,
					 			"change-value",
								G_CALLBACK(set_ansi_color),
								win_data->current_vte);
					break;
			}
			gtk_box_pack_start (GTK_BOX(entry_hbox), dialog_data->adjustment, TRUE, TRUE, 0);
			hbox = gtk_hbox_new (FALSE, 0);
			gtk_box_pack_end (GTK_BOX(entry_hbox), hbox, FALSE, FALSE, 5);
			break;
		}
		case 4:
			label = gtk_label_new (_("Key Value:"));
			gtk_box_pack_start (GTK_BOX(entry_hbox), label, FALSE, FALSE, 0);
			key_value_label = gtk_label_new(NULL);
			gtk_label_set_selectable(GTK_LABEL(key_value_label), TRUE);
			gtk_box_pack_start (GTK_BOX(entry_hbox), key_value_label, FALSE, FALSE, 0);
			g_signal_connect_after( G_OBJECT(dialog), "key-press-event",
						G_CALLBACK(dialog_key_press), key_value_label);
			break;
	}

	// finish
	switch (style)
	{
		case 2:
		case 9:
		case 10:
			// Set the background color (style=10) will be no affect when using transparent_background
			// unless rgba is enabled.
			// So that we should not to use transparent background when changing the background color.
			if ((style==2) || ((style==10) && (!win_data->use_rgba)))
			{
				dialog_data->original_transparent_background = win_data->transparent_background;
				if (style == 2)
					win_data->transparent_background = TRUE;
				else
					win_data->transparent_background = FALSE;
				set_background_saturation(NULL, 0, win_data->background_saturation,
							  win_data->current_vte);
			}
			if (style==9 || style==10)
				g_signal_connect_after( dialog_data->adjustment, "color-changed",
							G_CALLBACK(adjest_vte_color),
							win_data->current_vte);
			break;
#ifdef ENABLE_RGBA
		case 8:
			dialog_data->original_transparent_window = win_data->transparent_window;
			dialog_data->original_window_opacity = win_data->window_opacity;
			win_data->transparent_window = TRUE;
			set_window_opacity( NULL, 0, win_data->window_opacity, page_data->window);
			break;
#endif
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		{
			// win_data->lost_focus = FALSE;

			gint i;
			// only bold the 1st page
			gboolean is_bold = TRUE;
			// the color for using in 1st page, can NOT be free()!
			gchar *current_color = NULL;
			// the text for using in 1st page, can NOT be free()!
			gchar *display_text = NULL;
			// vte = the 1st page of current notebook.
			// the follow code is just to store the is_bold data of 1st page... orz
			struct Page *page_data = get_page_data_from_nth_page(win_data, 0);
			dialog_data->tab_1_is_bold = page_data->is_bold;
			g_signal_connect_after( dialog_data->adjustment, "color-changed",
						G_CALLBACK(adjest_vte_color), page_data->vte);

			// add new pages to PAGE_COLOR+1.
			for (i=dialog_data->total_page;i<=PAGE_COLOR;i++)
				add_page(win_data,
					 page_data,
					 NULL,
					 NULL,
					 NULL,
					 NULL,
					 FALSE,
					 NULL);
			// we need to set the current page to 1st page after adding page.
			gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), 0);

			for (i=0;i<=PAGE_COLOR;i++)
			{
				// ste the page name and color for demo.
				struct Page *page_data = get_page_data_from_nth_page(win_data, i);
				page_data->page_update_method_orig = page_data->page_update_method;
				// g_debug ("??? Save: (%d) page_data->page_update_method = %d", i+1, page_data->page_update_method);
				if (i)
				{
					display_text = _(page_color[i-1].comment);
					current_color = win_data->user_page_color[i-1];
				}
				else
				{
					display_text = _("Bold Demo Text");
					current_color = color_data->original_page_color;
				}
				
				// get the demo_vte and demo_text.
				// the change of color will be shown immediately in 1st page and the page here.
				if ((i+10)==style)
				{
					color_data->demo_vte = page_data->vte;
					color_data->demo_text = display_text;
				}
				// g_debug("!!! updating %d page name!", i+1);
				update_page_name(page_data->window,
						 page_data->vte,
						 NULL,
						 page_data->label,
						 i+1,
						 display_text,
						 current_color,
						 FALSE,
						 is_bold,
						 FALSE,
						 NULL,
						 page_data->custom_window_title,
						 FALSE);
				// only 1st page is bold.
				is_bold = FALSE;
			}
			tab_bar_is_shown = gtk_notebook_get_show_tabs GTK_NOTEBOOK(win_data->notebook);
			if (! tab_bar_is_shown)
				hide_and_show_tabs_bar(win_data, TRUE);
			break;
		}
		case 19:
		{
			// win_data->using_custom_color = FALSE :
			// the color data of this page is not inited.
			if ( ! (win_data->using_custom_color))
			{
				// save the datas for default theme.
				win_data->color_theme_orig = win_data->color_theme;
				win_data->using_custom_color_orig = win_data->using_custom_color;
				// init the color data if user were using default config.
				// use "vte_fixed" as the default
				win_data->color_theme = g_strdup("vte_fixed");
				init_user_color(win_data);
			}
			win_data->color_brightness_orig = win_data->color_brightness;
			//gint i;
			//for (i=0; i<COLOR; i++)
			//	g_debug("win_data->color[%d] = %x, %x, %x, %x",
			//		i,
			//		win_data->color[i].pixel,
			//		win_data->color[i].red,
			//		win_data->color[i].green,
			//		win_data->color[i].blue );
			break;
		}
	}

	// We'll use gtk_dialog_run() to show the whole dialog
	// g_debug("Finish creating dialog!");
	gtk_widget_show_all (main_hbox);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
	{
		switch (style)
		{
			// style 1: change the tab's name
			case 1:
				// g_debug("Got label name: %s\n",gtk_entry_get_text(GTK_ENTRY(entry)));
#ifdef DETAIL
				g_debug("* free page_data->custom_page_name %p (%s) when changing the tab's name",
					page_data->custom_page_name, page_data->custom_page_name);
#endif
				g_free(page_data->custom_page_name);
				if (strlen(gtk_entry_get_text(GTK_ENTRY(entry))))
					// store page_data->page_name
					page_data->custom_page_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
				else
					page_data->custom_page_name = NULL;
				
				// g_debug("Get the page name = %s, color = %s",
				// 	page_data->custom_page_name, page_data->tab_color);
				if (page_data->page_update_method == 1)
					page_data->window_title_updated = 1;
				get_and_update_page_name(page_data, FALSE);

				break;
			// style  2: change the saturation of background
			case 2:
			// style  9: change the foreground color
			case 9:
			// style 10: change the background color
			case 10:
			// style 11: change the text color of window title
			case 11:
			// style 12: change the text color of cmdline
			case 12:
			// style 13: change the text color of current dir
			case 13:
			// style 14: change the text color of custom page name
			case 14:
			// style 15: change the text color of root privileges cmdline
			case 15:
			// style 16: change the text color of normal text
			case 16:
			{
				GtkWidget *tmp_vte;
				gint i;
				
				// g_debug("Setting the colors. Type = %d", style);
				switch (style)
				{
					case 2:
						win_data->background_saturation = gtk_range_get_value(GTK_RANGE(dialog_data->adjustment)) + 0.0005;
						break;
#ifdef ENABLE_GDKCOLOR_TO_STRING
					case 9:
#ifdef DETAIL
						g_debug("* free win_data->foreground_color %p (%s) in dialog.c",
							win_data->foreground_color, win_data->foreground_color);
#endif
						g_free(win_data->foreground_color);
						win_data->foreground_color = 
							gdk_color_to_string(&(color_data->fg_color));
						win_data->fg_color = color_data->fg_color;
						adjust_ansi_color_severally(&(win_data->fg_color_inactive),
									    &(win_data->fg_color),
									    win_data->color_brightness_inactive);
						break;
					case 10:
#ifdef DETAIL
						g_debug("* free win_data->background_color %p (%s) in dialog.c",
							win_data->background_color, win_data->background_color);
#endif
						g_free(win_data->background_color);
						win_data->background_color =
							gdk_color_to_string(&(color_data->bg_color));
						win_data->bg_color = color_data->bg_color;
						// FIXME: GtkColorSelection have no ALPHA-CHANGED signal.
						//	  so that the following code should be
						//	  marked for temporary.
						// background_saturation = 
						//	gtk_color_selection_get_current_alpha(
						//		GTK_COLOR_SELECTION(adjustment))/65535;
						break;
					case 11:
					case 12:
					case 13:
					case 14:
					case 15:
					case 16:
#ifdef DETAIL
						g_debug("* free win_data->user_page_color[%d] %p (%s) in dialog.c",
							style-11,
							win_data->user_page_color[style-11],
							win_data->user_page_color[style-11]);
#endif
						g_free(win_data->user_page_color[style-11]);
						win_data->user_page_color[style-11] = gdk_color_to_string(&(color_data->original_color));
						break;
#endif
				}
				if (style>=11 && style<=16)
					// recover the color of pages
					recover_page_colors(dialog, page_data->window, page_data->notebook);
				else
				{
					// change the saturation of background and foreground/background color
					for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
					{
						tmp_vte=(GtkWidget *)g_object_get_data(G_OBJECT(
									gtk_notebook_get_tab_label(
									    GTK_NOTEBOOK(win_data->notebook),
										gtk_notebook_get_nth_page(
										    GTK_NOTEBOOK(win_data->notebook),i))),
											"VteBox");
						if ((style==2) || ((style==10) && (!win_data->use_rgba)))
							set_background_saturation (NULL, 0, 
										win_data->background_saturation,
										tmp_vte);
	
						if (style==9 || style==10)
							adjest_vte_color (GTK_COLOR_SELECTION(dialog_data->adjustment), tmp_vte);
					}
				}
				break;
			}
			// style 3: confirm to close multi pages
			case 3:
			{
				// we need to destroy the dialog before confirm a working vte.
#ifdef DETAIL
				g_debug("+ destory dialog %p before confirm a working vte", dialog);
#endif
				gtk_widget_destroy(dialog);
				gint i;
				GtkWidget *tmp_vte;

				for (i=dialog_data->total_page-1;i>-1;i--)
				{
					tmp_vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
										GTK_NOTEBOOK(win_data->notebook),
										   gtk_notebook_get_nth_page(
										      GTK_NOTEBOOK(
										    	 win_data->notebook),i))),
									      "VteBox");
					if (close_page(tmp_vte, TRUE)==FALSE)
						break;
				}
				break;
			}
			
			// style 4: get function key value, copy the text to clipboard
			case 4:
				gtk_clipboard_set_text (gtk_clipboard_get (GDK_SELECTION_PRIMARY),
							gtk_label_get_text(GTK_LABEL(key_value_label)), -1);
				extern GtkClipboard *pclipboard;
				gtk_clipboard_set_text (pclipboard,
							gtk_label_get_text(GTK_LABEL(key_value_label)), -1);
				break;
#ifdef ENABLE_RGBA
			// style 8: change the opacity of window
			case 8:
				win_data->window_opacity =
					gtk_range_get_value(GTK_RANGE(dialog_data->adjustment))+0.0005;
				break;
			// style 19: change the color of ansi
			case 19:
			{
				GtkWidget *tmp_vte;
				gint i;
				for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
				{
					tmp_vte=(GtkWidget *)g_object_get_data(G_OBJECT(
								gtk_notebook_get_tab_label(
								    GTK_NOTEBOOK(win_data->notebook),
									gtk_notebook_get_nth_page(
									    GTK_NOTEBOOK(win_data->notebook),
											 i))),
										"VteBox");
					vte_terminal_set_colors(VTE_TERMINAL(tmp_vte),
								&(win_data->fg_color),
								&(win_data->bg_color),
								win_data->color,
								16);
					vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color));
				}
			}
#endif
		}
	}
	else
	{
		switch (style)
		{
			// style  2: change the saturation of background
			case 2:
			// style  9: change the foreground color
			case 9:
			// style 10: change the background color
			case 10:
				if ((style==2) || ((style==10) && (!win_data->use_rgba)))
				{
					win_data->transparent_background = dialog_data->original_transparent_background;
					// g_debug("transparent_background = %d", transparent_background);
					set_background_saturation(NULL,
								  0,
								  win_data->background_saturation,
								  win_data->current_vte);
				}
				if (style==9)
					color_data->fg_color = color_data->original_color;
				else if (style==10)
					color_data->bg_color = color_data->original_color;

				if (style!=2)
				{
					color_data->recover = TRUE;
					adjest_vte_color(GTK_COLOR_SELECTION(dialog_data->adjustment),
									     win_data->current_vte);
				}
				break;
			// style 3: confirm to close multi pages
			case 3:
#ifdef DETAIL
				g_debug("+ destory dialog %p after confirm to close multi pages.", dialog);
#endif
				gtk_widget_destroy(dialog);
				break;
#ifdef ENABLE_RGBA
			// style  8: change the opacity of window
			case 8:
				win_data->transparent_window = dialog_data->original_transparent_window;
				win_data->window_opacity = dialog_data->original_window_opacity;
				set_window_opacity( NULL, 0, win_data->window_opacity, page_data->window);
				break;
#endif
			// style 11: change the text color of window title
			case 11:
			// style 12: change the text color of cmdline
			case 12:
			// style 13: change the text color of current dir
			case 13:
			// style 14: change the text color of custom page name
			case 14:
			// style 15: change the text color of root privileges cmdline
			case 15:
			// style 16: change the text color of normal text
			case 16:
				recover_page_colors(dialog, page_data->window, page_data->notebook);
				break;
			case 19:
				// FIXME: How to recover for the "win_data->color_theme = NULL"?
				// save the datas for default theme.
				if ( ! (win_data->using_custom_color_orig))
				{
#ifdef DETAIL
					g_debug("* free win_data->color_theme %p (%s) in dialog()",
						win_data->color_theme, win_data->color_theme);
#endif
					g_free(win_data->color_theme);
					win_data->color_theme = win_data->color_theme_orig;
					win_data->using_custom_color = win_data->using_custom_color_orig;
				}
				win_data->color_brightness = win_data->color_brightness_orig;
				adjust_ansi_color(win_data->color,
						  win_data->color_orig,
						  win_data->color_brightness);
				vte_terminal_set_colors(VTE_TERMINAL(win_data->current_vte),
							&(win_data->fg_color),
							&(win_data->bg_color),
							win_data->color,
							16);
				vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color));
			break;

		}
		response = FALSE;
	}

	if (style==5)
		g_free(string);

	// free string datas
#ifdef DETAIL
	g_debug("* free temp_str[0] %p (%s) in dialog()", temp_str[0], temp_str[0]);
#endif
	g_free(temp_str[0]);
#ifdef DETAIL
	g_debug("* free temp_str[1] %p (%s) in dialog()", temp_str[1], temp_str[1]);
#endif
	g_free(temp_str[1]);

	// destroy dialog.
	if (style !=3)
	{
#ifdef DETAIL
		g_debug("+ destory dialog %p !", dialog);
#endif
		gtk_widget_destroy(dialog);
	}

#ifdef DETAIL
	g_debug("* free dialog_data %p in dialog()", dialog_data);
#endif
	g_free(dialog_data);
#ifdef DETAIL
	g_debug("* free color_data %p in dialog()", color_data);
#endif
	g_free(color_data);

	if ((style >= 11) && (style <= 16))
	{
		if (!tab_bar_is_shown)
			hide_and_show_tabs_bar(win_data, FALSE);
		// win_data->lost_focus = lost_focus;
		g_free(string);
	}

	dialog_actived = FALSE;
	return response;
}

// The returned string should be freed when no longer needed.
gchar *get_colorful_profile(struct Window *win_data) 
{
	gchar *profile;
	if (win_data->use_custom_profile)
		profile = g_strdup_printf("%s", win_data->profile);
	else
		profile = g_strdup_printf("%s/%s", g_get_user_config_dir(), RCFILE);

	gchar *default_encoding = get_encoding_from_menu_item_name(win_data->default_encoding);
	// g_debug("default_encoding = %s", default_encoding);
	gchar *locale_profile = convert_str_to_utf8(profile, default_encoding);
	
	if (locale_profile)
	{
		g_free(profile);
		profile = locale_profile;
	}

	// g_debug("profile = %s", profile);
	gchar *markup_profile = g_markup_escape_text(profile, -1);
	gchar *color_profile = g_strdup_printf("<b><span foreground=\"blue\">%s</span></b>", markup_profile);

	g_free(default_encoding);
	g_free(profile);
	g_free(markup_profile);
	return color_profile;
}

gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *key_value_label)
{
#ifdef DETAIL
	g_debug("! Launch dialog_key_press() with key value = %s", gdk_keyval_name(event->keyval));
#endif

	gchar *key_value = g_strdup("");
	gint i=0;

	for (i=0; i<MOD; i++)
		if (event->state & modkeys[i].mod)
			key_value = dialog_key_press_join_string(key_value, "+", modkeys[i].name);

	if (strlen(key_value))
		key_value = dialog_key_press_join_string(key_value, " ", gdk_keyval_name(event->keyval));
	
	gchar *temp_str[2];
	temp_str[0] = g_markup_escape_text(key_value, -1);
	temp_str[1] = g_strconcat("<b><span foreground=\"", "blue", "\">",temp_str[0] ,"</span></b>", NULL);
	gtk_label_set_markup (GTK_LABEL (key_value_label), temp_str[1]);

#ifdef DETAIL
	g_debug("* free key_value %p (%s) in dialog_key_press()", key_value, key_value);
#endif
	g_free(key_value);
#ifdef DETAIL
	g_debug("* free temp_str[0] %p (%s) in dialog_key_press()", temp_str[0], temp_str[0]);
#endif
	g_free(temp_str[0]);
#ifdef DETAIL
	g_debug("* free temp_str[1] %p (%s) in dialog_key_press()", temp_str[1], temp_str[1]);
#endif
	g_free(temp_str[1]);
	return TRUE;
}

// 
gchar *dialog_key_press_join_string(gchar *key_value, gchar *separator, gchar *append)
{
#ifdef DETAIL
	g_debug("! Launch dialog_key_press_join_string() with key_value = %s, separator = %s, append = %s", key_value, separator, append);
#endif

	gchar *join_string = NULL;
	// We will not check NULL value!
	if (strlen(key_value)==0)
		join_string = g_strdup(append);
	else
		join_string = g_strdup_printf("%s%s%s",key_value, separator, append);
	
#ifdef DETAIL
	g_debug("* free key_value %p (%s) in dialog_key_press_join_string", key_value, key_value);
#endif
	g_free(key_value);
	return join_string;
}

void adjest_vte_color(GtkColorSelection *colorselection, GtkWidget *vte)
{
#ifdef DETAIL
	g_debug("! Launch adjest_vte_color() in vte %p", vte);
#endif

	// g_debug("Changing the color for vte %p", vte);
	struct ColorSelect *color_data = (struct ColorSelect*)g_object_get_data(G_OBJECT(colorselection), "Color_Data");
	
	switch (color_data->type)
	{
		case 9:
			if (! color_data->recover)
				gtk_color_selection_get_current_color (colorselection, &(color_data->fg_color));
			vte_terminal_set_color_foreground(VTE_TERMINAL(vte), &(color_data->fg_color));
			vte_terminal_set_color_bold (VTE_TERMINAL(vte), &(color_data->fg_color));
			break;
		case 10:
			if (! color_data->recover)
			{
				gtk_color_selection_get_current_color (colorselection, &(color_data->bg_color));
				// FIXME: GtkColorSelection have no ALPHA CHANGED signal.
				//	  so that the following code should be marked for temporary
				//if (use_rgba)
				//{
				//	gint alpha = gtk_color_selection_get_current_alpha(colorselection);
				//	g_debug("Current alpha = %d", alpha);
				//	vte_terminal_set_opacity(VTE_TERMINAL(vte), alpha);
				//	vte_terminal_set_background_saturation( VTE_TERMINAL(vte), 1-alpha/65535);
				//}
			}
			//else if (use_rgba)
			//	set_background_saturation(NULL, 0, background_saturation, vte);
			
			vte_terminal_set_color_background(VTE_TERMINAL(vte), &(color_data->bg_color));
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		{
			// the change of color will be shown(demo) in 1st page and demo page.
			gchar *current_color;
			gtk_color_selection_get_current_color (colorselection, &(color_data->original_color));
#ifdef ENABLE_GDKCOLOR_TO_STRING
			current_color = gdk_color_to_string(&(color_data->original_color));
#endif
			struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
			update_page_name(page_data->window, vte, NULL, page_data->label, 1,
					  _("Bold Demo Text"), current_color, FALSE, TRUE, FALSE, NULL,
					  page_data->custom_window_title, FALSE);
			
			page_data = (struct Page *)g_object_get_data(G_OBJECT(color_data->demo_vte), "Page_Data");
			update_page_name(page_data->window, color_data->demo_vte, NULL, page_data->label,
					 page_data->page_no+1, color_data->demo_text, current_color, FALSE, FALSE,
					 FALSE, NULL, page_data->custom_window_title, FALSE);
#ifdef DETAIL
			g_debug("* free current_color %p (%s) current_color in adjest_vte_color()",
				current_color, current_color);
#endif
			g_free(current_color);
			break;
		}
	}
}

void recover_page_colors(GtkWidget *dialog, GtkWidget *window, GtkWidget *notebook)
{
#ifdef DETAIL
	g_debug("! Launch recover_page_colors() in window %p", window);
#endif

	struct Dialog *dialog_data = (struct Dialog *)g_object_get_data(G_OBJECT(dialog), "Dialog_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");

	gint i;
	GtkWidget *vte;
	
	// g_debug("page_cmdline_color = %s", win_data->page_cmdline_color);
	// g_debug("page_dir_color = %s", win_data->page_dir_color);
	// g_debug("page_custom_color = %s", win_data->page_custom_color);
	// g_debug("page_root_color = %s", win_data->page_root_color);
	// g_debug("page_normal_color = %s", win_data->page_normal_color);

	// delete the tab that we add when demo
	win_data->kill_color_demo_vte = TRUE;
	for (i=PAGE_COLOR+1; i>dialog_data->total_page;i--)
	{
		vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
							GTK_NOTEBOOK(notebook),
							   gtk_notebook_get_nth_page(
							      GTK_NOTEBOOK(notebook),i-1))),
						      "VteBox");
		// g_debug("Closing %d page in recover_page_colors...", i);
		close_page (vte, TRUE);
	}
	win_data->kill_color_demo_vte = FALSE;

	// recover the title/color of pages
	// g_debug("dialog_data->total_page = %d", dialog_data->total_page);
	for (i=0; i<dialog_data->total_page;i++)
	{
		vte=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
							GTK_NOTEBOOK(notebook),
							   gtk_notebook_get_nth_page(
							      GTK_NOTEBOOK(notebook), i))),
						      "VteBox");

		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");

		// restore the is_bold of 1st page
		// g_debug("i = %d", i);
		if (i==0)
			page_data->is_bold = dialog_data->tab_1_is_bold;
		
		// g_debug("Launch get_and_update_page_name() in recover_page_colors() for %d page!", i);
		// g_debug("page_data->page_update_method = %d", page_data->page_update_method);
		page_data->page_update_method = page_data->page_update_method_orig;
		if (page_data->page_update_method == 1)
			page_data->window_title_updated = 1;
		get_and_update_page_name(page_data, FALSE);
	}
	// reset to the current page
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), dialog_data->current_page_no);
}

void error_dialog(struct Window *win_data, gchar *error_message, gchar *encoding, gint style)
{
#ifdef DETAIL
	g_debug("! Launch error_dialog() with win_data = %p, error_message = %s, style = %d",
		win_data, error_message, style);
#endif
	struct ErrorData old_err_str = {0};
	GtkWidget *old_window = NULL;

	if (style==18)
	{
		old_err_str = err_str;
		err_str.string = g_strdup(error_message);
		err_str.encoding = encoding;
	}
	else
	{
		if ((win_data==NULL))
		{
			err_str.string = g_strdup_printf("error_dialog(%d): "
							 "win_data=NULL\n\n"
							 "Please report bug to %s, Thanks!",
							 style, PACKAGE_BUGREPORT);
			error_dialog(NULL, err_str.string, encoding, 18);
#ifdef DETAIL
			g_debug("* free err_str.string %p (%s) in err_dialog()", err_str.string, err_str.string);
#endif
			g_free(err_str.string);
			err_str.string = NULL;
		}
		else
		{
			// g_debug ("win_data->temp_str = %s, error_message =%s", win_data->temp_str, error_message);
			old_err_str = win_data->err_str;
			win_data->err_str.string = error_message;
			win_data->err_str.encoding = encoding;
		}
	}
	
	// the win_data of style 18 is NULL
	if (win_data)
	{
		old_window = menu_active_window;
		menu_active_window = win_data->window;
		// g_debug("set menu_active_window = %p", menu_active_window);
	}
	dialog (NULL, style);

	// the win_data of style 18 is NULL
	if (win_data)
	{
		menu_active_window = old_window;
		// g_debug("set menu_active_window = %p", menu_active_window);
	}

	if (style==18)
	{
		g_free(err_str.string);
		err_str = old_err_str;
	}
	else
		win_data->err_str = old_err_str;
}

gboolean set_ansi_color(GtkRange *range, GtkScrollType scroll, gdouble value, GtkWidget *vte)
{
#ifdef DETAIL
	g_debug("! Launch set_ansi_color() with value = %f, vte = %p", value, vte);
#endif
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window),
								     "Win_Data");
	// g_debug("Get win_data = %d when set background saturation!", win_data);
	win_data->color_brightness = value;
	adjust_ansi_color(win_data->color, win_data->color_orig, win_data->color_brightness);
	vte_terminal_set_colors(VTE_TERMINAL(vte), &(win_data->fg_color),
				&(win_data->bg_color), win_data->color, 16);
	vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color));

	return FALSE;
}
