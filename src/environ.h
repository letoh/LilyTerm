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

#ifndef ENVIRON_H
#define ENVIRON_H

#include <gtk/gtk.h>
// for strlen()
#include <string.h>
// for setenv() and unsetenv()
#include <stdlib.h>

gchar *convert_array_to_string(gchar **array, gchar separator);
void set_VTE_CJK_WIDTH_environ(gint VTE_CJK_WIDTH);
gchar *get_VTE_CJK_WIDTH_str(gint VTE_CJK_WIDTH);
gint get_default_VTE_CJK_WIDTH();
gchar *get_default_LC_TYPE();
G_CONST_RETURN char *get_encoding_from_locale();
void restore_SYSTEM_VTE_CJK_WIDTH_STR();
gchar *convert_str_to_utf8(gchar *string, gchar *encoding_str);
#endif
