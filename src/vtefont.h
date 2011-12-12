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

#ifndef VTEFONT_H
#define VTEFONT_H

#include <gtk/gtk.h>
#include <vte/vte.h>

#include "data.h"
#include "profile.h"
#include "window.h"

void set_vte_font(GtkWidget *widget, gint type);
void reset_vte_size(GtkWidget *vte, gchar *new_font_name, gint type);
gchar *get_resize_font(GtkWidget *vte, gint type);
void apply_font_to_every_vte(GtkWidget *window, gchar *new_font_name, gint colum, gint row);
gboolean check_if_every_vte_is_using_restore_font_name (struct Window *win_data,
							struct Page *page_data);
#endif
