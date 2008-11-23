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


#include "notebook.h"

// For command line options.
extern gchar *command_line;
extern gchar **parameter;
// For showing the Menu if right click on a vtebox.
extern GtkWidget *menu;
// For Transparent Window in menu
extern gboolean show_transparent_menu;
#ifdef ENABLE_RGBA
extern GtkWidget *menuitem_trans_win;
extern gboolean transparent_window;
#endif
// For Transparent Background in menu
extern GtkWidget *menuitem_trans_bg;
extern gboolean transparent_background;
extern gboolean use_rgba;
// For Set the radio_list to System Defalut
extern GtkWidget *default_encoding;
extern gchar **supported_locales;
// For monitor the pid's stat
extern gboolean page_shows_current_cmdline;
extern gboolean window_shows_current_page;

extern gchar *default_font_name;
extern gint default_column;
extern gint default_row;
gboolean lost_focuse;
extern gboolean update_hints;
extern GtkWidget *window;
extern GtkWidget *notebook;
GtkWidget *current_vtebox=NULL;

extern gboolean force_resize_window;
gint keep_vtebox_size;

extern gint scrollback_lines;
extern GtkWidget *menuitem_scrollback_lines;

void add_page(gboolean run_once)
{
	// the component of a single page
	struct Page *new_page = g_new0(struct Page, 1);
	const gchar *directory = g_getenv("PWD");
	gint column, row;

	// create label
	new_page->label = gtk_label_new(NULL);
	gtk_label_set_max_width_chars (GTK_LABEL(new_page->label), 20);
	gtk_label_set_ellipsize(GTK_LABEL(new_page->label), PANGO_ELLIPSIZE_MIDDLE);

	// create a hbox
	new_page->hbox = gtk_hbox_new(FALSE, 0);

	// create vtebox
	new_page->vtebox = vte_terminal_new();

	// Get current vtebox size, font name and directory. for init a new tab.
	if ( gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)) != -1 && current_vtebox != NULL)
	{
		// Got the current page's directory
		struct Page *prev_page = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
		directory = g_file_read_link(g_strdup_printf("/proc/%d/cwd", prev_page->pid), NULL);
		// g_debug("use the directory %s (/proc/%d/cwd)\n", directory, prev_page->pid);
		
		// Got the font name ,column and row for prev page
		new_page->font_name = g_strdup(prev_page->font_name);
		column = vte_terminal_get_column_count(VTE_TERMINAL(current_vtebox));
		row = vte_terminal_get_row_count(VTE_TERMINAL(current_vtebox));
	}
	else
	{
		 new_page->font_name = g_strdup(default_font_name);
		 column = default_column;
		 row = default_row;
	}
	// g_debug("Update current_vtebox with %d x %d!", column, row);
	// g_debug("Using the font : %s\n", new_page->font_name);

	// Init new page. run_once: some settings only need run once.
	// run_once only = TRUE when init LilyTerm in main().
	init_new_page(new_page->vtebox, new_page->font_name, column, row, run_once);
	gtk_box_pack_start(GTK_BOX(new_page->hbox), new_page->vtebox, TRUE, TRUE, 0);
	// the close page event
	g_signal_connect(G_OBJECT(new_page->vtebox), "child_exited", G_CALLBACK(close_page), FALSE);
	// when get focus, update `current_vtebox' and hints
	g_signal_connect(G_OBJECT(new_page->vtebox), "grab-focus", G_CALLBACK(vtebox_grab_focuse), NULL);
	// if get focuse, the size of vtebox is NOT resizeable.
	g_signal_connect_after(G_OBJECT(new_page->vtebox), "focus-in-event", G_CALLBACK(vtebox_get_focuse), NULL);
	// if lost focuse, the size of vtebox is resizeable.
	g_signal_connect(G_OBJECT(new_page->vtebox), "focus-out-event", G_CALLBACK(vtebox_lost_focuse), NULL);
	// trying to current the vtebox size when the font rendering / theme changed.
	g_signal_connect_after(G_OBJECT(new_page->vtebox), "style-set", G_CALLBACK(vtebox_style_set), NULL);
	g_signal_connect_after(G_OBJECT(new_page->vtebox), "size-allocate", G_CALLBACK(vtebox_size_allocate), NULL);

	// show the menu
	g_signal_connect(G_OBJECT(new_page->vtebox), "button-press-event",
			 G_CALLBACK(vtebox_button_press), NULL);

	// scrollbar
	new_page->scrollbar = gtk_vscrollbar_new(vte_terminal_get_adjustment(VTE_TERMINAL(new_page->vtebox)));
	gtk_box_pack_start(GTK_BOX(new_page->hbox), new_page->scrollbar, FALSE, FALSE, 0);

	// add the new page to notebook
	new_page->current_page_no = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), new_page->hbox, new_page->label);

#ifdef ENABLE_TAB_REORDER
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), new_page->hbox,TRUE);
	gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), new_page->hbox,FALSE);
#endif
	g_object_set(gtk_widget_get_parent(new_page->label), "can-focus", FALSE, NULL);
	
	// show the page bar if necessary
	if (new_page->current_page_no == 1)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);

	// Execute programs in the vtebox
	//if (parameter != NULL)
	//	g_debug ("Run Command: %s %s\n", command_line, *parameter);
	new_page->pid = vte_terminal_fork_command(VTE_TERMINAL(new_page->vtebox),
						  command_line, parameter, NULL,
						  directory, TRUE, TRUE, TRUE);
	// We MUST clear command_line and parameter after runed -e option.
	command_line = NULL;
	parameter = NULL;

	// for debug only
	// gchar *pid = g_strdup_printf("%d", new_page->pid);
	// gtk_label_set_text(GTK_LABEL(new_page->label), pid);
	// g_free(pid);

	// set the tab name.
	// we store stat_path here for performance.
	new_page->stat_path = g_strdup_printf("/proc/%d/stat", (gint) new_page->pid);
	// we need to g_free it in update_tab_name(). so we need to set it to NULL first.
	new_page->label->name = NULL;
	new_page->custom_page_name = NULL;
	update_tab_name(new_page->stat_path, new_page->label, new_page->pid, &(new_page->tpgid),
			new_page->current_page_no + 1, new_page->custom_page_name);
	// g_debug("Got label name from update_tab_name(): %s\n", new_page->label->name);
	if (scrollback_lines)
		new_page->use_scrollback_lines = TRUE;

	// Monitor cmdline
	if (page_shows_current_cmdline)
		// monitor_cmdline(new_page->monitor, new_page->pid);
		// monitor_cmdline(new_page->channel, new_page->pid);
#ifdef USE_TIMEOUT_SECONDS
		new_page->timeout_id = g_timeout_add_seconds (1, (GSourceFunc)monitor_cmdline, NULL);
#else
		new_page->timeout_id = g_timeout_add (1000, (GSourceFunc)monitor_cmdline, NULL);
#endif

	// finish!
	// g_debug("Showing the new page!");
	gtk_widget_show_all(new_page->hbox);
	gtk_widget_queue_draw (new_page->vtebox);
	gtk_widget_set_redraw_on_allocate (new_page->vtebox, TRUE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), new_page->current_page_no);
	gtk_window_set_focus(GTK_WINDOW(window), new_page->vtebox);

	// Store the new page data
	new_page->encoding = default_encoding;
	g_object_set_data(G_OBJECT(new_page->vtebox), "Data", new_page);
	g_object_set_data(G_OBJECT(new_page->label), "VteBox", new_page->vtebox);
}

gboolean close_page (GtkWidget *widget, gboolean need_safe_close)
{
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	// g_debug("Total Page (Notebook):%d\n", total_page);

	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
	// g_debug("Deleting Page ID: %d\n", current_data->pid);

	// only "child_exited" don't need need_safe_close to kill SHELL
	if (need_safe_close)
	{
		gint tpgid = get_tpgid(current_data->stat_path, current_data->pid);
		// g_debug("pid=%d, and tpgid=%d\n", current_data->pid, tpgid);
		if (current_data->pid != tpgid)
			if (dialog(NULL, 7)==FALSE) return FALSE;
	}

	// remove timeout event for page_shows_current_cmdline
	if (page_shows_current_cmdline)
	//	monitor_cmdline_cancel(current_data->monitor);
		g_source_remove (current_data->timeout_id);

	// stop running shell
	if (need_safe_close)
	{
		// if the tab is not close by <Ctrl><D>, we need to launch kill()
		// g_debug("Trying to kill %d!\n", current_data->pid);
		kill(current_data->pid, SIGKILL);
	}

	// if it is the last page, exit application!
	if (total_page == 1)
	{
		// g_debug("It is the last page!\n");
		gtk_main_quit();
	}
	else
	{
		// remove current page
		// g_debug ("The %d page is going to be removed!\n", current_data->current_page_no);
		// use current_data->current_page_no. DANGEROUS!
		gtk_notebook_remove_page(GTK_NOTEBOOK(notebook), current_data->current_page_no);
		
		// set the current page
		// g_debug ("Setting current page to %d!\n", current_data->current_page_no);
		if ( current_data->current_page_no < total_page-1 )
			gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), current_data->current_page_no);
		
		// hide the tab bar if necessary
		if (total_page == 2)
		{
			// hide the page bar
			gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), FALSE);
			// Yes, just set it to 1 x 1,
			// and the window will be resized to correct geometry automatically.
			gtk_window_resize(GTK_WINDOW(window), 1, 1);
		}
	}
	if (current_data->current_page_no < total_page-1)
		reorder_page_number(NULL, NULL);

	// free the memory used by this page
	// g_debug("freeing current_data!\n");
	g_free(current_data->stat_path);
	g_free(current_data);
	return TRUE;
}

void vtebox_grab_focuse(GtkWidget *vtebox, gpointer user_data)
{
	if (current_vtebox != vtebox)
	{
		//g_debug ("Update current_vtebox! : %d", vtebox);
		current_vtebox = vtebox;
	
		// we should bind the hints information on vtebox.
		// Or the geometry of vtebox may be changed when deleting the vtebox hold hints info.
		// g_debug("Updating hints for %d page!\n", gtk_notebook_get_n_pages (notebook));
		if (update_hints)
			// if update_hints = TRUE, we should update the FONT hints for every vtebox.
			window_resizable(vtebox, 1, -1);
		else
			// else, updte the hints without font
			window_resizable(vtebox, 0, -1);
	
		if (window_shows_current_page)
		{
			struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
			if (current_data!=NULL)
				update_window_title(current_data->label->name);
		}
	}
}

void vtebox_get_focuse(GtkWidget *vtebox, gpointer user_data)
{
	//g_debug("! vte_get_focuse!");
	//g_debug("Got force_resize_window = %d", force_resize_window);
	//g_debug("Current vtebox size is %dx%d\n",
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

	// we should NOT put the following codes in vtebox_grab_focuse()
	// vtebox_grab_focuse(): only runs when switching tabs
	// vtebox_get_focuse(): always runs at any time.
	
	// force_resize_window: using in change the font of vtebox.
	if (vtebox!=NULL && (!force_resize_window))
		window_resizable(vtebox, 0, -1);
	// else
	//	force_resize_window = FALSE;

	lost_focuse = FALSE;
	keep_vtebox_size = 0;
}

void vtebox_lost_focuse(GtkWidget *vtebox, gpointer user_data)
{
	//g_debug("! vte_lost_focuse!");
	//g_debug("Current vtebox size is %dx%d\n",
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

	if (vtebox!=NULL && (!force_resize_window))
		window_resizable(vtebox, 0, 1);
	
	lost_focuse = TRUE;
}

void vtebox_style_set (GtkWidget *vtebox, GtkStyle *previous_style, gpointer user_data)
{
	//g_debug("vtebox_style_set!");
	//g_debug("Current vtebox size is %dx%d\n",
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

	if (lost_focuse && vtebox!=NULL)
	{
		// g_debug("Restoring the data...");
		window_resizable(vtebox, 2, -1);
		// retore the vtebox size
		struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
		vte_terminal_set_size (VTE_TERMINAL(vtebox), current_data->column, current_data->row);
		gtk_window_resize(GTK_WINDOW(window), 1, 1);
		keep_vtebox_size++;
	}
}

void vtebox_size_allocate (GtkWidget *vtebox, GtkAllocation *allocation, gpointer user_data)
{
	//g_debug("vtebox_size_allocate! : %d", vtebox);
	//g_debug("Current vtebox size is %dx%d\n",
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

	if (vtebox!=NULL)
	{
		if (keep_vtebox_size)
		{
			keep_vtebox_size--;

			if (keep_vtebox_size<=0)
			{
				// g_debug("Updating the hints data!\n");
				if (update_hints)
					window_resizable(vtebox, 2, 1);
				else
					window_resizable(vtebox, 1, 1);
				keep_vtebox_size = 0;
			}
		}
		else
		{
			// g_debug("! Saving the data...");
			struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
			current_data->column = vte_terminal_get_column_count(VTE_TERMINAL(vtebox));
			current_data->row = vte_terminal_get_row_count(VTE_TERMINAL(vtebox));
		}
	}
	force_resize_window = FALSE;
}

gboolean vtebox_button_press(GtkWidget *widget, GdkEventButton *event, gpointer user_data)
{
	if (event->button == 3)
	{
		struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
		if (*supported_locales!=NULL)
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(current_data->encoding), TRUE);
		if (show_transparent_menu)
		{
#ifdef ENABLE_RGBA
			if (use_rgba)
				GTK_CHECK_MENU_ITEM(menuitem_trans_win)->active = transparent_window;
#endif
			GTK_CHECK_MENU_ITEM(menuitem_trans_bg)->active = transparent_background;
		}
		if (scrollback_lines)
			GTK_CHECK_MENU_ITEM(menuitem_scrollback_lines)->active = current_data->use_scrollback_lines;
		gtk_menu_popup(GTK_MENU(menu), NULL, NULL, NULL, NULL, event->button, event->time);
		return TRUE;
	}
	return FALSE;
}

