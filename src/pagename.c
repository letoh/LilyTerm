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

#include "pagename.h"
extern gboolean dialog_actived;

void reorder_page_number(GtkNotebook *notebook, GtkWidget *child, guint page_num, GtkWidget *window)
{
#ifdef DETAIL
	g_debug("! Launch reorder_page_number() for window %p", window);
#endif
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	// g_debug("total_page = %d", total_page);
	struct Page *page_data;
	gint i;
	
	for (i=0;i<total_page;i++)
	{
		page_data = get_page_data_from_nth_page (win_data, i);
		// g_debug("Got page_data = %p", page_data);
		// we store current_page_no in "struct Page" for performance.
		page_data->page_no = i;

		// g_debug("Launch get_and_update_page_name() in reorder_page_number()");

		// g_debug("page_data->label = %p "
		//	"page_data->page_name = %s "
		//	"page_data->custom_page_name = %s "
		//	"page_data->tab_color = %s "
		//	"win_data->default_encoding = %p "
		//	"page_data->encoding_str = %s", 
		//	page_data->label,
		//	page_data->page_name,
		//	page_data->custom_page_name,
		//	page_data->tab_color,
		//	win_data->default_encoding,
		//	page_data->encoding_str);

		// Don't update page name when win_data->kill_color_demo_vte.
		// Or LilyTerm will got warning: "Failed to set text from markup due to error parsing markup"
		if (((win_data->page_shows_number) || win_data->adding_page) &&
		    (! dialog_actived) &&
		    (! win_data->kill_color_demo_vte))
			update_page_name(window,
					 page_data->vte,
					 page_data->page_name,
					 page_data->label,
					 i+1,
					 page_data->custom_page_name,
					 page_data->tab_color,
					 page_data->is_root,
					 page_data->is_bold,
					 compare_win_page_encoding(win_data->default_encoding,
			 					   page_data->encoding_str),
					 page_data->encoding_str,
					 page_data->custom_window_title,
					 FALSE);
	}
}

gboolean monitor_cmdline(struct Page *page_data)
{
#ifdef FULL
	g_debug("! Launch monitor_cmdline() for vte %p", page_data->vte);
#endif
	gboolean lost_focus = *(page_data->lost_focus);

	// g_debug("Get page_data->keep_vte_size = %d in monitor_cmdline()", *(page_data->keep_vte_size));

	// g_debug ("\nvte_terminal_get_status_line = %s,\n"
	//	"vte_terminal_get_icon_title = %s,\n"
	//	"vte_terminal_get_window_title = %s\n",
	//	vte_terminal_get_status_line(VTE_TERMINAL(page_data->vte)),
	//	vte_terminal_get_icon_title(VTE_TERMINAL(page_data->vte)),
	//	vte_terminal_get_window_title(VTE_TERMINAL(page_data->vte)));

	// The pagename won't be updated if LilyTerm is not on focus or when resizing.
	// But it will still update the window title.
	// 0xfe = 11,111,110
	if ((*(page_data->keep_vte_size)&0xfffc) || 
	    (lost_focus && (*(page_data->current_vte) != (page_data->vte))) ||
	    page_data->custom_page_name ||
	    dialog_actived) 
		return TRUE;

	if (page_data->pid<1) return FALSE;

	// if we should call get_and_update_page_name or not
	gboolean page_name_changed = FALSE;
	gint page_update_method = page_data->page_update_method;
	page_data->page_update_method = 0;
	// g_debug("INIT: lost_focus = %d", lost_focus);
	// g_debug("INIT: page_data->window_title_updated = %d", page_data->window_title_updated);

	if (lost_focus)
		page_name_changed = check_cmdline (page_data, *(page_data->window_title_tpgid));
	else
	{
		page_name_changed = check_cmdline (page_data, page_data->displayed_tpgid);
		if ((page_name_changed)	&& page_data->check_root_privileges)
			page_data->is_root = check_is_root(page_data->new_tpgid);
	}

	// g_debug("lost_focus = %d", lost_focus);
	// g_debug("page_data->window_title_updated = %d", page_data->window_title_updated);
	
	if (check_window_title(page_data, lost_focus))
		page_name_changed = TRUE;

	gchar *old_pwd;
	gchar *new_pwd;
	
	new_pwd = get_tab_name_with_current_dir(page_data->new_tpgid);
	if (lost_focus)
		old_pwd = page_data->window_title_pwd;
	else
		old_pwd = page_data->pwd;

	if (check_pwd(page_data, old_pwd, new_pwd, page_update_method))
	{
		page_name_changed = TRUE;
		if (lost_focus)
			page_data->window_title_pwd = new_pwd;
		else
			page_data->pwd = new_pwd;
		new_pwd = old_pwd;
	}

	// g_debug("check_tpgid = %d, and new_tpgid = %d", *check_tpgid, page_data->new_tpgid);
	// only update the page name when tpgid is updated.
	// g_debug("Final: page_data->page_update_method = %d", page_data->page_update_method);
	if (page_name_changed)
	{
		// g_debug("!!! Update the page name for page_no %d", page_data->page_no);
		if (page_data->vte != *(page_data->current_vte))
			page_data->is_bold = page_data->bold_action_page_name;
		
		// g_debug("page_data->is_bold = %d for page_data %p", page_data->is_bold, page_data);

		// g_debug("Launch get_and_update_page_name() in monitor_cmdline()");
		if (( ! get_and_update_page_name(page_data, *(page_data->lost_focus))) && (! lost_focus))
		{
			// if update "tab title" fault. upate window title will never fault.
			// g_debug("!! Tab name is NOT changed to '%s'", page_data->pwd);
			// exchange old_pwd and new_pwd again!
			new_pwd = page_data->pwd;
			page_data->pwd = old_pwd;
			// g_debug("Restore page_data->pwd to %s", page_data->pwd);
		}
	}
	else
	{
		page_data->page_update_method = page_update_method;
		// g_debug("Restore page_data->page_update_method to %d!!!", page_data->page_update_method);
	}

	g_free(new_pwd);
	
	return TRUE;
}

gboolean check_cmdline (struct Page *page_data, pid_t check_tpgid)
{
	// g_debug("check_tpgid = %d", check_tpgid);

	gboolean page_name_changed = FALSE;
	if (page_data->page_shows_current_cmdline || page_data->page_shows_current_dir)
	{

		// find the new_tpgid
		page_data->new_tpgid = get_tpgid(page_data->pid);
		// g_debug("Get page_data->new_tpgid = %d, check_tpgid = %d",
		//      page_data->new_tpgid, check_tpgid);

		if (check_tpgid != page_data->new_tpgid)
		{
			// g_debug("Trying to update Cmdline!!!");
			page_name_changed = TRUE;
			page_data->page_update_method = 2;
		}
		// else if (page_data->new_tpgid==page_data->pid)
		else if (page_data->window_title_updated == 0)
			page_data->window_title_updated = -1;

		// g_debug("page_data->window_title_updated = %d", page_data->window_title_updated);
		// g_debug("page_data->page_update_method = %d", page_data->page_update_method);
	}
	// g_debug("the original tpgid is %d, and got tpgid from get_tpgid() is: %d\n",
	//      page_data->displayed_tpgid, page_data->new_tpgid);
	return page_name_changed;
}

gboolean check_window_title (struct Page *page_data, gboolean lost_focus)
{
	gboolean page_name_changed = FALSE;

	if ((page_data->page_shows_window_title) && (page_data->window_title_updated >= 1))
	{
		// don't update window title when lost focus
		// g_debug("lost_focus = %d", lost_focus);
		// g_debug("page_data->window_title_updated = %d", page_data->window_title_updated);
		if (! (lost_focus && (page_data->window_title_updated == 2)))
		{
			// g_debug("Trying to update Window title!!!");
			page_name_changed = TRUE;
			page_data->page_update_method = 1;
			page_data->window_title_updated = 1;
		}
	}
	return page_name_changed;
}

gboolean check_pwd(struct Page *page_data, gchar *pwd, gchar *new_pwd, gint page_update_method)
{
	// g_debug("pwd = %s", pwd);
	// g_debug("new_pwd = %s", new_pwd);	

	gboolean page_name_changed = FALSE;

	// update the page name with PWD when pid=tpgid
	// g_debug ("page_name_changed = %d, ", page_name_changed);
	if (page_data->page_shows_current_dir &&
	    (page_data->new_tpgid == page_data->pid) &&
	    (page_data->window_title_updated == -1))
	{
		if (pwd==NULL)
			page_name_changed = TRUE;
		else
		{
			// Aviod segfalt when strcmp(pwd, NULL).
			if (new_pwd==NULL)
				page_name_changed = FALSE;
			else if (strcmp(pwd, new_pwd))
				page_name_changed = TRUE;
		}

		if ((*(page_data->window_title_tpgid) != page_data->displayed_tpgid) &&
		    (page_update_method == 2))
			page_name_changed = TRUE;

		if (page_name_changed)
		{
			// g_debug("Trying to update PWD!!!");
			page_data->page_update_method = 3;
			page_name_changed = TRUE;
		}
		// g_debug("page_data->window_title_pwd = %s\npage_name_changed = %d",
		//      page_data->window_title_pwd, page_name_changed);
	}
	return page_name_changed;
}


// it will update the text in label ,page_name, and the title of window
// return FALSE if update the label (and page_name) fault.
// updating the title of window will never fault.
gboolean get_and_update_page_name(struct Page *page_data, gboolean lost_focus)
{
#ifdef DETAIL
	g_debug("! Launch get_and_update_page_name() for vte %p", page_data->vte);
#endif

	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when update tab name!", win_data);

	// page_color should not be free().
	gchar *page_name = NULL, *page_color = NULL;

	// g_debug("Get pid=%d, tpgid=%d", page_data->pid, page_data->new_tpgid);
	// g_debug("win_data->page_shows_window_title = %d", win_data->page_shows_window_title);
	// g_debug("page_data->window_title_updated = %d", page_data->window_title_updated);
	// g_debug("lost_focus = %d", lost_focus);
	// g_debug("page_data->page_update_method = %d", page_data->page_update_method);
	
	switch (page_data->page_update_method)
	{
		case 1:
			update_page_name_wintitle(&page_name, &page_color, win_data, page_data);
			break;
		case 2:
			update_page_name_cmdline(&page_name, &page_color, win_data, page_data);
			break;
		case 3:
			update_page_name_pwd(&page_name, &page_color, win_data, page_data, lost_focus);
			break;
		case 4:
			update_page_name_normal(&page_name, &page_color, win_data, page_data);
			break;
	}
	if (page_name==NULL)
	{
		update_page_name_wintitle(&page_name, &page_color, win_data, page_data);
		update_page_name_cmdline(&page_name, &page_color, win_data, page_data);
		update_page_name_pwd(&page_name, &page_color, win_data, page_data, lost_focus);
		update_page_name_normal(&page_name, &page_color, win_data, page_data);
	}

	if (page_data->is_root)
		page_color = win_data->user_page_color[4];
	
	// even though custom_page_name is setted, we still need to set the page_name for it.
	// therefore if we disable custom_page_name sometime, we still have the page_name for use.
	if (page_data->custom_page_name!=NULL)
	{
		page_color = win_data->user_page_color[3];
		// g_debug("page_shows_custom_dir : page_name = %s, color = %s", page_name, page_color);
	}

	// g_debug("Final : page_name = %s, color = %s", page_name, page_color);

	if (win_data->use_color_page)
		page_data->tab_color = page_color;
	else
		// we Must clean the value for page_color here. for drag&drop tabs.
		page_data->tab_color = NULL;

	gchar *local_page_name = convert_str_to_utf8(page_name, page_data->encoding_str);
	if (local_page_name)
	{
		g_free(page_name);
		page_name = local_page_name;
	}
#ifdef DETAIL
	g_debug("* free page_data->page_name %p (%s) in get_and_update_page_name()",
		page_data->page_name,
		page_data->page_name);
#endif
	g_free(page_data->page_name);
	page_data->page_name = page_name;

	gboolean return_value = FALSE;
	// g_debug ("Launch update_page_name() in get_and_update_page_name()!!!");
	if (update_page_name(page_data->window, page_data->vte, page_data->page_name, page_data->label,
			    page_data->page_no+1, page_data->custom_page_name, page_color, page_data->is_root,
			    page_data->is_bold, compare_win_page_encoding(win_data->default_encoding,
			    						  page_data->encoding_str),
			    page_data->encoding_str, page_data->custom_window_title, lost_focus))
	{
		// g_debug("Update page_data->displayed_tpgid (%d) = page_data->new_tpgid (%d)",
		//	page_data->displayed_tpgid, page_data->new_tpgid);
		if (! lost_focus)
		{
			page_data->displayed_tpgid = page_data->new_tpgid;
			g_free(page_data->window_title_pwd);
			page_data->window_title_pwd = g_strdup(page_data->pwd);
		}
		*(page_data->window_title_tpgid) = page_data->new_tpgid;

		return_value = TRUE;
	}
	else if (page_data->page_update_method == 1)
		page_data->window_title_updated = 2;

	// update page_data->pwd or page_data->window_title_pwd, if it was not updated
	if (page_data->page_update_method == 1)
	{
		if (win_data->page_shows_current_cmdline)
			page_data->displayed_tpgid = page_data->new_tpgid;
		
		if (win_data->page_shows_current_dir)
		{       
			g_free(page_data->pwd);
			page_data->pwd = get_tab_name_with_current_dir(page_data->new_tpgid);
		}
	}

	win_data->window_title_tpgid = page_data->new_tpgid;
	// g_debug("Final: page_data->displayed_tpgid = %d", page_data->displayed_tpgid);
	// g_debug("Final: page_data->window_title_tpgid = %d", *(page_data->window_title_tpgid));

	// g_debug("(get_and_update_page_name): return_value = %d", return_value);
	return return_value;
}

void update_page_name_wintitle(gchar **page_name,
			       gchar **page_color,
			       struct Window *win_data,
			       struct Page *page_data)
{
	// g_debug("Trying to update Window Title!");
	if (win_data->page_shows_window_title && (*page_name==NULL) && (page_data->window_title_updated == 1))
	{
		*page_name = g_strdup(vte_terminal_get_window_title(VTE_TERMINAL(page_data->vte)));
		*page_color = win_data->user_page_color[0];
		page_data->window_title_updated = 0;
		// g_debug("Window title updated: *page_name = %s, color = %s", *page_name, *page_color);
		page_data->page_update_method = 1;
	}
}

void update_page_name_cmdline(gchar **page_name,
			     gchar **page_color,
			     struct Window *win_data,
			     struct Page *page_data)
{
	// g_debug("Trying to update Cmdline!");
	// if win_data->page_shows_current_dir = 0 && win_data->page_shows_window_title = 0
	// We need to show /bin/bash on the tab name.
	// g_debug("page_name = %p (%s)", *page_name, *page_name);
	// g_debug("win_data->page_shows_current_cmdline = %d", win_data->page_shows_current_cmdline);
	// g_debug("page_data->pid = %d, page_data->new_tpgid = %d", page_data->pid, page_data->new_tpgid);
	if ( (*page_name==NULL) && win_data->page_shows_current_cmdline &&
	    ((! (win_data->page_shows_current_dir || 
		 ( win_data->page_shows_window_title && 
		   (vte_terminal_get_window_title(VTE_TERMINAL(page_data->vte))!=NULL) &&
		   (page_data->window_title_updated != -1)))) ||
	     (page_data->pid!=page_data->new_tpgid)))
	{
		*page_name = get_tab_name_with_cmdline(page_data->new_tpgid);
		*page_color = win_data->user_page_color[1];
		// g_debug("Cmdline updated: *page_name = %s, color = %s", *page_name, *page_color);
		page_data->page_update_method = 2;
	}
}

void update_page_name_pwd(gchar **page_name,
			  gchar **page_color,
			  struct Window *win_data,
			  struct Page *page_data,
			  gboolean lost_focus)
{
	// g_debug("Trying to update PWD!");
	if (win_data->page_shows_current_dir && (*page_name==NULL))
	{
		// g_free(page_data->pwd);
		// g_debug("!!! Getting new page_data->pwd");
		// page_data->pwd = get_tab_name_with_current_dir(page_data->new_tpgid);
		// page_name = get_tab_name_with_current_dir(pid);
		if (lost_focus)
		{
			*page_name = g_strdup(page_data->window_title_pwd);
		}
		else
		{
			// g_debug("page_data->pwd (get_and_update_page_name())= %s", page_data->pwd);
			*page_name = g_strdup(page_data->pwd);
		}

		if (*page_name)
		{
			*page_color = win_data->user_page_color[2];
			page_data->window_title_updated = -1;
		}
		// g_debug("PWD updated: *page_name = %s, color = %s", *page_name, *page_color);
		page_data->page_update_method = 3;
	}
}

void update_page_name_normal(gchar **page_name,
			     gchar **page_color,
			     struct Window *win_data,
			     struct Page *page_data)
{
	// g_debug("Trying to update to NORMAL page!");
	if (*page_name==NULL)
	{
		if (page_data->page_name)
			*page_name = g_strdup(page_data->page_name);
		else
			*page_name = get_tab_name_with_page_names(win_data);
		if (win_data->use_color_page)
			*page_color = win_data->user_page_color[5];
		else
			*page_color = NULL;
		// g_debug("page_shows_normal_dir : page_name = %s, color = %s", *page_name, *page_color);
		page_data->page_update_method = 4;
	}
}

gboolean update_page_name(GtkWidget *window, GtkWidget *vte, gchar *page_name, GtkWidget *label,
			  gint page_no, gchar *custom_page_name, const gchar *tab_color, gboolean is_root,
			  gboolean is_bold, gboolean show_encoding, gchar *encoding_str,
			  gboolean custom_window_title, gboolean lost_focus)
{
#ifdef DETAIL
	g_debug("! Launch update_page_name() with vte = %p, label_name = %s, page_no = %d, "
		"custom_page_name = %s, tab_color = %s, is_root = %d, is_bold = %d, "
		"show_encoding = %d, encoding_str = %s",
		vte, page_name, page_no, custom_page_name, tab_color, is_root, is_bold,
		show_encoding, encoding_str);
#endif

	// page_name = NULL when init a new page.
	if (page_name == NULL)
	{
		// g_debug("page_name = NULL!! trying to call get_and_update_page_name()");
		get_and_update_page_name((struct Page *)g_object_get_data(G_OBJECT(vte), "Page_Data"), FALSE);
	}

	gboolean page_name_updated = FALSE;
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	
	// We don't update label name when the size of window is changing.
	// 0xfe = 11,111,110
	// g_debug("win_data->keep_vte_size = %x", win_data->keep_vte_size);
	if (!(win_data->keep_vte_size&0xfffc))
	{
		// g_debug("Updating %d page name to %s...", page_no, page_name);
		gchar *label_name = NULL;
	
		if (custom_page_name==NULL)
			label_name = page_name;
		else
			label_name = custom_page_name;
	
		if (! lost_focus)
			gtk_widget_set_tooltip_text(label, label_name);

		// g_debug("[1] Updating %d page name to %s...", page_no, label_name);

		// FIXME: label_name = NULL when init page.
		if (label_name==NULL) return FALSE;

		if (win_data->page_shows_number)
			label_name = g_strdup_printf("(%d) %s", page_no, label_name);
		else
			label_name = g_strdup(label_name);
		// g_debug("[2] Updating %d page name to %s...", page_no, label_name);
		// label_name will be freed later.
		
		if (win_data->page_shows_encoding && show_encoding && encoding_str)
		{
			gchar *temp_str = g_strdup_printf("%s (%s)", label_name, encoding_str);
#ifdef DETAIL
			g_debug("* free label_name %p (%s) in update_page_name()", label_name, label_name);
#endif
			g_free(label_name);
			label_name = temp_str;
			// g_debug("[3] Updating %d page name to %s...", page_no, label_name);
		}

		// if the window is lost focus, don't update the tab name
		// g_debug("lost_focus = %d", lost_focus);
		if (! lost_focus)
		{
			// win_data->keep_vte_size |= 1;
			// g_debug("window_resizable in update_page_name! and keep_vte_size =%d", keep_vte_size);
			// window_resizable(window, vte, 2, 1);
			keep_window_size (win_data, vte, 0x3);
			if (win_data->use_color_page && (tab_color != NULL))
			{
				gchar *temp_str[2];
				temp_str[0] = g_markup_escape_text(label_name, -1);
				if (is_bold)
					temp_str[1] = g_strconcat("<b><span foreground=\"", tab_color,"\">",
								  temp_str[0], "</span></b>", NULL);
				else
					temp_str[1] = g_strconcat("<span foreground=\"", tab_color,"\">",
								  temp_str[0], "</span>", NULL);
				gtk_label_set_markup (GTK_LABEL(label), temp_str[1]);
				// g_debug("[FINAL] Updating %d page name to %s...", page_no, temp_str[1]);
				// g_debug("temp_str[0] = %s, temp_str[1] = %s",
				//	temp_str[0],
				//	temp_str[1]);
#ifdef DETAIL
				g_debug("* free temp_str[0] %p (%s) in update_page_name()", temp_str[0], temp_str[0]);
#endif
				g_free(temp_str[0]);
#ifdef DETAIL
				g_debug("* free temp_str[1] %p (%s) in update_page_name()", temp_str[1], temp_str[1]);
#endif
				g_free(temp_str[1]);
			}
			else
				gtk_label_set_text(GTK_LABEL(label), label_name);
			// g_debug("Updated the tab name to %s!", page_name);
			page_name_updated = TRUE;
		}
		// else
		//	g_debug("!!! the window is lost focus, don't update the tab name");
		
		// free the data
#ifdef DETAIL
		g_debug("* free label_name %p (%s) in update_page_name()", label_name, label_name);
#endif
		g_free(label_name);
	}

	// we should update window title if page name changed.
	if (win_data->window_shows_current_page && (! custom_window_title))
	{
		gint current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(win_data->notebook));

		// we only update the window title for current page
		// g_debug("current_page_no = %d, page_no = %d", current_page_no, page_no);

		// FIXME: A dirty hack... current_page_no=-1 when creating a 2rd new window.
		// if (win_data->adding_page && (current_page_no<0)) current_page_no = 0;

		if (current_page_no == (page_no-1))
		{
			if (custom_page_name==NULL)
			{
				// g_debug("page_name = %s before calling update_window_title()", page_name);
				update_window_title(window, page_name);
			}
			else
			{
				// g_debug("custom_page_name = %s before calling update_window_title()", custom_page_name);
				update_window_title(window, custom_page_name);
			}
		}
	}
	// g_debug("(update_page_name): page_name_updated = %d", page_name_updated);
	return page_name_updated;
}

void update_window_title(GtkWidget *window, gchar *name)
{
#ifdef DETAIL
	g_debug("! Launch update_window_title with window = %p, and name = %s", window, name);
#endif
	if (name == NULL) return;

	gchar *window_title;
	window_title = g_strdup_printf("%s - %s", name, PACKAGE_NAME);
	// g_debug("Update the window title to %s!", window_title);
	gtk_window_set_title(GTK_WINDOW(window), window_title);
#ifdef DETAIL
	g_debug("* free window_title %p (%s) in update_window_title()", window_title, window_title);
#endif
	g_free(window_title);
}

// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_page_names(struct Window *win_data)
{
#ifdef FULL
	g_debug("! Launch get_tab_name_with_page_names() with win_data = %p", win_data);
#endif

	// g_debug("Get win_data = %d when get tab name with page names!", win_data);
	// g_debug("Got win_data->page_names_no = %d, win_data->max_page_names_no = %d",
	//	win_data->page_names_no, win_data->max_page_names_no);

	if (win_data->page_names_no > win_data->max_page_names_no)
		win_data->page_names_no = win_data->max_page_names_no;

	if (win_data->reuse_page_names && (win_data->splited_page_names[win_data->page_names_no]==NULL))
		win_data->page_names_no=0;

	if (win_data->splited_page_names[win_data->page_names_no]!=NULL)
		return g_strdup(win_data->splited_page_names[win_data->page_names_no]);
	else
		return g_strdup(win_data->page_name);
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_cmdline(pid_t tpgid)
{
#ifdef DETAIL
	g_debug("! Launch get_tab_name_with_cmdline() for tpgid %d", tpgid);
#endif

	if (tpgid>0)
		return get_cmdline(tpgid);
	else
		return NULL;
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_current_dir(pid_t pid)
{
#ifdef FULL
	g_debug("! Launch get_tab_name_with_current_dir() for pid %d", pid);
#endif
	if (pid>0)
	{
		gchar *cwd_path = g_strdup_printf("/proc/%d/cwd", pid);
		gchar *current_dir = g_file_read_link(cwd_path, NULL);
#ifdef FULL
		g_debug("* free cwd_path %p (%s) in get_tab_name_with_current_dir()", cwd_path, cwd_path);
#endif
		g_free(cwd_path);
		return current_dir;
	}
	else
		return NULL;
}


gint get_tpgid(pid_t pid)
{
#ifdef FULL
	g_debug("! Launch get_tpgid() for pid %d", pid);
#endif

	if (pid<1) return 0;

	pid_t tmp_tpgid = 0;
	pid_t new_tpgid = pid;

	while (tmp_tpgid != new_tpgid)
	{
		tmp_tpgid = new_tpgid;
		gchar *stat = NULL;
		gsize length;
		gchar *stat_path = g_strdup_printf("/proc/%d/stat", (gint) tmp_tpgid);

		if (g_file_get_contents (stat_path, &stat, &length, NULL))
		{
			gchar **stats = g_strsplit_set(stat, " ", 8);
			new_tpgid = atoi(stats[7]);
#ifdef FULL
			g_debug("* free stats %p in get_tpgid()", stats);
#endif
			g_strfreev(stats);
		}
#ifdef FULL
		g_debug("* free stat %p (%s) in get_tpgid()", stat, stat);
#endif
		g_free(stat);

#ifdef FULL
		g_debug("* free stat_path %p (%s) in get_tpgid()", stat_path, stat_path);
#endif
		g_free(stat_path);
	}
	// g_debug("The pid =%d, tpgid=%d", pid, new_tpgid);
	return new_tpgid;
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_cmdline(pid_t tpgid)
{
#ifdef DETAIL
	g_debug("! Launch get_cmdline() for tpgid %d", tpgid);
#endif

	if (tpgid<1) return NULL;

	gchar *cmdline=NULL, *cmdline_path;
	gint i;
	gsize length;
	gint timeout=0;
	
	cmdline_path = g_strdup_printf("/proc/%d/cmdline", (gint) tpgid);
	// g_debug("Trying get the cmdline in %s\n", cmdline_path);
	while (g_file_get_contents (cmdline_path, &cmdline, &length, NULL))
	{
		timeout++;
		// g_debug("The cmdline length is %d\n", length);
		if (length==0)
		{
			// we should wait until "/proc/%d/cmdline" is not empty
			// usleep(100000);
			sleep (1);
			g_free(cmdline);
			cmdline = NULL;
		}
		else
		{
			// convert '\0' to ' '
			for (i=0;i<(length-1);i++)
				if (*(cmdline+i)==0)
					*(cmdline+i)=' ';
			break;
		}
		// we only try for 10 times
		if (timeout>=10)
			// We don't need to free cmdline here
			break;
	}
#ifdef DETAIL
	g_debug("* free cmdline_path %p (%s) in get_cmdline()", cmdline_path, cmdline_path);
#endif
	g_free(cmdline_path);

	return cmdline;
//	return g_strdup(cmdline);
}

gboolean check_is_root(pid_t tpgid)
{
#ifdef DETAIL
	g_debug("! Launch check_is_root() for tpgid %d", tpgid);
#endif

	if (tpgid < 1) return FALSE;

	gchar *status=NULL, *status_path;
	gsize length;
	gint timeout=0;
	gboolean is_root=FALSE;
	gboolean uid_checked = FALSE, gid_checked = FALSE;

	status_path = g_strdup_printf("/proc/%d/status", (gint) tpgid);
	// g_debug("Trying get the status in %s\n", status_path);
	while (g_file_get_contents (status_path, &status, &length, NULL))
	{
		timeout++;
		// g_debug("The status length is %d\n", length);
		if (length==0)
			// we should wait until "/proc/%d/status" is not empty
			// usleep(100000);
			sleep (1);
		else
		{
			// g_debug("Status = %s", status);
			gchar **status_line = g_strsplit_set(status, "\n", 0);
			gchar **status_data;
			gint i=0;
			
			while (status_line[i]!=NULL)
			{
				// g_debug("%d) %s",i ,status_line[i]);
				status_data = g_strsplit_set(status_line[i], "\t", 0);

				if (strcmp(status_data[0], "Uid:")==0)
				{
					is_root = check_status_data(status_data);
					uid_checked = TRUE;
				}
				else if (strcmp(status_data[0], "Gid:")==0)
				{
					is_root = check_status_data(status_data);
					gid_checked = TRUE;
				}
#ifdef DETAIL
				g_debug("* free statys_data %p in check_is_root()", status_data);
#endif
				g_strfreev(status_data);
				if ((is_root) || (uid_checked && gid_checked))
					break;
				i++;
			}
#ifdef DETAIL
			g_debug("* free statys_line %p in check_is_root()", status_line);
#endif
			g_strfreev(status_line);
			break;
		}
		// we only try for 10 times
		if (timeout>=10)
			break;
	}
#ifdef DETAIL
	g_debug("* free status %p (%s) in check_is_root()", status, status);
#endif
	g_free(status);
#ifdef DETAIL
	g_debug("* free status_path %p (%s) in check_is_root()", status_path, status_path);
#endif
	g_free(status_path);

	return is_root;
}

gboolean check_status_data(gchar **status_data)
{
#ifdef DETAIL
	g_debug("! Launch check_status_data() !");
#endif

	gint i;
	for (i=1;i<5;i++)
	{
		// g_debug("Checking %s...", status_data[i]);
		if (atoi(status_data[i])==0)
		{
			// g_debug("IT IS ROOT (%d)!", atoi(status_data[i]));
			return TRUE;
		}
	}
	return FALSE;
}

void update_page_window_title (VteTerminal *vte, struct Page *page_data)
{
#ifdef DETAIL
	g_debug("! Launch update_page_window_title() !");
#endif
	page_data->window_title_updated = 1;
	// get_and_update_page_name(page_data);
	monitor_cmdline(page_data);
}

//void monitor_cmdline(GFileMonitor *monitor, pid_t pid)
//{
//	GError *error = NULL;
//	gchar *stat_path = g_strdup_printf("/proc/%d/stat", (gint) pid);
//	GFile *file = g_file_new_for_path(stat_path);
//	monitor = g_file_monitor_file (file, 0, NULL, &error);
//	if (monitor)
//		g_signal_connect (monitor, "changed", (GCallback)monitor_cmdline_changed, NULL);
//	else
//	{
//		g_free (monitor);
//		monitor = NULL;
//	}
//}

//void monitor_cmdline_changed(GFileMonitor *monitor, GFile *file, GFile *other_file,
//			     GFileMonitorEvent event_type, gpointer user_data)
//{
//	g_debug("Got monitor_cmdline_changed message\n");
//	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vte), "Page_Data");
//	gint current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
//
//	get_and_update_page_name(page_data->label, page_data->pid, &(page_data->tpgid), current_page_no+1);
//}

//void monitor_cmdline_cancel(GFileMonitor *monitor)
//{
//	g_io_channel_close(channel);
//	g_file_monitor_cancel (monitor);
//}

