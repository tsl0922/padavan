/* Please use this file as a template when introducing new command to
 * util-linux package.
 * -- remove above */
/*
 * fixme-command-name - purpose of it
 *
 * Copyright (c) 20nn  Example Commercial, Inc
 * Written by Your Name <you@example.com>
 *
 * This program is free software; you can redistribute it and/or
 * modify it under the terms of the GNU General Public License as
 * published by the Free Software Foundation.
 *
 * This program is distributed in the hope that it would be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License along
 * with this program; if not, write to the Free Software Foundation, Inc.,
 * 51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
 */

#include <getopt.h>
#include <stdio.h>
#include <unistd.h>

#include "c.h"
#include "closestream.h"
#include "nls.h"

/*
 * FIXME: remove this comment.
 * Other usage() constants that are not demonstrated below:
 * USAGE_FUNCTIONS USAGE_COMMANDS USAGE_COLUMNS
 */
static void __attribute__((__noreturn__)) usage(void)
{
	fputs(USAGE_HEADER, stdout);
	printf(_(" %s [options] file...\n"), program_invocation_short_name);

	fputs(USAGE_SEPARATOR, stdout);
	puts(_("Short program description."));

	fputs(USAGE_OPTIONS, stdout);
	puts(_(" -n, --no-argument       option does not use argument"));
	puts(_("     --optional[=<arg>]  option argument is optional"));
	puts(_(" -r, --required <arg>    option requires an argument"));
	puts(_(" -z                      no long option"));
	puts(_("     --xyzzy             a long option only"));
	puts(_(" -e, --extremely-long-long-option\n"
	       "                         use next line for description when needed"));
	puts(_(" -l, --long-explanation  an example of very verbose, and chatty option\n"
	       "                           description on two, or multiple lines, where the\n"
	       "                           consecutive lines are intended by two spaces"));
	puts(_(" -f, --foobar            next option description resets indent"));
	fputs(USAGE_SEPARATOR, stdout);
	printf(USAGE_HELP_OPTIONS(25)); /* char offset to align option descriptions */
	printf(USAGE_MAN_TAIL("fixme-command-name(1)"));
	exit(EXIT_SUCCESS);
}

int main(int argc, char **argv)
{
	int c;

	enum {
		OPT_XYZZY = CHAR_MAX + 1,
		OPT_OPTIONAL	/* see howto-man-page.txt about short option */
	};
	static const struct option longopts[] = {
		{ "no-argument",                no_argument,       NULL, 'n'          },
		{ "optional",                   optional_argument, NULL, OPT_OPTIONAL },
		{ "required",                   required_argument, NULL, 'r'          },
		{ "extremely-long-long-option", no_argument,       NULL, 'e'          },
		{ "xyzzy",                      no_argument,       NULL, OPT_XYZZY    },
		{ "long-explanation",           no_argument,       NULL, 'l'          },
		{ "foobar",                     no_argument,       NULL, 'f'          },
		{ "version",                    no_argument,       NULL, 'V'          },
		{ "help",                       no_argument,       NULL, 'h'          },
		{ NULL, 0, NULL, 0 }
	};

	setlocale(LC_ALL, "");
	bindtextdomain(PACKAGE, LOCALEDIR);
	textdomain(PACKAGE);
	close_stdout_atexit();

	while ((c = getopt_long(argc, argv, "nr:zelfVh", longopts, NULL)) != -1)
		switch (c) {
		case 'n':
		case OPT_OPTIONAL:
		case 'r':
		case 'z':
		case OPT_XYZZY:
		case 'e':
		case 'l':
		case 'f':
			break;
		case 'V':
			print_version(EXIT_SUCCESS);
		case 'h':
			usage();
		default:
			errtryhelp(EXIT_FAILURE);
		}

	return EXIT_SUCCESS;
}
