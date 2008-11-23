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

// The defalut Page Name
extern gchar *page_name;
extern gboolean page_number;

extern GtkWidget *window;
extern GtkWidget *notebook;
extern GtkWidget *current_vtebox;

extern gboolean use_rgba;
#ifdef ENABLE_RGBA
extern gint transparent_window;
gboolean original_transparent_window;
extern gdouble window_opacity;
gdouble original_window_opacity;
#endif
extern gdouble background_saturation;
extern gint transparent_background;
gboolean original_transparent_background;

gint total_page;
gint current_page_no;
gchar *demo_text;
GtkWidget *demo_vtebox;
gchar *original_page_color = NULL;
extern gchar *page_cmdline_color;
extern gchar *page_dir_color;
extern gchar *page_custom_color;
extern gchar *page_normal_color;
extern gchar *page_root_color;
gboolean tab_1_is_bold;

GdkColor original_color;
extern gchar *foreground_color;
extern gchar *background_color;
GdkColor fg_color;
GdkColor bg_color;
gboolean is_fg;

GtkWidget *adjustment;

extern GError *error;

gboolean dialog (GtkWidget *widget, gint style)
{
	// g_debug("Running dialog mod%d\n", style);
	
	// style  1: change the tab's name
	// style  2: change the saturation of background
	// style  3: confirm close multi pages
	// style  4: get funuction key value
	// style  5: usage message
	// style  6: failt when saving settings
	// style  7: confirm close running application
	// style  8: change the opacity of window
	// style  9: change the foreground color
	// style 10: change the background color
	// style 11: change the text color of cmdline
	// style 12: change the text color of current dir
	// style 13: change the text color of custom page name
	// style 14: change the text color of root privileges cmdline
	// style 15: change the text color of normal text
	// style 16: change the background color of root privileges page
	
	// string: can NOT be free()
	// temp_str: SHOULD be free()
	gchar *string=NULL, *temp_str[2]= {NULL};
	GtkWidget *dialog, *main_hbox, *icon_vbox, *icon, *main_right_vbox,
		  *state_vbox, *title_hbox, *title, *label, *entry, *entry_hbox, *key_value_label,
		  *state_bottom_hbox;
	gboolean response = TRUE;

	// For change the text color of tab
	total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));

	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
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
								      GTK_WINDOW(window),
								      GTK_DIALOG_NO_SEPARATOR | 
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_OK,
								      GTK_RESPONSE_OK,
								      GTK_STOCK_CANCEL,
								      GTK_RESPONSE_CANCEL,
								      NULL);
			else
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(window),
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
								      GTK_WINDOW(window),
								      GTK_DIALOG_NO_SEPARATOR |
									GTK_DIALOG_DESTROY_WITH_PARENT,
								      GTK_STOCK_COPY,
								      GTK_RESPONSE_OK,
								      GTK_STOCK_CLOSE,
								      GTK_RESPONSE_CANCEL,
								      NULL);
			else
				dialog = gtk_dialog_new_with_buttons (string,
								      GTK_WINDOW(window),
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
			dialog = gtk_dialog_new_with_buttons (string,
							      GTK_WINDOW(window),
							      GTK_DIALOG_NO_SEPARATOR | 
								GTK_DIALOG_DESTROY_WITH_PARENT,
							      GTK_STOCK_OK,
							      GTK_RESPONSE_OK,
							      NULL);
			break;
	}
	
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
			GString *help_msg = got_help_message();
			temp_str[1] = g_markup_escape_text(help_msg->str, -1);
			temp_str[0] = g_strconcat("<tt>", temp_str[1], "</tt>", NULL);
			g_string_free(help_msg, TRUE);
			break;
		}
		case 6:
		{
			temp_str[0] = g_strdup_printf(_("Error while writing profile '%s/%s':\n\n%s"),
						   g_get_user_config_dir(), RCFILE, error->message);
			break;
		}
		case 7:
		{
			temp_str[1] = get_cmdline(get_tpgid(current_data->pid));
			temp_str[0] = g_strdup_printf(_("There is still a running foreground program on #%d tab:"
						   "\n\n\t%s\n\n"
						   "Continue anyway?\n"),
						   current_data->page_no+1, temp_str[1]);
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
			adjustment = gtk_color_selection_new();
			gtk_color_selection_set_has_opacity_control(GTK_COLOR_SELECTION(adjustment), FALSE);
			gtk_color_selection_set_has_palette(GTK_COLOR_SELECTION(adjustment), FALSE);
			switch (style)
			{
				case 9:
					original_color = fg_color;
					break;
				case 10:
					original_color = bg_color;
					break;
				case 11:
					original_page_color = page_cmdline_color;
					gdk_color_parse(page_cmdline_color, &original_color);
					break;
				case 12:
					original_page_color = page_dir_color;
					gdk_color_parse(page_dir_color, &original_color);
					break;
				case 13:
					original_page_color = page_custom_color;
					gdk_color_parse(page_custom_color, &original_color);
					break;
				case 14:
					original_page_color = page_root_color;
					gdk_color_parse(page_root_color, &original_color);
					break;
				case 15:
					original_page_color = page_normal_color;
					gdk_color_parse(page_normal_color, &original_color);
					break;
			}
			is_fg = style;
			
			gtk_color_selection_set_previous_color (GTK_COLOR_SELECTION(adjustment), &original_color);
			gtk_color_selection_set_current_color (GTK_COLOR_SELECTION(adjustment), &original_color);
			
			gtk_box_pack_start (GTK_BOX(title_hbox), adjustment, TRUE, TRUE, 0);
			g_signal_connect_after( adjustment, "color-changed",
						G_CALLBACK(set_vtebox_color), current_vtebox);
			break;
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
			title = gtk_label_new (NULL);
			gtk_label_set_markup (GTK_LABEL (title), temp_str[0]);
			gtk_label_set_selectable(GTK_LABEL(title), TRUE);
			break;
		case 6:
			title = gtk_label_new (temp_str[0]);
		case 7:
			title = gtk_label_new (temp_str[0]);
			gtk_label_set_max_width_chars (GTK_LABEL(title), 50);
			gtk_label_set_ellipsize(GTK_LABEL(title), PANGO_ELLIPSIZE_MIDDLE);
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
			if (current_data->custom_page_name == NULL)
				gtk_entry_set_text(GTK_ENTRY(entry), current_data->label->name);
			else
				gtk_entry_set_text(GTK_ENTRY(entry), current_data->custom_page_name);
			gtk_box_pack_start (GTK_BOX (entry_hbox), entry, TRUE, TRUE, 0);
			// set activate = TRUE when [enter] pressed in entry
			gtk_entry_set_activates_default(GTK_ENTRY(entry), TRUE);
			break;
		case 2:
		case 8:
		{
			GtkWidget *hbox = gtk_hbox_new (FALSE, 0);
			gtk_box_pack_start (GTK_BOX(entry_hbox), hbox, FALSE, FALSE, 10);
			adjustment = gtk_hscale_new_with_range( 0, 1, 0.001);
			gtk_widget_set_size_request(adjustment, 200, -1);
			if (style==2)
			{
				gtk_range_set_value(GTK_RANGE(adjustment), background_saturation);
				g_signal_connect_after( adjustment, "change-value",
							G_CALLBACK(set_background_saturation), current_vtebox);
			}
#ifdef ENABLE_RGBA
			else
			{
				gtk_range_set_value(GTK_RANGE(adjustment), window_opacity);
				g_signal_connect_after( adjustment, "change-value",
							G_CALLBACK(set_window_opacity), NULL);
			}
#endif
			gtk_box_pack_start (GTK_BOX(entry_hbox), adjustment, TRUE, TRUE, 0);
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
		case 10:
			if ((style==2) || ((style==10) && (!use_rgba)))
			{
				original_transparent_background = transparent_background;
				if (style == 2)
					transparent_background = TRUE;
				else
					transparent_background = FALSE;
				set_background_saturation( NULL, 0, background_saturation, current_vtebox);
			}
			break;
#ifdef ENABLE_RGBA
		case 8:
			original_transparent_window = transparent_window;
			original_window_opacity = window_opacity;
			transparent_window = TRUE;
			set_window_opacity( NULL, 0, window_opacity, NULL);
			break;
#endif
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
			{
			gint i;
			gboolean is_bold = TRUE;
			gchar *current_color;
			gchar *display_text;
			GtkWidget *vtebox;

			vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
								GTK_NOTEBOOK(notebook),
								   gtk_notebook_get_nth_page(
								      GTK_NOTEBOOK(notebook),0))),
							      "VteBox");
			struct Page *current_data = (struct Page *)g_object_get_data(
								G_OBJECT(vtebox), "Data");
			
			tab_1_is_bold = current_data->is_bold;

			for (i=total_page;i<6;i++)
				add_page(FALSE);

			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);

			for (i=0;i<6;i++)
			{
				vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
									GTK_NOTEBOOK(notebook),
									   gtk_notebook_get_nth_page(
									      GTK_NOTEBOOK(notebook),i))),
								      "VteBox");
				struct Page *current_data = (struct Page *)g_object_get_data(
									G_OBJECT(vtebox), "Data");
				
				switch (i)
				{
					case 0:
						display_text = _("Bold Demo Text");
						current_color = original_page_color;
						break;
					case 1:
						display_text = _("Running Command");
						current_color = page_cmdline_color;
						break;
					case 2:
						display_text = _("Current Dir");
						current_color = page_dir_color;
						break;
					case 3:
						display_text = _("Custom Tab Name");
						current_color = page_custom_color;
						break;
					case 4:
						display_text = _("Root Privileges");
						current_color = page_root_color;
						break;
					case 5:
						display_text = _("Normal Text");
						current_color = page_normal_color;
						break;
				}

				if ((i+10)==style)
				{
					demo_vtebox=vtebox;
					demo_text = display_text;
				}

				update_page_name(vtebox, current_data->label, i+1,
						 display_text, current_color,
						 FALSE, is_bold);

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
				g_free(current_data->custom_page_name);
				if (strlen(gtk_entry_get_text(GTK_ENTRY(entry))))
				{
					// store current_data->label->name
					current_data->custom_page_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
					current_data->tab_color = page_custom_color;

					// Update `page_name' for saving profile
					g_free(page_name);
					page_name = g_strdup(current_data->custom_page_name);
				}
				else
				{
					current_data->custom_page_name = NULL;
					current_data->tab_color = page_normal_color;
				}
				
				// g_debug("Get the page name = %s, color = %s",
				// 	current_data->custom_page_name, current_data->tab_color);
				update_page_name(current_data->vtebox, current_data->label, current_data->page_no+1,
						 current_data->custom_page_name, current_data->tab_color,
						 current_data->is_root, TRUE);
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
				
				switch (style)
				{
					case 2:
						background_saturation = gtk_range_get_value(GTK_RANGE(adjustment)) + 0.0005;
						break;
#ifdef ENABLE_GDKCOLOR_TO_STRING
					case 9:
						g_free(foreground_color);
						foreground_color = gdk_color_to_string(&fg_color);
						break;
					case 10:
						g_free(background_color);
						background_color = gdk_color_to_string(&bg_color);
						// FIXME: GtkColorSelection have no ALPHA-CHANGED signal.
						//	  so that the following code should be marked for temporary.
						// background_saturation = gtk_color_selection_get_current_alpha(
						//		 		GTK_COLOR_SELECTION(adjustment))/65535;
						break;
					case 11:
						g_free(page_cmdline_color);
						page_cmdline_color = gdk_color_to_string(&original_color);
						break;
					case 12:
						g_free(page_dir_color);
						page_dir_color = gdk_color_to_string(&original_color);
						break;
					case 13:
						g_free(page_custom_color);
						page_custom_color = gdk_color_to_string(&original_color);
						break;
					case 14:
						g_free(page_root_color);
						page_root_color = gdk_color_to_string(&original_color);
						break;
					case 15:
						g_free(page_normal_color);
						page_normal_color = gdk_color_to_string(&original_color);
						break;
#endif
				}
				if (style >10 && style < 16)
					recover_page_colors();
				else
				{
					for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));i++)
					{
						vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(
									gtk_notebook_get_tab_label(
									    GTK_NOTEBOOK(notebook),
										gtk_notebook_get_nth_page(
										    GTK_NOTEBOOK(notebook),i))),
											"VteBox");
						if ((style==2) || ((style==10) && (!use_rgba)))
							set_background_saturation (NULL, 0, 
										   background_saturation, 
										   vtebox);
	
						if (style==9 || style==10)
							set_vtebox_color (NULL, vtebox);
					}
				}
				break;
			}
			// style 3: confirm close multi pages
			case 3:
			{
				// destroy the dialog before comfirm a working vtebox.
				gtk_widget_destroy(dialog);
				gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));

				gint i;
				for (i=0;i<total_page;i++)
				{
					// g_debug("Trying to close %d vtebox !\n", i);
					if (close_page(current_vtebox, TRUE)==FALSE)
						break;
				}
				break;
			}

			// style 4: get funuction key value
			case 4:
				gtk_clipboard_set_text (gtk_clipboard_get (GDK_SELECTION_PRIMARY),
							gtk_label_get_text(GTK_LABEL(key_value_label)), -1);
				break;
#ifdef ENABLE_RGBA
			// style 8: change the opacity of window
			case 8:
				window_opacity = gtk_range_get_value(GTK_RANGE(adjustment))+0.0005;
				break;
#endif
		}
	}
	else
	{
		switch (style)
		{
			case 2:
			case 9:
			case 10:
				if ((style==2) || ((style==10) && (!use_rgba)))
				{
					transparent_background = original_transparent_background;
					// g_debug("transparent_background = %d", transparent_background);
					set_background_saturation( NULL, 0, background_saturation,
								   current_vtebox);
				}
				if (style==9)
					fg_color = original_color;
				else if (style==10)
					bg_color = original_color;

				if (style!=2)
					set_vtebox_color(NULL, current_vtebox);
				break;
			case 3:
				gtk_widget_destroy(dialog);
				break;
#ifdef ENABLE_RGBA
			case 8:
				transparent_window = original_transparent_window;
				window_opacity = original_window_opacity;
				set_window_opacity( NULL, 0, window_opacity, NULL);
				break;
#endif
			case 11:
			case 12:
			case 13:
			case 14:
			case 15:
				recover_page_colors();
				break;
		}
		response = FALSE;
	}

	g_free(temp_str[0]);
	g_free(temp_str[1]);
	if (style !=3)
		gtk_widget_destroy(dialog);
	return response;
}

gboolean dialog_key_press(GtkWidget *widget, GdkEventKey *event, GtkWidget *key_value_label)
{
	gchar *key_value = "";
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
	return TRUE;
}

gchar *dialog_key_press_join_string(gchar *key_value, gchar *separator, gchar *append)
{
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

void set_vtebox_color (GtkColorSelection *colorselection, GtkWidget *vtebox)
{
	// colorselection != NULL -> setting the vtebox color
	// colorselection != NULL -> restoring the vtebox color
	
	switch (is_fg)
	{
		case 9:
			if (colorselection != NULL)
				gtk_color_selection_get_current_color (colorselection, &fg_color);
			vte_terminal_set_color_foreground(VTE_TERMINAL(vtebox), &fg_color);
			break;
		case 10:
			if (colorselection != NULL)
			{
				gtk_color_selection_get_current_color (colorselection, &bg_color);
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
			//	set_background_saturation( NULL, 0, background_saturation, current_vtebox);
	
			vte_terminal_set_color_background(VTE_TERMINAL(vtebox), &bg_color);
			break;
		case 11:
		case 12:
		case 13:
		case 14:
		case 15:
		{
			gchar *current_color;
			gtk_color_selection_get_current_color (colorselection, &original_color);
#ifdef ENABLE_GDKCOLOR_TO_STRING
			current_color = gdk_color_to_string(&original_color);
#endif
			struct Page *current_data = (struct Page *)g_object_get_data(
								G_OBJECT(current_vtebox), "Data");

			update_page_name(current_vtebox, current_data->label, 1,
					  _("Bold Demo Text"), current_color, FALSE, TRUE);
			
			current_data = (struct Page *)g_object_get_data(G_OBJECT(demo_vtebox), "Data");
			update_page_name(demo_vtebox, current_data->label, current_data->page_no+1,
					 demo_text, current_color, FALSE, FALSE);
			g_free(current_color);
			break;
		}
	}
}

void recover_page_colors()
{
	gint i;
	GtkWidget *vtebox;
	
	//g_debug("page_cmdline_color = %s", page_cmdline_color);
	//g_debug("page_dir_color = %s", page_dir_color);
	//g_debug("page_custom_color = %s", page_custom_color);
	//g_debug("page_root_color = %s", page_root_color);
	//g_debug("page_normal_color = %s", page_normal_color);

	for (i=6; i>total_page;i--)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
							GTK_NOTEBOOK(notebook),
							   gtk_notebook_get_nth_page(
							      GTK_NOTEBOOK(notebook),i-1))),
						      "VteBox");
		close_page (vtebox, TRUE);
		// g_debug("Closing %d page...", i);
	}
	for (i=0; i<total_page;i++)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
							GTK_NOTEBOOK(notebook),
							   gtk_notebook_get_nth_page(
							      GTK_NOTEBOOK(notebook), i))),
						      "VteBox");

		struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");

		if (i==0)
			current_data->is_bold = tab_1_is_bold;

		update_tab_name(vtebox, current_data->label, current_data->pid, current_data->tpgid,
				i+1, current_data->custom_page_name, current_data->pwd,
				current_data->is_root, current_data->is_bold);
	}
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), current_page_no);
}
