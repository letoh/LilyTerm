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

#include "main.h"

// For command line options.
gchar *command_line=NULL;
gchar **parameter=NULL;
gchar *profile=NULL;

GtkWidget *window;
GtkWidget *notebook;
extern GtkWidget *current_vtebox;

extern struct KeyValue pagekeys[KEYS];
gboolean enable_function_key = TRUE;

// Trying to keep the vtebox size:
// 1, When the page bar was hidden. (on_focuse), do nothing
// 2, When the page bar was shown. (on_focuse), using gtk_window_resize.
// 3, When the font was changed by right click menu. (lost_focuse), using gtk_widget_set_size_request
// 4, Increase/decrease window size. (on_focuse), using gtk_widget_set_size_request
// 5, Resotre to system/default font. (on_focuse), using gtk_widget_set_size_request
// 6, Theme has changed. (lost_focuse) (need column * row info), We save the column * row info for it when lost_focuse.
// 7, Using Dir/Cmdline on pagename. (on_focuse), do nothing

gboolean lost_focuse = FALSE;
gint style_set = 0;

int main( int   argc,
	  char *argv[] )
{
	// i18n support
	setlocale(LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	textdomain (PACKAGE);

	// deal the command line optins
	command_option(&argc, argv);

	// init the gtk+2 engine
	gtk_init(&argc, &argv);

	// create main window
	window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME);
	GdkPixbuf *icon = gdk_pixbuf_new_from_file(ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png", NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);
	if (icon) g_object_unref(icon);
	
	// get user settings. we may init RGBA so that we need create main window first.
	get_user_settings();
	
	// close application if [Close Button] clicked
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(main_quit), NULL);
	// if function key pressed
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(window_key_press), NULL);
	// if get focuse, the size of vtebox is NOT resizeable.
	g_signal_connect_after(G_OBJECT(window), "focus-in-event", G_CALLBACK(window_get_focuse), NULL);
	// if lost focuse, the size of vtebox is resizeable.
	g_signal_connect(G_OBJECT(window), "focus-out-event", G_CALLBACK(window_lost_focuse), NULL);
	// if the theme/fonts changed
	g_signal_connect_after(G_OBJECT(window), "style-set", G_CALLBACK(window_style_set), NULL);
	g_signal_connect(G_OBJECT(window), "size-allocate", G_CALLBACK(window_size_allocate), NULL);

	// create notebook
	notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(notebook), TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
	gtk_container_add(GTK_CONTAINER(window), notebook);
#ifdef ENABLE_TAB_REORDER
	g_signal_connect(G_OBJECT(notebook), "page-reordered", G_CALLBACK(reorder_page_number), NULL);
#endif

	// add a new page to notebook. run_once=TRUE.
	add_page(TRUE);

	// gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_NORTH);
	gtk_window_move (GTK_WINDOW(window), 0, 0);

	// finish!
	gtk_widget_show_all(window);

	// create menu
	// input method menu can't not be shown before windows is shown.
	create_menu();
	
	gtk_main();
	
	return 0;
}

gboolean main_quit()
{
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	if (total_page>1)
		// confirm to close multi pages.
		dialog(NULL, 3);
	else
		close_page(current_vtebox, TRUE);
	
	// It will be segmentation fault if retrun FALSE
	return TRUE;
}

void command_option(int  *argc,
		    char *argv[])
{
	gint i;
	for (i=0;i<*argc;i++)
	{
		// g_debug("%2d (Total %d): %s\n",i, *argc, argv[i]);
		if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version")))
		{
			g_print("%s %s\n", PACKAGE_NAME, VERSION);
			exit (0);
		}
		else if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")))
		{
			g_print("\n%s\n", got_help_message()->str);
			exit (0);
		}
		else if ((!strcmp(argv[i], "-p")) || (!strcmp(argv[i], "--profile")))
		{
			g_print(save_user_settings(NULL, NULL)->str);
			exit (0);
		}
		else if ((!strcmp(argv[i], "-e")) || (!strcmp(argv[i], "--execute")))
		{
			if (++i==*argc)
				g_critical("missing command after -e/--execute option!\n");
			else
			{
				command_line = argv[i];
				parameter = &(argv[i]);
				// g_debug("Command = %s %s\n", command_line, *parameter);
			}
		}
		else if ((!strcmp(argv[i], "-u")) || (!strcmp(argv[i], "--user_profile")))
		{
			if (++i==*argc)
				g_critical("missing file name after -u/--user_profile!\n");
			else
			{
				//gint j;
				//for (j=i+1;j<*argc;j++)
				//	*(argv[j]-1) = ' ';
				//*argc = i+1;
				profile = g_strdup(argv[i]);
			}
			// g_debug ("Using Profile = %s", profile);

			//for (j=0;j<*argc;j++)
			//	g_debug("%2d (Total %d): %s\n",j, *argc, argv[j]);
		}
	}
	// trying to got witch profile to use
	if (profile==NULL)
	{
		profile = g_strdup_printf("%s/%s", g_get_user_config_dir(), RCFILE);
		// g_debug("Trying to use user's profile %s...\n", profile);
		if ( ! g_file_test(profile , G_FILE_TEST_EXISTS))
		{
			// g_debug("Can not find user's profile. trying system profile...\n");
			g_free(profile);
			const char * const *system_dirs = g_get_system_config_dirs();
			for (i=0; system_dirs[i] != NULL; i++)
			{
				profile = g_strdup_printf("%s/%s", system_dirs[i], RCFILE);
				// g_debug("Trying to use %s...\n", profile);
				if ( g_file_test(profile , G_FILE_TEST_EXISTS))
					break;
				else
				{
					g_free(profile);
					profile = NULL;
				}
			}
			//if (profile==NULL)
			//	g_debug("Sorry, can not find any profile. use program defaults.\n");
		}
	}
}

GString *got_help_message()
{
	GString *help_message;
	gint j;
	const char * const *system_dirs = g_get_system_config_dirs();
	
	help_message = g_string_new("");
	g_string_append_printf( help_message,	 
					_("%s is a libvte based X Terminal Emulater.\n\n"), PACKAGE_NAME);
	g_string_append_printf( help_message,	 
					_("Use -e/--execute {Command} to run a command after %s start.\n"), PACKAGE_NAME);
	g_string_append( help_message,	_("Use -v/--version to show the version infomation.\n"));
	g_string_append( help_message,	_("Use -p/--profile to got a profile sample.\n"));
	g_string_append_printf( help_message,	 
					_("Use -u/--user_profile {%s} to use a specified profile.\n\n"), RCFILE);
	for (j=0; system_dirs[j] != NULL; j++)
		g_string_append_printf( help_message,
					_("The %s system config is: %s/%s\n"), PACKAGE_NAME, system_dirs[j], RCFILE);
	g_string_append_printf( help_message,	 
					_("And your %s profile is: %s/%s\n\n"),
								PACKAGE_NAME, g_get_user_config_dir(), RCFILE);
	g_string_append( help_message,	_("Default shortcut key: (It may custom by editing user's profile)\n\n"));
	g_string_append( help_message,  _("  * <Ctrl><`>\t\tDisable/Enable function keys\n"));
	g_string_append( help_message,	_("  * <Ctrl><T/W>\t\tAdd a New tab/Close current tab\n"));
	g_string_append( help_message,	_("  * <Ctrl><E>\t\tRename current tab\n"));
	g_string_append( help_message,	_("  * <Ctrl><PgUp/PgDn>\tSwitch to Prev/Next tab\n"));
	g_string_append( help_message,	_("  * <Ctrl><Home/End>\tSwitch to First/Last tab\n"));
	g_string_append( help_message,	_("  * <Ctrl><Left/Right>\tMove current page Forward/Backward\n"));
	g_string_append( help_message,	_("  * <Ctrl><Up/Down>\tMove current page to First/Last\n"));
	g_string_append( help_message,	_("  * <Ctrl><F1~F12>\tSwitch to 1st ~ 12th tab\n"));
	g_string_append( help_message,	_("  * <Ctrl><O>\t\tSelect all the text in the Vte Terminal box\n"));
	g_string_append( help_message,	_("  * <Ctrl><X>\t\tCopy the text to clipboard\n"));
	g_string_append( help_message,	_("  * <Ctrl><V>\t\tPast the text in clipboard\n"));
	g_string_append( help_message,  _("  * <Ctrl><+/->\t\tIncrease/Decrease the font size of current tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><Enter>\tReset the font size of current tab\n"));
	g_string_append( help_message,	_("  * <Shift><Insert>\tPast the text in primary clipboard\n"));
	g_string_append( help_message,	_("\t\t\t(i.e. Emulate a middle button mouse click to past the text)\n\n"));
	g_string_append_printf( help_message,	
					_("Please report bug to %s. Thank you for using %s!\n"),
								PACKAGE_BUGREPORT, PACKAGE);
	return help_message;
}

gboolean window_key_press(GtkWidget *widget, GdkEventKey *event, gpointer user_data)
{
	// don't check if no ctrl/shift/alt key pressed!
	if (event->state & ALL_ACCELS_MASK)
	{
		// don't check if only shift key pressed!
		if (event->state & SHIFT_ONLY_MASK)
		{
			// g_debug("ALL_ACCELS_MASK = %X\n", ALL_ACCELS_MASK);
			// g_debug("Got the function key: %s (status: %X, check: %X)\n",
			//	   gdk_keyval_name(event->keyval), event->state,
			//	   event->state|GDK_LOCK_MASK|GDK_MOD2_MASK);
			gint i, keyval, mods;
			if ((event->keyval>=GDK_a) && (event->keyval<=GDK_z))
				keyval = event->keyval - GDK_a + GDK_A;
			else
				keyval = event->keyval;
			mods = event->state|GDK_LOCK_MASK|GDK_MOD2_MASK;
			
			if (enable_function_key)
			{
				for (i=0;i<KEYS;i++)
				{
					// g_debug("Checking... key: %s (status: %X)\n",
					//	   gdk_keyval_name(pagekeys[i].key), event->state|GDK_LOCK_MASK);
					if ((mods==pagekeys[i].mods) && (keyval==pagekeys[i].key))
					{
						// deal the function key
						deal_key_press(i);
						return TRUE;
					}
				}
			}
			else
			{
				if ((mods==pagekeys[0].mods) && (keyval==pagekeys[0].key))
				{
					// deal the function key
					deal_key_press(0);
					return TRUE;
				}
			}
		}
	}
	return FALSE;
}

// Default function key:
//  [0] <Ctrl><~>		Disable/Enable function keys
//  [1] <Ctrl><T>		New tab
//  [2] <Ctrl><W>		Close current tab
//  [3] <Ctrl><E>		Rename current tab
//  [4] <Ctrl><PageUp>		Switch to prev tab
//  [5] <Ctrl><PageDown>	Switch to next tab
//  [6] <Ctrl><Home>		Switch to first tab
//  [7] <Ctrl><End>		Switch to last tab
//  [8] <Ctrl><Left>		Move current page forward
//  [9] <Ctrl><Right>		Move current page backward
// [10] <Ctrl><Up>		Move current page to first
// [11] <Ctrl><Down>		Move current page to last
// [12-23] <Ctrl><F1~F12>	Switch to 1st ~ 12th tab
// [24] <Ctrl><O>		Select all the text in the Vte Terminal box
// [25] <Ctrl><X>		Copy the text to clipboard
// [26] <Ctrl><V>		Past the text in clipboard
//	<Shift><Insert>		Pase clipboard.
void deal_key_press(gint type)
{
	// gint current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");

	switch (type)
	{
		case 0:
			enable_function_key = ! enable_function_key;
			break;
		case 1:
			// add a new page
			add_page(0);
			break;
		case 2:
			// close page
			// g_debug("Trying to close page!\n");
			close_page (current_vtebox, TRUE);
			break;
		case 3:
			// edit page's label
			dialog(NULL, 1);
			break;
		case 4:
			// switch to pre page
			if (current_data->current_page_no)
				gtk_notebook_prev_page(GTK_NOTEBOOK(notebook));
			else
				gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), total_page -1);
			break;
		case 5:
			// switch to next page
			if (current_data->current_page_no == (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook)) - 1))
				gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
			else
				gtk_notebook_next_page(GTK_NOTEBOOK(notebook));
			break;
		case 6:
			// switch to first page
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), 0);
			break;
		case 7:
			// switch to last page
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), total_page-1);
			break;
		case 8:
			// move current page forward
			gtk_notebook_reorder_child(GTK_NOTEBOOK(notebook), current_data->hbox, 
						   current_data->current_page_no -1);
			break;
		case 9:
			// move current page backward
			if (current_data->current_page_no == (gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook))-1))
				 gtk_notebook_reorder_child(GTK_NOTEBOOK(notebook), current_data->hbox, 0);
			else
				gtk_notebook_reorder_child(GTK_NOTEBOOK(notebook),
							   current_data->hbox, current_data->current_page_no+1);
			break;
		case 10:
			// move current page to first
			gtk_notebook_reorder_child(GTK_NOTEBOOK(notebook), current_data->hbox, 0);
			break;
		case 11:
			// move current page to last
			gtk_notebook_reorder_child(GTK_NOTEBOOK(notebook), current_data->hbox, -1);
			break;
		case 12:
		case 13:
		case 14:
		case 15:
		case 16:
		case 17:
		case 18:
		case 19:
		case 20:
		case 21:
		case 22:
		case 23:
			// switch to #%d page
			if (total_page > type-12)
				gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), type-12);
			break;
		case 24:
#ifdef ENABLE_SELECT_ALL
			vte_terminal_select_all(VTE_TERMINAL(current_vtebox));
#endif
			break;
		case 25:
			vte_terminal_copy_clipboard(VTE_TERMINAL(current_vtebox));
			break;
		case 26:
			vte_terminal_paste_clipboard(VTE_TERMINAL(current_vtebox));
			break;
		case 27:
			set_vtebox_font(NULL, 1);
			break;
		case 28:
			set_vtebox_font(NULL, 2);
			break;
		case 29:
			set_vtebox_font(NULL, 0);
			break;
	}
}
gboolean window_get_focuse(GtkWidget *window, GdkEventFocus *event, gpointer user_data)
{
	//g_debug("! window_get_focuse! and style_set = %d", style_set);
	//g_debug("Current %d vtebox size is %dx%d\n",
	//	gtk_notebook_get_n_pages (notebook),
	//	vte_terminal_get_column_count(VTE_TERMINAL(current_vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(current_vtebox)));
	
	if (lost_focuse && current_vtebox!=NULL && (!style_set))
		window_resizable(current_vtebox, 0, -1);

	lost_focuse = FALSE;
	return FALSE;
}

gboolean window_lost_focuse (GtkWidget *window, GdkEventFocus *event, gpointer user_data)
{
	//g_debug("! vteboxvte_lost_focuse!");
	//g_debug("Current %d vtebox size is %dx%d\n",
	//	gtk_notebook_get_n_pages (notebook),
	//	vte_terminal_get_column_count(VTE_TERMINAL(current_vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(current_vtebox)));

	if (current_vtebox!=NULL && (!style_set))
		window_resizable(current_vtebox, 0, 1);

	lost_focuse = TRUE;
	return FALSE;
}

void window_style_set (GtkWidget *window, GtkStyle *previous_style, gpointer user_data)
{
	//g_debug("window_style_set!");
	//g_debug("Current %d vtebox size is %dx%d\n",
	//	gtk_notebook_get_n_pages (notebook),
	//	vte_terminal_get_column_count(VTE_TERMINAL(current_vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(current_vtebox)));
	
	// g_debug("Updating hints for %d page to 0!\n", gtk_notebook_get_n_pages (notebook));
	window_resizable(current_vtebox, 2, -1);
	gtk_window_resize(GTK_WINDOW(window), 1, 1);
	// we need to launch window_size_allocate twice.
	style_set = 2;
}

void window_size_allocate (GtkWidget *window, GtkAllocation *allocation, gpointer user_data)
{
	//g_debug("window_size-allocate!");
	if (style_set)
	{
		style_set --;
		// g_debug("! window_size_allocate");
		
		if (style_set==0)
		{
			// g_debug("Updating hints for %d page to FONT!\n", gtk_notebook_get_n_pages (notebook));
			if (lost_focuse)
				window_resizable(current_vtebox, 1, 1);
			else
				window_resizable(current_vtebox, 1, -1);
		}
	}

	// GtkRequisition window_requisition;
	// gtk_window_get_size(GTK_WINDOW(window), &window_requisition.width, &window_requisition.height);
	// g_debug("! The final window size is %d x %d", window_requisition.width, window_requisition.height);
}
