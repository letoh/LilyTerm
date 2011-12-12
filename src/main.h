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

#ifndef MAIN_H
#define MAIN_H

#include <gtk/gtk.h>
// for g_get_tmp_dir()
#include <glib.h>
// for L10n
#include <locale.h>
#include <glib/gi18n.h>
// for exit()
#include <stdlib.h>
// for strcmp()
#include <string.h>
// for socket()
#include <sys/un.h>
#include <sys/socket.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>

#include "window.h"

void command_option(int argc, char *argv[]);
GString *got_help_message();
gboolean init_socket_data();
gboolean set_fd_non_block(gint *fd);
gboolean query_socket();
gboolean send_socket(int argc, char *argv[]);
gboolean init_socket_server();
gboolean accept_socket(GIOChannel *source, GIOCondition condition, gpointer user_data);
gboolean read_socket(GIOChannel *channel, GIOCondition condition, gpointer user_data);
gboolean socket_fault(int i, GError *error, GIOChannel* channel, gboolean unref);
gboolean clear_channel(GIOChannel* channel, gboolean unref);
void shutdown_socket_server();
#endif
