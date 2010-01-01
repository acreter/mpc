/*
 * Copyright (C) 2008-2010 The Music Player Daemon Project
 * http://www.musicpd.org
 *
 * This program is free software; you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include "sticker.h"
#include "util.h"

#include <mpd/client.h>

#include <string.h>
#include <stdio.h>

#if defined(LIBMPDCLIENT_CHECK_VERSION)
#if LIBMPDCLIENT_CHECK_VERSION(2,1,0)

static void my_finishCommand(struct mpd_connection *conn) {
	if (!mpd_response_finish(conn))
		printErrorAndExit(conn);
}

static void
recv_print_stickers(struct mpd_connection *connection)
{
	struct mpd_pair *pair;

	while ((pair = mpd_recv_sticker(connection)) != NULL) {
		printf("%s=%s\n", pair->name, pair->value);
		mpd_return_sticker(connection, pair);
	}
}

static void
recv_print_stickers2(struct mpd_connection *connection)
{
	struct mpd_pair *pair;
	bool first = true;

	while ((pair = mpd_recv_pair(connection)) != NULL) {
		if (strcmp(pair->name, "file") == 0) {
			if (first)
				first = false;
			else
				putchar('\n');
			printf("%s:", pair->value);
		} else if (!first && strcmp(pair->name, "sticker") == 0)
			printf(" %s", pair->value);

		mpd_return_pair(connection, pair);
	}

	if (!first)
		putchar('\n');
}

int
cmd_sticker(int argc, char **argv, struct mpd_connection *conn)
{
	if(!strcmp(argv[1], "set"))
	{
		if(argc < 4)
		{
			fputs("syntax: sticker <uri> set <key> <value>\n", stderr);
			return 0;
		}

		mpd_send_sticker_set(conn, "song", argv[0], argv[2], argv[3]);
		my_finishCommand(conn);
		return 0;
	}
	else if(!strcmp(argv[1], "get"))
	{
		if(argc < 3)
		{
			fputs("syntax: sticker <uri> get <key>\n", stderr);
			return 0;
		}

		mpd_send_sticker_get(conn, "song", argv[0], argv[2]);
		recv_print_stickers(conn);
		my_finishCommand(conn);
	}
	else if(!strcmp(argv[1], "find"))
	{
		if(argc < 3)
		{
			fputs("syntax: sticker <dir> find <key>\n", stderr);
			return 0;
		}

		mpd_send_sticker_find(conn, "song", argv[0], argv[2]);
		recv_print_stickers2(conn);
		my_finishCommand(conn);
	} else if (strcmp(argv[1], "delete") == 0) {
		if(argc < 2)
		{
			fputs("syntax: sticker <uri> delete [key]\n", stderr);
			return 0;
		}

		mpd_send_sticker_delete(conn, "song", argv[0], argc > 2 ? argv[2] : NULL);
		my_finishCommand(conn);
	}
	else if(!strcmp(argv[1], "list"))
	{
		if(argc < 2)
		{
			fputs("syntax: sticker <uri> list\n", stderr);
			return 0;
		}

		mpd_send_sticker_list(conn, "song", argv[0]);
		recv_print_stickers(conn);
		my_finishCommand(conn);
	}
	else
		fputs("error: unknown command.\n", stderr);

	return 0;
}

#endif /* libmpdclient 2.1 */
#endif /* libmpdclient > 2.0 */
