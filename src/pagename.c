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


#include "pagename.h"

// The defalut Page Name
extern GtkWidget *current_vtebox;

void reorder_page_number(GtkNotebook *notebook, GtkWidget *child, guint page_num, GtkWidget *window)
{
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get window = %d and win_data = %d when reorder page number!", window, win_data);
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	GtkWidget *vtebox;
	struct Page *page_data;
	gint i;
	
	for (i=0;i<total_page;i++)
	{
		// Got the vtebox for this page (index by label)
		vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(
							gtk_notebook_get_tab_label(GTK_NOTEBOOK(notebook),
								gtk_notebook_get_nth_page(
									GTK_NOTEBOOK(notebook), i))),
							"VteBox");
		// Got the struct Page for this page (index by vtebox)
		page_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Page_Data");
		// we store current_page_no in "struct Page" for performance.
		page_data->page_no = i;

		if (win_data->page_shows_number)
			update_page_name(window, vtebox, page_data->label, i+1, page_data->custom_page_name,
					 page_data->tab_color, page_data->is_root, page_data->is_bold,
					 page_data->encoding != win_data->default_encoding, page_data->encoding);
	}
}

gboolean monitor_cmdline(struct Page *page_data)
{
	// g_debug("Updating the page name for %d.", vtebox);
	// The pagename won't be updated if LilyTerm is not on focuse.
	if (*(page_data->lost_focuse) || (*(page_data->keep_vtebox_size)&0x1e))
		return TRUE;

	gboolean update_pwd = FALSE;
	pid_t old_tpgid = page_data->tpgid;

	if (page_data->pid<1)
	{
		page_data->tpgid = 0;
		return FALSE;
	}
	else
	{
		page_data->tpgid = get_tpgid(page_data->pid);
		if ((old_tpgid != page_data->tpgid) && *(page_data->check_root_privileges))
			page_data->is_root = check_is_root(page_data->tpgid);
	}
	
	// g_debug("the original tpgid is %d, and got tpgid from get_tpgid() is: %d\n",
	//	old_tpgid, page_data->tpgid);
	
	if (*(page_data->page_shows_current_dir) &&
	    page_data->tpgid == page_data->pid &&
	    page_data->custom_page_name==NULL)
	{
		gchar *new_dir = get_tab_name_with_current_dir(page_data->pid);
		if (page_data->pwd==NULL)
			update_pwd = TRUE;
		else if (strcmp(page_data->pwd, new_dir))
			update_pwd = TRUE;

		if (update_pwd)
		{
			// g_debug("PWD '%s' is changed. Updating tab name...", new_dir);
			g_free(page_data->pwd);
			page_data->pwd = new_dir;
		}
		else
		{
			// g_debug("PWD '%s' is not changed. Have no need to update tab name...", new_dir);
			g_free(new_dir);
		}
	}

	// only updte the page name when tpgid is updated.
	if (update_pwd ||
	   ((*(page_data->page_shows_current_cmdline) && (old_tpgid != page_data->tpgid)) ||
	    (*(page_data->page_shows_current_dir) && update_pwd && (page_data->tpgid == page_data->pid))))
	{
		if (page_data->vtebox != current_vtebox)
			page_data->is_bold = *(page_data->bold_action_page_name);
		update_tab_name(page_data);
	}
	return TRUE;
}

// it will update the text in label ,label->name, and the title of window
void update_tab_name(struct Page *page_data)
{
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(page_data->window), "Win_Data");
	// g_debug("Get win_data = %d when update tab name!", win_data);

	// page_name = label->name, so that it don't need to be freed
	// page_color should not be freed too.
	gchar *page_name = NULL, *page_color = NULL;

	// Please resure that page_name = NULL when init
	// g_free(label->name);
	// g_debug("Get pid=%d, tpgid=%d", page_data->pid, page_data->tpgid);

	// got the tab name
	if (win_data->page_shows_current_cmdline &&
	    ((!win_data->page_shows_current_dir) ||
	     (win_data->page_shows_current_dir && (page_data->pid!=page_data->tpgid))))
	{
		page_name = get_tab_name_with_cmdline(page_data->tpgid);
		if (page_name!=NULL)
			page_color = win_data->page_cmdline_color;
		// g_debug("page_shows_current_cmdline : page_name = %s, color = %s", page_name, page_color);
	}
	
	if (win_data->page_shows_current_dir && (page_name==NULL))
	{
		// page_name = get_tab_name_with_current_dir(pid);
		page_name = g_strdup(page_data->pwd);
		if (page_name!=NULL)
			page_color = win_data->page_dir_color;
		// g_debug("page_shows_current_dir : page_name = %s, color = %s", page_name, page_color);
	}

	if (page_name==NULL)
	{
		page_name = get_tab_name_with_page_names(win_data);
		page_color = win_data->page_normal_color;
		// g_debug("page_shows_normal_dir : page_name = %s, color = %s", page_name, page_color);
	}

	if (page_data->is_root)
		page_color = win_data->page_root_color;
	
	// even though custom_page_name is setted, we still need to set the page_name for it.
	// therefore if we disable custom_page_name sometime, we still have the page_name for use.
	if (page_data->custom_page_name!=NULL)
	{
		page_color = win_data->page_custom_color;
		// g_debug("page_shows_custom_dir : page_name = %s, color = %s", page_name, page_color);
	}

	// g_debug("Final : page_name = %s, color = %s", page_name, page_color);

	if (win_data->use_color_page)
		page_data->tab_color = page_color;

	g_free(page_data->label->name);
	// page_data->label->name will be freed whe page_data->label is destroyed.
	page_data->label->name = page_name;

	update_page_name(page_data->window, page_data->vtebox, page_data->label, page_data->page_no+1,
			 page_data->custom_page_name, page_color, page_data->is_root,
			 page_data->is_bold, page_data->encoding != win_data->default_encoding, page_data->encoding);
}

void update_page_name(GtkWidget *window, GtkWidget *vtebox, GtkWidget *label, gint page_no, gchar *custom_page_name, const gchar *tab_color, gboolean is_root, gboolean is_bold, gboolean show_encoding, GtkWidget *encoding)
{
	struct Window *win_data = (struct Window *)g_object_get_data(G_OBJECT(window), "Win_Data");
	// g_debug("Get win_data = %d when update page name!", win_data);

	if (win_data->keep_vtebox_size&0x1e) return;
	
	// FIXME: why 
	if (label->name==NULL) return;
	
	// g_debug("Updating %d page name to %s...", page_no, label->name);
	gchar *page_name = NULL;

	if (custom_page_name==NULL)
		page_name = label->name;
	else
		page_name = custom_page_name;
	
	// g_debug("Updating %d page name to %s...", page_no, page_name);
	// page_name will be freed later.
	if (win_data->page_shows_number)
		page_name = g_strdup_printf("(%d) %s", page_no, page_name);
	
	if (win_data->page_shows_encoding && show_encoding && encoding)
	{
		gchar *temp_str;
		gchar **locales = g_strsplit_set(encoding->name, ".", 0);
		
		if (locales[1])
			temp_str = g_strdup_printf("%s (%s)", page_name, locales[1]);
		else
			temp_str = g_strdup_printf("%s (%s)", page_name, locales[0]);
		g_strfreev(locales);
		if (win_data->page_shows_number) g_free(page_name);
		page_name = temp_str;
	}

	win_data->keep_vtebox_size |= 1;
	// g_debug("window_resizable in update_page_name! and keep_vtebox_size =%d", keep_vtebox_size);
	window_resizable(window, vtebox, 2, 1);
	if (win_data->use_color_page)
	{
		gchar *temp_str[2];
		temp_str[0] = g_markup_escape_text(page_name, -1);
		if (is_bold)
			temp_str[1] = g_strconcat("<b><span foreground=\"", tab_color,"\">", temp_str[0], "</span></b>",
						  NULL);
		else
			temp_str[1] = g_strconcat("<span foreground=\"", tab_color,"\">", temp_str[0], "</span>", NULL);
		gtk_label_set_markup (GTK_LABEL(label), temp_str[1]);
		g_free(temp_str[0]);
		g_free(temp_str[1]);
	}
	else
		gtk_label_set_text(GTK_LABEL(label), page_name);
	
	if ((win_data->page_shows_number) ||
	    (win_data->page_shows_encoding && show_encoding && encoding))
		g_free(page_name);
	
	// we should update window title if page name changed.
	if (win_data->window_shows_current_page)
	{
		gint current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(win_data->notebook));

		// we only update the window title for current page
		if (current_page_no == (page_no-1))
		{
			if (custom_page_name==NULL)
				update_window_title(window, label->name);
			else
				update_window_title(window, custom_page_name);
		}
	}
}

void update_window_title(GtkWidget *window, gchar *name)
{
	gchar *window_title;
	window_title = g_strdup_printf("%s - %s", name, PACKAGE_NAME);
	gtk_window_set_title(GTK_WINDOW(window), window_title);
	g_free(window_title);
}

// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_page_names(struct Window *win_data)
{
	// g_debug("Get win_data = %d when get tab name with page names!", win_data);
	if (win_data->reuse_page_names)
		if (win_data->splited_page_names[win_data->page_names_no]==NULL)
			win_data->page_names_no=0;

	if (win_data->splited_page_names[win_data->page_names_no]!=NULL)
	{
		win_data->page_names_no++;
		return g_strdup(win_data->splited_page_names[win_data->page_names_no-1]);
	}
	else
		return g_strdup(win_data->page_name);
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_cmdline(pid_t tpgid)
{
	if (tpgid>0)
		return get_cmdline(tpgid);
	else
		return NULL;
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_current_dir(pid_t pid)
{
	if (pid>0)
		return g_strdup(g_file_read_link(g_strdup_printf("/proc/%d/cwd", pid), NULL));
	else
		return NULL;
}


gint get_tpgid(pid_t pid)
{
	if (pid<1) return 0;

	pid_t tmp_tpgid = 0;
	pid_t new_tpgid = pid;
	gchar *stat_path, *stat, **stats=NULL;
	gsize length;

	while (tmp_tpgid != new_tpgid)
	{
		tmp_tpgid = new_tpgid;
		stat_path = g_strdup_printf("/proc/%d/stat", (gint) tmp_tpgid);
		if (g_file_get_contents (stat_path, &stat, &length, NULL))
		{
			stats = g_strsplit_set(stat, " ", 8);
			new_tpgid = atoi(stats[7]);
			g_free(stat);
			g_strfreev(stats);
		}
		g_free(stat_path);
	}
	return new_tpgid;
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_cmdline(pid_t tpgid)
{
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
			// we should wait until "/proc/%d/cmdline" is not empty
			usleep(100000);
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
			// FIXME: should we free it here?
			break;
	}
	g_free(cmdline_path);

	return g_strdup(cmdline);
}

gboolean check_is_root(pid_t tpgid)
{
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
			usleep(100000);
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

				g_strfreev(status_data);
				if ((is_root) || (uid_checked && gid_checked))
					break;
				i++;
			}
			g_strfreev(status_line);
			break;
		}
		// we only try for 10 times
		if (timeout>=10)
			break;
	}
	g_free(status);
	g_free(status_path);

	return is_root;
}

gboolean check_status_data(gchar **status_data)
{
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
//	struct Page *page_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Page_Data");
//	gint current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
//
//	update_tab_name(page_data->label, page_data->pid, &(page_data->tpgid), current_page_no+1);
//}

//void monitor_cmdline_cancel(GFileMonitor *monitor)
//{
//	g_io_channel_close(channel);
//	g_file_monitor_cancel (monitor);
//}

