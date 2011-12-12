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

#include "environ.h"

// The returned string should be freed when no longer needed.
gchar *convert_array_to_string(gchar **array, gchar separator)
{
	GString *array_str = g_string_new (NULL);
	gint i=0;

	if (array!=NULL)
	{
		while (array[i]!=NULL)
		{
			// g_debug("%d: %s", i, array[i]);
			if (array_str->str)
				g_string_append_printf(array_str, "%c%s", separator, array[i]);
			else
				g_string_append_printf(array_str, "%s", array[i]);
			i++;
		}
	}
	// g_debug ("Got array_str = %s", array_str->str);
	return g_string_free(array_str, FALSE);
}

void set_VTE_CJK_WIDTH_environ(gint VTE_CJK_WIDTH)
{
	// Set the VTE_CJK_WIDTH environment
	switch (VTE_CJK_WIDTH)
	{
		case 0:
			// Don't touch VTE_CJK_WIDTH
			break;
		//case 1:
		//	// The VTE_CJK_WIDTH will be setted depend on the LC environ in newer libvte version.
		//	unsetenv("VTE_CJK_WIDTH");
		//	break;
		case 1:
			// VTE_CJK_WIDTH=narrow only work for vte >= 0.16.14
			setenv("VTE_CJK_WIDTH", "narrow", TRUE);
			break;
		case 2:
			setenv("VTE_CJK_WIDTH", "wide", TRUE);
			break;
	}
}

gchar *get_VTE_CJK_WIDTH_str(gint VTE_CJK_WIDTH)
{
	switch (VTE_CJK_WIDTH)
	{
		case 0:
			return NULL;
		case 1:
			return "narrow";
		case 2:
			return "wide";
	}
	return NULL;
}

gint get_default_VTE_CJK_WIDTH()
{

	const gchar *VTE_CJK_WIDTH = g_getenv("VTE_CJK_WIDTH");
	if (VTE_CJK_WIDTH==NULL)
		return 0;
	else
	{
		// VTE_CJK_WIDTH only work under UTF-8
		if ((g_ascii_strcasecmp (VTE_CJK_WIDTH, "wide")==0) ||
		    (g_ascii_strcasecmp (VTE_CJK_WIDTH, "1")==0))
			return 2;
		else if ((g_ascii_strcasecmp (VTE_CJK_WIDTH, "narrow")==0) ||
			 (g_ascii_strcasecmp (VTE_CJK_WIDTH, "0")==0))
			return 1;
		else
			return 0;
	}
}

// get default locale from environ
// The returned string should NOT free()!
gchar *get_default_LC_TYPE()
{
	char *LC_CTYPE = (char*)g_getenv("LC_CTYPE");
	char *LC_ALL = (char*)g_getenv("LC_ALL");
	char *LANG = (char*)g_getenv("LANG");

	if ( (!LC_CTYPE) && LANG)
		LC_CTYPE = LANG;

	if (LC_ALL)
		LC_CTYPE = LC_ALL;

//	g_debug("Get LC_CTYPE = %s", LC_CTYPE);
	return LC_CTYPE;
}

// the returned string CAN NOT be free!!
G_CONST_RETURN char *get_encoding_from_locale()
{
	G_CONST_RETURN char *locale_encoding;

	g_get_charset(&locale_encoding);
	// g_debug("The locale_encoding is %s", locale_encoding);
	return locale_encoding;
}

void restore_SYSTEM_VTE_CJK_WIDTH_STR ()
{
	extern gchar *SYSTEM_VTE_CJK_WIDTH_STR;
	if (SYSTEM_VTE_CJK_WIDTH_STR)
		g_setenv("VTE_CJK_WIDTH", SYSTEM_VTE_CJK_WIDTH_STR, TRUE);
	else
		g_unsetenv("VTE_CJK_WIDTH");
}

// The returned string should be freed when no longer needed.
gchar *convert_str_to_utf8(gchar *string, gchar *encoding_str)
{
	gchar *local_string = NULL;
	
	if (encoding_str)
	{
		//g_debug("string = %s", string);
		//g_debug("encoding_str = %s", encoding_str);
		local_string = g_convert_with_fallback (string,
							-1,
							"UTF-8",
							encoding_str,
							"_",
							NULL,
							NULL,
							NULL);
		//g_debug("local_string = %s", local_string);

		if ((local_string == NULL ) || (local_string[0]=='\0'))
		{
			g_free(local_string);
			local_string = g_strdup(string);
			gint i = 0;
			while (local_string[i])
			{
				if (local_string[i] < 32 || local_string[i]>126)
					local_string[i] = '?';
				i++;
			}
		}
	}
	return local_string;
}
