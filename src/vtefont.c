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

#include "vtefont.h"

extern gchar *default_font_name;
extern gchar *system_font_name;
gchar *restore_font_name = NULL;
// For restore to default font and size
extern gint default_column;
extern gint default_row;
extern gint system_column;
extern gint system_row;

extern GtkWidget *window;
extern GtkWidget *notebook;
extern GtkWidget *current_vtebox;

gchar *new_font_name;

gboolean force_resize_window = FALSE;
gboolean update_hints=FALSE;
// 0 : User default font
// 1 : System default font

void set_vtebox_font(GtkWidget *widget, gint type)
{
	// type 0: reset current page's font size
	// type 1: increase current page's font size
	// type 2: decrease current page's font size
	// type 3: increase window size & font size for every vtebox
	// type 4: decrease window size & font size for every vtebox
	// type 5: reset window size & font size to default for every vtebox
	// type 6: reset window size & font size to system for every vtebox
	// type 7: change every vtebox to the selected font name

	switch (type)
	{
		case 0:
			// reset current page's font size
			get_resize_font(2);
			reset_vtebox_size(0);
			break;
		case 1:
			// increase current page's font size
			get_resize_font(6);
			reset_vtebox_size(0);
			break;
		case 2:
			// decrease current page's font size
			get_resize_font(7);
			reset_vtebox_size(0);
			break;
		case 3:
			// increase window size & font size for every vtebox
			get_resize_font(4);
			reset_vtebox_size(1);
			break;
		case 4:
			// decrease window size & font size for every vtebox
			get_resize_font(5);
			reset_vtebox_size(1);
			break;
		case 5:
			// reset window size & font size to default for every vtebox
			get_resize_font(0);
			reset_vtebox_size(2);
			break;
		case 6:
			// reset window size & font size to system for every vtebox
			get_resize_font(1);
			reset_vtebox_size(3);
			break;
		case 7:
			// change every vtebox to the selected font name
			get_resize_font(3);
			reset_vtebox_size(1);
			break;
	}
	g_free(new_font_name);
}

// it will update the new_font_name and current_data->font_name
void get_resize_font(gint type)
{
	// we must insure that vtebox!=NULL
	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");

	// type 0: restore font to default_font_name 
	// type 1: restore font to system_font_name
	// type 2: restore font to restore_font_name
	// type 3: do nothing but only update new_font_name
	// type 4: increase by *1.1
	// type 5: decrease by /1.1
	// type 6: increase by +1
	// type 7: decrease by -1

	if (restore_font_name == NULL)
	{
		restore_font_name = g_strdup(current_data->font_name);
		if (type==2)
			return;
	}

	if (type<3)
		g_free(current_data->font_name);
	
	switch (type)
	{
		case 0:
			// restore font to default_font_name
			current_data->font_name = g_strdup(default_font_name);
			current_data->font_size = 0;
			break;
		case 1:
			// restore font to default_font_name
			current_data->font_name = g_strdup(system_font_name);
			current_data->font_size = 0;
			break;
		case 2:
			// restore font to default_font_name
			current_data->font_name = g_strdup(restore_font_name);
			current_data->font_size = 0;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		{
			gint oldfontsize, fontsize;
			// g_debug("old font name: %s\n", current_data->font_name);
			PangoFontDescription *font_desc = pango_font_description_from_string(
								current_data->font_name);
			// increase/decrease font
			oldfontsize = (pango_font_description_get_size(font_desc)/PANGO_SCALE);
			if (current_data->font_size==0)
				current_data->font_size = pango_font_description_get_size(font_desc);
			
			switch (type)
			{
				case 4:
					current_data->font_size = current_data->font_size*1.12 + 0.5;
					break;
				case 5:
					current_data->font_size = current_data->font_size/1.12 + 0.5;
					break;
				case 6:
					current_data->font_size = current_data->font_size + PANGO_SCALE;
					break;
				case 7:
					current_data->font_size = current_data->font_size - PANGO_SCALE;
					break;
			}
			// g_debug("font_size = %d", current_data->font_size);
			fontsize = (current_data->font_size)/PANGO_SCALE;
			
			switch (type)
			{
				case 4:
				case 6:
					if (oldfontsize==fontsize)
						fontsize++;
					break;
				case 5:
				case 7:
					if (oldfontsize==fontsize)
						fontsize--;
					if (fontsize<1)
						fontsize=1;
					break;
			}
			
			// g_debug("Trying to change the font size to %d.\n", fontsize);
			pango_font_description_set_size(font_desc, fontsize*PANGO_SCALE);
			g_free(current_data->font_name);
			current_data->font_name = pango_font_description_to_string(font_desc);
			break;
		}
	}
	// g_debug("new font name: %s\n", current_data->font_name);
	new_font_name = g_strdup(current_data->font_name);
	if (type<6 && type!=2)
	{
		g_free(restore_font_name);
		restore_font_name = g_strdup(current_data->font_name);
	}
}

void reset_vtebox_size(gint type)
{
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	
	// type 0: change current page's font
	// type 1: apply current column & row to every vtebox
	// type 2: apply default column & row to every vtebox
	// type 3: apply system column & row to every vtebox

	switch (type)
	{
		case 0:
			// change current page's font
			// g_debug("Trying to apply font %s to vtebox\n", current_font_name);
			vte_terminal_set_font_from_string(VTE_TERMINAL(current_vtebox), new_font_name);
			window_resizable(current_vtebox, 2, 1);
			if (total_page==1)
				update_hints = FALSE;
			else
				update_hints = TRUE;
			break;
		case 1:
			// increase/decrease window size & font size for every vtebox
			// g_debug("Trying to apply font %s to every vtebox\n", current_font_name);
			// struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
			apply_font_to_every_vtebox( vte_terminal_get_column_count(VTE_TERMINAL(current_vtebox)),
						    vte_terminal_get_row_count(VTE_TERMINAL(current_vtebox)));
			break;
		case 2:
			// reset window size & font size for every vtebox
			// g_debug("Trying to apply font %s to every vtebox\n", current_font_name);
			apply_font_to_every_vtebox(default_column, default_row);
			break;
		case 3:
			// reset window size & font size for every vtebox
			// g_debug("Trying to apply font %s to every vtebox\n", current_font_name);
			apply_font_to_every_vtebox(system_column, system_row);
			break;
	}
}

void apply_font_to_every_vtebox(gint column, gint row)
{
	GtkWidget *vtebox;
	struct Page *current_data;
	gint i;

	// g_debug("Trying to apply every vtebox to %dx%d!", column, row);
	// g_debug("Trying to apply font %s to every vtebox!\n", new_font_name);
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));i++)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT( gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),
									gtk_notebook_get_nth_page(
										GTK_NOTEBOOK(notebook), i))),
								"VteBox");
		current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
		// g_debug("The default font for %d page is: %s (%s)\n", i, current_data->font_name, new_font_name);
		vte_terminal_set_font_from_string(VTE_TERMINAL(vtebox), new_font_name);

		vte_terminal_set_size(VTE_TERMINAL(vtebox), column, row);
		g_free(current_data->font_name);
		current_data->font_name = g_strdup(new_font_name);
		// g_debug("The new font for %d page is: %s (%s)\n", i, current_data->font_name, new_font_name);
	}

	window_resizable(current_vtebox, 1, -1);
	// Yes, just set it to 1 x 1,
	// and the window will be resized to correct geometry automatically.
	gtk_window_resize(GTK_WINDOW(window), 1, 1);

	// g_debug("Set hints to FALSE!\n");
	force_resize_window = TRUE;
	update_hints = FALSE;
}