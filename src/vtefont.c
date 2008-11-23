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

extern GtkWidget *current_vtebox;

void set_vtebox_font(GtkWidget *widget, gint type)
{
	if (current_vtebox == NULL) return;
	GtkWidget *vtebox = current_vtebox;
	gchar *new_font_name = NULL;
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
			new_font_name = get_resize_font(vtebox, 2);
			reset_vtebox_size(vtebox, new_font_name, 0);
			break;
		case 1:
			// increase current page's font size
			new_font_name = get_resize_font(vtebox, 6);
			reset_vtebox_size(vtebox, new_font_name, 0);
			break;
		case 2:
			// decrease current page's font size
			new_font_name = get_resize_font(vtebox, 7);
			reset_vtebox_size(vtebox, new_font_name, 0);
			break;
		case 3:
			// increase window size & font size for every vtebox
			new_font_name = get_resize_font(vtebox, 4);
			reset_vtebox_size(vtebox, new_font_name, 1);
			break;
		case 4:
			// decrease window size & font size for every vtebox
			new_font_name = get_resize_font(vtebox, 5);
			reset_vtebox_size(vtebox, new_font_name, 1);
			break;
		case 5:
			// reset window size & font size to default for every vtebox
			new_font_name = get_resize_font(vtebox, 0);
			reset_vtebox_size(vtebox, new_font_name, 2);
			break;
		case 6:
			// reset window size & font size to system for every vtebox
			new_font_name = get_resize_font(vtebox, 1);
			reset_vtebox_size(vtebox, new_font_name, 3);
			break;
		case 7:
			// change every vtebox to the selected font name
			new_font_name = get_resize_font(vtebox, 3);
			reset_vtebox_size(vtebox, new_font_name, 1);
			break;
	}
	g_free(new_font_name);
}

// it will update the new_font_name and page_data->font_name
gchar *get_resize_font(GtkWidget *vtebox, gint type)
{
	// we must insure that vtebox!=NULL
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when get resize font!", win_data);

	// type 0: restore font to default_font_name 
	// type 1: restore font to system_font_name
	// type 2: restore font to restore_font_name
	// type 3: do nothing but only update new_font_name
	// type 4: increase by *1.1
	// type 5: decrease by /1.1
	// type 6: increase by +1
	// type 7: decrease by -1

	if (win_data->restore_font_name == NULL)
	{
		win_data->restore_font_name = g_strdup(page_data->font_name);
		// g_debug("Restore the font to %s!", restore_font_name);
		if (type==2)
			return g_strdup(page_data->font_name);
	}

	if (type<3)
		g_free(page_data->font_name);
	
	// we use font_size to save current font size
	// font_size = (the size in font_name) * PANGO_SCALE
	// if font_size == 0 -> use the data in font_name

	switch (type)
	{
		case 0:
			// restore font to default_font_name
			page_data->font_name = g_strdup(win_data->default_font_name);
			page_data->font_size = 0;
			break;
		case 1:
			// restore font to default_font_name
			page_data->font_name = g_strdup(win_data->system_font_name);
			page_data->font_size = 0;
			break;
		case 2:
			// restore font to default_font_name
			page_data->font_name = g_strdup(win_data->restore_font_name);
			page_data->font_size = 0;
			break;
		case 4:
		case 5:
		case 6:
		case 7:
		{
			gint oldfontsize, fontsize;
			// g_debug("old font name: %s\n", page_data->font_name);
			PangoFontDescription *font_desc = pango_font_description_from_string(
								page_data->font_name);
			// increase/decrease font
			oldfontsize = (pango_font_description_get_size(font_desc)/PANGO_SCALE);
			if (page_data->font_size==0)
				page_data->font_size = pango_font_description_get_size(font_desc);
			
			switch (type)
			{
				case 4:
					page_data->font_size = page_data->font_size*1.12 + 0.5;
					break;
				case 5:
					page_data->font_size = page_data->font_size/1.12 + 0.5;
					break;
				case 6:
					page_data->font_size = page_data->font_size + PANGO_SCALE;
					break;
				case 7:
					page_data->font_size = page_data->font_size - PANGO_SCALE;
					break;
			}
			// g_debug("font_size = %d", page_data->font_size);
			fontsize = (page_data->font_size)/PANGO_SCALE;
			
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
			g_free(page_data->font_name);
			page_data->font_name = pango_font_description_to_string(font_desc);
			break;
		}
	}
	// g_debug("new font name: %s\n", page_data->font_name);
	if (type<6 && type!=2)
	{
		g_free(win_data->restore_font_name);
		win_data->restore_font_name = g_strdup(page_data->font_name);
	}
	return g_strdup(page_data->font_name);
}

void reset_vtebox_size(GtkWidget *vtebox, gchar *new_font_name, gint type)
{
	// type 0: change current page's font
	// type 1: apply current column & row to every vtebox
	// type 2: apply default column & row to every vtebox
	// type 3: apply system column & row to every vtebox

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when reset vtebox size!", win_data);

	switch (type)
	{
		case 0:
			// We need to apply a new font to a single vtebox.
			// so that we should insure that this won't chage the size of window.
			// g_debug("Trying to apply font %s to vtebox\n", current_font_name);
			vte_terminal_set_font_from_string(VTE_TERMINAL(vtebox), new_font_name);
			
			win_data->update_hints = 2;
			// g_debug("window_resizable in change current font!");
			window_resizable(page_data->window, win_data->current_vtebox, 2, 1);

			break;
		case 1:
			// increase/decrease window size & font size for every vtebox
			// g_debug("Trying to apply font %s to every vtebox\n", current_font_name);
			// struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Page_Data");
			apply_font_to_every_vtebox( page_data->window, new_font_name,
					vte_terminal_get_column_count(VTE_TERMINAL(win_data->current_vtebox)),
					vte_terminal_get_row_count(VTE_TERMINAL(win_data->current_vtebox)));
			break;
		case 2:
			// reset window size & font size for every vtebox
			// g_debug("Trying to apply font %s to every vtebox\n", current_font_name);
			apply_font_to_every_vtebox(page_data->window, new_font_name,
						   win_data->default_column, win_data->default_row);
			break;
		case 3:
			// reset window size & font size for every vtebox
			// g_debug("Trying to apply font %s to every vtebox\n", current_font_name);
			apply_font_to_every_vtebox(page_data->window, new_font_name,
						   win_data->system_column, win_data->system_row);
			break;
	}
}

void apply_font_to_every_vtebox(GtkWidget *window, gchar *new_font_name, gint column, gint row)
{
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when apply font to every vtebox!", win_data);

	GtkWidget *vtebox = NULL;
	struct Page *page_data;
	gint i;

	// g_debug("Trying to apply every vtebox to %dx%d!", column, row);
	// g_debug("Trying to apply font %s to every vtebox!\n", new_font_name);
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(gtk_notebook_get_tab_label(
								GTK_NOTEBOOK(win_data->notebook),
									gtk_notebook_get_nth_page(
										GTK_NOTEBOOK(win_data->notebook), i))),
								"VteBox");
		page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
		// g_debug("The default font for %d page is: %s (%s)\n", i, page_data->font_name, new_font_name);
		vte_terminal_set_font_from_string(VTE_TERMINAL(vtebox), new_font_name);
		vte_terminal_set_size(VTE_TERMINAL(vtebox), column, row);
		g_free(page_data->font_name);
		page_data->font_name = g_strdup(new_font_name);

		// g_debug("The new font for %d page is: %s (%s)\n", i, page_data->font_name, new_font_name);
	}

	// g_debug("Set hints to FALSE!\n");
	win_data->update_hints = 1;
	win_data->keep_vtebox_size |= 0x30;
	// g_debug("window_resizable in apply_font_to_every_vtebox!");
	window_resizable(window, vtebox, 2, 1);
}
