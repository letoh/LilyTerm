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
extern gchar *page_name;
extern gchar *page_names;
extern gchar **splited_page_names;
extern gint page_names_no;
extern gboolean reuse_page_names;
extern gboolean page_shows_current_cmdline;
extern gboolean page_shows_current_dir;
extern gboolean use_color_page;
extern gchar *page_cmdline_color;
extern gchar *page_dir_color;
extern gchar *page_custom_color;
extern gchar *page_normal_color;
extern gboolean window_shows_current_page;
extern gboolean page_number;

extern GtkWidget *window;
extern GtkWidget *notebook;

extern gboolean lost_focuse;
extern gboolean add_remove_page;
extern gboolean style_set;

void reorder_page_number(GtkWidget *widget, gpointer user_data)
{
	gint total_page = gtk_notebook_get_n_pages(GTK_NOTEBOOK(notebook));
	GtkWidget *vtebox;
	struct Page *current_data;
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
		current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
		// we store current_page_no in "struct Page" for performance.
		current_data->current_page_no = i;
		
		if (page_number)
			update_page_name(current_data->label, i+1, current_data->custom_page_name,
					 current_data->tab_color);
	}
}

gboolean monitor_cmdline(GtkWidget *vtebox)
{
	// The pagename won't be updated if LunaTerm is not on focuse.
	if (lost_focuse || add_remove_page || style_set)
		return TRUE;

	gboolean update_pwd = FALSE;

	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
	pid_t old_tpgid = current_data->tpgid;

	if (current_data->pid==0)
	{
		current_data->tpgid = 0;
		return TRUE;
	}
	else
		current_data->tpgid = get_tpgid(current_data->stat_path, current_data->pid);
	
	// g_debug("the original tpgid is %d, and got tpgid from get_tpgid() is: %d\n",
	//	old_tpgid, current_data->tpgid);
	
	if (page_shows_current_dir && (current_data->tpgid == current_data->pid) && current_data->custom_page_name==NULL)
	{
		gchar *new_dir = get_tab_name_with_current_dir(current_data->pid);
		// g_debug("Old dir =%s", current_data->pwd);
		// g_debug("New dir =%s", new_dir);
		if (current_data->pwd==NULL)
			update_pwd = TRUE;
		else if (strcmp(current_data->pwd, new_dir))
			update_pwd = TRUE;

		if (update_pwd)
		{
			// g_debug("PWD '%s' is changed. Updating tab name...", new_dir);
			g_free(current_data->pwd);
			current_data->pwd = new_dir;
		}
		else
		{
			// g_debug("PWD '%s' is not changed. Have no need to update tab name...", new_dir);
			g_free(new_dir);
		}
	}

	// only updte the page name when tpgid is updated.
	if (update_pwd ||
	   ((page_shows_current_cmdline && (old_tpgid != current_data->tpgid)) ||
	    (page_shows_current_dir && update_pwd && (current_data->tpgid == current_data->pid))))
		update_tab_name(current_data->stat_path, current_data->label,
				current_data->pid, current_data->tpgid,
				current_data->current_page_no+1, current_data->custom_page_name,
				current_data->pwd);
	return TRUE;
}

// it will update the text in label ,label->name, and the title of window
void update_tab_name(gchar *stat_path, GtkWidget *label, pid_t pid, pid_t tpgid,
		     gint page_no, gchar *custom_page_name, const gchar *pwd)
{
	// page_name = label->name, so that it don't need to be freed
	// page_color should not be freed too.
	gchar *page_name = NULL, *page_color = NULL;
	// Please resure that page_name = NULL when init
	// g_free(label->name);
	// g_debug("Get pid=%d, tpgid=%d", pid, tpgid);

	// got the tab name
	if (page_shows_current_cmdline &&
	   ((!page_shows_current_dir) || (page_shows_current_dir && (pid!=tpgid))))
	{
		// we get stat_path from "struct Page" for performance.
		page_name = get_tab_name_with_cmdline(tpgid);
		if (page_name!=NULL)
			page_color = page_cmdline_color;
		// g_debug("page_shows_current_cmdline : page_name = %s, color = %s", page_name, page_color);
	}
	
	if (page_shows_current_dir && (page_name==NULL))
	{
		// page_name = get_tab_name_with_current_dir(pid);
		page_name = g_strdup(pwd);
		if (page_name!=NULL)
			page_color = page_dir_color;
		// g_debug("page_shows_current_dir : page_name = %s, color = %s", page_name, page_color);
	}
	
	if (page_name==NULL)
	{
		page_name = get_tab_name_with_page_names();
		page_color = page_normal_color;
		// g_debug("page_shows_normal_dir : page_name = %s, color = %s", page_name, page_color);
	}

	// even though custom_page_name is setted, we still need to set the page_name for it.
	// therefore if we disable custom_page_name sometime, we still have the page_name for use.
	if (custom_page_name!=NULL)
	{
		page_color = page_custom_color;
		// g_debug("page_shows_custom_dir : page_name = %s, color = %s", page_name, page_color);
	}
	
	// g_debug("Final : page_name = %s, color = %s", page_name, page_color);

	if (use_color_page)
	{
		GtkWidget *vtebox=(GtkWidget *)g_object_get_data(G_OBJECT(label), "VteBox");
		if (vtebox!=NULL)
		{
			struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(vtebox), "Data");
			if (current_data!=NULL)
				current_data->tab_color = page_color;
		}
	}
	
	g_free(label->name);
	label->name = page_name;

	update_page_name(label, page_no, custom_page_name, page_color);
}

void update_page_name(GtkWidget *label, gint page_no, gchar *custom_page_name, const gchar *tab_color)
{
	if (lost_focuse || add_remove_page || style_set)
		return;

	// g_debug("Updating page name to %s...", label->name);
	gchar *page_name = NULL;

	if (custom_page_name==NULL)
		page_name = label->name;
	else
		page_name = custom_page_name;
	
	if (page_number)
	{
		gchar *temp_name = g_strdup_printf("(%d) %s", page_no, page_name);
		page_name = temp_name;
	}

	if (use_color_page)
	{
		gchar *temp_str[2];
		temp_str[0] = g_markup_escape_text(page_name, -1);
		temp_str[1] = g_strconcat("<span foreground=\"", tab_color,"\">", temp_str[0], "</span>", NULL);
		gtk_label_set_markup (GTK_LABEL(label), temp_str[1]);
		g_free(temp_str[0]);
		g_free(temp_str[1]);
	}
	else
		gtk_label_set_text(GTK_LABEL(label), page_name);
	
	if (page_number)
		g_free(page_name);
	
	// we should update window title if page name changed.
	if (window_shows_current_page)
	{
		if (custom_page_name==NULL)
			update_window_title(label->name);
		else
			update_window_title(custom_page_name);
	}
}

void update_window_title(gchar *name)
{
	gchar *window_title = g_strdup_printf("%s - %s", name, PACKAGE_NAME);
	gtk_window_set_title(GTK_WINDOW(window), window_title);
	g_free(window_title);
}

// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_page_names()
{
	if (reuse_page_names)
		if (splited_page_names[page_names_no]==NULL)
			page_names_no=0;

	if (splited_page_names[page_names_no]!=NULL)
	{
		page_names_no++;
		return g_strdup(splited_page_names[page_names_no-1]);
	}
	else
		return g_strdup(page_name);
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_cmdline(pid_t tpgid)
{
	if (tpgid!=0)
		return get_cmdline(tpgid);
	else
		return NULL;
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_tab_name_with_current_dir(pid_t pid)
{
	if (pid!=0)
		return g_strdup(g_file_read_link(g_strdup_printf("/proc/%d/cwd", pid), NULL));
	else
		return NULL;
}


gint get_tpgid(gchar *stat_path, pid_t pid)
{
#ifdef USE_LIBGTOP
	glibtop_proc_uid buf;
	glibtop_get_proc_uid(&buf, pid);
	return buf.tpgid;
#else
	// we get stat_path from "struct Page" for performance.
	gchar *stat, **stats=NULL;
	gsize length;
	gint tpgid=0;
	
	if (g_file_get_contents (stat_path, &stat, &length, NULL))
	{
		// we don't need to strsplit whole string, for performance.
		stats = g_strsplit_set(stat, " ", LINUX_TPGID+1);
		tpgid = atoi(stats[LINUX_TPGID]);
	}
	
	g_free(stat);
	g_strfreev(stats);
	return tpgid;
#endif	
}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
gchar *get_cmdline(pid_t tpgid)
{
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
//	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
//	gint current_page_no = gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook));
//
//	update_tab_name(current_data->label, current_data->pid, &(current_data->tpgid), current_page_no+1);
//}

//void monitor_cmdline_cancel(GFileMonitor *monitor)
//{
//	g_io_channel_close(channel);
//	g_file_monitor_cancel (monitor);
//}

