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


#include "window.h"

// if total_window = 0, call gtk_window_quit()
gint total_window = 0;
// for update current_vtebox when window is on_focuse
extern GtkWidget *current_vtebox;
GtkWidget *active_window;

void new_window(int argc, char *argv[])
{
	total_window++;

	struct Window *win_data = g_new0(struct Window, 1);
	// g_debug ("win_data = %d", win_data);

	init_window_option(win_data);
	window_option(win_data, argc, argv);

	// create main window
	GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(window), PACKAGE_NAME);
	GdkPixbuf *icon = gdk_pixbuf_new_from_file(ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png", NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);
	if (icon) g_object_unref(icon);

	// save the data first
	// g_debug("Save window = %d and win_data = %d when init window!", window, win_data);
	g_object_set_data(G_OBJECT(window), "Win_Data", win_data);

	// get user settings. we may init RGBA so that we need create main window first.
	get_user_settings(window, win_data);
	
	// close application if [Close Button] clicked
	g_signal_connect(G_OBJECT(window), "delete_event", G_CALLBACK(window_quit), win_data);
	// if function key pressed
	g_signal_connect(G_OBJECT(window), "key-press-event", G_CALLBACK(window_key_press), win_data);
	// if get focuse, the size of vtebox is NOT resizeable.
	g_signal_connect_after(G_OBJECT(window), "focus-in-event", G_CALLBACK(window_get_focuse), win_data);
	// if lost focuse, the size of vtebox is resizeable.
	g_signal_connect(G_OBJECT(window), "focus-out-event", G_CALLBACK(window_lost_focuse), win_data);
	// if the theme/fonts changed
	g_signal_connect_after(G_OBJECT(window), "style-set", G_CALLBACK(window_style_set), win_data);
	g_signal_connect(G_OBJECT(window), "size_request", G_CALLBACK(window_size_request), win_data);
	g_signal_connect(G_OBJECT(window), "size-allocate", G_CALLBACK(window_size_allocate), win_data);

	// create notebook
	win_data->notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(win_data->notebook), TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(win_data->notebook), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(win_data->notebook), FALSE);
	gtk_container_add(GTK_CONTAINER(window), win_data->notebook);
#ifdef ENABLE_TAB_REORDER
	g_signal_connect(G_OBJECT(win_data->notebook), "page-reordered", G_CALLBACK(reorder_page_number), window);
#endif
	// add a new page to notebook. run_once=TRUE.
	GtkWidget *vtebox = add_page(window, win_data->notebook, NULL, NULL, TRUE);
	// vtebox = NULL: the creation of vtebox is fault.
	if (vtebox == NULL)
	{
		// g_debug("the creation of vtebox is fault! current_vtebox = %d", current_vtebox);
		win_data->current_vtebox = NULL;
		return;
	}
	else
		win_data->current_vtebox = vtebox;
	
	int i;
	for (i=1;i<win_data->init_tab_number;i++)
		 add_page(window, win_data->notebook, NULL, NULL, FALSE);

	// gtk_window_set_gravity(GTK_WINDOW(window), GDK_GRAVITY_NORTH);
	gtk_window_move (GTK_WINDOW(window), 0, 0);

	// finish!
	gtk_widget_show_all(window);
	window_resizable(window, win_data->current_vtebox, 1, 1);

	// create menu
	// input method menu can't not be shown before window is shown.
	create_menu(window);
}

gboolean window_quit(GtkWidget *window, GdkEvent *event, struct Window *win_data)
{
	// g_debug("Get win_data = %d when window quit!", win_data);

	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));
	if (total_page>1)
		// confirm to close multi pages.
		dialog(NULL, 3);
	else
		close_page(win_data->current_vtebox, TRUE);
	
	// g_debug("Close window finish!");
	// It will be segmentation fault if retrun FALSE
	return TRUE;
}

void window_option(struct Window *win_data, int argc, char *argv[])
{
	// g_debug("Get win_data = %d in window option!", win_data);

	gint i;
	for (i=0;i<argc;i++)
	{
		// g_debug("%2d (Total %d): %s\n",i, argc, argv[i]);
		if ((!strcmp(argv[i], "-u")) || (!strcmp(argv[i], "--user_profile")))
		{
			if (++i==argc)
				g_critical("missing file name after -u/--user_profile!\n");
			else
			{
				//gint j;
				//for (j=i+1;j<argc;j++)
				//	*(argv[j]-1) = ' ';
				//argc = i+1;
				win_data->profile = g_strdup(argv[i]);
				win_data->use_custom_profile = TRUE;
			}
			// g_debug ("Using Profile = %s", profile);

			//for (j=0;j<argc;j++)
			//	g_debug("%2d (Total %d): %s\n",j, argc, argv[j]);
		}
		else if ((!strcmp(argv[i], "-t")) || (!strcmp(argv[i], "--tab")))
		{
			if (++i==argc)
				g_critical("missing tab number after -t/--tab!\n");
			else
				win_data->init_tab_number = atoi(argv[i]);
			if (win_data->init_tab_number<1)
				win_data->init_tab_number=1;
			// g_debug("Init LilyTerm with %d page(s)!", init_tab_number);
		}
		else if ((!strcmp(argv[i], "-e")) || (!strcmp(argv[i], "-x")) || (!strcmp(argv[i], "--execute")))
		{
			if (++i==argc)
				g_critical("missing command after -e/-x/--execute option!\n");
			else
			{
				win_data->command_line = argv[i];
				win_data->parameters = &(argv[i]);
				win_data->parameter = argc-i;
				//g_debug("Command = %s %s (Total = %d)\n",
				//	win_data->command_line, *(win_data->parameters), win_data->parameter);
			}
		}
	}
	// trying to got witch profile to use
	if (win_data->profile==NULL)
	{
		win_data->profile = g_strdup_printf("%s/%s", g_get_user_config_dir(), RCFILE);
		// g_debug("Trying to use user's profile %s...\n", profile);
		if ( ! g_file_test(win_data->profile , G_FILE_TEST_EXISTS))
		{
			// g_debug("Can not find user's profile. trying system profile...\n");
			g_free(win_data->profile);
			const char * const *system_dirs = g_get_system_config_dirs();
			for (i=0; system_dirs[i] != NULL; i++)
			{
				win_data->profile = g_strdup_printf("%s/%s", system_dirs[i], RCFILE);
				// g_debug("Trying to use %s...\n", profile);
				if ( g_file_test(win_data->profile , G_FILE_TEST_EXISTS))
					break;
				else
				{
					g_free(win_data->profile);
					win_data->profile = NULL;
				}
			}
			//if (profile==NULL)
			//	g_debug("Sorry, can not find any profile. use program defaults.\n");
		}
	}
}

gboolean window_key_press(GtkWidget *window, GdkEventKey *event, struct Window *win_data)
{
	// g_debug ("Get win_data = %d in key_press", win_data);
	if (win_data->keep_vtebox_size) return TRUE;
	
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
		
		if (win_data->enable_function_key)
		{
			for (i=0;i<KEYS;i++)
			{
				// g_debug("Checking... key: %s (status: %X)\n",
				//	   gdk_keyval_name(win_data->pagekeys[i].key),
				//	   event->state|GDK_LOCK_MASK);
				if ((mods==win_data->user_keys[i].mods) && (keyval==win_data->user_keys[i].key))
				{
					// deal the function key
					deal_key_press(window, i, win_data);
					return TRUE;
				}
			}
		}
		else
		{
			if ((mods==win_data->user_keys[0].mods) && (keyval==win_data->user_keys[0].key))
			{
				// deal the function key
				deal_key_press(window, 0, win_data);
				return TRUE;
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
void deal_key_press(GtkWidget *window, gint type, struct Window *win_data)
{
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vtebox), "Page_Data");

	switch (type)
	{
		case 0:
			win_data->enable_function_key = ! win_data->enable_function_key;
			break;
		case 1:
			// add a new page
			add_page(window, win_data->notebook, NULL, NULL, FALSE);
			break;
		case 2:
			// close page
			// g_debug("Trying to close page!\n");
			close_page (win_data->current_vtebox, TRUE);
			break;
		case 3:
			// edit page's label
			dialog(NULL, 1);
			break;
		case 4:
			// switch to pre page
			if (page_data->page_no)
				gtk_notebook_prev_page(GTK_NOTEBOOK(win_data->notebook));
			else
				gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), total_page -1);
			break;
		case 5:
			// switch to next page
			if (page_data->page_no == (gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook)) - 1))
				gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), 0);
			else
				gtk_notebook_next_page(GTK_NOTEBOOK(win_data->notebook));
			break;
		case 6:
			// switch to first page
			gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), 0);
			break;
		case 7:
			// switch to last page
			gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), total_page-1);
			break;
		case 8:
			// move current page forward
			gtk_notebook_reorder_child(GTK_NOTEBOOK(win_data->notebook), page_data->hbox, 
						   page_data->page_no -1);
			break;
		case 9:
			// move current page backward
			if (page_data->page_no == (gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook))-1))
				 gtk_notebook_reorder_child(GTK_NOTEBOOK(win_data->notebook), page_data->hbox, 0);
			else
				gtk_notebook_reorder_child(GTK_NOTEBOOK(win_data->notebook),
							   page_data->hbox, page_data->page_no+1);
			break;
		case 10:
			// move current page to first
			gtk_notebook_reorder_child(GTK_NOTEBOOK(win_data->notebook), page_data->hbox, 0);
			break;
		case 11:
			// move current page to last
			gtk_notebook_reorder_child(GTK_NOTEBOOK(win_data->notebook), page_data->hbox, -1);
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
				gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), type-12);
			break;
		case 24:
#ifdef ENABLE_SELECT_ALL
			vte_terminal_select_all(VTE_TERMINAL(win_data->current_vtebox));
#endif
			break;
		case 25:
			vte_terminal_copy_clipboard(VTE_TERMINAL(win_data->current_vtebox));
			break;
		case 26:
			vte_terminal_paste_clipboard(VTE_TERMINAL(win_data->current_vtebox));
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

gboolean window_get_focuse(GtkWidget *window, GdkEventFocus *event, struct Window *win_data)
{
	// g_debug("Get window =%d, win_data = %d and update current_vtebox when window_get_focuse!", window, win_data);
	win_data->lost_focuse = FALSE;

	// And, update the current_vtebox info
	current_vtebox = win_data->current_vtebox;
	active_window = window;
	return FALSE;
}

gboolean window_lost_focuse (GtkWidget *window, GdkEventFocus *event, struct Window *win_data)
{
	// g_debug("Get win_data = %d when vteboxvte_lost_focuse!", win_data);

	win_data->lost_focuse = TRUE;
	active_window = NULL;
	return FALSE;
}

void window_style_set (GtkWidget *window, GtkStyle *previous_style, struct Window *win_data)
{
	// g_debug("Get win_data = %d when window_style_set!", win_data);

	win_data->keep_vtebox_size |= 8;
	// g_debug("window_resizable in window_style_set! keep_vtebox_size = %d", keep_vtebox_size);
	window_resizable(window, win_data->current_vtebox, 2, 1);
}

void window_size_request (GtkWidget *window, GtkRequisition *requisition, struct Window *win_data)
{
	// g_debug("window_size-request! and keep_vtebox_size = %d, win_data = %d", win_data->keep_vtebox_size, win_data);
	if (win_data->keep_vtebox_size&0x1b)
	{
		// g_debug("Got keep_vtebox_size (before) = %d", keep_vtebox_size);
		GtkRequisition window_requisition;

		gtk_widget_get_child_requisition (window, &window_requisition);
		// g_debug("! The requested window requisition is %d x %d",
		//	window_requisition.width, window_requisition.height);
		gtk_window_resize(GTK_WINDOW(window), window_requisition.width, window_requisition.height);

		win_data->keep_vtebox_size &= 0x2d;

		// g_debug("Got keep_vtebox_size (after) = %d\n", keep_vtebox_size);
	}
}

void window_size_allocate (GtkWidget *window, GtkAllocation *allocation, struct Window *win_data)
{
	// g_debug("window_size-allocate!, and win_data->keep_vtebox_size = %d, win_data = %d", win_data->keep_vtebox_size, win_data);

	if (win_data->keep_vtebox_size)
	{
		win_data->keep_vtebox_size = 0;
		// g_debug("window_resizable in window_size_allocate!");
		window_resizable(window, win_data->current_vtebox, win_data->update_hints, 1);
	}

	// GtkRequisition window_requisition;
	// gtk_window_get_size(GTK_WINDOW(window), &window_requisition.width, &window_requisition.height);
	// g_debug("! The final window size is %d x %d", window_requisition.width, window_requisition.height);
}

