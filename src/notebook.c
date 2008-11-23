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
// For scrollback_lines history
extern gint scrollback_lines;
extern GtkWidget *menuitem_scrollback_lines;
// Using pwd/cmdline on page name
extern gchar *page_name;
extern gboolean page_shows_current_cmdline;
extern gboolean window_shows_current_page;
extern gchar *page_normal_color;
extern gint page_width;
extern gboolean fixed_page_width;
// For init a new page
extern gchar *default_font_name;
extern gint default_column;
extern gint default_row;

extern GtkWidget *window;
extern GtkWidget *notebook;
GtkWidget *current_vtebox=NULL;

extern gboolean lost_focuse;
extern gboolean update_hints;
extern gboolean update_hints_once;
extern gint style_set;
gboolean add_remove_page = FALSE;

void add_page(gboolean run_once)
{
	if (add_remove_page) return;
	add_remove_page = TRUE;

	// the component of a single page
	struct Page *current_data = g_new0(struct Page, 1);
	const gchar *directory = g_getenv("PWD");
	
	// create label
	current_data->label = gtk_label_new(page_name);
	if (fixed_page_width)
		gtk_label_set_width_chars(GTK_LABEL(current_data->label), page_width);
	else
		gtk_label_set_max_width_chars(GTK_LABEL(current_data->label), page_width);
	gtk_label_set_ellipsize(GTK_LABEL(current_data->label), PANGO_ELLIPSIZE_MIDDLE);

	// create vtebox
	current_data->vtebox = vte_terminal_new();

	// save the data first
	g_object_set_data(G_OBJECT(current_data->vtebox), "Data", current_data);
	g_object_set_data(G_OBJECT(current_data->label), "VteBox", current_data->vtebox);

	// create a hbox
	current_data->hbox = gtk_hbox_new(FALSE, 0);

	// Get current vtebox size, font name and directory. for init a new tab.
	if ( gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)) != -1 && current_vtebox != NULL)
	{
		// Got the current page's directory
		struct Page *prev_page = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
		directory = g_file_read_link(g_strdup_printf("/proc/%d/cwd", prev_page->pid), NULL);
		// g_debug("use the directory %s (/proc/%d/cwd)\n", directory, prev_page->pid);
		
		// Got the font name ,column and row for prev page
		current_data->font_name = g_strdup(prev_page->font_name);
		current_data->column = vte_terminal_get_column_count(VTE_TERMINAL(current_vtebox));
		current_data->row = vte_terminal_get_row_count(VTE_TERMINAL(current_vtebox));
	}
	else
	{
		 current_data->font_name = g_strdup(default_font_name);
		 current_data->column = default_column;
		 current_data->row = default_row;
	}
	// g_debug("Init New vtebox with %d x %d!", current_data->column, current_data->row);
	// g_debug("Using the font : %s\n", current_data->font_name);

	// Init new page. run_once: some settings only need run once.
	// run_once only = TRUE when init LunaTerm in main().
	init_new_page(current_data->vtebox, current_data->font_name, current_data->column, current_data->row, run_once);
	gtk_box_pack_start(GTK_BOX(current_data->hbox), current_data->vtebox, TRUE, TRUE, 0);
	// the close page event
	g_signal_connect(G_OBJECT(current_data->vtebox), "child_exited", G_CALLBACK(close_page), FALSE);
	// when get focus, update `current_vtebox', hints, and windows title
	g_signal_connect(G_OBJECT(current_data->vtebox), "grab-focus", G_CALLBACK(vtebox_grab_focuse), NULL);
	// after size-allocate, save the column x row info!
	g_signal_connect_after(G_OBJECT(current_data->vtebox), "size-allocate", G_CALLBACK(vtebox_size_allocate), NULL);
	// launch vtebox_style_set()! becouse the font size may not be corrent before size-request.
	g_signal_connect(G_OBJECT(current_data->vtebox), "size-request", G_CALLBACK(vtebox_size_request), NULL);
	// show the menu
	g_signal_connect(G_OBJECT(current_data->vtebox), "button-press-event",
			 G_CALLBACK(vtebox_button_press), NULL);

	// scrollbar
	current_data->scrollbar = gtk_vscrollbar_new(vte_terminal_get_adjustment(VTE_TERMINAL(current_data->vtebox)));
	gtk_box_pack_start(GTK_BOX(current_data->hbox), current_data->scrollbar, FALSE, FALSE, 0);

	// add the new page to notebook
	current_data->current_page_no = gtk_notebook_append_page(GTK_NOTEBOOK(notebook), current_data->hbox, current_data->label);

#ifdef ENABLE_TAB_REORDER
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), current_data->hbox,TRUE);
	gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), current_data->hbox,FALSE);
#endif
	g_object_set(gtk_widget_get_parent(current_data->label), "can-focus", FALSE, NULL);
	
	// Execute programs in the vtebox
	//if (parameter != NULL)
	//	g_debug ("Run Command: %s %s\n", command_line, *parameter);
	current_data->pid = vte_terminal_fork_command(VTE_TERMINAL(current_data->vtebox),
						  command_line, parameter, NULL,
						  directory, TRUE, TRUE, TRUE);
	current_data->pwd = g_strdup(directory);
	// treat '-e option' as `custom_page_name'
	if (parameter != NULL && page_shows_current_cmdline)
		current_data->custom_page_name = get_cmdline(current_data->pid);
	else
		current_data->custom_page_name = NULL;
	
	// We MUST clear command_line and parameter after runed -e option.
	command_line = NULL;
	parameter = NULL;

	// for debug only
	// gchar *pid = g_strdup_printf("%d", current_data->pid);
	// gtk_label_set_text(GTK_LABEL(current_data->label), pid);
	// g_free(pid);
	
	// set the tab name.
	// we store stat_path here for performance.
	current_data->stat_path = g_strdup_printf("/proc/%d/stat", (gint) current_data->pid);
	current_data->tpgid = current_data->pid;
	current_data->tab_color = page_normal_color;
	// we need to g_free it in update_tab_name(). so we need to set it to NULL first.
	current_data->label->name = NULL;
	//update_tab_name(current_data->stat_path, current_data->label, current_data->pid, current_data->tpgid,
	//		current_data->current_page_no + 1, current_data->custom_page_name, directory);
	// g_debug("Got label name from update_tab_name(): %s\n", current_data->label->name);
	if (scrollback_lines)
		current_data->use_scrollback_lines = TRUE;

	// Monitor cmdline
	if (page_shows_current_cmdline)
		// monitor_cmdline(current_data->monitor, current_data->pid);
		// monitor_cmdline(current_data->channel, current_data->pid);
#ifdef USE_TIMEOUT_SECONDS
		current_data->timeout_id = g_timeout_add_seconds (1, (GSourceFunc)monitor_cmdline, current_data->vtebox);
#else
		current_data->timeout_id = g_timeout_add (1000, (GSourceFunc)monitor_cmdline, current_data->vtebox);
#endif

	// Store the new page data
	current_data->encoding = default_encoding;
	// g_object_set_data(G_OBJECT(current_data->vtebox), "Data", current_data);
	// g_object_set_data(G_OBJECT(current_data->label), "VteBox", current_data->vtebox);

	// show the page bar if necessary
	if (current_data->current_page_no == 1)
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);

	// finish!
	// g_debug("Showing the new page!");
	gtk_widget_show_all(current_data->hbox);
	//gtk_widget_queue_draw (current_data->vtebox);
	//gtk_widget_set_redraw_on_allocate (current_data->vtebox, TRUE);
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), current_data->current_page_no);
	gtk_window_set_focus(GTK_WINDOW(window), current_data->vtebox);

	// we move update_tab_name here becouse the page_name can't be update when add_remove_page = TRUE
	add_remove_page = FALSE;
	update_tab_name(current_data->stat_path, current_data->label, current_data->pid, current_data->tpgid,
			current_data->current_page_no + 1, current_data->custom_page_name, directory);
}

gboolean close_page (GtkWidget *vtebox, gboolean need_safe_close)
{
	if (add_remove_page) return FALSE;
	add_remove_page = TRUE;

	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	// g_debug("Total Page (Notebook):%d\n", total_page);

	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
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

	// kill running shell
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
			update_hints = FALSE;
		}
	}

	// free the memory used by this page
	// g_debug("freeing current_data!\n");
	g_free(current_data->stat_path);
	g_free(current_data);

	add_remove_page = FALSE;

	// we move reorder_page_number here becouse the page_name can't be update when add_remove_page = TRUE
	if (current_data->current_page_no < total_page-1)
		reorder_page_number(NULL, NULL);

	return TRUE;
}

void vtebox_grab_focuse(GtkWidget *vtebox, gpointer user_data)
{
	if (current_vtebox != vtebox || (current_vtebox == NULL))
	{
		// g_debug ("Update current_vtebox! : %d", vtebox);
		current_vtebox = vtebox;
	
		// we should bind the hints information on vtebox.
		// Or the geometry of vtebox may be changed when deleting the vtebox hold hints info.
		// g_debug("Updating hints for %d page in vtebox_grab_focuse!\n", gtk_notebook_get_n_pages (notebook));
		if (update_hints)
			// if update_hints > 0, we should update the FONT hints for every vtebox.
			window_resizable(vtebox, 2, -1);
		else
			// else, updte the hints without font. It can help to hold the currect vtebox size.
			window_resizable(vtebox, 0, -1);
	
		if (window_shows_current_page)
		{
			struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
			if (current_data!=NULL)
				update_window_title(current_data->label->name);
		}
	}
}

void set_vtebox_geometry(GtkWidget *vtebox)
{
	//g_debug("set_vtebox_geometry!");
	//g_debug("Current %d vtebox size is %dx%d\n",
	//	gtk_notebook_get_n_pages (notebook),
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

	if (vtebox!=NULL)
	{
		// retore the vtebox size
		struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
		if (current_data!=NULL)
		{
			gint vtebox_width, vtebox_height, x_pad, y_pad;
			
			//g_debug("vtebox char size (fixing): %d x %d",
			//	vte_terminal_get_char_width(VTE_TERMINAL(vtebox)),
			//	vte_terminal_get_char_height(VTE_TERMINAL(vtebox)));
			
			//g_debug("fixing vtebox to %d x %d", current_data->column, current_data->row);
			vte_terminal_set_size (VTE_TERMINAL(vtebox), current_data->column, current_data->row);
			
			vte_terminal_get_padding(VTE_TERMINAL(vtebox), &x_pad, &y_pad);
			vtebox_width = x_pad + vte_terminal_get_char_width(VTE_TERMINAL(vtebox)) * current_data->column;
			vtebox_height = y_pad + vte_terminal_get_char_height(VTE_TERMINAL(vtebox)) * current_data->row;
			gtk_widget_set_size_request(vtebox, vtebox_width, vtebox_height);

			//g_debug("Restore the size to %d x %d ( %d x %d )\n",
			//	vtebox_width, vtebox_height, current_data->column, current_data->row);
		}
	}
}

void vtebox_size_request (GtkWidget *vtebox, GtkRequisition *requisition, gpointer user_data)
{
	//g_debug("vtebox_size_request! : %d", vtebox);
	//g_debug("Current %d vtebox size is %dx%d\n",
	//	gtk_notebook_get_n_pages (notebook),
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

	//g_debug("style_set = %d", style_set);
	if (style_set)
	{
		set_vtebox_geometry(vtebox);

	//	g_debug("Current %d vtebox size is %dx%d\n",
	//		gtk_notebook_get_n_pages (notebook),
	//		vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//		vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));
	}
}

void vtebox_size_allocate (GtkWidget *vtebox, GtkAllocation *allocation, gpointer user_data)
{
	//g_debug("vtebox_size_allocate! : %d", vtebox);
	//g_debug("Current %d vtebox size is %dx%d\n",
	//	gtk_notebook_get_n_pages (notebook),
	//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
	//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));
	
	if (lost_focuse || update_hints_once)
	{
		// g_debug("! Saving the column x row data of %d vtebox to %d x %d...\n",
		//	gtk_notebook_get_n_pages (notebook),
		//	vte_terminal_get_column_count(VTE_TERMINAL(vtebox)),
		//	vte_terminal_get_row_count(VTE_TERMINAL(vtebox)));

		struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
		current_data->column = vte_terminal_get_column_count(VTE_TERMINAL(vtebox));
		current_data->row = vte_terminal_get_row_count(VTE_TERMINAL(vtebox));

		if (!update_hints_once)
			// if the size is changed by mouse moving, set the vtebox size.
			// or it will recover to the original size after the mouse button release.
			set_vtebox_geometry(vtebox);
		
		update_hints_once = FALSE;
	}
}

gboolean vtebox_button_press(GtkWidget *vtebox, GdkEventButton *event, gpointer user_data)
{
	if (event->button == 3)
	{
		struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
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

