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

#include "notebook.h"

// GtkWidget *current_vte=NULL;
extern GtkWidget *active_window;
extern GtkWidget *menu_active_window;
extern GtkClipboard *pclipboard;

gboolean menu_actived;

// prev_vte: the new vte will be the clone of prev_vte.
// menuitem_encoding: using right menu to open a new vtw with specified locale.
//		      Lilyterm will update page_data->encoding_str with it.
// locale: the environ LANG, LANGUAGE, and LC_ALL used in vte_terminal_fork_command().
// user_environ: it will be used in vte_terminal_fork_command(),
//		 and will be saved in page_data->environ.
//		 sould be separated with <tab>.
// run_once: some data that should be inited for the first vte of a LilyTerm window.
// encoding: the encoding come from environ. only for new window.
struct Page *add_page(struct Window *win_data,
		      struct Page *page_data_prev,
		      GtkWidget *menuitem_encoding,
		      gchar *encoding,
		      gchar *locale,
		      gchar *user_environ,
		      gboolean run_once,
		      gchar *VTE_CJK_WIDTH_STR)
{
#ifdef DETAIL
	if (menuitem_encoding)
		g_debug("! Launch add_page() with win_data = %p, page_data_prev = %p, "
			"menuitem_encoding->name = %s, encoding = %s,"
			"locale = %s, environ = %s, run_once = %d, VTE_CJK_WIDTH_STR = %s",
			win_data, page_data_prev, gtk_widget_get_name(menuitem_encoding), encoding,
			locale, user_environ, run_once, VTE_CJK_WIDTH_STR);
	else
		g_debug("! Launch add_page() with win_data = %p, page_data_prev = %p, "
			"encoding = %s,"
			"locale = %s, environ = %s, run_once = %d, VTE_CJK_WIDTH_STR = %s",
			win_data, page_data_prev, encoding, 
			locale, user_environ, run_once, VTE_CJK_WIDTH_STR);
#endif

// ---- Clone the page_data ---- //

	struct Page *page_data = g_new0(struct Page, 1);
	if (page_data_prev)
		page_data_dup(page_data_prev, page_data);
	else
		init_page_parameters(win_data, page_data);

// ---- deal the parameters and set the environs ----//

	GString *environ_str = g_string_new(win_data->environment);

	// set the encoding
	if (encoding || menuitem_encoding)
	{
		g_free(page_data->encoding_str);

		if (encoding)
			page_data->encoding_str = g_strdup(encoding);
		else
			page_data->encoding_str = get_encoding_from_menu_item_name(menuitem_encoding);
		// g_debug("page_data->encoding_str = %s", page_data->encoding_str);
	}

	// set the locale.
	// g_debug("locale = %s", locale);
	if (locale)
	{
		g_free(page_data->locale);
		page_data->locale = g_strdup(locale);
		g_string_append_printf (environ_str,
					"\tLANG=%s\tLANGUAGE=%s\tLC_ALL=%s",
					locale, locale, locale);
	}

	// set the environ that user specify in profile
	if (user_environ)
	{
		g_free(page_data->environ);
		page_data->environ = g_strdup(user_environ);
		if (environ_str->len)
			environ_str = g_string_append(environ_str, "\t");
		g_string_append_printf (environ_str, "%s", user_environ);
	}

	if (environ_str->len)
		environ_str = g_string_append(environ_str, "\t");
	g_string_append_printf (environ_str, "TERM=%s", win_data->emulate_term);

	// g_debug("final (in add_page) : environ_str = %s", environ_str->str);
	gchar **new_environs = g_strsplit(environ_str->str, "\t", -1);
	g_string_free(environ_str, TRUE);

// ---- Setting the VTE_CJK_WIDTH environment ---- //

	// g_debug("The VTE_CJK_WIDTH_STR got in add_page is %s", VTE_CJK_WIDTH_STR);
	if (VTE_CJK_WIDTH_STR && (VTE_CJK_WIDTH_STR[0] != '\0'))
	{
		// g_debug("VTE_CJK_WIDTH_STR = %s in add_page", VTE_CJK_WIDTH_STR);
		g_setenv("VTE_CJK_WIDTH", VTE_CJK_WIDTH_STR, TRUE);
		g_free(page_data->VTE_CJK_WIDTH_STR);
		page_data->VTE_CJK_WIDTH_STR = g_strdup(VTE_CJK_WIDTH_STR);
	}
	else
	{
		// g_debug("page_data->VTE_CJK_WIDTH_STR = %s", page_data->VTE_CJK_WIDTH_STR);
		if (page_data->VTE_CJK_WIDTH_STR && (page_data->VTE_CJK_WIDTH_STR[0] != '\0'))
			g_setenv("VTE_CJK_WIDTH", page_data->VTE_CJK_WIDTH_STR, TRUE);
		else
			g_unsetenv("VTE_CJK_WIDTH");
	}
	// g_debug("Get VTE_CJK_WIDTH = %s in add_page()", g_getenv("VTE_CJK_WIDTH"));

// ---- create vte ---- //

	page_data->vte = vte_terminal_new();
	//g_debug("The default encoding of vte is %s",
	//	vte_terminal_get_encoding(VTE_TERMINAL(page_data->vte)));

	// save the data first
	g_object_set_data(G_OBJECT(page_data->vte), "Page_Data", page_data);

	//g_debug("call set_encoding() by %p to %s", page_data->vte, page_data->encoding_str);
	vte_terminal_set_encoding(VTE_TERMINAL(page_data->vte), page_data->encoding_str);
	//g_debug("The encoding of new vte is %s",
	//	vte_terminal_get_encoding(VTE_TERMINAL(page_data->vte)));

// ---- Execute programs in the vte ---- //

	//if (command_line==NULL)
	//	command_line = g_getenv("SHELL");

	//if (win_data->command)
	//	g_debug("cmmand line = %s", win_data->command);
	//if (win_data->argv)
	//	g_debug("parameters = %s", *(win_data->argv));

	page_data->pid = vte_terminal_fork_command(VTE_TERMINAL(page_data->vte),
						   win_data->command, win_data->argv,
						   new_environs, page_data->pwd, TRUE, TRUE, TRUE);
	g_strfreev(new_environs);
	// treat '-e option' as `custom_page_name'
	if (win_data->argv != NULL && win_data->page_shows_current_cmdline)
		page_data->custom_page_name = get_cmdline(page_data->pid);
//	else
//	      page_data->custom_page_name = NULL;

// ---	Create a page and init the page data ---- //

	// create label
	// g_debug("Creating label!!");
	page_data->label = gtk_label_new(win_data->page_name);
	set_page_width(win_data, page_data);
	gtk_label_set_ellipsize(GTK_LABEL(page_data->label), PANGO_ELLIPSIZE_MIDDLE);
	g_object_set_data(G_OBJECT(page_data->label), "VteBox", page_data->vte);
	// when draging the tab on a vte, or draging a vte to itself, may change the size of vte.
        g_signal_connect(G_OBJECT(page_data->label), "size_request",
	                         G_CALLBACK(label_size_request), page_data);

	// create a hbox
	page_data->hbox = gtk_hbox_new(FALSE, 0);

	// Get current vte size. for init a new tab.
	guint column, row;
	if (page_data_prev)
	{
		column = vte_terminal_get_column_count(VTE_TERMINAL(page_data_prev->vte));
		row = vte_terminal_get_row_count(VTE_TERMINAL(page_data_prev->vte));
		// g_debug("got prev_vte = %d x %d", column, row);

	}
	else
	{
		column = win_data->default_column;
		row = win_data->default_row;
	}

	// g_debug("Init New vte with %d x %d!", column, row);
	// g_debug("Using the font : %s\n", page_data->font_name);

// ---- Init the new page ---- //

	// Init new page. run_once: some settings only need run once.
	// run_once only = TRUE when init LilyTerm in main().
	init_new_page(win_data, page_data, column, row, run_once);

	page_data->scrollbar = gtk_vscrollbar_new(vte_terminal_get_adjustment(VTE_TERMINAL(page_data->vte)));
	pack_vte_and_scrollbar_to_hbox(win_data, page_data);
	
	// the close page event
	g_signal_connect(G_OBJECT(page_data->vte), "child_exited", G_CALLBACK(close_page), FALSE);

	// when get focus, update `current_vte', hints, and window title
	g_signal_connect(G_OBJECT(page_data->vte), "grab-focus", G_CALLBACK(vte_grab_focus), NULL);

	// show the menu
	g_signal_connect(G_OBJECT(page_data->vte), "button-press-event",
			 G_CALLBACK(vte_button_press), NULL);
	add_remove_window_title_changed_signal(page_data);

//	GdkColor root_color;
//	gdk_color_parse("#FFFAFE", &root_color);
//	gtk_widget_modify_bg(notebook, GTK_STATE_NORMAL, &root_color);

// ---- Init other datas of the page ---- //

	// set the tab name.
	page_data->new_tpgid = page_data->pid;
	page_data->displayed_tpgid = 0;
	// page_data->tab_color = win_data->user_page_color[5];
	page_data->is_root = check_is_root(page_data->pid);
	page_data->is_bold = win_data->bold_current_page_name;

// ---- add the new page to notebook ---- //

	// Note that due to historical reasons,
	// GtkNotebook refuses to switch to a page unless the child widget is visible.
	// Therefore, it is recommended to show child widgets before adding them to a notebook.
	gtk_widget_show_all(page_data->hbox);

	// if (! win_data->show_scrollbar)
	//	hide_scrollbar(win_data, page_data);

	// FIXME: dirty hack
	// if (page_data->window==win_data->window), notebook_page_added() won't work.
	page_data->window = NULL;
	// g_debug("page_data->vte = %p", page_data->vte);
	// g_debug("win_data->current_vte = %p", win_data->current_vte);
	page_data->page_no = gtk_notebook_append_page(GTK_NOTEBOOK(page_data->notebook),
						      page_data->hbox, page_data->label);
	// g_debug("The new page no is %d", page_data->page_no);
	// move the page to next to original page
	if (page_data_prev)
	{
		gtk_notebook_reorder_child(GTK_NOTEBOOK(page_data->notebook), page_data->hbox,
        	                                        page_data_prev->page_no + 1);
		// g_debug("New Page No after move to next to prev page = %d", page_data->page_no);
	}
	win_data->current_vte = page_data->vte;

// ---- Monitor cmdline ---- //

	// g_debug("!!! Launch get_and_update_page_name() in add_page()!");
	// if (! page_data->page_shows_window_title)
	//if (! get_and_update_page_name(page_data))
	//{
	//	g_debug("Update Page Name Failt!");
	//}

	if (win_data->page_shows_current_cmdline || win_data->page_shows_current_dir || win_data->page_shows_window_title)
	{
		add_remove_page_timeout_id(win_data, page_data);
		// g_debug("Timeout Added: %d (%d)", page_data->timeout_id, page_data->vte);
	}
	else
	{
		// g_debug("Launch get_and_update_page_name() in add_page()!");
		win_data->page_names_no++;
	}
	// g_debug("Got label name from get_and_update_page_name(): %s\n", page_data->page_name);

// ---- Restore the VTE_CJK_WIDTH environment ---- //

	restore_SYSTEM_VTE_CJK_WIDTH_STR();
	// g_debug("Restore VTE_CJK_WIDTH = %s in add_page()", g_getenv("VTE_CJK_WIDTH"));

// ---- Finish ---- //

	// finish!
	// gtk_window_set_focus(GTK_WINDOW(window), page_data->vte);

	// g_debug("The final encoding of vte is %s" ,vte_terminal_get_encoding(VTE_TERMINAL(page_data->vte)));

	// page_data->pid < 0: Error occurred when creating sub process.
	// FIXME: how about "current_vte != page_data->vte"??
	if (page_data->pid < 0)
	{
		// g_debug("Error occurred when creating sub process");
		GString *arg_str = g_string_new(NULL);
		// Trying to avoid double_free()
		gint i;
		for (i=0; i<win_data->argc; i++)
			g_string_append_printf(arg_str, "%s ", win_data->argv[i]);
		// g_debug("page_data->encoding_str = %s", page_data->encoding_str);
		error_dialog(win_data, arg_str->str, page_data->encoding_str, 17);
		close_page (page_data->vte, FALSE);
#ifdef DETAIL
		g_debug("* free arg_str %p (%s) addpage()", arg_str, arg_str->str);
#endif
		g_string_free(arg_str, TRUE);
		return NULL;
	}
	else
	{
		// We MUST clear command_line and parameters after ran -e option.
		win_data->command = NULL;
		win_data->argc = 0;
		win_data->argv = NULL;
		// FIXME: Calling notebook_page_added() here
		// page_data->window=NULL;
		// notebook_page_added(GTK_NOTEBOOK(page_data->notebook), NULL, page_data->page_no, win_data);
		return page_data;
	}
}

void label_size_request (GtkWidget *label, GtkRequisition *requisition, struct Page *page_data)
{
#ifdef DETAIL
        g_debug("! Launch label_size_request() with page_data = %p", page_data);
#endif
        struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
        keep_window_size (win_data, page_data->vte, 0x3);
}

gboolean close_page(GtkWidget *vte, gboolean need_safe_close)
{
#ifdef DETAIL
	g_debug("! Launch close_page() with vte = %p, and need_safe_close = %d", vte, need_safe_close);
#endif

	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	// g_debug("Get page_data = %p when closing page!", page_data);

	// g_debug("Deleting Page ID: %d\n", page_data->pid);
	if (page_data->pid<1)
		need_safe_close = FALSE;

	// only "child_exited" don't need need_safe_close to kill SHELL
	if (need_safe_close)
		if (page_data->pid != get_tpgid(page_data->pid))
			if (dialog(NULL, 7)==FALSE)
				return FALSE;

	// remove timeout event for page_shows_current_cmdline
	if (page_data->page_shows_current_cmdline || page_data->page_shows_current_dir || page_data->page_shows_window_title)
	//	monitor_cmdline_cancel(page_data->monitor);
		g_source_remove (page_data->timeout_id);

	// kill running shell
	if (need_safe_close)
	{
		// FIXME: Trying to not use kill()?
		// if the tab is not close by <Ctrl><D>, we need to launch kill()
		// g_debug("Trying to kill %d!\n", page_data->pid);
		kill(page_data->pid, SIGKILL);
	}

	// set the current page
	// g_debug ("Setting current page to %d!\n", page_num);
	if (page_data->page_no==(gtk_notebook_get_n_pages(GTK_NOTEBOOK(page_data->notebook))-1))
		gtk_notebook_set_current_page(GTK_NOTEBOOK(page_data->notebook), page_data->page_no-1);
	else
		gtk_notebook_set_current_page(GTK_NOTEBOOK(page_data->notebook), page_data->page_no+1);

#ifdef DETAIL
	g_debug("* Free page_data->page_name %p (%s)", page_data->page_name, page_data->page_name);
#endif
	g_free(page_data->page_name);

	// Note that due to historical reasons,
	// GtkNotebook refuses to switch to a page unless the child widget is visible.
	// Therefore, it is recommended to show child widgets before adding them to a notebook.
	gtk_widget_hide_all(page_data->hbox);

	// gtk_widget_destroy(page_data->vte);
	// remove current page
	// use page_data->page_no. DANGEROUS!
	// g_debug ("The %d page is going to be removed!\n", page_data->page_no);
	gtk_notebook_remove_page(GTK_NOTEBOOK(page_data->notebook), page_data->page_no);

	// free the memory used by this page
#ifdef DETAIL
	g_debug("* Starting to Free page_data!\n");
#endif
#ifdef DETAIL
	g_debug("* Free custom_page_name %p (%s)", page_data->custom_page_name, page_data->custom_page_name);
#endif
	g_free(page_data->custom_page_name);
#ifdef DETAIL
	g_debug("* Free pwd %p (%s)", page_data->pwd, page_data->pwd);
#endif
	g_free(page_data->pwd);
#ifdef DETAIL
	g_debug("* Free font_name %p (%s)", page_data->font_name, page_data->font_name);
#endif
	g_free(page_data->font_name);

#ifdef DETAIL
	g_debug("* Free encoding %p (%s)", page_data->encoding_str, page_data->encoding_str);
#endif
	g_free(page_data->encoding_str);
#ifdef DETAIL
	g_debug("* Free locale %p (%s)", page_data->locale, page_data->locale);
#endif
	g_free(page_data->locale);
#ifdef DETAIL
	g_debug("* Free VTE_CJK_WIDTH_STR %p (%s)", page_data->VTE_CJK_WIDTH_STR, page_data->VTE_CJK_WIDTH_STR);
#endif
	g_free(page_data->VTE_CJK_WIDTH_STR);
#ifdef DETAIL
	g_debug("* Free window_title_path %p (%s)", page_data->window_title_path, page_data->window_title_path);
#endif
	g_free(page_data->window_title_pwd);
#ifdef DETAIL
	g_debug("* Free environ %p (%s)", page_data->environ, page_data->environ);
#endif
	g_free(page_data->environ);

	// FIXME: Calling remove_notebook_page() here
	// GtkNotebook may drag a page to itself,
	// So a GtkNotebook may be page_removed() -> page_added() with the same page
	// but the GtkNotebook may be destoried after page_removed()
	// So that we should call page_removed() here, not using "page-removed" signal... -_-|||
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window),
									     "Win_Data");
	remove_notebook_page (GTK_NOTEBOOK(page_data->notebook), NULL, page_data->page_no, win_data);

//#ifdef DETAIL
//	g_debug("* Free encoding %p", page_data->environments);
//#endif
//	g_strfreev(page_data->environments);

#ifdef DETAIL
	g_debug("* Free page_data %p", page_data);
#endif
	g_free(page_data);

	return TRUE;
}

void vte_grab_focus(GtkWidget *vte, gpointer user_data)
{
#ifdef DETAIL
	g_debug("! Launch vte_grab_focus() with vte = %p", vte);
#endif
	// g_debug("vte = %p grub focus !", vte);
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %p when vte grab focus!", win_data);

	//if (win_data->lost_focus)
	//	return;

	if (page_data->vte_is_inactived)
	{
		vte_terminal_set_colors(VTE_TERMINAL(win_data->current_vte),
					&(win_data->fg_color),
					&(win_data->bg_color),
					win_data->color,
					16);
		vte_terminal_set_color_bold (VTE_TERMINAL(page_data->vte), &(win_data->fg_color));
		page_data->vte_is_inactived = FALSE;
	}


	// Don't update page name when win_data->kill_color_demo_vte.
	// Or LilyTerm will got warning: "Failed to set text from markup due to error parsing markup"
	if ((win_data->current_vte != vte || (win_data->current_vte == NULL)) && (! win_data->kill_color_demo_vte))
	{
		// g_debug("first, un_bold prev page.");
		if ((win_data->bold_current_page_name || win_data->bold_action_page_name) &&
		    (win_data->current_vte != NULL))
		{
			struct Page *prev_data = (struct Page *)g_object_get_data(
							G_OBJECT(win_data->current_vte), "Page_Data");
			// win_data->current_vte == NULL: when the creation of sub process failt.
			// like 'lilyterm -e xxx' in a lilyterm
			if (prev_data!=NULL && win_data->current_vte != NULL)
			{
				prev_data->is_bold = FALSE;
				// g_debug ("Launch update_page_name() to un_bold prev page!!!");
				update_page_name (page_data->window, win_data->current_vte, prev_data->page_name,
						  prev_data->label, prev_data->page_no+1, prev_data->custom_page_name,
						  prev_data->tab_color, prev_data->is_root, FALSE,
						  compare_win_page_encoding(win_data->default_encoding,
						  			    prev_data->encoding_str),
						  prev_data->encoding_str, page_data->custom_window_title,
						  FALSE);
			}
		}
		// g_debug ("Update current_vte! (%p), and update_hints = %d", vte, win_data->update_hints);
		// current_vte = vte;
		win_data->current_vte = vte;

		// We will not update the page name when colse color demo vte
		// if (win_data->dialog_actived) return;

		struct Page *page_data = NULL;
		if (win_data->window_shows_current_page ||
		    win_data->bold_current_page_name ||
		    win_data->bold_action_page_name)
			page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
		
		//if (keep_vte_size==0)
		//{
		//	// g_debug("window_resizable in vte_grab_focus!");
		//	// we should bind the hints information on current vte.
		//	// Or the geometry of vte may be changed when deleting the vte hold hints info.
		//	// It can help to hold the currect vte size.
		//	g_debug("Update hints!")
		//	window_resizable(vte, update_hints, 1);
		//}
		
		// update the window title
		if (win_data->window_shows_current_page && (! page_data->custom_window_title))
			if (page_data!=NULL)
				update_window_title(page_data->window, page_data->page_name);

		// bold/un_bold current page
		// g_debug("then, bold/un_bold current page.");
		if (win_data->bold_current_page_name || win_data->bold_action_page_name)
		{
			page_data->is_bold = win_data->bold_current_page_name;
			// g_debug("page_data->encoding = %p win_data->default_encoding = %p",
			//	page_data->encoding, win_data->default_encoding);
			// if (win_data->default_encoding)
			//	g_debug("win_data->default_encoding->name = %s, "
			//		"page_data->encoding_str = %s",
			//		gtk_widget_get_name(win_data->default_encoding), page_data->encoding_str);
			update_page_name (page_data->window, vte, page_data->page_name, page_data->label,
					  page_data->page_no+1, page_data->custom_page_name, page_data->tab_color,
					  page_data->is_root, page_data->is_bold,
					  compare_win_page_encoding(win_data->default_encoding,
					  			    page_data->encoding_str),
					  page_data->encoding_str, page_data->custom_window_title,
					  FALSE);
		}
	}
}

gboolean vte_button_press(GtkWidget *vte, GdkEventButton *event, gpointer user_data)
{
#ifdef DETAIL
	g_debug("! Launch vte_button_press for vte %p", vte);
#endif
	// We may click mouse button on a lost focus window to popup it's Menu
	// So that we should find the active_window via page_data
	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
	menu_active_window = page_data->window;
	// g_debug("set menu_active_window = %p", menu_active_window);
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	if (! win_data->enable_function_key) return FALSE;

	if (event->button == 3)
	{
		menu_actived = TRUE;
		struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data");
		// g_debug ("Get win_data = %d in show_menu", win_data);
		if (win_data->supported_locales!=NULL)
		{
			// We will not update coding here.
			win_data->query_encoding = TRUE;
			//if (page_data->encoding!=NULL)
			//	g_debug ("Update the encoding of page %d in menu to %s",
			//		 page_data->page_no, gtk_widget_get_name(page_data->encoding));
			// g_debug("Set the menuitem %d t active", page_data->encoding);

			// menu is inited after add_page(). so that the encoding_str of 1st page is NULL.
			if (page_data->encoding_str==NULL)
				page_data->encoding_str = get_encoding_from_menu_item_name(
										win_data->default_encoding);

			// gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(page_data->encoding), TRUE);
			// Trying to find the right encoding menuitem...
			GSList *menuitem_list = gtk_radio_menu_item_get_group(
							GTK_RADIO_MENU_ITEM(win_data->default_encoding));
			while (menuitem_list)
			{
				// g_debug("Checking %s...", gtk_widget_get_name(GTK_WIDGET(menuitem_list->data)));
				if ( ! compare_win_page_encoding(menuitem_list->data,
								 page_data->encoding_str))
				{
					gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(
										menuitem_list->data), TRUE);
					break;
				}
				// g_debug("menuitem_list->next = %p", menuitem_list->next);
				if (menuitem_list->next == NULL)
				{
					GtkWidget *menuitem = add_menuitem_to_encoding_sub_menu(
								win_data, -1, page_data->encoding_str);
					gtk_widget_show (menuitem);
					gtk_check_menu_item_set_active (GTK_CHECK_MENU_ITEM(menuitem),
									TRUE);
				}
				menuitem_list = menuitem_list->next;
			}
			// query done
			win_data->query_encoding = FALSE;
		}

		if (win_data->show_transparent_menu)
		{
#ifdef ENABLE_RGBA
			if (win_data->use_rgba)
				GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_win)->active = win_data->transparent_window;
#endif
			// The following codes is only working with vte > 0.19
			// gint transparent;
			// g_object_get (vte,
			//	      "background-transparent", &transparent,
			//	      NULL);
			// GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_bg)->active = transparent;
			GTK_CHECK_MENU_ITEM(win_data->menuitem_trans_bg)->active =
								win_data->transparent_background;
		}
		
		if (win_data->show_copy_paste_menu)
		{
			// enable win_data->menuitem_copy_url or not
			if ( win_data->enable_hyperlink)
			{
				gint tag;
				// g_free(win_data->menuitem_copy_url->name);
				gchar *url = get_url(event, page_data, &tag);
				// g_debug("url = %s", url);
				gtk_widget_set_name (win_data->menuitem_copy_url, url);
				// g_debug("name of win_data->menuitem_copy_url is %s",
				//	gtk_widget_get_name(win_data->menuitem_copy_url));
				gtk_widget_set_sensitive (win_data->menuitem_copy_url, url!=NULL);
				g_free(url);
			}

			// enable win_data->menuitem_copy or not
			gtk_widget_set_sensitive (win_data->menuitem_copy,
						  vte_terminal_get_has_selection (VTE_TERMINAL(vte)));

			// enable win_data->menuitem_paste or not
			gchar *temp_str = gtk_clipboard_wait_for_text (pclipboard);
			// g_debug("clipboard = %s", temp_str);
			gtk_widget_set_sensitive (win_data->menuitem_paste,
						  temp_str != NULL);
			g_free(temp_str);
		}


		// if (page_data->use_scrollback_lines)
		// GTK_CHECK_MENU_ITEM(win_data->menuitem_scrollback_lines)->active = page_data->use_scrollback_lines;
		GTK_CHECK_MENU_ITEM(win_data->menuitem_cursor_blinks)->active = win_data->cursor_blinks;
		GTK_CHECK_MENU_ITEM(win_data->menuitem_audible_bell)->active = win_data->audible_bell;

		if (gtk_notebook_get_show_tabs GTK_NOTEBOOK(win_data->notebook))
		{
			gtk_widget_hide (win_data->menuitem_show_tabs_bar);
			gtk_widget_show (win_data->menuitem_hide_tabs_bar);
		}
		else
		{
			gtk_widget_show (win_data->menuitem_show_tabs_bar);
			gtk_widget_hide (win_data->menuitem_hide_tabs_bar);
		}

		gtk_menu_popup(GTK_MENU(win_data->menu), NULL, NULL, NULL, NULL, event->button, event->time);
		return TRUE;
	}
	else if (event->button == 1)
	{
		// return if hyperlink is disabled.
		if ( ! win_data->enable_hyperlink) return FALSE;
		
		gint tag;
		gchar *url = get_url(event, page_data, &tag);
		// g_debug("get url = %s", url);

		// url = NULL -> we don't need to free it.
		if (url != NULL)
		{
			gchar *full_command = g_strdup_printf("%s\t%s", win_data->user_command[tag].command, url);
			// g_debug ("full_command = %s", full_command);

			gchar **argv = g_strsplit(full_command, "\t", -1);
			gint argc = 0;
			while (argv[argc])
				argc++;

			switch (win_data->user_command[tag].method)
			{
				case 0:
				{
					// 0: new tab
					win_data->command = argv[0];
					win_data->argv = argv;
					win_data->argc = argc;
					// g_debug("win_data->user_command[tag].environ, = %s",
					//	 win_data->user_command[tag].environ);
					add_page(win_data,
						 page_data,
						 NULL,
						 NULL,
						 NULL,
						 win_data->user_command[tag].environ,
						 FALSE,
						 get_VTE_CJK_WIDTH_str(win_data->user_command[tag].VTE_CJK_WIDTH));
					break;
				}
				case 1:
				{
					// FIXME: use vfork()/execvp() may cause zombie process.
					// But we can't get if the child process is succeed or not
					// if using system() here.
					// system(full_command);
					
					set_VTE_CJK_WIDTH_environ(win_data->user_command[tag].VTE_CJK_WIDTH);
					GString *environ_str = g_string_new(win_data->environment);
					if (win_data->user_command[tag].environ &&
					    win_data->user_command[tag].environ[0] != '\0')
						g_string_append_printf (environ_str,
									"\t%s",
									win_data->user_command[tag].environ);
					gchar **new_environs = g_strsplit(environ_str->str, "\t", -1);
					if (gdk_spawn_on_screen_with_pipes(
							gdk_screen_get_default(),
							NULL,
							argv,
							new_environs,
							G_SPAWN_SEARCH_PATH,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL,
							NULL) == FALSE)
					{
						gint i=0;
						while (full_command[i])
						{
							if (full_command[i]=='\t')
								full_command[i]=' ';
							i++;
						}
						error_dialog(win_data, full_command, NULL, 17);
					}
					restore_SYSTEM_VTE_CJK_WIDTH_STR();
					g_string_free(environ_str, TRUE);
					g_strfreev(new_environs);
					break;
				}
				case 2:
					// FIXME: (gchar *) get_encoding_from_locale() don't work if zh_TW in win_data->user_command[tag].environ
					new_window(argc,
						   argv,
						   win_data->environment,
						   get_current_pwd_by_pid(page_data->pid),
						   get_VTE_CJK_WIDTH_str(win_data->user_command[tag].VTE_CJK_WIDTH),
						   win_data->user_command[tag].environ,
						   (gchar *) get_encoding_from_locale(),
						   NULL,
						   NULL);
					break;
			}
#ifdef DETAIL
			g_debug("* free argv %p in vte_button_press()", argv);
#endif
			g_strfreev(argv);

#ifdef DETAIL
			g_debug("* free full_command %p (%s) in vte_button_press()", full_command, full_command);
#endif
			g_free(full_command);
#ifdef DETAIL
			g_debug("* free url %p (%s) in vte_button_press()", url, url);
#endif
			g_free(url);
			return TRUE;
		}
	}
	return FALSE;
}

gboolean compare_win_page_encoding(GtkWidget *encoding, gchar *encoding_str)
{
#ifdef DETAIL
	g_debug("! Launch compare_win_page_encoding with encoding = %p, encoding_str = %s",
		 encoding, encoding_str);
#endif
	gboolean return_value;

	// FIXME: Should we return TRUE or FALSE here?
	if (encoding==NULL || encoding_str==NULL)
		return FALSE;
	
	gchar *menuitem_encoding = get_encoding_from_menu_item_name(encoding);
	return_value = g_ascii_strcasecmp(menuitem_encoding, encoding_str);
	// g_debug("encoding->name = %s, encoding_str = %s, (%d)", gtk_widget_get_name (encoding), encoding_str, return_value);
#ifdef DETAIL
	g_debug("* free menuitem_encoding %p (%s) in compare_win_page_encoding()", menuitem_encoding, menuitem_encoding);
#endif
	g_free(menuitem_encoding);
	return return_value;
}

// The returned string should be freed when no longer needed.
gchar *get_encoding_from_menu_item_name(GtkWidget *menuitem)
{
	if (menuitem==NULL)
		return NULL;
	
//	gchar **locales = g_strsplit( gtk_widget_get_name (menuitem), ".", 0);

	gchar **locales;
	if (strcmp(gtk_widget_get_name (menuitem), "ANSI_X3.4-1968"))
		// zh_TW.Big5 -> [0]zh_TW, [1]Big5
		locales = g_strsplit(gtk_widget_get_name (menuitem), ".", 0);
	else
		// the encoding of "C" is "ANSI_X3.4-1968"
		locales = g_strsplit(gtk_widget_get_name (menuitem), "+", 0);

	gchar *encoding;
	if (locales[1])
		encoding = g_strdup(locales[1]);
	else
		encoding = g_strdup(gtk_widget_get_name (menuitem));
	
#ifdef DETAIL
	g_debug("* free locales %p in get_encoding_from_menu_item_name()", locales);
#endif
	g_strfreev(locales);

	return encoding;
}

// The returned string should be freed when no longer needed.
gchar *get_url (GdkEventButton *event, struct Page *page_data, gint *tag)
{
	gint pad_x, pad_y;

	vte_terminal_get_padding (VTE_TERMINAL(page_data->vte), &pad_x, &pad_y);
	return vte_terminal_match_check(VTE_TERMINAL(page_data->vte),
					(event->x - pad_x/2) /
						vte_terminal_get_char_width(
							VTE_TERMINAL(page_data->vte)),
					(event->y - pad_y/2) /
						vte_terminal_get_char_height(
							VTE_TERMINAL(page_data->vte)),
					tag);
}
