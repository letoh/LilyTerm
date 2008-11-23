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

// For single process
extern gint total_window;
extern gboolean single_process;

extern GtkWidget *active_window;
GtkWidget *current_vtebox=NULL;

GtkWidget *add_page(GtkWidget *window, GtkWidget *notebook, GtkWidget *menuitem_encoding,
		    gchar *locale, gchar *user_environ, gboolean run_once, gint VTE_CJK_WIDTH)
{
	GtkWidget *prev_vtebox = current_vtebox;
#ifdef DEBUG
	if (menuitem_encoding)
		g_debug("! Launch add_page() with window = %p, encoding = %s, locale = %s,"
			"environ = %s, run_once = %d, VTE_CJK_WIDTH = %d",
			window, menuitem_encoding->name, locale, user_environ, run_once, VTE_CJK_WIDTH);
	else
		g_debug("! Launch add_page() with window = %p, encoding = %p, locale = %s,"
			"environ = %s, run_once = %d, VTE_CJK_WIDTH = %d",
			window, menuitem_encoding, locale, user_environ, run_once, VTE_CJK_WIDTH);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when adding page!", win_data);

	extern char **environ;
	gchar **environ_orig = environ;
	environ = win_data->environment;

	//if (environ)
	//{
	//	gint i = 0;
	//	while (environ[i])
	//		g_debug("%d, %s", i, environ[i++]);
	//}
	
	// Set the VTE_CJK_WIDTH environment
	set_VTE_CJK_WIDTH_environ(VTE_CJK_WIDTH);
	// g_debug("Get VTE_CJK_WIDTH =%d", VTE_CJK_WIDTH);
	
	guint column, row;
	gchar **LC_ALL=NULL;

	// the component of a single page
	struct Page *page_data = g_new0(struct Page, 1);
	page_data->window = window;
	page_data->notebook = notebook;
	page_data->VTE_CJK_WIDTH = VTE_CJK_WIDTH;
	
	// create label
	page_data->label = gtk_label_new(win_data->page_name);
	if (win_data->fixed_page_width)
		gtk_label_set_width_chars(GTK_LABEL(page_data->label), win_data->page_width);
	else
		gtk_label_set_max_width_chars(GTK_LABEL(page_data->label), win_data->page_width);
	gtk_label_set_ellipsize(GTK_LABEL(page_data->label), PANGO_ELLIPSIZE_MIDDLE);

	// create vtebox
	page_data->vtebox = vte_terminal_new();
	// g_debug("The default encoding of vtebox is %s",
	// 					vte_terminal_get_encoding(VTE_TERMINAL(page_data->vtebox)));

	// save the data first
	g_object_set_data(G_OBJECT(page_data->vtebox), "Page_Data", page_data);
	g_object_set_data(G_OBJECT(page_data->label), "VteBox", page_data->vtebox);

	// create a hbox
	page_data->hbox = gtk_hbox_new(FALSE, 0);

	// Get current vtebox size, font name and directory. for init a new tab.
	if ( gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook)) != -1 && prev_vtebox != NULL)
	{
		// Got the current page's directory
		struct Page *prev_page = (struct Page *)g_object_get_data(G_OBJECT(prev_vtebox), "Page_Data");
		gchar *priv_pwd = g_strdup_printf("/proc/%d/cwd", prev_page->pid);
		page_data->pwd = g_file_read_link(priv_pwd, NULL);
		g_free(priv_pwd);
		// g_debug("use the directory %s (/proc/%d/cwd)\n", directory, prev_page->pid);
		
		// Got the font name ,column and row for prev page
		page_data->font_name = g_strdup(prev_page->font_name);
		column = vte_terminal_get_column_count(VTE_TERMINAL(prev_vtebox));
		row = vte_terminal_get_row_count(VTE_TERMINAL(prev_vtebox));
		if (win_data->supported_locales!=NULL)
		{
			// the encoding of 1st page will be NULL. so...
			if (prev_page->encoding == NULL)
				prev_page->encoding = win_data->default_encoding;
			if (menuitem_encoding==NULL)
				page_data->encoding = prev_page->encoding;
			else
				page_data->encoding = menuitem_encoding;
			// g_debug("call set_encoding() by %p to %p", page_data->vtebox, page_data->encoding);
			set_encoding(page_data->encoding, page_data->vtebox);
		}
		else
			set_encoding(NULL, page_data->vtebox);
		
		if (locale==NULL)
			page_data->locale = g_strdup(prev_page->locale);
		else
			page_data->locale = g_strdup(locale);
		if (user_environ==NULL)
			page_data->environ = g_strdup(prev_page->environ);
		else
			page_data->environ = g_strdup(user_environ);
	}
	else
	{
		page_data->font_name = g_strdup(win_data->default_font_name);
		column = win_data->default_column;
		row = win_data->default_row;
		if (locale==NULL)
			page_data->locale = g_strdup("");
		else
			page_data->locale = g_strdup(locale);
		if (user_environ==NULL)
			page_data->environ = g_strdup("");
		else
			page_data->environ = g_strdup(user_environ);
		page_data->pwd = g_strdup(g_getenv("PWD"));
		// g_debug("call set_encoding() with menuitem = NULL");
		set_encoding(NULL, page_data->vtebox);
	}
	// g_debug("Set the encoding of vtebox to %s" ,vte_terminal_get_encoding(VTE_TERMINAL(page_data->vtebox)));
	gchar *temp_LC_ALL;
	// g_debug("page_data->locale = %s", page_data->locale);
	if (strlen(page_data->locale))
		temp_LC_ALL = g_strdup_printf("LANG=%s LANGUAGE=%s LC_ALL=%s %s",
				page_data->locale, page_data->locale, page_data->locale, page_data->environ);
	else
		temp_LC_ALL = g_strdup_printf("%s", page_data->environ);
	LC_ALL = g_strsplit_set(temp_LC_ALL, " ", 0);
	// g_debug("Set locale to %s, default is %s", temp_LC_ALL, win_data->default_locale);
	g_free(temp_LC_ALL);
	// g_debug("Init New vtebox with %d x %d!", column, row);
	// g_debug("Using the font : %s\n", page_data->font_name);

	// Init new page. run_once: some settings only need run once.
	// run_once only = TRUE when init LilyTerm in main().
	init_new_page(window, win_data, page_data->vtebox, page_data->font_name, column, row,
		      run_once, win_data->enable_hyperlink);

	// g_debug("win_data->scrollbar_position = %d", win_data->scrollbar_position);
	if (win_data->scrollbar_position==2)
		gtk_box_pack_end(GTK_BOX(page_data->hbox), page_data->vtebox, TRUE, TRUE, 0);
	else
		gtk_box_pack_start(GTK_BOX(page_data->hbox), page_data->vtebox, TRUE, TRUE, 0);
	// the close page event
	g_signal_connect(G_OBJECT(page_data->vtebox), "child_exited", G_CALLBACK(close_page), FALSE);
	// when get focus, update `current_vtebox', hints, and window title
	g_signal_connect(G_OBJECT(page_data->vtebox), "grab-focus", G_CALLBACK(vtebox_grab_focus), window);
	
	// show the menu
	g_signal_connect(G_OBJECT(page_data->vtebox), "button-press-event",
			 G_CALLBACK(vtebox_button_press), window);

	if (win_data->scrollbar_position)
	{
		// scrollbar
		page_data->scrollbar = gtk_vscrollbar_new(vte_terminal_get_adjustment(VTE_TERMINAL(page_data->vtebox)));
		if (win_data->scrollbar_position==1)
			gtk_box_pack_start(GTK_BOX(page_data->hbox), page_data->scrollbar, FALSE, FALSE, 0);
		else
			gtk_box_pack_end(GTK_BOX(page_data->hbox), page_data->scrollbar, FALSE, FALSE, 0);
	}

//	GdkColor root_color;
//	gdk_color_parse("#FFFAFE", &root_color);
//	gtk_widget_modify_bg(notebook, GTK_STATE_NORMAL, &root_color);
	// add the new page to notebook.
	// Note that due to historical reasons,
	// GtkNotebook refuses to switch to a page unless the child widget is visible.
	// Therefore, it is recommended to show child widgets before adding them to a notebook.
	gtk_widget_show_all(page_data->hbox);
	page_data->page_no = gtk_notebook_append_page(GTK_NOTEBOOK(notebook),
						      page_data->hbox, page_data->label);
	// g_debug("The new page no is %d", page_data->page_no);

#ifdef ENABLE_TAB_REORDER
	gtk_notebook_set_tab_reorderable(GTK_NOTEBOOK(notebook), page_data->hbox, TRUE);
	gtk_notebook_set_tab_detachable(GTK_NOTEBOOK(notebook), page_data->hbox, FALSE);
#endif
	// g_object_set(gtk_widget_get_parent(page_data->label), "can-focus", FALSE, NULL);
	if (win_data->fill_tab_bar)
		gtk_notebook_set_tab_label_packing(GTK_NOTEBOOK(notebook), page_data->hbox, TRUE, TRUE, GTK_PACK_START);

	//if (command_line==NULL)
	//	command_line = g_getenv("SHELL");

	// Execute programs in the vtebox
	//if (win_data->command_line)
	//	g_debug("cmmand line = %s", win_data->command_line);
	//if (win_data->parameters)
	//	g_debug("parameters = %s", *(win_data->parameters));
	//if (LC_ALL)
	//	g_debug("LC_ALL = %s", *LC_ALL);
	page_data->pid = vte_terminal_fork_command(VTE_TERMINAL(page_data->vtebox),
						      win_data->command_line, win_data->parameters, LC_ALL,
						      page_data->pwd, TRUE, TRUE, TRUE);
	// treat '-e option' as `custom_page_name'
	if (win_data->parameters != NULL && win_data->page_shows_current_cmdline)
		page_data->custom_page_name = get_cmdline(page_data->pid);
//	else
//		page_data->custom_page_name = NULL;

	win_data->environment = environ;
	environ = environ_orig;	
	// set the tab name.
	page_data->tpgid = page_data->pid;
	page_data->tab_color = win_data->page_normal_color;
	page_data->is_root = check_is_root(page_data->pid);
	page_data->is_bold = win_data->bold_current_page_name;
	// we need to g_free it in update_tab_name(). so we need to set it to NULL first.
	page_data->label->name = NULL;
	
	if (win_data->scrollback_lines)
		page_data->use_scrollback_lines = TRUE;

	// some data came from window. for the performance of monitor_cmdline
	page_data->lost_focus = &(win_data->lost_focus);
	page_data->keep_vtebox_size = &(win_data->keep_vtebox_size);
	page_data->check_root_privileges = &(win_data->check_root_privileges);
	page_data->page_shows_current_dir = &(win_data->page_shows_current_dir);
	page_data->page_shows_current_cmdline = &(win_data->page_shows_current_cmdline);
	page_data->bold_action_page_name = &(win_data->bold_action_page_name);
	
	// Monitor cmdline
	if (win_data->page_shows_current_cmdline || win_data->page_shows_current_dir)
		// monitor_cmdline(page_data->monitor, page_data->pid);
		// monitor_cmdline(page_data->channel, page_data->pid);
#ifdef USE_TIMEOUT_SECONDS
		page_data->timeout_id = g_timeout_add_seconds (1, (GSourceFunc)monitor_cmdline, page_data);
#else
		page_data->timeout_id = g_timeout_add (1000, (GSourceFunc)monitor_cmdline, page_data);
#endif
	// g_debug("Timeout Added: %d (%d)", page_data->timeout_id, page_data->vtebox);
	update_tab_name(page_data);
	// g_debug("Got label name from update_tab_name(): %s\n", page_data->label->name);

	// show the page bar if necessary
	if (page_data->page_no == 1)
	{
		// It will done in update_page_name()
		// keep_vtebox_size |= 2;
		gtk_notebook_set_show_tabs(GTK_NOTEBOOK(notebook), TRUE);
		gtk_notebook_set_show_border(GTK_NOTEBOOK(notebook), TRUE);
		GTK_WIDGET_UNSET_FLAGS(notebook, GTK_CAN_FOCUS);
	}

	// finish!
	gtk_notebook_set_current_page(GTK_NOTEBOOK(notebook), page_data->page_no);
	gtk_window_set_focus(GTK_WINDOW(window), page_data->vtebox);

	// g_debug("The final encoding of vtebox is %s" ,vte_terminal_get_encoding(VTE_TERMINAL(page_data->vtebox)));

	// page_data->pid < 0: Error occurred when creating sub process.
	if (page_data->pid < 0)
	{
		// g_debug("Error occurred when creating sub process");
		GString *arg_str = g_string_new("");
		// Trying to avoid double_free()
		gchar *old_temp_str;
		
		gint i;
		for (i=0; i<win_data->parameter; i++)
			g_string_append_printf(arg_str, "%s ", win_data->parameters[i]);
		old_temp_str = win_data->temp_str;
		win_data->temp_str = arg_str->str;
		dialog (NULL, 16);
		close_page (page_data->vtebox, FALSE);
		win_data->temp_str = old_temp_str;
		g_string_free(arg_str, TRUE);
	}

	g_strfreev(LC_ALL);
	
	// g_debug("current_vtebox = %d and page_data->vtebox = %d", current_vtebox, page_data->vtebox);
	if ((current_vtebox == NULL) || (current_vtebox != page_data->vtebox))
	{
		// current_vtebox = NULL means pid < 0 and the whole window is destroyed.
		// g_debug("whole window is destroyed!");
		return NULL;
	}
	else
	{
		// We MUST clear command_line and parameters after ran -e option.
		win_data->command_line = NULL;
		win_data->parameters = NULL;
		return page_data->vtebox;
	}
}

gboolean close_page(GtkWidget *vtebox, gboolean need_safe_close)
{
#ifdef DEBUG
	g_debug("! Launch close_page() with vtebox = %p, and need_safe_close = %d", vtebox, need_safe_close);
#endif

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
	// g_debug("Get page_data = %d when closing page!", page_data);
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when closing page!", win_data);

	// g_debug("Deleting Page ID: %d\n", page_data->pid);

	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(page_data->notebook));
	// g_debug("Total Page (Notebook):%d\n", total_page);

	if (page_data->pid<1)
		need_safe_close = FALSE;

	// only "child_exited" don't need need_safe_close to kill SHELL
	if (need_safe_close)
		if (page_data->pid != get_tpgid(page_data->pid))
			if (dialog(NULL, 7)==FALSE)
				return FALSE;

	// remove timeout event for page_shows_current_cmdline
	if (win_data->page_shows_current_cmdline||win_data->page_shows_current_dir)
	//	monitor_cmdline_cancel(page_data->monitor);
		g_source_remove (page_data->timeout_id);

	// kill running shell
	if (need_safe_close)
	{
		// if the tab is not close by <Ctrl><D>, we need to launch kill()
		// g_debug("Trying to kill %d!\n", page_data->pid);
		kill(page_data->pid, SIGKILL);
	}

	// if it is the last page, exit application!
	if (total_page == 1)
	{
		total_window--;
		if (total_window)
		{
			// close window only
			int i;
			// g_debug("Free user_keys");
			for (i=0; i<KEYS; i++)
				g_free(win_data->user_keys[i].value);

			// g_debug("Free user_command");
			for (i=0; i<COMMAND; i++)
			{
				g_free(win_data->user_command[i].command);
				g_free(win_data->user_command[i].environ);
				g_strfreev(win_data->user_command[i].environments);
			}

			// g_debug("Free menu");
			if (win_data->menu!=NULL)
				gtk_widget_destroy(win_data->menu);

			// g_debug("** Free argv");
			g_free(win_data->argv);
			// command_line and parameters can NOT be freed.
			// They are a pointer for win_data->argv.
			// win_data->command_line = NULL;
			// win_data->parameters = NULL;
			
			// g_debug("Free win_data->profile");
			g_free(win_data->profile);

			// g_debug("Free page_names");
			g_free(win_data->page_name);
			g_free(win_data->page_names);
			g_strfreev(win_data->splited_page_names);
			// g_debug("Free colors");
			g_free(win_data->foreground_color);
			g_free(win_data->background_color);
			g_free(win_data->page_cmdline_color);
			g_free(win_data->page_dir_color);
			g_free(win_data->page_custom_color);
			g_free(win_data->page_root_color);
			g_free(win_data->page_normal_color);
			// g_debug("Free font_name");
			g_free(win_data->default_font_name);
			g_free(win_data->system_font_name);
			// g_debug("Free word_chars");
			g_free(win_data->word_chars);
			// g_debug("** Free supported_locales");
			g_strfreev(win_data->supported_locales);
			// g_debug("Free locales_list");
			g_free(win_data->locales_list);
			// g_debug("** Free default_locale");
			g_free(win_data->default_locale);
			// g_debug("Free restore_font_name");
			g_free(win_data->restore_font_name);
			// g_debug("Clean win_data : %p", win_data);
			// g_debug("Free win_data");
			g_free(win_data);
			
			// g_debug("Clean active_window : %d", page_data->window);
			if (active_window==page_data->window)
				active_window = NULL;
			// g_debug("Destroy window: %d", page_data->window);
			gtk_widget_destroy(page_data->window);

			// g_debug("Set current_vtebox to NULL!");
			if (active_window != NULL)
			{
				// g_debug("Set current_vtebox = win_data->current_vtebox (%p)",
				// 	   win_data->current_vtebox);
				// active_window != NULL: We may trying to find the activing vtebox
				// It only happened run 'lilyterm -e xxx' in a lilyterm box
				struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(
										active_window), "Win_Data");
				current_vtebox = win_data->current_vtebox;
			}
			else
			{
				// g_debug("CAREFUL: current_vtebox = NULL!!");
				// No activing window: It only happened run 'lilyterm -e xxx' outside lilyterm.
				current_vtebox = NULL;
			}
		}
		else
		{
			// g_debug("It is the last page!\n");
			if (GTK_WIDGET_MAPPED(page_data->window))
			{
				if (single_process)
					shutdown_socket_server();
				gtk_main_quit();
			}
			else
				exit (1);
		}
	}
	else
	{
		// set the current page
		// g_debug ("Setting current page to %d!\n", page_data->page_no);
		if ( page_data->page_no==total_page-1)
			gtk_notebook_set_current_page(GTK_NOTEBOOK(page_data->notebook), page_data->page_no-1);
		else
			gtk_notebook_set_current_page(GTK_NOTEBOOK(page_data->notebook), page_data->page_no+1);

		// hide the tab bar if necessary
		if (total_page == 2)
		{
			// hide the page bar
			win_data->keep_vtebox_size |= 6;
			// we need to set the hints, or the window size may be incorrect.
			// g_debug("window_resizable in remove_page!");
			window_resizable(page_data->window, win_data->current_vtebox, 2, 1);
			gtk_notebook_set_show_tabs(GTK_NOTEBOOK(page_data->notebook), FALSE);
			gtk_notebook_set_show_border(GTK_NOTEBOOK(page_data->notebook), FALSE);
			win_data->update_hints = 1;
		}
		else
			// bind the hints information!
			// g_debug("Update the hints to %d", current_vtebox);
			window_resizable(page_data->window, win_data->current_vtebox, win_data->update_hints, 1);

		// Note that due to historical reasons,
		// GtkNotebook refuses to switch to a page unless the child widget is visible.
		// Therefore, it is recommended to show child widgets before adding them to a notebook.
		gtk_widget_hide_all(page_data->hbox);
		// remove current page
		// use page_data->page_no. DANGEROUS!
		// g_debug ("The %d page is going to be removed!\n", page_data->page_no);
		gtk_notebook_remove_page(GTK_NOTEBOOK(page_data->notebook), page_data->page_no);
	
		// g_debug("Reordering the page and update page name!");
		if (page_data->page_no < total_page-1)
			reorder_page_number(GTK_NOTEBOOK(page_data->notebook), NULL, 0, page_data->window);
	}

	// free the memory used by this page
	// g_debug("Free page_data!\n");
	// g_debug("Free custom_page_name");
	g_free(page_data->custom_page_name);
	// g_debug("Free pwd");
	g_free(page_data->pwd);
	// g_debug("Free font_name");
	g_free(page_data->font_name);
	// g_debug("Free locale");
	g_free(page_data->locale);
	// g_debug("Free environ");
	g_free(page_data->environ);
	// g_debug("Free page_data");
	g_free(page_data);

	return TRUE;
}

void vtebox_grab_focus(GtkWidget *vtebox, GtkWidget *window)
{
#ifdef DEBUG
	g_debug("! Launch vtebox_grab_focus() with window = %p, vte = %p", window, vtebox);
#endif

	// g_debug("vtebox grub focus !");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when vtebox grab focus!", win_data);

	// Don't update page name when win_data->kill_color_demo_vte.
	// Or LilyTerm will got warning: "Failed to set text from markup due to error parsing markup"
	if (current_vtebox != vtebox || (current_vtebox == NULL))
	{
		// first, un_bold prev page.
		if ((win_data->bold_current_page_name || win_data->bold_action_page_name) &&
		     (! win_data->kill_color_demo_vte))
		{
			if (current_vtebox != NULL)
			{
				struct Page *prev_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Page_Data");
				// win_data->current_vtebox == NULL: when the creation of sub process failt.
				// like 'lilyterm -e xxx' in a lilyterm
				if (prev_data!=NULL && win_data->current_vtebox != NULL)
				{
					prev_data->is_bold = FALSE;
					update_page_name (window, win_data->current_vtebox, prev_data->label,
							  prev_data->page_no+1, prev_data->custom_page_name,
							  prev_data->tab_color, prev_data->is_root, FALSE,
							  prev_data->encoding != win_data->default_encoding,
							  prev_data->encoding);
				}
			}
		}
		// g_debug ("Update current_vtebox! (%p), and update_hints = %d", vtebox, win_data->update_hints);
		current_vtebox = vtebox;
		win_data->current_vtebox = vtebox;

		// We will not update the page name when colse color demo vte
		if (win_data->kill_color_demo_vte) return;

		struct Page *page_data = NULL;
		if (win_data->window_shows_current_page ||
		    win_data->bold_current_page_name ||
		    win_data->bold_action_page_name)
			page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
		
		//if (keep_vtebox_size==0)
		//{
		//	// g_debug("window_resizable in vtebox_grab_focus!");
		//	// we should bind the hints information on current vtebox.
		//	// Or the geometry of vtebox may be changed when deleting the vtebox hold hints info.
		//	// It can help to hold the currect vtebox size.
		//	g_debug("Update hints!")
		//	window_resizable(vtebox, update_hints, 1);
		//}
		
		// update the window title
		if (win_data->window_shows_current_page)
			if (page_data!=NULL)
				update_window_title(window, page_data->label->name);

		// bold/un_bold current page
		if (win_data->bold_current_page_name || win_data->bold_action_page_name)
		{
			page_data->is_bold = win_data->bold_current_page_name;
			// g_debug("page_data->encoding = %p win_data->default_encoding = %p",
			//	page_data->encoding, win_data->default_encoding);
			update_page_name (window, vtebox, page_data->label, page_data->page_no+1,
					  page_data->custom_page_name, page_data->tab_color,
					  page_data->is_root, page_data->is_bold,
					  page_data->encoding != win_data->default_encoding, page_data->encoding);
		}
	}
}

gboolean vtebox_button_press(GtkWidget *vtebox, GdkEventButton *event, GtkWidget *window)
{
#ifdef DEBUG
	g_debug("! Launch vtebox_button_press for window %p", window);
#endif
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	if (! win_data->enable_function_key) return FALSE;

	if (event->button == 3)
	{
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
		// g_debug ("Get win_data = %d in show_menu", win_data);
		if (win_data->supported_locales!=NULL)
		{
			// We will not update coding here.
			win_data->query_encoding = TRUE;
			//if (page_data->encoding!=NULL)
			//	g_debug ("Update the encoding of page %d in menu to %s",
			//		 page_data->page_no, page_data->encoding->name);
			// g_debug("Set the menuitem %d t active", page_data->encoding);

			// menu is inited after page. so that the encoding of 1st page is 0.
			if (page_data->encoding==0)
				page_data->encoding = win_data->default_encoding;
			gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(page_data->encoding), TRUE);
			// query done
			win_data->query_encoding = FALSE;
		}

		if (win_data->show_transparent_menu)
		{
#ifdef ENABLE_RGBA
			if (win_data->use_rgba)
				GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_win)->active = win_data->transparent_window;
#endif
			GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_bg)->active = win_data->transparent_background;
		}
		if (win_data->scrollback_lines)
			GTK_CHECK_MENU_ITEM(win_data->menuitem_scrollback_lines)->active = page_data->use_scrollback_lines;
		gtk_menu_popup(GTK_MENU(win_data->menu), NULL, NULL, NULL, NULL, event->button, event->time);
		return TRUE;
	}
	else if (event->button == 1)
	{
		gint pad_x, pad_y, tag;
		gchar *url = NULL;
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");

		// return if hyperlink is disabled.
		if ( ! win_data->enable_hyperlink) return FALSE;
		
		vte_terminal_get_padding (VTE_TERMINAL(vtebox), &pad_x, &pad_y);
		url = vte_terminal_match_check( VTE_TERMINAL(vtebox),
						(event->x - pad_x/2) / vte_terminal_get_char_width(
											VTE_TERMINAL(vtebox)),
						(event->y - pad_y/2) / vte_terminal_get_char_height(
											VTE_TERMINAL(vtebox)),
						&tag);
		// g_debug("get url = %s", url);
		if (url != NULL)
		{
			win_data->temp_str = g_strdup_printf("%s %s", win_data->user_command[tag].command, url);
			// g_debug ("full_command = %s", win_data->temp_str);

			switch (win_data->user_command[tag].method)
			{
				case 0:
				{
					// 0: new tab
					gchar **argv = g_strsplit(win_data->temp_str, " ", -1);
					win_data->command_line = argv[0];
					win_data->parameters = argv;
					win_data->parameter = 2;
					// we don't need to free win_data->temp_str. it will be free in add_page.
					add_page(page_data->window, page_data->notebook, NULL, page_data->locale,
						 win_data->user_command[tag].environ, FALSE,
						 win_data->user_command[tag].VTE_CJK_WIDTH);
					break;
				}
				case 1:
				{
					// We don't use system here because we can't get the error report.
					// system(win_data->temp_str);
					pid_t pid = vfork();
					switch (pid)
					{
						case -1:
							dialog(NULL, 16);
							break;
						case 0:
						{
							gchar **argv = g_strsplit(win_data->temp_str, " ", -1);
							// Trying to set the VTE_CJK_WIDTH and eviron
							gchar **new_environ = apply_new_environ(win_data->environment,
											win_data->user_command[tag].environ);
							extern gchar **environ;
							gchar **environ_orig = environ;
							environ = new_environ;
							if (win_data->user_command[tag].VTE_CJK_WIDTH)
								set_VTE_CJK_WIDTH_environ(
										win_data->user_command[tag].VTE_CJK_WIDTH);
							//gint i=0;
							//while (environ[i])
							//	g_debug("%s", environ[i++]);
							if (execvp(argv[0], argv)<0)
								dialog(NULL, 16);
							environ = environ_orig;
							exit (0);
							break;
						}
					}
					break;
				}
				case 2:
				{
					// 1: new window
					gchar *command = g_strdup_printf("-e %s", win_data->temp_str);
					gchar **argv = g_strsplit(command, " ", -1);
					gchar *user_environ = g_strdup_printf("%s %s", page_data->locale,
									 win_data->user_command[tag].environ);
					new_window(3, argv, win_data->environment,
						   win_data->user_command[tag].VTE_CJK_WIDTH, user_environ);
					g_free(user_environ);
					g_strfreev(argv);
					g_free(command);
					break;
				}
			}
			g_free(win_data->temp_str);
			win_data->temp_str = NULL;
			g_free(url);
			return TRUE;
		}
	}
	return FALSE;
}

void set_VTE_CJK_WIDTH_environ(gint VTE_CJK_WIDTH)
{
	// Set the VTE_CJK_WIDTH environment
	switch (VTE_CJK_WIDTH)
	{
		case 0:
		case 1:
			unsetenv("VTE_CJK_WIDTH");
			break;
		//case 1:
		//	// VTE_CJK_WIDTH=narrow only work for vte >= 0.16.14
		//	setenv("VTE_CJK_WIDTH", "narrow", TRUE);
		//	break;
		case 2:
			setenv("VTE_CJK_WIDTH", "wide", TRUE);
			break;
	}
}
