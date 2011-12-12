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

#include "vtefont.h"

// extern GtkWidget *current_vte;
extern GtkWidget *menu_active_window;

void set_vte_font(GtkWidget *widget, gint type)
{
#ifdef DETAIL
	g_debug("! Launch set_vte_font with type = %d", type);
#endif
	// GtkWidget *vte = current_vte;
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(menu_active_window), "Win_Data");
	GtkWidget *vte = win_data->current_vte;
	gchar *new_font_name = NULL;
	// type 0: reset current page's font size
	// type 1: increase current page's font size
	// type 2: decrease current page's font size
	// type 3: increase window size & font size for every vte
	// type 4: decrease window size & font size for every vte
	// type 5: reset window size & font size to default for every vte
	// type 6: reset window size & font size to system for every vte
	// type 7: change every vte to the selected font name

	switch (type)
	{
		case 0:
			// reset current page's font size
			new_font_name = get_resize_font(vte, 2);
			reset_vte_size(vte, new_font_name, 0);
			break;
		case 1:
			// increase current page's font size
			new_font_name = get_resize_font(vte, 6);
			reset_vte_size(vte, new_font_name, 0);
			break;
		case 2:
			// decrease current page's font size
			new_font_name = get_resize_font(vte, 7);
			reset_vte_size(vte, new_font_name, 0);
			break;
		case 3:
			// increase window size & font size for every vte
			new_font_name = get_resize_font(vte, 4);
			reset_vte_size(vte, new_font_name, 1);
			break;
		case 4:
			// decrease window size & font size for every vte
			new_font_name = get_resize_font(vte, 5);
			reset_vte_size(vte, new_font_name, 1);
			break;
		case 5:
			// reset window size & font size to default for every vte
			new_font_name = get_resize_font(vte, 0);
			reset_vte_size(vte, new_font_name, 2);
			break;
		case 6:
			// reset window size & font size to system for every vte
			new_font_name = get_resize_font(vte, 1);
			reset_vte_size(vte, new_font_name, 3);
			break;
		case 7:
			// change every vte to the selected font name
			new_font_name = get_resize_font(vte, 3);
			reset_vte_size(vte, new_font_name, 1);
			break;
	}
#ifdef DETAIL
	g_debug("* free new_font_name %p (%s) in set_vte_font()", new_font_name, new_font_name);
#endif
	g_free(new_font_name);
}

// it will update the new_font_name and page_data->font_name
gchar *get_resize_font(GtkWidget *vte, gint type)
{
#ifdef DETAIL
	g_debug("! Launch get_resize_font for vte %p with type %d", vte, type);
#endif

	// we must insure that vte!=NULL
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when get resize font!", win_data);

	// type 0: restore font to default_font_name 
	// type 1: restore font to system_font_name
	// type 2: restore font to restore_font_name
	// type 3: do nothing but only update new_font_name
	// type 4: increase window by *1.1 or +1
	// type 5: decrease window by /1.1 or -1
	// type 6: increase font by *1.1 or +1
	// type 7: decrease font by /1.1 or -1

	if (win_data->restore_font_name == NULL)
	{
		// win_data->restore_font_name = g_strdup(page_data->font_name);
		win_data->restore_font_name = g_strdup(win_data->default_font_name);
		// g_debug("Restore the font to %s!", win_data->restore_font_name);
		if (type==2)
			return g_strdup(page_data->font_name);
	}

	if (type<3)
	{
#ifdef DETAIL
		g_debug("* free page_data->font_name %p (%s) in get_resize_font() when type<3",
			page_data->font_name, page_data->font_name);
#endif
		g_free(page_data->font_name);
	}
	
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
				// g_debug("window_resize_ratio = %f",  win_data->window_resize_ratio);
				case 4:
					if (win_data->window_resize_ratio)
						page_data->font_size = page_data->font_size *
								       win_data->window_resize_ratio +
								       0.5;
					else
						page_data->font_size = page_data->font_size + PANGO_SCALE;
					break;
				case 5:
					if (win_data->window_resize_ratio)
						page_data->font_size = page_data->font_size /
								       win_data->window_resize_ratio +
								       0.5;
					else
						page_data->font_size = page_data->font_size - PANGO_SCALE;
					break;
				case 6:
					if (win_data->font_resize_ratio)
						page_data->font_size = page_data->font_size *
								       win_data->font_resize_ratio +
								       0.5;
					else
						page_data->font_size = page_data->font_size + PANGO_SCALE;
					break;
				case 7:
					if (win_data->font_resize_ratio)
						page_data->font_size = page_data->font_size /
								       win_data->font_resize_ratio +
								       0.5;
					else
						page_data->font_size = page_data->font_size - PANGO_SCALE;
					break;
			}
			// g_debug("font_size = %d", page_data->font_size);
			fontsize = (page_data->font_size)/PANGO_SCALE;
			
			// to avoid the fontsize is unchanged or = 0
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
#ifdef DETAIL
			g_debug("* free page_data->font_name %p (%s) in get_resize_font()",
				page_data->font_name, page_data->font_name);
#endif
			g_free(page_data->font_name);
			page_data->font_name = pango_font_description_to_string(font_desc);
			break;
		}
	}

	// g_debug("new font name: %s\n", page_data->font_name);

	if (type<6 && type!=2)
	{
		// We need to update "win_data->restore_font_name"
		// if not using <Ctrl><+ - enter> to change the font size.
#ifdef DETAIL
		g_debug("* free win_data->restore_font_name %p (%s) in get_resize_font()",
			win_data->restore_font_name, win_data->restore_font_name);
#endif
		g_free(win_data->restore_font_name);
		win_data->restore_font_name = g_strdup(page_data->font_name);
	}
	else if (type >=6)
	{
		// Check if we can specify page_data->font_size = 0
		if ( ! strcmp(page_data->font_name, win_data->restore_font_name))
		{
			page_data->font_size = 0;
			// g_debug("The font is restored to win_data->restore_font_name");
		}

	}
	return g_strdup(page_data->font_name);
}

void reset_vte_size(GtkWidget *vte, gchar *new_font_name, gint type)
{
#ifdef DETAIL
	g_debug("! Launch reset_vte_size() with vte = %p, new_font_name = %s, type = %d",
		vte, new_font_name, type);
#endif

	// type 0: change current page's font
	// type 1: apply current column & row to every vte
	// type 2: apply default column & row to every vte
	// type 3: apply system column & row to every vte

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when reset vte size!", win_data);

	switch (type)
	{
		case 0:
			// We need to apply a new font to a single vte.
			// so that we should insure that this won't chage the size of window.
			// g_debug("Trying to apply font %s to vte\n", current_font_name);
			vte_terminal_set_font_from_string_full( VTE_TERMINAL(vte),
								new_font_name,
								win_data->font_anti_alias);
			update_window_hint(win_data, page_data);
			break;
		case 1:
			// increase/decrease window size & font size for every vte
			// g_debug("Trying to apply font %s to every vte\n", current_font_name);
			// struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vte), "Page_Data");
			apply_font_to_every_vte( page_data->window, new_font_name,
					vte_terminal_get_column_count(VTE_TERMINAL(win_data->current_vte)),
					vte_terminal_get_row_count(VTE_TERMINAL(win_data->current_vte)));
			break;
		case 2:
			// reset window size & font size for every vte
			// g_debug("Trying to apply font %s to every vte\n", current_font_name);
			apply_font_to_every_vte(page_data->window, new_font_name,
						   win_data->default_column, win_data->default_row);
			break;
		case 3:
			// reset window size & font size for every vte
			// g_debug("Trying to apply font %s to every vte\n", current_font_name);
			apply_font_to_every_vte(page_data->window, new_font_name,
						   win_data->system_column, win_data->system_row);
			break;
	}
}

void apply_font_to_every_vte(GtkWidget *window, gchar *new_font_name, gint column, gint row)
{
#ifdef DETAIL
	g_debug("! Launch apply_font_to_every_vte() with window = %p, new_font_name = %s,"
		" column = %d, row = %d", window, new_font_name, column, row);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when apply font to every vte!", win_data);

	struct Page *page_data = NULL;
	gint i;

	// g_debug("Trying to apply every vte to %dx%d!", column, row);
	// g_debug("Trying to apply font %s to every vte!\n", new_font_name);
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		page_data = get_page_data_from_nth_page(win_data, i);
		// g_debug("The default font for %d page is: %s (%s)\n", i, page_data->font_name, new_font_name);
		vte_terminal_set_font_from_string_full(VTE_TERMINAL(page_data->vte),
						       new_font_name,
						       win_data->font_anti_alias);
		vte_terminal_set_size(VTE_TERMINAL(page_data->vte), column, row);
#ifdef DETAIL
		g_debug("* free page_data->font_name %p (%s) in apply_font_to_every_vte()",
			page_data->font_name, page_data->font_name);
#endif
		g_free(page_data->font_name);
		page_data->font_name = g_strdup(new_font_name);

		// g_debug("The new font for %d page is: %s (%s)\n", i, page_data->font_name, new_font_name);
	}

	// g_debug("Set hints to FALSE!\n");
	win_data->update_hints = 1;
	// win_data->keep_vte_size |= 0x30;
	// g_debug("window_resizable in apply_font_to_every_vte!");
	// window_resizable(window, page_data->vte, 2, 1);
	keep_window_size (win_data, page_data->vte, 0x380);
}

gboolean check_if_every_vte_is_using_restore_font_name (struct Window *win_data,
							struct Page *page_data)
{
	if (win_data->restore_font_name == NULL)
	//	win_data->restore_font_name = g_strdup(page_data->font_name);
		win_data->restore_font_name = g_strdup(win_data->default_font_name);
	
	gint i;
	gboolean return_value = TRUE;
	for (i=0;i<gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));i++)
	{
		page_data = get_page_data_from_nth_page(win_data, i);
		if (strcmp(page_data->font_name, win_data->restore_font_name))
		{
			return_value = FALSE;
			break;
		}
	}
	return return_value;
}

