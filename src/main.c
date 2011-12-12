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

#include "main.h"

// for using in socket
gboolean single_process = TRUE;
gint socket_fd = 0;
struct sockaddr_un address = {0};
int address_len = 0;
GIOChannel *main_channel = NULL;
GtkClipboard *pclipboard;
// gchar *command_line_path;
gchar *SYSTEM_VTE_CJK_WIDTH_STR;

//		  
// single_process ----------------(N)------------------------------------> new_window() ------>	shutdown_socket_serve()
//	|				^					^		  unlink
// init_socket_data()			|					|		+ clear_channel()
//   socket_fd -------------------(N)---|					|		    g_io_channel_shutdown
//   set_fd_non_block ------------(N)---/					|		    (g_io_channel_unref)
//	|									|
// query_socket()								|
//   connect ---------------------(N)-------> init_socket_server() -------------|
//	|				^	unlink				|
// send_socket()			|	bind -----------------------(N)-|
// * g_io_channel_unix_new -------(N)---|	listen ---------------------(N)-|
//   g_io_channel_set_encoding ---(N)---|     * g_io_channel_unix_new ------(N)-|
//   g_io_channel_set_buffered		|	g_io_channel_set_encoding --(N)-|
//   g_io_channel_write_chars ----(N)---|	g_io_add_watch -------------(N)-/
//   g_io_channel_flush ----------(N)---/	  |
// + clear_channel()				  `---- accept_socket()
//     g_io_channel_shutdown				  (condition)
//     (g_io_channel_unref)				  accept
//	|						  set_fd_non_block
//   exit()						* g_io_channel_unix_new
//							  g_io_channel_set_encoding
//							  g_io_add_watch
//							    |
//							    `-- read_socket()
//								  g_io_channel_read_line
//								    new_window()
//								+ clear_channel()
//								    g_io_channel_shutdown
//								    (g_io_channel_unref)
//

int main( int   argc,
	  char *argv[] )
{
	// command_line_path = argv[0];

	// g_debug ("argc = %d", argc);
	// print_array ("argv", argv);
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
			// trying to connect to an existing LilyTerm
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
	gchar *environ_str = convert_array_to_string(environ, '\t');
	// g_debug("Got environ_str (in main.c) = %s", environ_str);
	pclipboard = gtk_clipboard_get(GDK_SELECTION_CLIPBOARD);
	SYSTEM_VTE_CJK_WIDTH_STR = (char *) g_getenv("VTE_CJK_WIDTH");
	// g_debug ("Got SYSTEM_VTE_CJK_WIDTH_STR = %s", SYSTEM_VTE_CJK_WIDTH_STR);
	// FIXME: signal(SIGCHLD, SIG_IGN);
	// The first window of LilyTerm
	
	if ( new_window(argc,
			argv,
			environ_str,
			NULL,
			SYSTEM_VTE_CJK_WIDTH_STR,
			NULL,
			(gchar *)get_encoding_from_locale(),
			NULL,
			NULL) )
	{
		// The argv of "main" LilyTerm can't be free.
		// Set it to NULL here to avoid double_free().
		argv=NULL;
	
		// g_debug("Start to run LilyTerm");
		gtk_main();
	}
	g_free(environ_str);
	return 0;
}

void command_option(int   argc,
		    char *argv[])
{
#ifdef DETAIL
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
			
			// LilyTerm will exit immediately, so we don't need to free the got_help_message... XD
			g_print("\n%s\n", got_help_message()->str);

			// GString *help_msg = got_help_message();
			// g_print("\n%s\n", help_msg->str);
			// g_string_free (help_msg, TRUE);

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
#ifdef DETAIL
	g_debug("! Launch got_help_message()!");
#endif

	GString *help_message = g_string_new(NULL);
	gint j;
	const char * const *system_dirs = g_get_system_config_dirs();

	g_string_append_printf( help_message,
					_("%s is a libvte based X Terminal Emulator.\n\n"), PACKAGE_NAME);
	g_string_append( help_message,  _("Use -e/-x/--execute {Command} to run a command when start up."
					  " (Must be the final option).\n"));
	g_string_append( help_message,  _("Use -T/--title {title} to specify the window title.\n"));
	g_string_append( help_message,  _("Use -t/--tab {number} to open multi tabs when start up.\n"));
	g_string_append( help_message,  _("Use -d/--directory {directory} to specify the init directory when start up.\n"));
	g_string_append( help_message,  _("Use -s/--separate to run in separate process.\n"));
	g_string_append( help_message,  _("Use -v/--version to show the version information.\n"));
	g_string_append( help_message,  _("Use -p/--profile to got a profile sample.\n"));
	g_string_append_printf( help_message,
					_("Use -u/--user_profile {%s} to use a specified profile.\n\n"), RCFILE);
	for (j=0; system_dirs[j] != NULL; j++)
		g_string_append_printf( help_message,
					_("The %s system config is: %s/%s\n"), PACKAGE_NAME, system_dirs[j], RCFILE);
	// We should *NOT* free() the string returned from g_get_user_config_dir(),
	// but valgrind says it may cause memory leak...
	g_string_append_printf( help_message,
					_("And your %s profile is: %s/%s\n\n"),
					  PACKAGE_NAME, g_get_user_config_dir(), RCFILE);
	g_string_append( help_message,  _("Default shortcut key: (It may custom by editing user's profile)\n\n"));
	g_string_append( help_message,  _("  * <Ctrl><`>\t\tDisable/Enable hyperlinks, function keys and right click menu\n"));
	g_string_append( help_message,  _("  * <Ctrl><T/Q>\t\tAdd a New tab/Close current tab\n"));
	g_string_append( help_message,  _("\t\t\t(Using <Ctrl><D> or 'exit' to close tabs is recommended)\n"));
	g_string_append( help_message,  _("  * <Ctrl><E>\t\tRename current tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><PgUp/PgDn>\tSwitch to Prev/Next tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><Home/End>\tSwitch to First/Last tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><Left/Right>\tMove current page Forward/Backward\n"));
	g_string_append( help_message,  _("  * <Ctrl><Up/Down>\tMove current page to First/Last\n"));
	g_string_append( help_message,  _("  * <Ctrl><F1~F12>\tSwitch to 1st ~ 12th tab\n"));
	g_string_append( help_message,  _("  * <Ctrl><O>\t\tSelect all the text in the Vte Terminal box\n"));
	g_string_append( help_message,  _("  * <Ctrl><X/V>\t\tCopy the text to clipboard / Paste the text in clipboard\n"));
	g_string_append( help_message,  _("  * <Ctrl><+/-/Enter>\tIncrease/Decrease/Reset the font size of current tab\n"));
	g_string_append( help_message,  _("  * <Alt><F11/Enter>\tSwitch between fullwindow/unfullwindow and fullscreen/unfullscreen state\n"));
	g_string_append( help_message,  _("  * <Shift><Insert>\tPaste the text in primary clipboard\n"));
	g_string_append( help_message,  _("\t\t\t(i.e. Emulate a middle button mouse click to paste the text)\n\n"));
	g_string_append_printf( help_message,
					_("Please report bug to %s. Thank you for using %s!\n"),
								PACKAGE_BUGREPORT, PACKAGE_NAME);
	return help_message;
}

// it will return TRUE if init socket data successfully
gboolean init_socket_data()
{
#ifdef DETAIL
	g_debug("! Launch init_socket_data() to init LilyTerm socket!");
#endif
	GError *error = NULL;

	// clean data first
	bzero(&address, sizeof(address));
	// init the address of socket
	address.sun_family = AF_UNIX;
	// the max size of saddr.sun_path in Linux is 108!

	gchar *display = gdk_get_display();
#ifdef ENABLE_DEVELOP
	g_snprintf(address.sun_path, 108, "%s/.%s_dev_%s%s",
		   g_get_tmp_dir() ,PACKAGE, g_get_user_name(), display);
#else
	g_snprintf(address.sun_path, 108, "%s/.%s_%s%s",
		   g_get_tmp_dir() ,PACKAGE, g_get_user_name(), display);
#endif
	// g_debug("The socket file is %s", address.sun_path);
	address_len = sizeof(address);
#ifdef DETAIL
	g_debug("* free display %p (%s) in :nit_socket_data()", display, display);
#endif
	g_free(display);

	// init socket
	socket_fd = socket(AF_UNIX, SOCK_STREAM, 0);
	if (socket_fd < 0)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(1, error, NULL, FALSE);

	// set this socket is non-block
	return set_fd_non_block(&socket_fd);
}

// it will return TRUE if successed
gboolean set_fd_non_block(gint *fd)
{
#ifdef DETAIL
	g_debug("! Launch set_fd_non_block()!");
#endif
	GError *error = NULL;
	gint flags = fcntl(*fd, F_GETFL, 0);
	if (fcntl(*fd, F_SETFL, O_NONBLOCK|flags) < 0)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(8, error, NULL, FALSE);
	return TRUE;
}

// it will return TRUE if successed
gboolean query_socket()
{
#ifdef DETAIL
	g_debug("! Launch query_socket() to connect to an existing LilyTerm !");
#endif
	GError *error = NULL;

	if (connect(socket_fd, (struct sockaddr *)&address, address_len) < 0)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(2, error, NULL, FALSE);
	return TRUE;
}

// it will return TRUE if successed
gboolean send_socket( int   argc,
		      char *argv[])
{
#ifdef DETAIL
	g_debug("! Launch send_socket() to send data to the exiting LilyTerm !");
#endif

	GError *error = NULL;
	gsize len;
	extern gchar **environ;

	//	      0			  1	   2   3		 4	     5
	// send data: SOCKET_DATA_VERSION ENCODING PWD VTE_CJK_WIDTH_STR environment argv.
	GString *arg_str = g_string_new (NULL);

	const gchar *VTE_CJK_WIDTH_STR = g_getenv("VTE_CJK_WIDTH");
	gchar *PWD = g_get_current_dir();
	// g_debug("PWD = %s", PWD);

	// VTE_CJK_WIDTH can't = NULL
	if (VTE_CJK_WIDTH_STR == NULL)
		VTE_CJK_WIDTH_STR = "";
	
	g_string_printf (arg_str,
			 "%s\x10%s\x10%s\x10%s\x10",
			 SOCKET_DATA_VERSION,
			 get_encoding_from_locale(),
			 PWD,
			 VTE_CJK_WIDTH_STR);
	gchar *environ_str = convert_array_to_string(environ, '\t');
	gchar *argv_str = convert_array_to_string(argv, '\x10');
	
	g_string_append_printf(arg_str, "%s\x10%s\n", environ_str, argv_str);
	// g_debug("Sent data: %s", arg_str->str);
	g_free(PWD);
	g_free(environ_str);
	g_free(argv_str);

	// write data!
	GIOChannel* channel = g_io_channel_unix_new(socket_fd);
	// main_channel is NULL, so that we don't need to launch clear_channel()
	if (!channel) return socket_fault(12, NULL, NULL, FALSE);
	// set the channel to read binary file
	if (g_io_channel_set_encoding(channel, NULL, &error) == G_IO_STATUS_ERROR)
		return socket_fault(9, error, channel, TRUE);
	g_io_channel_set_buffered (channel, FALSE);

	if (g_io_channel_write_chars(channel, arg_str->str, -1, &len, &error)== G_IO_STATUS_ERROR)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(11, error, channel, TRUE);
	// flush writing datas
	if (g_io_channel_flush(channel, &error) == G_IO_STATUS_ERROR)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(13, error, channel, TRUE);
	
#ifdef DETAIL
	g_debug("* free arg_str %p (%s) in send_socket()", arg_str, arg_str->str);
#endif
	g_string_free(arg_str, TRUE);

	// So far so good. shutdown and clear channel!
	clear_channel(channel, TRUE);

	// FIXME: sleep for 1 sec to wait the socket server. any better idea?
	sleep(1);

	return TRUE;
}

// it will return TRUE if succeed
gboolean init_socket_server()
{
#ifdef DETAIL
	g_debug("! Launch init_socket_server() to init a LilyTerm socket server !");
#endif

	GError *error = NULL;

	// clear the prev file
	if (address.sun_path) unlink(address.sun_path);

	// bind the socket on a file
	if (bind(socket_fd, (struct sockaddr *)&address, address_len) < 0)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(3, error, NULL, FALSE);

	// create socket queue
	if (listen(socket_fd, 5) < 0)
		// main_channel is NULL, so that we don't need to launch clear_channel()
		return socket_fault(4, error, NULL, FALSE);

	main_channel = g_io_channel_unix_new(socket_fd);
	if (!main_channel) return socket_fault(12, NULL, NULL, FALSE);
	// set the channel to read binary file
	if (g_io_channel_set_encoding(main_channel, NULL, &error) == G_IO_STATUS_ERROR)
		return socket_fault(9, error, main_channel, TRUE);

	// if any request from client, call accept_socket().
	// the channel will be clean when main_quit()
	if ( ! g_io_add_watch ( main_channel, G_IO_IN|G_IO_HUP, (GIOFunc)accept_socket, NULL))
		return socket_fault(5, error, NULL, TRUE);

	return TRUE;
}

// it will return TRUE if succeed
gboolean accept_socket(GIOChannel *source, GIOCondition condition, gpointer user_data)
{
#ifdef DETAIL
	g_debug("! Launch accept_socket() to accept the request from client !");
#endif

	GError *error = NULL;

	if (condition & G_IO_HUP)
		return socket_fault(6, error, source, FALSE);
	else
	{
		gint read_fd = accept(g_io_channel_unix_get_fd(source), NULL, NULL);
		if (read_fd < 0)
			return socket_fault(1, error, source, FALSE);
		if ( ! set_fd_non_block(&read_fd)) return FALSE;

		GIOChannel* channel = g_io_channel_unix_new(read_fd);
		// channel is NULL, so that we don't need to launch clear_channel()
		if (!channel) return socket_fault(12, NULL, channel, FALSE);
		// set the channel to read binary file
		if (g_io_channel_set_encoding(channel, NULL, &error) == G_IO_STATUS_ERROR)
			return socket_fault(9, error, channel, TRUE);
		
		// read the data that client sent.
		if ( ! g_io_add_watch(channel, G_IO_HUP, read_socket, NULL))
			return socket_fault(5, error, channel, TRUE);
	}
	return TRUE;
}

// it will return TRUE if succeed
gboolean read_socket(GIOChannel *channel, GIOCondition condition, gpointer user_data)
{
#ifdef DETAIL
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
		//	     0			 1	  2   3			4	     5
		// get data: SOCKET_DATA_VERSION ENCODING PWD VTE_CJK_WIDTH_STR environments argv.
		// clear '\n' at the end of data[]
		data[len-1] = 0;

		datas = g_strsplit(data, "\x10", 6);
		// g_debug("The SOCKET_DATA_VERSION = %s ,and the data sent via socket is %s",
		//	   SOCKET_DATA_VERSION, datas[0]);
		if (strcmp(SOCKET_DATA_VERSION, datas[0]))
			// The SOCKET_DATA_VERSION != the data sent via socket
			dialog(NULL, 21);
		else
		{
			gchar **argv = g_strsplit(datas[5], "\x10", -1);
			gint argc = 0;
			while (argv[argc])
				argc ++;

			// g_debug("Final:");
			// g_debug("\tencoding = %s", datas[1]);
			// g_debug("\tPWD = %s", datas[2]);
			// g_debug("\tVTE_CJK_WIDTH_STR = %s", datas[3]);
			// g_debug("\tenvironments = %s", datas[4]);
			// g_debug("\targc =%d", argc);
			// g_debug("\targv =%s", datas[5]);
			// print_array("\targv", argv);
			new_window(argc, argv, datas[4], datas[2], datas[3], NULL, datas[1], NULL, NULL);
#ifdef DETAIL
			g_debug("* free argv %p in read_socket()", argv);
#endif
			g_strfreev(argv);
		}
#ifdef DETAIL
		g_debug("* free datas %p in read_socket()", datas);
#endif
		g_strfreev(datas);
#ifdef DETAIL
		g_debug("* free data %p (%s) in read_socket()", data, data);
#endif
		data[len-1] = '\n';
		g_free(data);
	}
	clear_channel(channel, TRUE);
	// return FALSE means this connection is finish.
	return FALSE;
}

// it will always return FALSE
gboolean socket_fault(int i, GError *error, GIOChannel* channel, gboolean unref)
{
#ifdef DETAIL
	g_debug("! Launch socket_fault() to show the error message !");
#endif

	switch (i)
	{
		case 1:
			g_warning("Error when create %s socket(%s): %s",
				  PACKAGE_NAME, address.sun_path, g_strerror (errno));
			break;
		case 2:
			g_message("Can NOT connect to a existing %s socket!", PACKAGE_NAME);
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
	clear_channel(channel, unref);
	return FALSE;
}

// it will return TRUE if successed
gboolean clear_channel(GIOChannel* channel, gboolean unref)
{
#ifdef DETAIL
	g_debug("! Launch clear_channel() to clear channel data !");
#endif
	if (channel == NULL) return TRUE;

	gboolean return_value = TRUE;
	GError *error = NULL;

	if (g_io_channel_shutdown(channel, TRUE, &error) == G_IO_STATUS_ERROR)
		return_value = socket_fault(10, error, NULL, FALSE);
	
	if (return_value && unref)
	{
		g_io_channel_unref(channel);
		channel = NULL;
	}
		
	return return_value;
}

void shutdown_socket_server()
{
#ifdef DETAIL
	g_debug("! Launch shutdown_socket_server() to shutdown the LilyTerm socket server!");
#endif

	if (main_channel) clear_channel(main_channel, TRUE);
	if (address.sun_path) unlink(address.sun_path);
}

//// The returned GString should be freed when no longer needed.
//GString *convert_string_array_to_0x9_separated_gstring (gchar **string_array)
//{
//	gint i = 0;
//	GString *string = g_string_new("");
//
//	if (string_array!=NULL)
//	{
//		while (string_array[i]!=NULL)
//		{
//			// g_debug("%d: %s", i, string_array[i]);
//			g_string_append_printf(string, "%s\x10", string_array[i]);
//			i++;
//		}
//	}
//	return string ;
//}
