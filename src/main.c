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

#include "main.h"

// for using in socket
gboolean single_process = TRUE;
gint socket_fd = 0;
struct sockaddr_un address = {0};
int address_len = 0;
GIOChannel* main_channel = NULL;

int main( int   argc,
	  char *argv[] )
{
	// i18n support. We need to support i18n under console, too.
	setlocale(LC_ALL, "");
	bindtextdomain (PACKAGE, LOCALEDIR);
	bind_textdomain_codeset (PACKAGE, "UTF-8");
	textdomain (PACKAGE);

	// deal the command line optins
	command_option(argc, argv);

	// init the gtk+2 engine
	gtk_init(&argc, &argv);

	// FIXME: we should get the single_process from profile. Current is command-line option only.
	if (single_process)
	{
		// init socket data
		if (init_socket_data())
		{
			// trying to connect to a exiting LilyTerm
			if (query_socket())
			{
				// success, sent the argc/argv to socket then quit
				// g_debug("A LilyTerm socket server is exist already. exiting...");
				if (send_socket(argc, argv))
					exit (0);
			}
			// no LilyTerm exist. create a socket server
			// g_debug("Creating a LilyTerm socket server...");
			init_socket_server();
		}
	}
	
	// start LilyTerm
	extern gchar **environ;
	new_window(argc, argv, environ, 0, NULL);
	// The argv of "main" LilyTerm can't be free.
	// Set it to NULL here to avoid double_free().
	argv=NULL;

	// g_debug("Start to run LilyTerm");
	gtk_main();
	
	return 0;
}

void command_option(int   argc,
		    char *argv[])
{
#ifdef DEBUG
	g_debug("! Launch command_option()!");
#endif

	gint i;
	for (i=0;i<argc;i++)
	{
		// g_debug("%2d (Total %d): %s\n",i, argc, argv[i]);
		if ((!strcmp(argv[i], "-v")) || (!strcmp(argv[i], "--version")))
		{
			g_print("%s %s\n", PACKAGE_NAME, VERSION);
			exit (0);
		}
		else if ((!strcmp(argv[i], "-h")) || (!strcmp(argv[i], "--help")))
		{
			g_print("\n%s\n", got_help_message()->str);
			exit (0);
		}
		else if ((!strcmp(argv[i], "-p")) || (!strcmp(argv[i], "--profile")))
		{
			g_print(save_user_settings(NULL, NULL)->str);
			exit (0);
		}
		else if ((!strcmp(argv[i], "-s")) || (!strcmp(argv[i], "--separate")))
			single_process = FALSE;
	}
}

GString *got_help_message()
{
#ifdef DEBUG
	g_debug("! Launch got_help_message()!");
#endif

	GString *help_message;
	gint j;
	const char * const *system_dirs = g_get_system_config_dirs();

	help_message = g_string_new("");
	g_string_append_printf( help_message,
					_("%s is a libvte based X Terminal Emulator.\n\n"), PACKAGE_NAME);
	g_string_append( help_message,  _("Use -e/-x/--execute {Command} to run a command when start up."
					  " (Must be the final option).\n"));
	g_string_append( help_message,  _("Use -t/--tab {number} to open multi tabs when start up.\n"));
	g_string_append( help_message,  _("Use -s/--separate to run in separate process.\n"));
	g_string_append( help_message,  _("Use -v/--version to show the version information.\n"));
	g_string_append( help_message,  _("Use -p/--profile to got a profile sample.\n"));
	g_string_append_printf( help_message,
					_("Use -u/--user_profile {%s} to use a specified profile.\n\n"), RCFILE);
	for (j=0; system_dirs[j] != NULL; j++)
		g_string_append_printf( help_message,
					_("The %s system config is: %s/%s\n"), PACKAGE_NAME, system_dirs[j], RCFILE);
	g_string_append_printf( help_message,
					_("And your %s profile is: %s/%s\n\n"),
								PACKAGE_NAME, g_get_user_config_dir(), RCFILE);
	g_string_append( help_message,  _("Default shortcut key: (It may custom by editing user's profile)\n\n"));
	g_string_append( help_message,  _("  * <Ctrl><`>\t\tDisable/Enable hyperlinks, function keys and right click menu\n"));
	g_string_append( help_message,  _("  * <Ctrl><T/Q>\t\tAdd a New tab/Close current tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><E>\t\tRename current tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><PgUp/PgDn>\tSwitch to Prev/Next tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><Home/End>\tSwitch to First/Last tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><Left/Right>\tMove current page Forward/Backward\n"));
	g_string_append( help_message,  _("  * <Ctrl><Up/Down>\tMove current page to First/Last\n"));
	g_string_append( help_message,  _("  * <Ctrl><F1~F12>\tSwitch to 1st ~ 12th tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><O>\t\tSelect all the text in the Vte Terminal box\n"));
	g_string_append( help_message,  _("  * <Ctrl><X>\t\tCopy the text to clipboard\n"));
	g_string_append( help_message,  _("  * <Ctrl><V>\t\tPast the text in clipboard\n"));
	g_string_append( help_message,  _("  * <Ctrl><+/->\t\tIncrease/Decrease the font size of current tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><Enter>\tReset the font size of current tab\n"));
	g_string_append( help_message,  _("  * <Shift><Insert>\tPast the text in primary clipboard\n"));
	g_string_append( help_message,  _("\t\t\t(i.e. Emulate a middle button mouse click to past the text)\n\n"));
	g_string_append_printf( help_message,
					_("Please report bug to %s. Thank you for using %s!\n"),
								PACKAGE_BUGREPORT, PACKAGE_NAME);
	return help_message;
}

// it will return TRUE if init socket data successfully
gboolean init_socket_data()
{
#ifdef DEBUG
	g_debug("! Launch init_socket_data() to init LilyTerm socket!");
#endif

	// clean data first
	bzero(&address, sizeof(address));
	// init the address of socket
	address.sun_family = AF_UNIX;
	// the max size of saddr.sun_path in Linux is 108!
#ifdef ENABLE_DEVELOP
	g_snprintf(address.sun_path, 108, "%s/.%s_dev_%s%s",
		   g_get_tmp_dir() ,PACKAGE, g_get_user_name(), gdk_get_display());
#else
	g_snprintf(address.sun_path, 108, "%s/.%s_%s%s",
		   g_get_tmp_dir() ,PACKAGE, g_get_user_name(), gdk_get_display());
#endif
	// g_debug("The socket file is %s", address.sun_path);
	address_len = sizeof(address);

	// init socket
	socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0)
		return socket_fault(1, NULL, NULL, FALSE);

	// set this socket is non-block
	return set_fd_non_block(&socket_fd);
}

// it will return TRUE if successed
gboolean set_fd_non_block(gint *fd)
{
#ifdef DEBUG
	g_debug("! Launch set_fd_non_block()!");
#endif

	gint flags = fcntl(*fd, F_GETFL, 0);
	if (fcntl(*fd, F_SETFL, O_NONBLOCK|flags) < 0)
		return socket_fault(8, NULL, NULL, FALSE);
	return TRUE;
}

// it will return TRUE if successed
gboolean query_socket()
{
#ifdef DEBUG
	g_debug("! Launch query_socket() to connect to a exiting LilyTerm !");
#endif

	if(connect(socket_fd, (struct sockaddr *)&address, address_len) < 0)
		return socket_fault(2, NULL, NULL, FALSE);
	return TRUE;
}

// it will return TRUE if successed
gboolean send_socket( int   argc,
		      char *argv[])
{
#ifdef DEBUG
	g_debug("! Launch send_socket() to send data to the exiting LilyTerm !");
#endif

	GError *error = NULL;
	gsize len;
	int i;
	extern gchar **environ;

	// send data: VTE_CJK_WIDTH argc argv environment.
	GString *arg_str = g_string_new(g_strdup_printf("%d ", argc));
	for (i=0; i<argc; i++)
		g_string_append_printf(arg_str, "%s ", argv[i]);
	i=0;
	if (environ!=NULL)
	{
		while (environ[i]!=NULL)
		{
			// g_debug("%d: %s", i, environ[i]);
			g_string_append_printf(arg_str, "%s ", environ[i]);
			i++;
		}
	}
	
	g_string_append_printf(arg_str, "\n");
	// g_debug("Sent data: %s", arg_str->str);

	// write data!
	GIOChannel* channel = g_io_channel_unix_new(socket_fd);
	if (!channel) return socket_fault(12, NULL, NULL, FALSE);
	// set the channel to read binary file
	if (g_io_channel_set_encoding(channel, NULL, &error) == G_IO_STATUS_ERROR)
		return socket_fault(9, error, channel, FALSE);
	g_io_channel_set_buffered (channel, FALSE);
	if (g_io_channel_write_chars(channel, arg_str->str, -1, &len, &error)== G_IO_STATUS_ERROR)
		return socket_fault(11, error, channel, FALSE);
	// flush writing datas
	if (g_io_channel_flush(channel, &error) == G_IO_STATUS_ERROR)
		return socket_fault(13, error, channel, FALSE);
	clear_channel(channel, FALSE);
	g_string_free(arg_str, TRUE);

	// FIXME: sleep for 1 sec to wait the socket server. any better idea?
	sleep(1);

	return TRUE;
}

// it will return TRUE if successed
gboolean init_socket_server()
{
#ifdef DEBUG
	g_debug("! Launch init_socket_server() to init a LilyTerm socket server !");
#endif

	GError *error = NULL;

	// clear the prev file
	unlink(address.sun_path);

	// bind the socket on a file
	if (bind(socket_fd, (struct sockaddr *)&address, address_len) < 0)
		return socket_fault(3, NULL, NULL, FALSE);

	// create socket queue
	if (listen(socket_fd, 5) < 0)
		return socket_fault(4, NULL, NULL, FALSE);

	main_channel = g_io_channel_unix_new(socket_fd);
	if (!main_channel) return socket_fault(12, NULL, NULL, FALSE);
	// set the channel to read binary file
	if (g_io_channel_set_encoding(main_channel, NULL, &error) == G_IO_STATUS_ERROR)
		return socket_fault(9, error, main_channel, FALSE);
	// if any request from client, call accept_socket().
	// the channel will be clean when main_quit()
	if ( ! g_io_add_watch ( main_channel, G_IO_IN|G_IO_HUP, (GIOFunc)accept_socket, NULL))
		return socket_fault(5, NULL, NULL, TRUE);

	return TRUE;
}

// it will return TRUE if successed
gboolean accept_socket(GIOChannel *source, GIOCondition condition, gpointer user_data)
{
#ifdef DEBUG
	g_debug("! Launch accept_socket() to accept the request from client !");
#endif

	GError *error = NULL;

	if (condition & G_IO_HUP)
		return socket_fault(6, NULL, NULL, TRUE);
	else
	{
		gint read_fd = accept(g_io_channel_unix_get_fd(source), NULL, NULL);
		if (read_fd < 0)
			return socket_fault(1, NULL, NULL, FALSE);
		if ( ! set_fd_non_block(&read_fd)) return FALSE;
		GIOChannel* channel = g_io_channel_unix_new(read_fd);
		if (!channel) return socket_fault(12, NULL, NULL, FALSE);
		// set the channel to read binary file
		if (g_io_channel_set_encoding(channel, NULL, &error) == G_IO_STATUS_ERROR)
			return socket_fault(9, error, channel, FALSE);
		// read the data that client sent.
		if ( ! g_io_add_watch(channel, G_IO_HUP, read_socket, NULL))
			return socket_fault(5, NULL, NULL, TRUE);
	}
	return TRUE;
}

// it will return TRUE if successed
gboolean read_socket(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
#ifdef DEBUG
	g_debug("! Launch read_socket() to read data !");
#endif

	GError *error = NULL;
	gchar *data, **datas;
	gsize len;
	gsize term;

	if (g_io_channel_read_line (channel, &data, &len, &term, &error) == G_IO_STATUS_ERROR)
		return socket_fault(7, error, channel, TRUE);
	// g_debug("Read %u bytes from Lilyterm socket: '%s'", len, data);
	if (len > 0)
	{
		// get data: VTE_CJK_WIDTH argc argv environment.
		// clear '\n' at the end of data[]
		data[len-1] = 0;

		// VTE_CJK_WIDTH
		datas = g_strsplit(data, " ", 2);
		gint argc = atoi(datas[0]);
		// g_debug("Get argc =%d", argc);
		
		// g_debug("Get Other data = %s", datas[1]);
		gchar **argv = g_strsplit(datas[1], " ", argc+1);
		g_strfreev(datas);
		
		// g_debug("Get environment = %s", argv[argc]);
		gchar **environment = g_strsplit(argv[argc], " ", -1);

		g_free(argv[argc]);
		argv[argc] = NULL;

		//g_debug("Final:\n");
		//g_debug("\tVTE_CJK_WIDTH =%d\n", VTE_CJK_WIDTH);
		//g_debug("\targc =%d\n", argc);
		//
		//g_debug("\targv=\n");
		//gint i = 0;
		//while (argv[i]!=NULL)
		//	g_debug("\t\t%d: %s\n", i-1, argv[i++]);

		//g_debug("\tenvironment =\n");
		//i = 0;
		//while (environment[i]!=NULL)
		//	g_debug("\t\t%d, %s\n", i-1, environment[i++]);

		new_window(argc, argv, environment, 0, NULL);
		g_free(data);
	}
	clear_channel(channel, TRUE);
	// return FALSE means this connection is finish.
	return FALSE;
}

// it will always return FALSE
gboolean socket_fault(int i, GError *error, GIOChannel* channel, gboolean unref)
{
#ifdef DEBUG
	g_debug("! Launch socket_fault() to show the error message !");
#endif

	switch (i)
	{
		case 1:
			g_warning("Error when create %s socket(%s): %s",
				  PACKAGE_NAME, address.sun_path, g_strerror (errno));
			break;
		case 2:
			g_warning("Can NOT connect to a existing %s socket!", PACKAGE_NAME);
			break;
		case 3:
			g_warning("Can NOT bind on the socket!");
			break;
		case 4:
			g_warning("Can NOT listen on the socket!");
			break;
		case 5:
			g_warning("Can not watch on the socket!");
			break;
		case 6:
			g_warning("Error when accepting client request via socket!");
			break;
		case 7:
			g_warning("Error when reading the data client sent via socket!");
			break;
		case 8:
			g_warning("Error when running fcntl command on socket!");
			break;
		case 9:
			g_warning("Error when setting the encoding of channel : %s", error->message);
			break;
		case 10:
			g_warning("Error when shutdowning a channel : %s", error->message);
			break;
		case 11:
			g_warning("Error when writing data to the channel : %s", error->message);
			break;
		case 12:
			g_warning("Can NOT create a channel for this socket");
			break;
		case 13:
			g_warning("Error when flushing the write buffer for the channel : %s", error->message);
			break;
	}
	if (error != NULL) g_clear_error (&error);
	if (channel != NULL) clear_channel(channel, unref);
	return FALSE;
}

// it will return TRUE if successed
gboolean clear_channel(GIOChannel* channel, gboolean unref)
{
#ifdef DEBUG
	g_debug("! Launch clear_channel() to clear channel data !");
#endif

	GError *error = NULL;

	if (unref)
		g_io_channel_unref(channel);
	if (g_io_channel_shutdown(channel, TRUE, &error) == G_IO_STATUS_ERROR)
		return socket_fault(10, error, NULL, FALSE);
	return TRUE;
}

void shutdown_socket_server()
{
#ifdef DEBUG
	g_debug("! Launch shutdown_socket_server() to shutdown the LilyTerm socket server!");
#endif

	clear_channel(main_channel, TRUE);
	unlink(address.sun_path);
}
