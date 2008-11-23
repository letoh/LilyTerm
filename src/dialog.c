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

#ifdef ENABLE_RGBA
extern gboolean use_rgba;
extern gint transparent_window;
extern gdouble window_opacity;
gdouble original_window_opacity;
#endif
extern gdouble background_saturation;
extern gint transparent_background;
gboolean original_transparent_background;

GtkWidget *adjustment;
gboolean BOTTON_ORDER=FALSE;

gboolean dialog (GtkWidget *widget, gint style)
{
	// g_debug("Running dialog mod%d\n", style);
	
	// style 1: change the tab's name
	// style 2: change the saturation of background
	// style 3: confirm close multi pages
	// style 4: get funuction key value
	// style 5: usage message
	// style 6: profile sample
	// style 7: confirm close running application
	// style 8: change the opacity of window
	
	gchar *string=NULL, *temp_str=NULL;
	GtkWidget *dialog, *main_hbox, *icon_vbox, *icon, *main_right_vbox,
		  *state_vbox, *title_hbox, *title, *label, *entry, *entry_hbox, *key_value_label,
		  *state_bottom_hbox;
	gboolean response = TRUE;

	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
	g_object_get (gtk_settings_get_default(), "gtk-alternative-button-order", &BOTTON_ORDER, NULL);

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
			string=_("Rename the tab");
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
			string=_("Profile sample");
			break;
		case 7:
			string=_("Confirm to close running foreground program");
			break;
		case 8:
			string=_("Change the opacity of window");
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
		case 6:
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
			gtk_window_set_resizable (GTK_WINDOW (dialog), TRUE);
			break;
		case 2:
		case 8:
			gtk_window_set_resizable (GTK_WINDOW (dialog), FALSE);
			break;
	}
	
	// dialog layout
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 7:
		case 8:
			gtk_container_set_border_width (GTK_CONTAINER (dialog), 10);
			break;
		case 5:
		case 6:
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
			temp_str = got_help_message();
			gchar *markup_str = g_markup_escape_text(temp_str, -1);
			string = g_strconcat("<tt>", markup_str, "</tt>", NULL);
			g_free(markup_str);
			break;
		}
		case 6:
		{
			temp_str = got_profile_sample();
			gchar *markup_str = g_markup_escape_text(temp_str, -1);
			string = g_strconcat("<small><tt>", markup_str, "</tt></small>", NULL);
			g_free(markup_str);
			break;
		}
		case 7:
			temp_str = get_cmdline(get_tpgid(current_data->stat_path, current_data->pid));
			string = g_strdup_printf(_("There is still a running foreground program on #%d tab:"
						   "\n\n\t%s\n\n"
						   "Continue anyway?\n"),
						   current_data->current_page_no+1, temp_str);
			break;
		case 8:
			string = _("Change the Opacity of window:");
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
		case 6:
			title = gtk_label_new (NULL);
			gtk_label_set_markup (GTK_LABEL (title), string);
			gtk_label_set_selectable(GTK_LABEL(title), TRUE);
			break;
		case 7:
			title = gtk_label_new (string);
			gtk_label_set_max_width_chars (GTK_LABEL(title), 50);
			gtk_label_set_ellipsize(GTK_LABEL(title), PANGO_ELLIPSIZE_MIDDLE);
			break;
	}

	gtk_box_pack_start (GTK_BOX(title_hbox), title, FALSE, FALSE, 0);

	// state_bottom_hbox
	switch (style)
	{
		case 1:
		case 2:
		case 3:
		case 4:
		case 7:
		case 8:
			state_bottom_hbox = gtk_hbox_new (FALSE, 3);
			gtk_box_pack_end (GTK_BOX(state_vbox), state_bottom_hbox, FALSE, FALSE, 0);
			entry_hbox = gtk_hbox_new (FALSE, 5);
			break;
	}

	// state_vbox
	switch (style)
	{
		case 1:
		case 2:
		case 8:
		case 4:
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
			original_transparent_background = transparent_background;
			transparent_background = TRUE;
			set_background_saturation( NULL, 0, background_saturation, current_vtebox);
			break;
#ifdef ENABLE_RGBA
		case 8:
			original_window_opacity = window_opacity;
			transparent_window = TRUE;
			set_window_opacity( NULL, 0, window_opacity, NULL);
			break;
#endif
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
					// store current_data->label->name
					current_data->custom_page_name = g_strdup(gtk_entry_get_text(GTK_ENTRY(entry)));
				else
					current_data->custom_page_name = NULL;
				
				update_page_name(current_data->label, current_data->current_page_no+1,
						 current_data->custom_page_name);
				break;

			// style 2: change the saturation of background
			case 2:
			{
				gint i;
				background_saturation=gtk_range_get_value(GTK_RANGE(adjustment));
				for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));i++)
					set_background_saturation (NULL, 0, background_saturation,
							 g_object_get_data(G_OBJECT(
								 gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),
									gtk_notebook_get_nth_page(
										GTK_NOTEBOOK(notebook), i))),
							"VteBox"));
				break;
			}

			// style 3: confirm close multi pages
			case 3:
			{
				gint i;
				gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
				for (i=0;i<total_page;i++)
				{
					// g_debug("Trying to close %d vtebox !\n", i);
					if (close_page(NULL, TRUE)==FALSE)
						break;
				}
				break;
			}

			// style 4: get funuction key value
			case 4:
				gtk_clipboard_set_text (gtk_clipboard_get (GDK_SELECTION_PRIMARY),
							gtk_label_get_text(GTK_LABEL(key_value_label)), -1);
				break;

			// style 6: profile sample
			case 6:
				gtk_clipboard_set_text (gtk_clipboard_get (GDK_SELECTION_PRIMARY), temp_str, -1);
				break;
#ifdef ENABLE_RGBA
			// style 8: change the opacity of window
			case 8:
				window_opacity=gtk_range_get_value(GTK_RANGE(adjustment));
				break;
#endif
		}
	}
	else
	{
		switch (style)
		{
			case 2:
				transparent_background = original_transparent_background;
				set_background_saturation( NULL, 0, background_saturation, current_vtebox);
				break;
#ifdef ENABLE_RGBA
			case 8:
				window_opacity = original_window_opacity;
				set_window_opacity( NULL, 0, window_opacity, NULL);
				break;
#endif
		}
		response = FALSE;
	}

	g_free(temp_str);
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
