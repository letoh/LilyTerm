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


#include "dialog.h"
extern GtkWidget *current_vtebox;
gchar *err_str;

gboolean dialog(GtkWidget *widget, gint style)
{
#ifdef DEBUG
	g_debug("! Launch dialog() with style = %d", style);
#endif
	GtkWidget *vtebox = current_vtebox;
	if (vtebox==NULL && style != 17)
	{
		err_str = g_strdup_printf("dialog(%d): current_vtebox = NULL\n\n"
					  "Please report bug to %s, Thanks!",
					  style, PACKAGE_BUGREPORT);
		dialog(NULL, 17);
		g_free(err_str);
		return FALSE;
	}
	struct Dialog *dialog_data = g_new0(struct Dialog, 1);
	struct ColorSelect *color_data = g_new0(struct ColorSelect, 1);
	color_data->original_page_color = NULL;

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
	// style 11: change the text color of cmdline
	// style 12: change the text color of current dir
	// style 13: change the text color of custom page name
	// style 14: change the text color of root privileges cmdline
	// style 15: change the text color of normal text
	// style 16: fault when creating the child process
	// style 17: error messages
	
	// string: can NOT be free()
	// temp_str: SHOULD be free()
	gchar *string=NULL, *temp_str[2]={NULL};
	GtkWidget *dialog = NULL, *main_hbox, *icon_vbox, *icon = NULL, *main_right_vbox,
		  *state_vbox, *title_hbox, *title = NULL, *label, *entry = NULL,
		  *entry_hbox = NULL, *key_value_label = NULL, *state_bottom_hbox;
	gboolean response = TRUE;

	struct Page *page_data = NULL;
	struct Window *win_data = NULL;
	if (vtebox!=NULL)
	{
		page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
		win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");

		// g_debug("Get win_data = %d when creating dialog!", win_data);
		// For change the text color of tab
		dialog_data->total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(page_data->notebook));
		dialog_data->current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(page_data->notebook));
	}

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
			string=_("Usage");
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
			string=_("Change the command line color on tab");
			break;
		case 12:
			string=_("Change the current directory color on tab");
			break;
		case 13:
			string=_("Change the custom tab text color");
			break;
		case 14:
			string=_("Change the root privileges color on tab");
			break;
		case 15:
			string=_("Change the normal tab text color");
			break;
		case 16:
			string=_("Error when creating child process");
			break;
		case 17:
			string=_("The following error is occurred:");
			break;
	}

	// Create the dialog window
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
		case 16:
			dialog = gtk_dialog_new_with_buttons (string,
							      GTK_WINDOW(page_data->window),
							      GTK_DIALOG_NO_SEPARATOR | 
								GTK_DIALOG_DESTROY_WITH_PARENT,
							      GTK_STOCK_OK,
							      GTK_RESPONSE_OK,
							      NULL);
			break;
		case 17:
			dialog = gtk_dialog_new_with_buttons (string,
							      NULL,
							      GTK_DIALOG_NO_SEPARATOR |
								GTK_DIALOG_DESTROY_WITH_PARENT,
							      GTK_STOCK_OK,
							      GTK_RESPONSE_OK,
							      NULL);
	}
	
	// save the dialog data first
	g_object_set_data(G_OBJECT(dialog), "Dialog_Data", dialog_data);

	// It is for "Error when creating child process" dialog only.
	if ((vtebox==NULL) || ((vtebox!=NULL) && (!GTK_WIDGET_MAPPED(page_data->window))))
		gtk_window_set_position (GTK_WINDOW (dialog), GTK_WIN_POS_CENTER);

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
			gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
			break;
		case 2:
		case 8:
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
		case 16:
		case 17:
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
			gtk_container_set_border_width (GTK_CONTAINER (dialog), 15);
			break;
	}

	// set default response when press enter in entry.
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
		case 16:
		case 17:
			icon_vbox = gtk_vbox_new (FALSE, 30);
			gtk_box_pack_start (GTK_BOX(main_hbox), icon_vbox, FALSE, FALSE, 0);
			switch (style)
			{
				case 1:
				case 2:
				case 4:
				case 8:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_INFO,
									 GTK_ICON_SIZE_DIALOG);
					break;
				case 3:
				case 7:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_QUESTION,
									 GTK_ICON_SIZE_DIALOG);
					break;
				case 6:
				case 16:
				case 17:
					icon = gtk_image_new_from_stock (GTK_STOCK_DIALOG_ERROR,
									 GTK_ICON_SIZE_DIALOG);
			}
			gtk_box_pack_start (GTK_BOX(icon_vbox), icon, FALSE, FALSE, 0);
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
			g_string_free(help_msg, TRUE);
			break;
		}
		case 6:
		{
			if (win_data->use_custom_profile)
				temp_str[0] = g_strdup_printf(_("Error while writing profile '%s':\n\n%s"),
								win_data->profile, win_data->temp_str);
			else
				temp_str[0] = g_strdup_printf(_("Error while writing profile '%s/%s':\n\n%s"),
							g_get_user_config_dir(), RCFILE, win_data->temp_str);
			break;
		}
		case 7:
		{
			// get the command line of running foreground program
			temp_str[1] = get_cmdline(get_tpgid(page_data->pid));
			temp_str[0] = g_strdup_printf(_("There is still a running foreground program on #%d tab:"
						   "\n\n\t%s\n\n"
						   "Continue anyway?\n"),
						   page_data->page_no+1, temp_str[1]);
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
					color_data->original_page_color = win_data->page_cmdline_color;
					gdk_color_parse(win_data->page_cmdline_color, &(color_data->original_color));
					break;
				case 12:
					color_data->original_page_color = win_data->page_dir_color;
					gdk_color_parse(win_data->page_dir_color, &(color_data->original_color));
					break;
				case 13:
					color_data->original_page_color = win_data->page_custom_color;
					gdk_color_parse(win_data->page_custom_color, &(color_data->original_color));
					break;
				case 14:
					color_data->original_page_color = win_data->page_root_color;
					gdk_color_parse(win_data->page_root_color, &(color_data->original_color));
					break;
				case 15:
					color_data->original_page_color = win_data->page_normal_color;
					gdk_color_parse(win_data->page_normal_color, &(color_data->original_color));
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
		case 16:
		{
			temp_str[0] = g_strdup_printf(_("Error while creating the child process:\n\n%s"),
							win_data->temp_str);
			break;
		}
	}

	// title
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 8:
			title = gtk_label_new (string);
			break;
		case 5:
			// set the label to be markuped
			title = gtk_label_new (NULL);
			gtk_label_set_markup (GTK_LABEL (title), temp_str[0]);
			gtk_label_set_selectable(GTK_LABEL(title), TRUE);
			break;
		case 6:
		case 16:
			title = gtk_label_new (temp_str[0]);
		case 7:
			// the command line may be too long. so we need to set the max_width
			title = gtk_label_new (temp_str[0]);
			gtk_label_set_max_width_chars (GTK_LABEL(title), 50);
			gtk_label_set_ellipsize(GTK_LABEL(title), PANGO_ELLIPSIZE_MIDDLE);
			break;
		case 17:
			title = gtk_label_new (err_str);
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
		case 16:
		case 17:
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
		case 16:
		case 17:
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
				gtk_entry_set_text(GTK_ENTRY(entry), page_data->label->name);
			else
				gtk_entry_set_text(GTK_ENTRY(entry), page_data->custom_page_name);
			gtk_box_pack_start (GTK_BOX (entry_hbox), entry, TRUE, TRUE, 0);
			// set activate = TRUE when [enter] pressed in entry
			gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
			break;
		case 2:
		case 8:
		{
			GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
			gtk_box_pack_start (GTK_BOX(entry_hbox), hbox, FALSE, FALSE, 10);
			dialog_data->adjustment = gtk_hscale_new_with_range( 0, 1, 0.001);
			gtk_widget_set_size_request(dialog_data->adjustment, 200, -1);
			if (style==2)
			{
				gtk_range_set_value(GTK_RANGE(dialog_data->adjustment), win_data->background_saturation);
				g_signal_connect_after( dialog_data->adjustment, "change-value",
							G_CALLBACK(set_background_saturation), vtebox);
			}
#ifdef ENABLE_RGBA
			else
			{
				gtk_range_set_value(GTK_RANGE(dialog_data->adjustment), win_data->window_opacity);
				g_signal_connect_after( dialog_data->adjustment, "change-value",
							G_CALLBACK(set_window_opacity), page_data->window);
			}
#endif
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
				set_background_saturation(NULL, 0, win_data->background_saturation, vtebox);
			}
			if (style==9 || style==10)
				g_signal_connect_after( dialog_data->adjustment, "color-changed",
							G_CALLBACK(set_vtebox_color), vtebox);
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
		{
			gint i;
			// only bold the 1st page
			gboolean is_bold = TRUE;
			// the color for using in 1st page
			gchar *current_color = NULL;
			// the text for using in 1st page
			gchar *display_text = NULL;
			// vtebox = the 1st page of current notebook.
			// the follow code is just to store the is_bold data of 1st page... orz
			GtkWidget *vtebox = (GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
								GTK_NOTEBOOK(win_data->notebook),
								   gtk_notebook_get_nth_page(
								      GTK_NOTEBOOK(win_data->notebook),0))),
							      "VteBox");
			struct Page *page_data = (struct Page *)g_object_get_data(
								G_OBJECT(vtebox), "Page_Data");
			dialog_data->tab_1_is_bold = page_data->is_bold;
			g_signal_connect_after( dialog_data->adjustment, "color-changed",
						G_CALLBACK(set_vtebox_color), vtebox);
			dialog_data->kill_color_demo_vte = &(win_data->kill_color_demo_vte);

			// add new pages to 6.
			for (i=dialog_data->total_page;i<6;i++)
				add_page(page_data->window, page_data->notebook, NULL, NULL, FALSE);
			// we need to set the current page to 1st page after adding page.
			gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), 0);

			for (i=0;i<6;i++)
			{
				// ste the page name and color for demo.
				vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
									GTK_NOTEBOOK(win_data->notebook),
									   gtk_notebook_get_nth_page(
									      GTK_NOTEBOOK(win_data->notebook),
											   i))),
								      "VteBox");
				struct Page *page_data = (struct Page *)g_object_get_data(
									G_OBJECT(vtebox), "Page_Data");
				
				switch (i)
				{
					case 0:
						display_text = _("Bold Demo Text");
						current_color = color_data->original_page_color;
						break;
					case 1:
						display_text = _("Running Command");
						current_color = win_data->page_cmdline_color;
						break;
					case 2:
						display_text = _("Current Dir");
						current_color = win_data->page_dir_color;
						break;
					case 3:
						display_text = _("Custom Tab Name");
						current_color = win_data->page_custom_color;
						break;
					case 4:
						display_text = _("Root Privileges");
						current_color = win_data->page_root_color;
						break;
					case 5:
						display_text = _("Normal Text");
						current_color = win_data->page_normal_color;
						break;
				}
				
				// get the demo_vtebox and demo_text.
				// the change of color will be shown immediately in 1st page and the page here.
				if ((i+10)==style)
				{
					color_data->demo_vtebox = vtebox;
					color_data->demo_text = display_text;
				}
				
				update_page_name(page_data->window, vtebox, page_data->label, i+1, display_text,
						 current_color, FALSE, is_bold, FALSE, NULL);
				// only 1st page is bold.
				is_bold = FALSE;
			}
			break;
		}
	}

	// We'll use gtk_dialog_run() to show the whole dialog
	gtk_widget_show_all (main_hbox);

	if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_OK)
	{
		switch (style)
		{
			// style 1: change the tab's name
			case 1:
				// g_debug("Got label name: %s\n",gtk_entry_get_text(GTK_ENTRY(entry)));
				g_free(page_data->custom_page_name);
				if (strlen(gtk_entry_get_text(GTK_ENTRY(entry))))
				{
					// store page_data->label->name
					page_data->custom_page_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
					page_data->tab_color = win_data->page_custom_color;
				}
				else
				{
					page_data->custom_page_name = NULL;
					page_data->tab_color = win_data->page_normal_color;
				}
				
				// g_debug("Get the page name = %s, color = %s",
				// 	page_data->custom_page_name, page_data->tab_color);
				update_page_name(page_data->window, page_data->vtebox, page_data->label,
						 page_data->page_no+1, page_data->custom_page_name,
						 page_data->tab_color, page_data->is_root, TRUE,
						 page_data->encoding != win_data->default_encoding, page_data->encoding);
				break;
			// style  2: change the saturation of background
			case 2:
			// style  9: change the foreground color
			case 9:
			// style 10: change the background color
			case 10:
			// style 11: change the text color of cmdline
			case 11:
			// style 12: change the text color of current dir
			case 12:
			// style 13: change the text color of custom page name
			case 13:
			// style 14: change the text color of root privileges cmdline
			case 14:
			// style 15: change the text color of normal text
			case 15:
			{
				GtkWidget *vtebox;
				gint i;
				
				// g_debug("Setting the colors. Type = %d", style);
				switch (style)
				{
					case 2:
						win_data->background_saturation = gtk_range_get_value(GTK_RANGE(dialog_data->adjustment)) + 0.0005;
						break;
#ifdef ENABLE_GDKCOLOR_TO_STRING
					case 9:
						g_free(win_data->foreground_color);
						win_data->foreground_color = gdk_color_to_string(&(color_data->fg_color));
						win_data->fg_color = color_data->fg_color;
						break;
					case 10:
						g_free(win_data->background_color);
						win_data->background_color = gdk_color_to_string(&(color_data->bg_color));
						win_data->bg_color = color_data->bg_color;
						// FIXME: GtkColorSelection have no ALPHA-CHANGED signal.
						//	  so that the following code should be marked for temporary.
						// background_saturation = gtk_color_selection_get_current_alpha(
						//		 		GTK_COLOR_SELECTION(adjustment))/65535;
						break;
					case 11:
						g_free(win_data->page_cmdline_color);
						win_data->page_cmdline_color = gdk_color_to_string(&(color_data->original_color));
						break;
					case 12:
						g_free(win_data->page_dir_color);
						win_data->page_dir_color = gdk_color_to_string(&(color_data->original_color));
						break;
					case 13:
						g_free(win_data->page_custom_color);
						win_data->page_custom_color = gdk_color_to_string(&(color_data->original_color));
						break;
					case 14:
						g_free(win_data->page_root_color);
						win_data->page_root_color = gdk_color_to_string(&(color_data->original_color));
						break;
					case 15:
						g_free(win_data->page_normal_color);
						win_data->page_normal_color = gdk_color_to_string(&(color_data->original_color));
						break;
#endif
				}
				if (style >10 && style < 16)
					// recover the color of pages
					recover_page_colors(dialog, page_data->window, page_data->notebook);
				else
				{
					// change the saturation of background and foreground/background color
					for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
					{
						vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(
									gtk_notebook_get_tab_label(
									    GTK_NOTEBOOK(win_data->notebook),
										gtk_notebook_get_nth_page(
										    GTK_NOTEBOOK(win_data->notebook),i))),
											"VteBox");
						if ((style==2) || ((style==10) && (!win_data->use_rgba)))
							set_background_saturation (NULL, 0, 
										win_data->background_saturation,
										vtebox);
	
						if (style==9 || style==10)
							set_vtebox_color (GTK_COLOR_SELECTION(dialog_data->adjustment), vtebox);
					}
				}
				break;
			}
			// style 3: confirm to close multi pages
			case 3:
			{
				// we need to destroy the dialog before confirm a working vtebox.
				gtk_widget_destroy(dialog);
				gint i;
				GtkWidget *vtebox;

				for (i=dialog_data->total_page-1;i>-1;i--)
				{
					vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
										GTK_NOTEBOOK(win_data->notebook),
										   gtk_notebook_get_nth_page(
										      GTK_NOTEBOOK(
										    	 win_data->notebook),i))),
									      "VteBox");
					if (close_page(vtebox, TRUE)==FALSE)
						break;
				}
				break;
			}
			
			// style 4: get function key value, copy the text to clipboard
			case 4:
				gtk_clipboard_set_text (gtk_clipboard_get (GDK_SELECTION_PRIMARY),
							gtk_label_get_text(GTK_LABEL(key_value_label)), -1);
				break;
#ifdef ENABLE_RGBA
			// style 8: change the opacity of window
			case 8:
				win_data->window_opacity = gtk_range_get_value(GTK_RANGE(dialog_data->adjustment))+0.0005;
				break;
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
					set_background_saturation(NULL, 0, win_data->background_saturation, vtebox);
				}
				if (style==9)
					color_data->fg_color = color_data->original_color;
				else if (style==10)
					color_data->bg_color = color_data->original_color;

				if (style!=2)
				{
					color_data->recover = TRUE;
					set_vtebox_color(GTK_COLOR_SELECTION(dialog_data->adjustment), vtebox);
				}
				break;
			// style 3: confirm to close multi pages
			case 3:
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
			// style 11: change the text color of cmdline
			case 11:
			// style 12: change the text color of current dir
			case 12:
			// style 13: change the text color of custom page name
			case 13:
			// style 14: change the text color of root privileges cmdline
			case 14:
			// style 15: change the text color of normal text
			case 15:
				recover_page_colors(dialog, page_data->window, page_data->notebook);
				break;
		}
		response = FALSE;
	}

	// free string datas
	g_free(temp_str[0]);
	g_free(temp_str[1]);
	// destroy dialog.
	if (style !=3)
		gtk_widget_destroy(dialog);
	g_free(dialog_data);
	g_free(color_data);
	return response;
}

gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *key_value_label)
{
#ifdef DEBUG
	g_debug("! Launch dialog_key_press() with key value = %s", gdk_keyval_name(event->keyval));
#endif

	gchar *key_value = g_strdup("");
	if (event->state & GDK_CONTROL_MASK)
		key_value = dialog_key_press_join_string(key_value, "+", "Ctrl");
	if (event->state & GDK_SHIFT_MASK)
		key_value = dialog_key_press_join_string(key_value, "+", "Shift");
	if (event->state & GDK_MOD1_MASK)
		key_value = dialog_key_press_join_string(key_value, "+", "Alt");
	if (event->state & GDK_MOD3_MASK)
		key_value = dialog_key_press_join_string(key_value, "+", "MOD3");
	if (event->state & GDK_MOD4_MASK)
		key_value = dialog_key_press_join_string(key_value, "+", "MOD4");
	if (event->state & GDK_MOD5_MASK)
		key_value = dialog_key_press_join_string(key_value, "+", "MOD5");
	
	if (strlen(key_value))
		key_value = dialog_key_press_join_string(key_value, " ", gdk_keyval_name(event->keyval));

	gtk_label_set_text(GTK_LABEL(key_value_label), key_value);
	g_free(key_value);
	return TRUE;
}

gchar *dialog_key_press_join_string(gchar *key_value, gchar *separator, gchar *append)
{
#ifdef DEBUG
	g_debug("! Launch dialog_key_press_join_string() with key_value = %s, separator = %s, append = %s", key_value, separator, append);
#endif

	gchar *join_string = NULL;
	// We will not check NULL value!
	if (strlen(key_value)==0)
		join_string = g_strdup(append);
	else
	{
		join_string = g_strdup_printf("%s%s%s",key_value, separator, append);
		g_free(key_value);
	}
	return join_string;
}

void set_vtebox_color(GtkColorSelection *colorselection, GtkWidget *vtebox)
{
#ifdef DEBUG
	g_debug("! Launch set_vtebox_color() in vte %p", vtebox);
#endif

	// g_debug("Changing the color");
	struct ColorSelect *color_data = (struct ColorSelect*)g_object_get_data(G_OBJECT(colorselection), "Color_Data");
	
	switch (color_data->type)
	{
		case 9:
			if (! color_data->recover)
				gtk_color_selection_get_current_color (colorselection, &(color_data->fg_color));
			vte_terminal_set_color_foreground(VTE_TERMINAL(vtebox), &(color_data->fg_color));
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
				//	vte_terminal_set_opacity(VTE_TERMINAL(vtebox), alpha);
				//	vte_terminal_set_background_saturation( VTE_TERMINAL(vtebox), 1-alpha/65535);
				//}
			}
			//else if (use_rgba)
			//	set_background_saturation(NULL, 0, background_saturation, vtebox);
			
			vte_terminal_set_color_background(VTE_TERMINAL(vtebox), &(color_data->bg_color));
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		{
			// the change of color will be shown(demo) in 1st page and demo page.
			gchar *current_color;
			gtk_color_selection_get_current_color (colorselection, &(color_data->original_color));
#ifdef ENABLE_GDKCOLOR_TO_STRING
			current_color = gdk_color_to_string(&(color_data->original_color));
#endif
			struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");

			update_page_name(page_data->window, vtebox, page_data->label, 1,
					  _("Bold Demo Text"), current_color, FALSE, TRUE, FALSE, NULL);
			
			page_data = (struct Page *)g_object_get_data(G_OBJECT(color_data->demo_vtebox), "Page_Data");
			update_page_name(page_data->window, color_data->demo_vtebox, page_data->label,
					 page_data->page_no+1, color_data->demo_text, current_color, FALSE, FALSE,
					 FALSE, NULL);
			g_free(current_color);
			break;
		}
	}
}

void recover_page_colors(GtkWidget *dialog, GtkWidget *window, GtkWidget *notebook)
{
#ifdef DEBUG
	g_debug("! Launch recover_page_colors() in window %p", window);
#endif

	struct Dialog *dialog_data = (struct Dialog *)g_object_get_data(G_OBJECT(dialog), "Dialog_Data");
	// struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	gint i;
	GtkWidget *vtebox;
	
	// g_debug("page_cmdline_color = %s", win_data->page_cmdline_color);
	// g_debug("page_dir_color = %s", win_data->page_dir_color);
	// g_debug("page_custom_color = %s", win_data->page_custom_color);
	// g_debug("page_root_color = %s", win_data->page_root_color);
	// g_debug("page_normal_color = %s", win_data->page_normal_color);

	// delete the tab that we add when demo
	*(dialog_data->kill_color_demo_vte) = TRUE;
	for (i=6; i>dialog_data->total_page;i--)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
							GTK_NOTEBOOK(notebook),
							   gtk_notebook_get_nth_page(
							      GTK_NOTEBOOK(notebook),i-1))),
						      "VteBox");
		// g_debug("Closing %d page in recover_page_colors...", i);
		close_page (vtebox, TRUE);
	}
	*(dialog_data->kill_color_demo_vte) = FALSE;

	// recover the title/color of pages
	// g_debug("dialog_data->total_page = %d", dialog_data->total_page);
	for (i=0; i<dialog_data->total_page;i++)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
							GTK_NOTEBOOK(notebook),
							   gtk_notebook_get_nth_page(
							      GTK_NOTEBOOK(notebook), i))),
						      "VteBox");

		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");

		// restore the is_bold of 1st page
		// g_debug("i = %d", i);
		if (i==0)
			page_data->is_bold = dialog_data->tab_1_is_bold;

		update_tab_name(page_data);
	}
	// reset to the current page
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), dialog_data->current_page_no);
}
