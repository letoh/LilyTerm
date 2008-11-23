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
// extern gboolean page_shows_current_dir;
extern gboolean window_shows_current_page;
extern gboolean page_number;

extern GtkWidget *window;
extern GtkWidget *notebook;
extern GtkWidget *current_vtebox;

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
			update_page_name(current_data->label, i+1, current_data->custom_page_name);
	}
}

gboolean monitor_cmdline(gpointer data)
{
	struct Page *current_data = (struct Page *)g_object_get_data(G_OBJECT(current_vtebox), "Data");
	pid_t tpgid = get_tpgid(current_data->stat_path, current_data->pid);
	// g_debug("the original tpgid is %d, and got tpgid from get_tpgid() is: %d\n", current_data->tpgid, tpgid);

	// only updte the page name when tpgid is updated.
	if (tpgid != current_data->tpgid)
		// tpgid will be updated in somewhere in update_tab_name() XD
		update_tab_name(current_data->stat_path, current_data->label,
				current_data->pid, &(current_data->tpgid),
				gtk_notebook_get_current_page(GTK_NOTEBOOK(notebook))+1,
				current_data->custom_page_name);
	return TRUE;
}

// it will update the text in label ,label->name, and the title of window
void update_tab_name(gchar *stat_path, GtkWidget *label, pid_t pid, pid_t *tpgid,
		     gint page_no, gchar *custom_page_name)
{
	// Please resure that label->name = NULL when init
	g_free(label->name);
	
	// got the tab name
	if (page_shows_current_cmdline)
		// we get stat_path from "struct Page" for performance.
		// tpgid will be update in get_tab_name_with_cmdline()
		label->name = get_tab_name_with_cmdline(stat_path, pid, tpgid);
	
	//if ((page_shows_current_dir) && ((label->name==NULL) || (pid!=*tpgid)))
	//	label->name = get_tab_name_with_dir(pid);
	
	if (label->name==NULL)
		label->name = get_tab_name_with_page_names();

	// we'll update the page name every time.
	// please don't update the tab name if unnecessary.
	update_page_name(label, page_no, custom_page_name);
}

void update_window_title(gchar *name)
{
	gchar *window_title = g_strdup_printf("%s - %s", name, PACKAGE_NAME);
	gtk_window_set_title(GTK_WINDOW(window), window_title);
	g_free(window_title);
}

void update_page_name(GtkWidget *label, gint page_no, gchar *custom_page_name)
{
	if (page_number)
	{
		gchar *page_name;
		if (custom_page_name==NULL)
			page_name = g_strdup_printf("(%d) %s", page_no, label->name);
		else
			page_name = g_strdup_printf("(%d) %s", page_no, custom_page_name);

		gtk_label_set_text(GTK_LABEL(label), page_name);
		g_free(page_name);
	}
	else
		if (custom_page_name==NULL)
			gtk_label_set_text(GTK_LABEL(label), label->name);

	// we should update window title if page name changed.
	if (window_shows_current_page)
		update_window_title(label->name);
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
// gchar *get_tab_name_with_dir(pid_t pid)
// {
//	// g_file_read_link will return NULL if an error occurred.
//	return g_strdup(g_file_read_link(g_strdup_printf("/proc/%d/cwd", pid), NULL));
//}

// It will return NULL if fault
// The returned string should be freed when no longer needed.
// It will update *tpgid too.
gchar *get_tab_name_with_cmdline(gchar *stat_path, pid_t pid, pid_t *tpgid)
{
	gchar *cmdline=NULL;
	
	// we get stat_path from "struct Page" for performance.
	*tpgid = get_tpgid(stat_path, pid);
	// g_debug("Got tpgid = %d\n", *tpgid);

	if (*tpgid!=0)
		cmdline = get_cmdline(*tpgid);
	// g_debug("Got Forground program = %s\n", cmdline);
	
	return cmdline;
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

