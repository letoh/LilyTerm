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

#include "window.h"

// if total_window = 0, call gtk_window_quit()
gint total_window = 0;
extern gboolean single_process;
GtkWidget *active_window;
GtkWidget *menu_active_window;
extern struct ErrorData err_str;

extern gboolean dialog_actived;
extern gboolean menu_actived;

// user_environ should be separated with <tab>
// win_data_orig and page_data_orig are used when draging a vte to root window.
GtkNotebook *new_window(int argc,
			char *argv[],
			gchar *environment,
			gchar *PWD,
			gchar *VTE_CJK_WIDTH_STR,
			gchar *user_environ,
			gchar *encoding,
			struct Window *win_data_orig,
			struct Page *page_data_orig)
{
#ifdef DETAIL
	g_debug("! Launch new_window() with argc = %d, VTE_CJK_WIDTH_STR = %s, "
		"user_environ = %s, win_data_orig = %p, page_data_orig = %p",
		argc, VTE_CJK_WIDTH_STR, user_environ, win_data_orig, page_data_orig);
	g_debug("! environment = %s", environment);
	print_array("! argv", argv);
#endif

	total_window++;
	struct Window *win_data = g_new0(struct Window, 1);
	if (PWD) chdir (PWD);

// ---- create main window ---- //

	win_data->window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
	gtk_window_set_title(GTK_WINDOW(win_data->window), PACKAGE_NAME);
	set_window_icon(win_data->window);

	// save the data first
	// g_debug("Save window = %p and win_data = %p when init window!", win_data->window, win_data);
	g_object_set_data(G_OBJECT(win_data->window), "Win_Data", win_data);

// ---- environ, command line option and user settings---- //

	if (win_data_orig==NULL)
	{
		if (! window_option(win_data, encoding, argc, argv))
		{
			// Don't execute the command!
			total_window--;
			destroy_window(win_data);
			return NULL;
		}
		get_user_settings(win_data->window, win_data, encoding);
	}
	else
		win_data_dup(win_data_orig, win_data);
	
	if (VTE_CJK_WIDTH_STR && VTE_CJK_WIDTH_STR[0] != '\0')
	{
		g_free(win_data->VTE_CJK_WIDTH_STR);
		win_data->VTE_CJK_WIDTH_STR = g_strdup(VTE_CJK_WIDTH_STR);
	}
	// g_debug("Got win_data->VTE_CJK_WIDTH_STR = %s after get_user_settings()", win_data->VTE_CJK_WIDTH_STR);
	if (win_data->init_dir==NULL)
		win_data->init_dir = g_strdup(PWD);
	// g_debug("win_data->init_dir = %s", win_data->init_dir);

	win_data->environment = g_strdup (environment);

// ---- the events of window---- //
	
	// close application if [Close Button] clicked
	g_signal_connect(G_OBJECT(win_data->window), "delete_event",
			 G_CALLBACK(window_quit), win_data);
	// if function key pressed
	g_signal_connect(G_OBJECT(win_data->window), "key-press-event",
			 G_CALLBACK(window_key_press), win_data);
	// if get focus, the size of vte is NOT resizeable.
	g_signal_connect_after(G_OBJECT(win_data->window), "focus-in-event",
			       G_CALLBACK(window_get_focus), win_data);
	// if lost focus, the size of vte is resizeable.
	g_signal_connect(G_OBJECT(win_data->window), "focus-out-event",
			 G_CALLBACK(window_lost_focus), win_data);
	// if the theme/fonts changed
	g_signal_connect_after(G_OBJECT(win_data->window), "style-set",
			       G_CALLBACK(window_style_set), win_data);
	g_signal_connect(G_OBJECT(win_data->window), "size_request",
			 G_CALLBACK(window_size_request), win_data);
	g_signal_connect(G_OBJECT(win_data->window), "size-allocate",
			 G_CALLBACK(window_size_allocate), win_data);
	// fullscreen/unfullscreen
	g_signal_connect(G_OBJECT(win_data->window), "window-state-event",
			 G_CALLBACK(window_state_event), win_data);

// ---- create notebook ---- //

	win_data->notebook = gtk_notebook_new();
	gtk_notebook_set_scrollable(GTK_NOTEBOOK(win_data->notebook), TRUE);
	gtk_notebook_set_show_tabs(GTK_NOTEBOOK(win_data->notebook), FALSE);
	gtk_notebook_set_show_border(GTK_NOTEBOOK(win_data->notebook), FALSE);
	// We used markup on tab names, so that we can't use popup on the notebook.
	// gtk_notebook_popup_enable(win_data->notebook);
	// Enable drag and drog
	gtk_notebook_set_group (GTK_NOTEBOOK(win_data->notebook), GINT_TO_POINTER (NOTEBOOK_GROUP));
	g_signal_connect(G_OBJECT(win_data->notebook), "page-added",
			 G_CALLBACK(notebook_page_added), win_data);
	// g_signal_connect(G_OBJECT(win_data->notebook), "page-removed",
	//		 G_CALLBACK(notebook_page_removed), win_data);
	if (win_data->tabbar_position)
		gtk_notebook_set_tab_pos(GTK_NOTEBOOK(win_data->notebook), GTK_POS_BOTTOM);
	gtk_container_add(GTK_CONTAINER(win_data->window), win_data->notebook);
#ifdef ENABLE_TAB_REORDER
	g_signal_connect(G_OBJECT(win_data->notebook), "page-reordered", G_CALLBACK(reorder_page_number), win_data->window);
#endif
	g_signal_connect(G_OBJECT(win_data->notebook), "create-window", G_CALLBACK(create_window), win_data);

// ---- add a new page---- //

	// add a new page to notebook. run_once=TRUE.
	if (page_data_orig==NULL)
	{
		// FIXME: Will it different if win_data_orig==NULL or not?
		gint i;
		// g_debug("win_data->init_tab_number = %d", win_data->init_tab_number);
		for (i=0;i<win_data->init_tab_number;i++)
		{
			// g_debug("send win_data->VTE_CJK_WIDTH_STR = %s to add_page()",
			// 	win_data->VTE_CJK_WIDTH_STR);
			// g_debug("win_data->init_tab_number = %d, i = %d", win_data->init_tab_number, i);
			if ( ! add_page(win_data,
				 	NULL,
					NULL,
					encoding,
					NULL,
					user_environ,
		 			FALSE,
					win_data->VTE_CJK_WIDTH_STR))
				return NULL;
		}
	}
	else
	{
		// It is for the new window which draged the vte tab to the root window
		// for create_menu()
		win_data->current_vte = win_data_orig->current_vte;
	}

	// gtk_window_set_gravity(GTK_WINDOW(win_data->window), GDK_GRAVITY_NORTH);
	// gtk_window_move (GTK_WINDOW(win_data->window), 0, 0);

// ---- finish! ---- //

	gtk_widget_show_all(win_data->window);
	// if ( ! win_data->show_scrollbar)
	//	hide_scrollbar(win_data, (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data"));
	window_resizable(win_data->window, win_data->current_vte, 1, 1);

	// create menu
	// input method menu can't not be shown before window is shown.
	create_menu(win_data);

	//if (page_data_orig!=NULL)
	//{
	//	// fix the color data after menu had been created.
	//	// g_debug("Launch get_and_update_page_name() in new_window()!");
	//	get_and_update_page_name(page_data_orig);
	//}

	return GTK_NOTEBOOK(win_data->notebook);
}

void set_window_icon(GtkWidget *window)
{
#ifdef DETAIL
	g_debug("! Launch set_window_icon on window = %p", window);
#endif

	GdkPixbuf *icon = gdk_pixbuf_new_from_file(ICONDIR G_DIR_SEPARATOR_S PACKAGE ".png", NULL);
	gtk_window_set_icon(GTK_WINDOW(window), icon);
	if (icon) g_object_unref(icon);
}

gboolean window_quit(GtkWidget *window, GdkEvent *event, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_quit() with window = %p, win_data = %p", window, win_data);
#endif
	menu_active_window = window;
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));
	if (total_page>1)
		// confirm to close multi pages.
		dialog(NULL, 3);
	else
		close_page(win_data->current_vte, TRUE);
	
	// g_debug("Close window finish!");
	// It will be segmentation fault if retrun FALSE
	return TRUE;
}

gboolean window_option(struct Window *win_data, gchar *encoding, int argc, char *argv[])
{
#ifdef DETAIL
	g_debug("! Launch window_option() with win_data = %p", win_data);
#endif
	// g_debug("argc = %d (in window_option)", argc);
	// print_array("argv (in window_option)", argv);


	// g_debug("Get win_data = %d in window option!", win_data);
	win_data->init_tab_number = 1;

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
			// g_debug ("Using Profile = %s", win_data->profile);

			//for (j=0;j<argc;j++)
			//	g_debug("%2d (Total %d): %s\n",j, argc, argv[j]);
		}
		else if ((!strcmp(argv[i], "-T")) || (!strcmp(argv[i], "--title")))
		{
			if (++i==argc)
				g_critical("missing window title after -T/--title!\n");
			else
			{
				win_data->custom_window_title = convert_str_to_utf8(argv[i], encoding);
				update_window_title(win_data->window, win_data->custom_window_title);
				// g_debug("The title of LilyTerm is specified to %s",
				//	win_data->custom_window_title);
			}
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
				gint j;
				GString *arg_str = g_string_new(NULL);
				for (j=0; j<argc-i; j++)
				{
					// g_debug("%2d = %s", j, argv[i+j]);
					g_string_append_printf(arg_str, "%s", argv[i+j]);
					if (j<argc-i-1)
						g_string_append(arg_str, " ");
				}
				// g_debug("arg_str = %s", arg_str->str);

				GtkWidget *menu_active_window_orig = menu_active_window;
				menu_active_window = win_data->window;
				// g_debug("set menu_active_window = %p", menu_active_window);
				struct ErrorData err_str_orig;
				err_str_orig = err_str;
				// err_str_orig->string  = err_str->string;
				// err_str_orig->encoding = 
				
				err_str.string = g_strdup(arg_str->str);
				err_str.encoding = encoding;

				gboolean return_value = dialog (NULL, 22);
				if (return_value)
				{
					// Yes, run the -e/-x command!
					win_data->command = argv[i];
					win_data->argv = &(argv[i]);
					win_data->argc = argc-i;
					// g_debug("win_data->command = %s", win_data->command);
				}
				g_free(err_str.string);

				menu_active_window = menu_active_window_orig;
				// g_debug("set menu_active_window = %p", menu_active_window);
				err_str = err_str_orig;
				g_string_free(arg_str, TRUE);

				if (! return_value)
					return FALSE;

				// g_debug("Command = %s", win_data->command);
				// g_debug("argc = %d", win_data->argc);
				// g_debug("argv =");
				// for (j=0; j<win_data->argc; j++)
				//	g_debug("\t%s", win_data->argv[j]);
			}
			break;
		}
		else if ((!strcmp(argv[i], "-d")) || (!strcmp(argv[i], "--directory")))
		{
			if (++i==argc)
				g_critical("missing tab number after -d/--directory!\n");
			else
				win_data->init_dir = g_strdup(argv[i]);
#ifdef DETAIL
			g_debug("+ Init LilyTerm with directory %s !", win_data->init_dir);
#endif
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
#ifdef DETAIL
			g_debug("* free win_data->profile %p in window_option() #1", win_data->profile);
#endif
			g_free(win_data->profile);
			win_data->profile = NULL;
			const char * const *system_dirs = g_get_system_config_dirs();
			for (i=0; system_dirs[i] != NULL; i++)
			{
				win_data->profile = g_strdup_printf("%s/%s", system_dirs[i], RCFILE);
				// g_debug("Trying to use %s...\n", profile);
				if ( g_file_test(win_data->profile , G_FILE_TEST_EXISTS))
					break;
				else
				{
#ifdef DETAIL
					g_debug("* free win_data->profile %p in window_option() #2", win_data->profile);
#endif
					g_free(win_data->profile);
					win_data->profile = NULL;
				}
			}
			//if (profile==NULL)
			//	g_debug("Sorry, can not find any profile. use program defaults.\n");
		}
	}
	return TRUE;
}

gboolean window_key_press(GtkWidget *window, GdkEventKey *event, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_key_press() with key = %s, state = %X, win_data = %p",
		 gdk_keyval_name(event->keyval), event->state, win_data);
#endif

	// g_debug ("Get win_data = %p in key_press", win_data);
	// g_debug ("win_data->keep_vte_size = %X, event->state = %X", win_data->keep_vte_size, event->state);
	if (win_data->keep_vte_size) return TRUE;
	
	// don't check if only shift key pressed!
	if (event->state & SHIFT_ONLY_MASK)
	{
		// g_debug("ALL_ACCELS_MASK = %X\n", ALL_ACCELS_MASK);
		// g_debug("Got the function key: %s (status: %X, check: %X)\n",
		//	gdk_keyval_name(event->keyval), event->state,
		//	event->state|GDK_LOCK_MASK|GDK_MOD2_MASK);
		gint i, keyval, mods;
		if ((event->keyval>=GDK_a) && (event->keyval<=GDK_z))
			keyval = event->keyval - GDK_a + GDK_A;
		else
			keyval = event->keyval;
		// mods = (event->state|GDK_LOCK_MASK|GDK_MOD2_MASK)&GDK_MODIFIER_MASK;
		mods = event->state | DUD_MASK;
		
		if (win_data->enable_function_key)
		{
			for (i=0;i<KEYS;i++)
			{
				// g_debug("Checking... key: %s (status: %X)\n",
				//	   gdk_keyval_name(win_data->user_keys[i].key),
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
//  [2] <Ctrl><Q>		Close current tab
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
// [26] <Ctrl><V>		Paste the text in clipboard
// [27] <Ctrl><+>		Increase the font size of current vte
// [28] <Ctrl><->		Decrease the font size of current vte
// [29] <Ctrl><Enter>		Reset the font of current tab to original size
// [30] <Alt><F11>		Asks to place window in the fullwindow/unfullwindow state
// [31] <Alt><Enter>		Asks to place window in the fullscreen/unfullscreen state
// [32] <Ctrl><PrintScreen>	Dump the runtime data of lilyterm for debug
//	<Shift><Insert>		Pase clipboard.
void deal_key_press(GtkWidget *window, gint type, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch deal_key_press() with window = %p, type = %d, win_data = %p", window, type, win_data);
#endif

	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data");

	switch (type)
	{
		case 0:
			win_data->enable_function_key = ! win_data->enable_function_key;
			if (win_data->enable_hyperlink)
			{
				gint i;
				for (i=0;i<total_page;i++)
					set_hyprelink(win_data,
						      get_page_data_from_nth_page(win_data, i));
			}
			break;
		case 1:
			// add a new page
			add_page(win_data,
				 page_data,
				 NULL,
				 NULL,
				 NULL,
				 NULL,
				 FALSE,
				 NULL);
			break;
		case 2:
			// close page
			// g_debug("Trying to close page!\n");
			close_page (win_data->current_vte, TRUE);
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
			vte_terminal_select_all(VTE_TERMINAL(win_data->current_vte));
#endif
			break;
		case 25:
			vte_terminal_copy_clipboard(VTE_TERMINAL(win_data->current_vte));
			break;
		case 26:
			vte_terminal_paste_clipboard(VTE_TERMINAL(win_data->current_vte));
			break;
		case 27:
			set_vte_font(NULL, 1);
			break;
		case 28:
			set_vte_font(NULL, 2);
			break;
		case 29:
			set_vte_font(NULL, 0);
			break;
		case 30:
		case 31:
			if (win_data->fullscreen)
			{
				gtk_window_unfullscreen(GTK_WINDOW(win_data->window));

				if (type == 31)
				{
					win_data->unfullscreen = 2;
					win_data->true_fullscreen = FALSE;
				}
			}
			else
			{
				if (type == 31)
				{
					// Trying to fullscreen and resizing
					gboolean tab_bar_is_shown = gtk_notebook_get_show_tabs GTK_NOTEBOOK(win_data->notebook);
					gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook));
					gint fullscreen_show_tab_bar = 1;
					if (total_page ==1)
					{       
						// Total Page = 1 but the tab bar is shown.
						if (tab_bar_is_shown)
							fullscreen_show_tab_bar = 2;
					}       
					else
					{
						// Total Page > 1 but the tab bar is hidden.
						if (! tab_bar_is_shown)
							fullscreen_show_tab_bar = 3;
					}
					hide_and_show_tabs_bar(win_data, FALSE);
					win_data->fullscreen_show_tab_bar = fullscreen_show_tab_bar;

					win_data->fullscreen_show_scroll_bar = win_data->show_scrollbar;
					GTK_CHECK_MENU_ITEM(win_data->menuitem_hide_scroll_bar)->active = FALSE;
					hide_scrollback_lines(win_data->menuitem_hide_scroll_bar, NULL);

					win_data->unfullscreen = -2;
					win_data->true_fullscreen = TRUE;
				}
				else
					gtk_window_fullscreen(GTK_WINDOW(win_data->window));
			}
			break;
#ifdef DEBUG
		case 32:
			dump_data(win_data, page_data);
			break;
#endif
	}
}

gboolean window_get_focus(GtkWidget *window, GdkEventFocus *event, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_get_focus() with window =%p, win_data = %p", window, win_data);
#endif
	// g_debug("Window get focus!");

	// update current_vte when window_get_focus!
	win_data->lost_focus = FALSE;

	// And, update the current_vte info
	// current_vte = win_data->current_vte;
	active_window = window;
	menu_active_window = window;
	// g_debug("set menu_active_window = %p", menu_active_window);

	if (win_data->color_brightness != win_data->color_brightness_inactive)
	{
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte),
									  "Page_Data");
		vte_terminal_set_colors(VTE_TERMINAL(win_data->current_vte),
					&(win_data->fg_color),
					&(win_data->bg_color),
					win_data->color,
					16);
		vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color));
		page_data->vte_is_inactived = FALSE;
	}

	menu_actived = FALSE;
	return FALSE;
}

gboolean window_lost_focus(GtkWidget *window, GdkEventFocus *event, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_lost_focus() with window =%p, win_data = %p", window, win_data);
#endif
	// g_debug("Window lost focus!");

	if (win_data->using_custom_color &&
	    (win_data->color_brightness != win_data->color_brightness_inactive) &&
	    (! menu_actived) &&
	    (! dialog_actived))
	{
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte),
									  "Page_Data");
		page_data->vte_is_inactived = TRUE;
		vte_terminal_set_colors(VTE_TERMINAL(win_data->current_vte),
					&(win_data->fg_color_inactive),
					&(win_data->bg_color),
					win_data->color_inactive,
					16);
		vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color_inactive));
	}
	if (win_data->page_shows_window_title &&
	    (vte_terminal_get_window_title(VTE_TERMINAL(win_data->current_vte))!=NULL))
	{
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data");

		if (win_data->page_shows_current_cmdline)
			win_data->window_title_tpgid = page_data->displayed_tpgid;
	
		if (win_data->page_shows_current_dir)
		{
			g_free(page_data->window_title_pwd);
			page_data->window_title_pwd = get_tab_name_with_current_dir(page_data->new_tpgid);
		}
	}
	win_data->lost_focus = TRUE;
	active_window = NULL;
	return FALSE;
}

void window_style_set(GtkWidget *window, GtkStyle *previous_style, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_style_set() with window =%p, win_data = %p", window, win_data);
#endif
	// g_debug("Launch window_style_set()!");

	// win_data->keep_vte_size |= 8;
	// g_debug("window_resizable in window_style_set! keep_vte_size = %d", keep_vte_size);
	// window_resizable(window, win_data->current_vte, 2, 1);
	keep_window_size (win_data, win_data->current_vte, 0x60);
}

void window_size_request (GtkWidget *window, GtkRequisition *requisition, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_size_request() with window =%p, win_data = %p, keep_vte_size = %x",
		 window, win_data, win_data->keep_vte_size);
#endif
	// FIXME: Don't do it when changing Themes
	// g_debug("Got keep_vte_size (before) = %x\n", win_data->keep_vte_size);
	if (win_data->keep_vte_size&0xa94a)
	{
		// g_debug("Got keep_vte_size (before) = %d", win_data->keep_vte_size);
		GtkRequisition window_requisition;

		gtk_widget_get_child_requisition (window, &window_requisition);
		// g_debug("! The requested window requisition is %d x %d",
		//	window_requisition.width, window_requisition.height);
		if ( window_requisition.width>0 &&  window_requisition.height>0 )
			gtk_window_resize(GTK_WINDOW(window),
					  window_requisition.width,
					  window_requisition.height);

		// 0xed = 11,101,101
		// 'Showing/Hiding tab bar' and 'Change vte font' should call gtk_window_resize() once
		win_data->keep_vte_size &= 0x54a5;

		// g_debug("Got keep_vte_size (after) = %x\n", win_data->keep_vte_size);
	}
}

void window_size_allocate(GtkWidget *window, GtkAllocation *allocation, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch window_size_allocate() with window =%p, win_data = %p, keep_vte_size = %x",
		 window, win_data, win_data->keep_vte_size);
#endif

	// g_debug("window_size-allocate!, and win_data->keep_vte_size = %d", win_data->keep_vte_size);
	// g_debug("Got keep_vte_size (finish) = %x\n", win_data->keep_vte_size);
	if (win_data->keep_vte_size & 0x54a5)
	{
		win_data->keep_vte_size = 0;
		// g_debug("window_resizable in window_size_allocate!");
		window_resizable(window, win_data->current_vte, win_data->update_hints, 1);
	}

	if ((win_data->keep_vte_size==0) && (win_data->unfullscreen>0))
	{
		// g_debug("Trying keep the size of window when unfullscreen, win_data->unfullscreen = %d", win_data->unfullscreen);
		win_data->unfullscreen--;

		if (win_data->unfullscreen == 0)
		{
			window_resizable(window, win_data->current_vte, 2, 1);
			// g_debug("win_data->fullscreen_show_tab_bar = %d", win_data->fullscreen_show_tab_bar);
			switch (win_data->fullscreen_show_tab_bar)
			{
				case 1:
					if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(win_data->notebook)) > 1)
						hide_and_show_tabs_bar(win_data, TRUE);
					else
						hide_and_show_tabs_bar(win_data, FALSE);
					break;
				case 2:
					hide_and_show_tabs_bar(win_data, TRUE);
					break;
				case 3:
					hide_and_show_tabs_bar(win_data, FALSE);
					break;
			}

			GTK_CHECK_MENU_ITEM(win_data->menuitem_hide_scroll_bar)->active =
				win_data->fullscreen_show_scroll_bar;
			hide_scrollback_lines(win_data->menuitem_hide_scroll_bar, NULL);

			// keep_window_size (win_data, win_data->current_vte, 128);
		}
	}

	if ((win_data->keep_vte_size==0) && (win_data->unfullscreen<0))
	{
		// g_debug("Trying keep the size of window when fullscreen, win_data->unfullscreen = %d", win_data->unfullscreen);
		win_data->unfullscreen++;
		if (win_data->unfullscreen == 0)
			gtk_window_fullscreen(GTK_WINDOW(win_data->window));
	}

	//GtkRequisition window_requisition;
	//gtk_window_get_size(GTK_WINDOW(window), &window_requisition.width, &window_requisition.height);
	//g_debug("! The final window size is %d x %d (keep_vte_size = %d)",
	//	 window_requisition.width, window_requisition.height, win_data->keep_vte_size);
}

void destroy_window(struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch destroy_window() win_data = %p", win_data);
#endif
	int i;

#ifdef DETAIL
	g_debug("* Free environment %p (%s)", win_data->environment, win_data->environment);
#endif
	g_free(win_data->environment);

#ifdef DETAIL
	g_debug("* Free default_locale %p (%s)", win_data->default_locale, win_data->default_locale);
#endif
	g_free(win_data->default_locale);

#ifdef DETAIL
	g_debug("* Free locales_list %p (%s)", win_data->locales_list, win_data->locales_list);
#endif
	g_free(win_data->locales_list);

#ifdef DETAIL
	g_debug("* Free emulate_term %p (%s)", win_data->emulate_term, win_data->emulate_term);
#endif
	g_free(win_data->emulate_term);

#ifdef DETAIL
	g_debug("* Free VTE_CJK_WIDTH_STR %p (%s)", win_data->VTE_CJK_WIDTH_STR, win_data->VTE_CJK_WIDTH_STR);
#endif
	g_free(win_data->VTE_CJK_WIDTH_STR);

#ifdef DETAIL
	g_debug("* Free supported_locales %p", win_data->supported_locales);
#endif
	g_strfreev(win_data->supported_locales);

#ifdef DETAIL
	g_debug("* Free init_dir %p (%s)", win_data->init_dir, win_data->init_dir);
#endif
	g_free(win_data->init_dir);


#ifdef DETAIL
	g_debug("* Free win_data->profile %p (%s)", win_data->profile, win_data->profile);
#endif
	g_free(win_data->profile);

#ifdef DETAIL
	g_debug("* Free win_data->custom_window_title %p (%s)",
		win_data->custom_window_title, win_data->custom_window_title);
#endif
	g_free(win_data->custom_window_title);

#ifdef DETAIL
	g_debug("* Free colors");
#endif
	g_free(win_data->foreground_color);
	g_free(win_data->background_color);
	for (i=0; i<PAGE_COLOR; i++)
		g_free(win_data->user_page_color[i]);

#ifdef DETAIL
	g_debug("* Free color_theme %s (%s)", win_data->color_theme, win_data->color_theme);
#endif
	g_free(win_data->color_theme);

	for (i=0; i<COLOR; i++)
	{
#ifdef DETAIL
		g_debug("* Free color %s (%s)", win_data->color_value[i], win_data->color_value[i]);
#endif
		g_free(win_data->color_value[i]);
	}

#ifdef DETAIL
	g_debug("* Free page_name %p (%s)", win_data->page_name, win_data->page_name);
#endif
	g_free(win_data->page_name);
#ifdef DETAIL
	g_debug("* Free page_names %p (%s)", win_data->page_names, win_data->page_names);
#endif
	g_free(win_data->page_names);
#ifdef DETAIL
	g_debug("* Free splited_page_names %p", win_data->splited_page_names);
#endif
	g_strfreev(win_data->splited_page_names);

#ifdef DETAIL
	g_debug("* Free font_name %p (%s)", win_data->default_font_name, win_data->default_font_name);
#endif
	g_free(win_data->default_font_name);
#ifdef DETAIL
	g_debug("* Free font_name %p (%s)", win_data->system_font_name, win_data->system_font_name);
#endif
	g_free(win_data->system_font_name);
#ifdef DETAIL
	g_debug("* Free restore_font_name %p (%s)", win_data->restore_font_name, win_data->restore_font_name);
#endif
	g_free(win_data->restore_font_name);
#ifdef DETAIL
	g_debug("* Free word_chars %p (%s)", win_data->word_chars, win_data->word_chars);
#endif
	g_free(win_data->word_chars);

	for (i=0; i<KEYS; i++)
	{
#ifdef DETAIL
		g_debug("* Free user_keys %p (%s)", win_data->user_keys[i].value, win_data->user_keys[i].value);
#endif
		g_free(win_data->user_keys[i].value);
	}

	for (i=0; i<COMMAND; i++)
	{
#ifdef DETAIL
		g_debug("* Free user_command %p (%s)",
			win_data->user_command[i].command, win_data->user_command[i].command);
#endif
		g_free(win_data->user_command[i].command);
#ifdef DETAIL
		g_debug("* Free user_environ %p (%s)",
			win_data->user_command[i].environ, win_data->user_command[i].environ);
#endif
		g_free(win_data->user_command[i].environ);
#ifdef DETAIL
		g_debug("* Free user_environments %p", win_data->user_command[i].environments);
#endif
		g_strfreev(win_data->user_command[i].environments);
	}

#ifdef DETAIL
	g_debug("* Free menu %p", win_data->menu);
#endif
	if (win_data->menu!=NULL)
		gtk_widget_destroy(win_data->menu);

	// directory can NOT be freed.
	// They are a pointer for win_data->argv.
	// g_debug("* Free directory %p", win_data->directory);
	// g_free(win_data->directory);
	
	// Clean active_window!
#ifdef DETAIL
	g_debug("+ Clean active_window : %p", win_data->window);
#endif
	if (active_window==win_data->window)
	{
		active_window = NULL;
		menu_active_window = NULL;
		// g_debug("set menu_active_window = %p", menu_active_window);
	}
#ifdef DETAIL
	g_debug("+ Destroy window: %p", win_data->window);
#endif
	gtk_widget_destroy(win_data->window);

//#ifdef DETAIL
//	g_debug("+ Set current_vte to NULL!");
//#endif
//	if (active_window != NULL)
//	{
//#ifdef DETAIL
//		g_debug("Set current_vte = win_data->current_vte (%p)",
//			   win_data->current_vte);
//#endif
//		// active_window != NULL: We may trying to find the activing vte
//		// It only happened run 'lilyterm -e xxx' in a lilyterm box
//		struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(
//								active_window), "Win_Data");
//		current_vte = win_data->current_vte;
//	}
//	else
//	{
//#ifdef DETAIL
//		g_debug("CAREFUL: current_vte = NULL!!");
//#endif
//		// No activing window: It only happened run 'lilyterm -e xxx' outside lilyterm.
//		current_vte = NULL;
//	}

#ifdef DETAIL
	g_debug("* Free win_data %p", win_data);
#endif
	g_free(win_data);
	win_data=NULL;
}

void notebook_page_added (GtkNotebook *notebook, GtkWidget *child, guint page_num, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch notebook_page_added() with notebook = %p, and win_data = %p", notebook, win_data);
#endif
	// g_debug("notebook_page_added !");

	GtkWidget *vte=(GtkWidget *)g_object_get_data(G_OBJECT(
					gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),
						gtk_notebook_get_nth_page(
							GTK_NOTEBOOK(notebook), page_num))), "VteBox");
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	// g_debug("Get page_data = %p", page_data);

	// FIXME: Calling remove_notebook_page() here
	// GtkNotebook may drag a page to itself,
	// So a GtkNotebook may be page_removed() -> page_added() with the same page
	// but the GtkNotebook may be destoried after page_removed()
	// So that we should call page_removed() here, not using "page-removed" signal... -_-|||

	// page_data->window==win_data->window -> this page is drag to itself.
	// g_debug("page_data->window = %p", page_data->window);
	// g_debug("win_data->window = %p", win_data->window);
	if (page_data->window==win_data->window)
	{
		reorder_page_after_added_removed_page(win_data, page_num);
		return;
	}
	// page_data->window = NULL: it is launched by add_page(),
	// page_data->window != NULL: this page is draged from another lilyterm window.
	else if (page_data->window != NULL)
	{
		struct Window *win_data_orig = (struct Window *)g_object_get_data(G_OBJECT(page_data->window),
										  "Win_Data");
		// if the color of current vte is "inactive", light up it!
//		if (win_data->using_custom_color &&
//		    (win_data->color_brightness != win_data->color_brightness_inactive) &&
//		    (! menu_actived) &&
//		    (! dialog_actived))
//		{
//			vte_terminal_set_colors(VTE_TERMINAL(win_data->current_vte),
//						&(win_data->fg_color),
//						&(win_data->bg_color),
//						win_data->color,
//						16);
//			vte_terminal_set_colors(VTE_TERMINAL(page_data->vte),
//						&(win_data->fg_color_inactive),
//						&(win_data->bg_color),
//						win_data->color_inactive,
//						16);
//
//		}
		page_data->window = win_data->window;
		page_data->notebook = win_data->notebook;
		// apply the new win_data to this page
		apply_new_win_data_to_page(win_data_orig, win_data, page_data);

		if (page_data->page_update_method == 1)
			page_data->window_title_updated = 1;
		get_and_update_page_name(page_data, FALSE);

		remove_notebook_page (GTK_NOTEBOOK(win_data_orig->notebook),
				      NULL,
				      page_data->page_no,
				      win_data_orig);
	}
	else
	{
		page_data->window = win_data->window;
		page_data->notebook = win_data->notebook;
	}

	// for drag&drop, or it will shows unnecessary "(UTF-8)" on the tab
	if (win_data->default_encoding != NULL)
	{
		// g_debug("page_data->encoding_str = %s, win_data->default_encoding->name = %s",
		//	page_data->encoding_str, gtk_widget_get_name (win_data->default_encoding));
		if (page_data->encoding_str==NULL)
			page_data->encoding_str = get_encoding_from_menu_item_name(
									win_data->default_encoding);
	}

#ifdef ENABLE_TAB_REORDER
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), page_data->hbox, TRUE);
	gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), page_data->hbox, TRUE);
#endif
	// g_object_set(gtk_widget_get_parent(page_data->label), "can-focus", FALSE, NULL);
	if (win_data->fill_tab_bar)
		gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebook), page_data->hbox,
						   TRUE, TRUE, GTK_PACK_START);
	else
		gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebook), page_data->hbox,
						   FALSE, FALSE, GTK_PACK_START);

	// some data came from window. for the performance of monitor_cmdline
	init_monitor_cmdline_datas(win_data, page_data);

	// g_debug("Reordering the page and update page name!");
	// win_data->adding_page = TRUE;
	// reorder_page_number(GTK_NOTEBOOK(notebook), NULL, 0, win_data->window);
	// win_data->adding_page = FALSE;
	// gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_data->page_no);
	// gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_num);
	// gtk_window_set_focus(GTK_WINDOW(win_data->window), page_data->vte);
	reorder_page_after_added_removed_page(win_data, page_num);

	// page_data->pid = 0 -> it is launched by add_page()
	if (page_data->pid)
	{
		// Trying to update page_data->tab_color
		// g_debug("Launch get_and_update_page_name() when pid > 0 !");
		// get_and_update_page_name(page_data);
		monitor_cmdline(page_data);
		// g_debug("page_data->tab_color = %s", page_data->tab_color);
	}

	// show the page bar if necessary
	// g_debug("Total Page (Notebook):%u\n",
	//	      (gtk_notebook_get_n_pages(GTK_NOTEBOOK(page_data->notebook))));
	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(page_data->notebook))==2 &&
	    (! win_data->true_fullscreen) &&
	    (! dialog_actived))
	{
		// It will done in update_page_name()
		// keep_vte_size |= 2;
		// while (win_data->keep_vte_size)
		//	usleep(100000);
		hide_and_show_tabs_bar(win_data, TRUE);
	}

	update_window_hint(win_data, page_data);

}

//void notebook_page_removed (GtkNotebook *notebook, GtkWidget *child, guint page_num, struct Window *win_data)
//{
//#ifdef DETAIL
//	g_debug("! Launch notebook_page_removed() with notebook = %p, and win_data = %p, page_num = %d",
//		notebook, win_data, page_num);
//#endif
//	g_debug("Reordering the page and update page name!");
//	win_data->adding_page = TRUE;
//	reorder_page_number(GTK_NOTEBOOK(win_data->notebook), NULL, 0, win_data->window);
//	win_data->adding_page = FALSE;
//	g_debug("Set current page!");
//	// gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_data->page_no);
//	gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), page_num);
//	g_debug("Set focus!");
//	gtk_window_set_focus(GTK_WINDOW(win_data->window), win_data->current_vte);
//	g_debug("keep_window_size");
//	keep_window_size (win_data, win_data->current_vte, 0x3000);
//}

void reorder_page_after_added_removed_page(struct Window *win_data, guint page_num)
{
#ifdef DETAIL
	g_debug("! Launch reorder_page_after_added_removed_page() with win_data = %p, page_num = %d",
		win_data, page_num);
#endif
		// g_debug("Reordering the page and update page name!");
		win_data->adding_page = TRUE;
		reorder_page_number(GTK_NOTEBOOK(win_data->notebook), NULL, 0, win_data->window);
		win_data->adding_page = FALSE;

		// g_debug("Set current page!");
		// gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_data->page_no);
		gtk_notebook_set_current_page(GTK_NOTEBOOK(win_data->notebook), page_num);
		// g_debug("Set focus!");
		gtk_window_set_focus(GTK_WINDOW(win_data->window), win_data->current_vte);
}

void remove_notebook_page (GtkNotebook *notebook, GtkWidget *child, guint page_num, struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch remove_notebook_page() with notebook = %p, and win_data = %p, page_num = %d", notebook, win_data, page_num);
#endif
	// g_debug("remove_notebook_page !");
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	// g_debug("Total Page after remove_notebook_page: %d\n", total_page);

	if (total_page)
	{
		// g_debug("Update the hint data!");
		struct Page *page_data = (struct Page *)g_object_get_data(
								G_OBJECT(win_data->current_vte),
								"Page_Data");
		update_window_hint(win_data, page_data);

		// g_debug("hide the tab bar if necessary");
		if (total_page == 1)
		{
			// hide the page bar
			// win_data->keep_vte_size |= 6;
			// we need to set the hints, or the window size may be incorrect.
			// g_debug("window_resizable in remove_page!");
			// window_resizable(win_data->window, win_data->current_vte, 2, 1);
			// keep_window_size (win_data, win_data->current_vte, 0x1c);
			// gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
			// gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), FALSE);
			hide_and_show_tabs_bar(win_data, FALSE);
			win_data->update_hints = 1;
		}
		// g_debug("Reordering the page and update page name!");
		reorder_page_number(GTK_NOTEBOOK(notebook), NULL, 0, win_data->window);
	}
	// if it is the last page, close window!
	else
	{
		total_window--;
		if (total_window)
			destroy_window(win_data);
		else
		{
#ifdef DETAIL
			g_debug("+ It is the last page, exit lilyterm!\n");
#endif
			if (GTK_WIDGET_MAPPED(win_data->window))
			{
				if (single_process)
					shutdown_socket_server();
				gtk_main_quit();
#ifdef DETAIL
			g_debug("+ lilyterm had been close normally!\n");
#endif
			}
			else
				exit (1);
		}
	}
}

void update_window_hint(struct Window *win_data,
			struct Page *page_data)
{
	if (gtk_notebook_get_n_pages(GTK_NOTEBOOK(page_data->notebook)) == 1 &&
	    page_data->font_size == 0)
		win_data->update_hints = 1;
	else if (check_if_every_vte_is_using_restore_font_name(win_data, page_data))
		win_data->update_hints = 1;
	else
		win_data->update_hints = 2;

	// g_debug("window_resizable in change current font!");
	window_resizable(page_data->window,
			 page_data->vte,
			 win_data->update_hints,
			 1);
}

GtkNotebook *create_window (GtkNotebook *notebook, GtkWidget *page, gint x, gint y,
			    struct Window *win_data)
{
#ifdef DETAIL
	g_debug("! Launch create_window() with notebook = %p, win_data = %p VTE_CJK_WIDTH_STR = %s",
		notebook, win_data, win_data->VTE_CJK_WIDTH_STR);
#endif
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(win_data->current_vte), "Page_Data");
	// we set the encoding=NULL here,
	// becouse new_window() with page_data != NULL will not call add_page()
	// FIXME: should we send get_encoding_from_menu_item_name(win_data->default_encoding) here?
	return new_window(0,
			  NULL,
			  win_data->environment,
			  page_data->pwd,
			  win_data->VTE_CJK_WIDTH_STR,
			  NULL,
			  NULL,
			  win_data,
			  page_data);
}

gboolean window_state_event (GtkWidget *widget, GdkEventWindowState *event, struct Window *win_data)
{
	if (event->changed_mask & GDK_WINDOW_STATE_FULLSCREEN)
		win_data->fullscreen = event->new_window_state & GDK_WINDOW_STATE_FULLSCREEN;
	if (! win_data->fullscreen)
		win_data->true_fullscreen = FALSE;

	return FALSE;
}

void keep_window_size (struct Window *win_data, GtkWidget *vte, guint keep_vte_size)
{
	win_data->keep_vte_size |= keep_vte_size;
	// g_debug("win_data->keep_vte_size = %d, and keep_vte_size = %d", 
	//	win_data->keep_vte_size, keep_vte_size);
	
	window_resizable(win_data->window, vte, 2, 1);
}

#ifdef DEBUG
void dump_data (struct Window *win_data, struct Page *page_data)
{
	gint i;

// ---- win_data ---- //

	g_debug("- win_data->environment = %s", win_data->environment);
	g_debug("- win_data->default_locale = %s", win_data->default_locale);
	g_debug("- win_data->locales_list = %s", win_data->locales_list);
	print_array("win_data->supported_locales", win_data->supported_locales);
	g_debug("- win_data->emulate_term = %s", win_data->emulate_term);
	g_debug("- win_data->VTE_CJK_WIDTH = %d", win_data->VTE_CJK_WIDTH);
	g_debug("- win_data->VTE_CJK_WIDTH_STR = %s", win_data->VTE_CJK_WIDTH_STR);
	g_debug("- win_data->argc = %d", win_data->argc);
	print_array("win_data->argv", win_data->argv);
	g_debug("- win_data->command = %s", win_data->command);
	g_debug("- win_data->init_tab_number = %d", win_data->init_tab_number);
	g_debug("- win_data->init_dir = %s", win_data->init_dir);
	g_debug("- win_data->use_custom_profile = %d", win_data->use_custom_profile);
	g_debug("- win_data->profile = %s", win_data->profile);
	g_debug("- win_data->fullscreen = %d", win_data->fullscreen);
	g_debug("- win_data->true_fullscreen = %d", win_data->true_fullscreen);
	g_debug("- win_data->fullscreen_show_tab_bar = %d", win_data->fullscreen_show_tab_bar);
	g_debug("- win_data->fullscreen_show_scroll_bar = %d", win_data->fullscreen_show_scroll_bar);
	g_debug("- win_data->unfullscreen = %d", win_data->unfullscreen);
	g_debug("- win_data->window = %p", win_data->window);
	g_debug("- win_data->notebook = %p", win_data->notebook);
	g_debug("- win_data->current_vte = %p", win_data->current_vte);
	g_debug("- win_data->window_shows_current_page = %d", win_data->window_shows_current_page);
	g_debug("- win_data->custom_window_title = %s", win_data->custom_window_title);
	// g_debug("- win_data->update_window_title_only = %d", win_data->update_window_title_only);
	g_debug("- win_data->window_title_tpgid = %d", win_data->window_title_tpgid);
	g_debug("- win_data->use_rgba = %d", win_data->use_rgba);
	g_debug("- win_data->use_rgba_orig = %d", win_data->use_rgba_orig);
#ifdef ENABLE_RGBA
	g_debug("- win_data->transparent_window = %d", win_data->transparent_window);
	g_debug("- win_data->window_opacity = %1.3f", win_data->window_opacity);
#endif
	g_debug("- win_data->enable_function_key = %d", win_data->enable_function_key);
	for (i=0; i<KEYS; i++)
	{
		g_debug("- win_data->user_keys[%d].value = %s", i, win_data->user_keys[i].value);
		g_debug("- win_data->user_keys[%d].key = 0x%x", i, win_data->user_keys[i].key);
		g_debug("- win_data->user_keys[%d].mods = 0x%x", i, win_data->user_keys[i].mods);
	}
	g_debug("- win_data->update_hints = %d", win_data->update_hints);
	g_debug("- win_data->lost_focus = %d", win_data->lost_focus);
	g_debug("- win_data->keep_vte_size = %d", win_data->keep_vte_size);
	g_debug("- win_data->menu = %p", win_data->menu);
	g_debug("- win_data->menu_actived = %d", win_data->menu_actived);
	g_debug("- win_data->encoding_sub_menu = %p", win_data->encoding_sub_menu);
	g_debug("- win_data->encoding_group = %p", win_data->encoding_group);
	g_debug("- win_data->default_encoding = %p", win_data->default_encoding);
	if (win_data->default_encoding)
		g_debug("- win_data->default_encoding_str = %s", gtk_widget_get_name (win_data->default_encoding));
#ifdef ENABLE_RGBA
	g_debug("- win_data->menuitem_trans_win = %p", win_data->menuitem_trans_win);
#endif
	g_debug("- win_data->menuitem_trans_bg = %p", win_data->menuitem_trans_bg);
//	g_debug("- win_data->menuitem_scrollback_lines = %p", win_data->menuitem_scrollback_lines);
	g_debug("- win_data->show_color_selection_menu = %d", win_data->show_color_selection_menu);
	g_debug("- win_data->show_resize_menu = %d", win_data->show_resize_menu);
	g_debug("- win_data->font_resize_ratio = %1.3f", win_data->font_resize_ratio);
	g_debug("- win_data->window_resize_ratio = %1.3f", win_data->window_resize_ratio);
	g_debug("- win_data->show_transparent_menu = %d", win_data->show_transparent_menu);
	g_debug("- win_data->show_input_method_menu = %d", win_data->show_input_method_menu);
	g_debug("- win_data->show_get_function_key_menu = %d", win_data->show_get_function_key_menu);
	g_debug("- win_data->show_change_page_name_menu = %d", win_data->show_change_page_name_menu);
	g_debug("- win_data->enable_hyperlink = %d", win_data->enable_hyperlink);
	for (i=0; i<COMMAND; i++)
	{
		g_debug("- win_data->user_command[%d].command = %s", i, win_data->user_command[i].command);
		g_debug("- win_data->user_command[%d].method = %d", i, win_data->user_command[i].method);
		g_debug("- win_data->user_command[%d].environ = %s", i, win_data->user_command[i].environ);
		gchar *temp_str = g_strdup_printf("win_data->user_command[%d].environments", i);
		print_array(temp_str, win_data->user_command[i].environments);
		g_free(temp_str);
		g_debug("- win_data->user_command[%d].VTE_CJK_WIDTH = %d",
			i,
			win_data->user_command[i].VTE_CJK_WIDTH);
	}
	g_debug("- win_data->menuitem_copy_url = %p", win_data->menuitem_copy_url);
	if (win_data->menuitem_copy_url)
		g_debug("- win_data->menuitem_copy_url->name = %s", gtk_widget_get_name (win_data->menuitem_copy_url));
	g_debug("- win_data->menuitem_cursor_blinks = %p", win_data->menuitem_cursor_blinks);
	g_debug("- win_data->menuitem_audible_bell = %p", win_data->menuitem_audible_bell);
	g_debug("- win_data->menuitem_show_tabs_bar = %p", win_data->menuitem_show_tabs_bar);
	g_debug("- win_data->menuitem_hide_tabs_bar = %p", win_data->menuitem_hide_tabs_bar);
	g_debug("- win_data->menuitem_hide_scroll_bar = %p", win_data->menuitem_hide_scroll_bar);
	g_debug("- win_data->show_copy_paste_menu = %d", win_data->show_copy_paste_menu);
	g_debug("- win_data->menuitem_copy = %p", win_data->menuitem_copy);
	g_debug("- win_data->menuitem_paste = %p", win_data->menuitem_paste);
	g_debug("- win_data->foreground_color = %s", win_data->foreground_color);
	g_debug("- win_data->background_color = %s", win_data->background_color);
	print_color("win_data->fg_color", win_data->fg_color);
	print_color("win_data->fg_color_inactive", win_data->fg_color_inactive);
	print_color("win_data->bg_color", win_data->bg_color);
	g_debug("- win_data->color_theme = %s", win_data->color_theme);
	g_debug("- win_data->color_theme_orig = %s", win_data->color_theme_orig);
	g_debug("- win_data->using_custom_color = %d", win_data->using_custom_color);
	g_debug("- win_data->using_custom_color_orig = %d", win_data->using_custom_color_orig);
	for (i=0; i<COLOR; i++)
	{
		g_debug("- win_data->color_value[%d] = %s", i, win_data->color_value[i]);

		gchar *temp_str = g_strdup_printf("win_data->color (%d)", i);
		print_color(temp_str, win_data->color[i]);
		g_free(temp_str);

		temp_str = g_strdup_printf("win_data->color_inactive (%d)", i);
		print_color(temp_str, win_data->color_inactive[i]);
		g_free(temp_str);

		temp_str = g_strdup_printf("win_data->color_orig (%d)", i);
		print_color(temp_str, win_data->color_orig[i]);
		g_free(temp_str);
	}
	g_debug("- win_data->color_brightness = %1.3f", win_data->color_brightness);
	g_debug("- win_data->color_brightness_orig = %1.3f", win_data->color_brightness_orig);
	g_debug("- win_data->color_brightness_inactive = %1.3f", win_data->color_brightness_inactive);
	g_debug("- win_data->page_width = %d", win_data->page_width);
	g_debug("- win_data->fixed_page_width = %d", win_data->fixed_page_width);
	g_debug("- win_data->fill_tab_bar = %d", win_data->fill_tab_bar);
	g_debug("- win_data->tabbar_position = %d", win_data->tabbar_position);
	g_debug("- win_data->page_name = %s", win_data->page_name);
	g_debug("- win_data->reuse_page_names = %d", win_data->reuse_page_names);
	g_debug("- win_data->page_names = %s", win_data->page_names);
	print_array("win_data->splited_page_names", win_data->splited_page_names);
	g_debug("- win_data->page_names_no = %d", win_data->page_names_no);
	g_debug("- win_data->max_page_names_no = %d", win_data->max_page_names_no);
	g_debug("- win_data->page_shows_number = %d", win_data->page_shows_number);
	g_debug("- win_data->page_shows_encoding = %d", win_data->page_shows_encoding);
	g_debug("- win_data->page_shows_current_cmdline = %d", win_data->page_shows_current_cmdline);
	g_debug("- win_data->page_shows_window_title = %d", win_data->page_shows_window_title);
	g_debug("- win_data->page_shows_current_dir = %d", win_data->page_shows_current_dir);
	g_debug("- win_data->use_color_page = %d", win_data->use_color_page);
	g_debug("- win_data->check_root_privileges = %d", win_data->check_root_privileges);
	g_debug("- win_data->bold_current_page_name = %d", win_data->bold_current_page_name);
	g_debug("- win_data->bold_action_page_name = %d", win_data->bold_action_page_name);
	for (i=0; i<PAGE_COLOR; i++)
		g_debug("- win_data->user_page_color[%d] = %s", i, win_data->user_page_color[i]);
	g_debug("- win_data->font_anti_alias = %d", win_data->font_anti_alias);
	g_debug("- win_data->default_font_name = %s", win_data->default_font_name);
	g_debug("- win_data->system_font_name = %s", win_data->system_font_name);
	g_debug("- win_data->restore_font_name = %s", win_data->restore_font_name);
	g_debug("- win_data->default_column = %d", win_data->default_column);
	g_debug("- win_data->default_row = %d", win_data->default_row);
	g_debug("- win_data->system_column = %d", win_data->system_column);
	g_debug("- win_data->system_row = %d", win_data->system_row);
	g_debug("- win_data->word_chars = %s", win_data->word_chars);
	g_debug("- win_data->show_scrollbar = %d", win_data->show_scrollbar);
	g_debug("- win_data->scrollbar_position = %d", win_data->scrollbar_position);
	g_debug("- win_data->transparent_background = %d", win_data->transparent_background);
	g_debug("- win_data->background_saturation = %1.3f", win_data->background_saturation);
//	g_debug("- win_data->use_scrollback_lines = %d", win_data->use_scrollback_lines);
	g_debug("- win_data->show_scrollbar = %d", win_data->show_scrollbar);
	g_debug("- win_data->scrollback_lines = %d", win_data->scrollback_lines);
	g_debug("- win_data->cursor_blinks = %d", win_data->cursor_blinks);
	g_debug("- win_data->audible_bell = %d", win_data->audible_bell);
	g_debug("- win_data->visible_bell = %d", win_data->visible_bell);
	g_debug("- win_data->erase_binding = %d", win_data->erase_binding);
	g_debug("- win_data->query_encoding = %d", win_data->query_encoding);
	g_debug("- win_data->kill_color_demo_vte = %d", win_data->kill_color_demo_vte);
	g_debug("- win_data->err_str.string = %s", win_data->err_str.string);
	g_debug("- win_data->err_str.encoding = %s", win_data->err_str.encoding);

// ---- page_data ---- //
	g_debug("- page_data->window = %p", page_data->window);
	g_debug("- page_data->notebook = %p", page_data->notebook);
	g_debug("- page_data->page_no = %d", page_data->page_no);
	g_debug("- page_data->label = %p", page_data->label);
	g_debug("- page_data->hbox = %p", page_data->hbox);
	g_debug("- page_data->vte = %p", page_data->vte);
	g_debug("- page_data->scrollbar = %p", page_data->scrollbar);
	g_debug("- page_data->encoding_str = %s", page_data->encoding_str);
//	g_debug("- page_data->locale = %s", page_data->locale);
//	g_debug("- page_data->environ = %s", page_data->environ);
//	g_debug("- page_data->VTE_CJK_WIDTH = %d", page_data->VTE_CJK_WIDTH);
	g_debug("- page_data->VTE_CJK_WIDTH_STR = %s", page_data->VTE_CJK_WIDTH_STR);
	g_debug("- page_data->page_name = %s", page_data->page_name);
	g_debug("- page_data->pid = %d", page_data->pid);
	g_debug("- page_data->new_tpgid = %d", page_data->new_tpgid);
	g_debug("- page_data->displayed_tpgid = %d", page_data->displayed_tpgid);
	g_debug("- page_data->custom_page_name = %s", page_data->custom_page_name);
	g_debug("- page_data->tab_color = %s", page_data->tab_color);
	g_debug("- page_data->pwd = %s", page_data->pwd);
	g_debug("- page_data->is_root = %d", page_data->is_root);
	g_debug("- page_data->is_bold = %d", page_data->is_bold);
	g_debug("- page_data->page_data->window_title_updated = %d", page_data->window_title_updated);
	g_debug("- page_data->page_update_method = %d", page_data->page_update_method);
	g_debug("- page_data->page_update_method_orig = %d", page_data->page_update_method_orig);
	g_debug("- page_data->window_title_signal = %ld", page_data->window_title_signal);
	g_debug("- page_data->timeout_id = %d", page_data->timeout_id);
	g_debug("- page_data->font_name = %s", page_data->font_name);
	g_debug("- page_data->font_size = %d", page_data->font_size);
	g_debug("- page_data->check_root_privileges = %d", page_data->check_root_privileges);
	g_debug("- page_data->page_shows_window_title = %d", page_data->page_shows_window_title);
	g_debug("- page_data->page_shows_current_dir = %d", page_data->page_shows_current_dir);
	g_debug("- page_data->page_shows_current_cmdline = %d", page_data->page_shows_current_cmdline);
	g_debug("- page_data->bold_action_page_name = %d", page_data->bold_action_page_name);
	g_debug("- page_data->*window_title_tpgid = %d", *(page_data->window_title_tpgid));
//	g_debug("- page_data->use_scrollback_lines = %d", page_data->use_scrollback_lines);
	g_debug("- page_data->*lost_focus = %d", *(page_data->lost_focus));
	g_debug("- page_data->*keep_vte_size = %d", *(page_data->keep_vte_size));
	g_debug("- page_data->*current_vte = %p", *(page_data->current_vte));
	g_debug("- page_data->window_title_pwd = %s", page_data->window_title_pwd);
	g_debug("- page_data->custom_window_title = %d", page_data->custom_window_title);
	// g_debug("- page_data->*update_window_title_only = %d", *(page_data->update_window_title_only));
	// g_debug("- page_data->force_using_cmdline = %d", page_data->force_using_cmdline);
	for (i=0; i<COMMAND; i++)
		g_debug("- page_data->tag[%d] = %d", i, page_data->tag[i]);
}

void print_array(gchar *name, gchar **data)
{
	if (data)
	{
		gint i = 0;
		while (data[i])
		{
			g_debug("- %s %d = %s", name, i, data[i]);
			i++;
		}
	}
}

void print_color(gchar *name, GdkColor color)
{
		g_debug("- %s = %x, %x, %x, %x",
			name,
			color.pixel,
			color.red,
			color.green,
			color.blue);
}
#endif
