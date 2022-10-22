%{
/**
 * SPDX-License-Identifier: GPL-3.0-or-later
 *
 * Parse a string into an internal timestamp.
 *
 * This file is based on gnulib parse-datetime.y-dd7a871 with
 * the other gnulib dependencies removed for use in util-linux.
 *
 * Copyright (C) 1999-2000, 2002-2017 Free Software Foundation, Inc.
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation; either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 *
 * Originally written by Steven M. Bellovin <smb@research.att.com> while
 * at the University of North Carolina at Chapel Hill.  Later tweaked by
 * a couple of people on Usenet.  Completely overhauled by Rich $alz
 * <rsalz@bbn.com> and Jim Berets <jberets@bbn.com> in August, 1990.
 *
 * Modified by Paul Eggert <eggert@twinsun.com> in August 1999 to do
 * the right thing about local DST.  Also modified by Paul Eggert
 * <eggert@cs.ucla.edu> in February 2004 to support
 * nanosecond-resolution timestamps, and in October 2004 to support
 * TZ strings in dates.
 */

/**
 * FIXME: Check for arithmetic overflow in all cases, not just
 * some of them.
 */

#include <sys/time.h>
#include <time.h>

#include "c.h"
#include "timeutils.h"
#include "hwclock.h"

/**
 * There's no need to extend the stack, so there's no need to involve
 * alloca.
 */
#define YYSTACK_USE_ALLOCA 0

/**
 * Tell Bison how much stack space is needed.  20 should be plenty for
 * this grammar, which is not right recursive.  Beware setting it too
 * high, since that might cause problems on machines whose
 * implementations have lame stack-overflow checking.
 */
#define YYMAXDEPTH 20
#define YYINITDEPTH YYMAXDEPTH

/**
 * Since the code of parse-datetime.y is not included in the Emacs executable
 * itself, there is no need to #define static in this file.  Even if
 * the code were included in the Emacs executable, it probably
 * wouldn't do any harm to #undef it here; this will only cause
 * problems if we try to write to a static variable, which I don't
 * think this code needs to do.
 */
#ifdef emacs
# undef static
#endif

#include <inttypes.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>


#include <stdarg.h>
#include "cctype.h"
#include "nls.h"

/**
 * Bison's skeleton tests _STDLIB_H, while some stdlib.h headers
 * use _STDLIB_H_ as witness.  Map the latter to the one bison uses.
 * FIXME: this is temporary.  Remove when we have a mechanism to ensure
 * that the version we're using is fixed, too.
 */
#ifdef _STDLIB_H_
# undef _STDLIB_H
# define _STDLIB_H 1
#endif

/**
 * Shift A right by B bits portably, by dividing A by 2**B and
 * truncating towards minus infinity.  A and B should be free of side
 * effects, and B should be in the range 0 <= B <= INT_BITS - 2, where
 * INT_BITS is the number of useful bits in an int.  GNU code can
 * assume that INT_BITS is at least 32.
 *
 * ISO C99 says that A >> B is implementation-defined if A < 0.  Some
 * implementations (e.g., UNICOS 9.0 on a Cray Y-MP EL) don't shift
 * right in the usual way when A < 0, so SHR falls back on division if
 * ordinary A >> B doesn't seem to be the usual signed shift.
 */
#define SHR(a, b) \
	(-1 >> 1 == -1 \
	 ? (a) >> (b) \
	 : (a) / (1 << (b)) - ((a) % (1 << (b)) < 0))

#define TM_YEAR_BASE 1900

#define HOUR(x) ((x) * 60)

#define STREQ(a, b) (strcmp (a, b) == 0)

/**
 * Convert a possibly-signed character to an unsigned character.  This is
 * a bit safer than casting to unsigned char, since it catches some type
 * errors that the cast doesn't.
 */
static unsigned char to_uchar (char ch) { return ch; }

/**
 * FIXME: It also assumes that signed integer overflow silently wraps around,
 * but this is not true any more with recent versions of GCC 4.
 */

/**
 * An integer value, and the number of digits in its textual
 * representation.
 */
typedef struct {
	int negative;
	intmax_t value;
	size_t digits;
} textint;

/* An entry in the lexical lookup table. */
typedef struct {
	char const *name;
	int type;
	int value;
} table;

/* Meridian: am, pm, or 24-hour style. */
enum { MERam, MERpm, MER24 };

enum { BILLION = 1000000000, LOG10_BILLION = 9 };

/* Relative year, month, day, hour, minutes, seconds, and nanoseconds. */
typedef struct {
	intmax_t year;
	intmax_t month;
	intmax_t day;
	intmax_t hour;
	intmax_t minutes;
	time_t seconds;
	long ns;
} relative_time;

#if HAVE_COMPOUND_LITERALS
# define RELATIVE_TIME_0 ((relative_time) { 0, 0, 0, 0, 0, 0, 0 })
#else
static relative_time const RELATIVE_TIME_0;
#endif

/* Information passed to and from the parser. */
typedef struct {
	/* The input string remaining to be parsed. */
	const char *input;

	/* N, if this is the Nth Tuesday. */
	intmax_t day_ordinal;

	/* Day of week; Sunday is 0. */
	int day_number;

	/* tm_isdst flag for the local zone. */
	int local_isdst;

	/* Time zone, in minutes east of UTC. */
	int time_zone;

	/* Style used for time. */
	int meridian;

	/* Gregorian year, month, day, hour, minutes, seconds, and ns. */
	textint year;
	intmax_t month;
	intmax_t day;
	intmax_t hour;
	intmax_t minutes;
	struct timespec seconds; /* includes nanoseconds */

	/* Relative year, month, day, hour, minutes, seconds, and ns. */
	relative_time rel;

	/* Presence or counts of some nonterminals parsed so far. */
	int timespec_seen;
	int rels_seen;
	size_t dates_seen;
	size_t days_seen;
	size_t local_zones_seen;
	size_t dsts_seen;
	size_t times_seen;
	size_t zones_seen;

	/* Table of local time zone abbreviations, null terminated. */
	table local_time_zone_table[3];
} parser_control;

union YYSTYPE;
static int yylex (union YYSTYPE *, parser_control *);
static int yyerror (parser_control const *, char const *);
static int time_zone_hhmm (parser_control *, textint, textint);

/**
 * Extract into *PC any date and time info from a string of digits
 * of the form e.g., YYYYMMDD, YYMMDD, HHMM, HH (and sometimes YYY,
 * YYYY, ...).
 */
static void digits_to_date_time(parser_control *pc, textint text_int)
{
	if (pc->dates_seen && ! pc->year.digits
	    && ! pc->rels_seen && (pc->times_seen || 2 < text_int.digits)) {
		pc->year = text_int;
	} else {
		if (4 < text_int.digits) {
			pc->dates_seen++;
			pc->day = text_int.value % 100;
			pc->month = (text_int.value / 100) % 100;
			pc->year.value = text_int.value / 10000;
			pc->year.digits = text_int.digits - 4;
		} else {
			pc->times_seen++;
			if (text_int.digits <= 2) {
				pc->hour = text_int.value;
				pc->minutes = 0;
			}
			else {
				pc->hour = text_int.value / 100;
				pc->minutes = text_int.value % 100;
			}
			pc->seconds.tv_sec = 0;
			pc->seconds.tv_nsec = 0;
			pc->meridian = MER24;
		}
	}
}

/* Increment PC->rel by FACTOR * REL (FACTOR is 1 or -1). */
static void apply_relative_time(parser_control *pc, relative_time rel,
				int factor)
{
	pc->rel.ns += factor * rel.ns;
	pc->rel.seconds += factor * rel.seconds;
	pc->rel.minutes += factor * rel.minutes;
	pc->rel.hour += factor * rel.hour;
	pc->rel.day += factor * rel.day;
	pc->rel.month += factor * rel.month;
	pc->rel.year += factor * rel.year;
	pc->rels_seen = 1;
}

/* Set PC-> hour, minutes, seconds and nanoseconds members from arguments. */
static void
set_hhmmss(parser_control *pc, intmax_t hour, intmax_t minutes,
	   time_t sec, long nsec)
{
	pc->hour = hour;
	pc->minutes = minutes;
	pc->seconds.tv_sec = sec;
	pc->seconds.tv_nsec = nsec;
}

%}

/**
 * We want a reentrant parser, even if the TZ manipulation and the calls to
 * localtime and gmtime are not reentrant.
 */
%define api.pure
%parse-param { parser_control *pc }
%lex-param { parser_control *pc }

/* This grammar has 31 shift/reduce conflicts. */
%expect 31

%union {
	intmax_t intval;
	textint textintval;
	struct timespec timespec;
	relative_time rel;
}

%token <intval> tAGO
%token tDST

%token tYEAR_UNIT tMONTH_UNIT tHOUR_UNIT tMINUTE_UNIT tSEC_UNIT
%token <intval> tDAY_UNIT tDAY_SHIFT

%token <intval> tDAY tDAYZONE tLOCAL_ZONE tMERIDIAN
%token <intval> tMONTH tORDINAL tZONE

%token <textintval> tSNUMBER tUNUMBER
%token <timespec> tSDECIMAL_NUMBER tUDECIMAL_NUMBER

%type <textintval> o_colon_minutes
%type <timespec> seconds signed_seconds unsigned_seconds

%type <rel> relunit relunit_snumber dayshift

%%

spec:
	  timespec
	| items
;

timespec:
	  '@' seconds {
		pc->seconds = $2;
		pc->timespec_seen = 1;
	  }
;

items:
	  /* empty */
	| items item
;

item:
	  datetime {
		pc->times_seen++; pc->dates_seen++;
	  }
	| time {
		pc->times_seen++;
	  }
	| local_zone {
		pc->local_zones_seen++;
	  }
	| zone {
		pc->zones_seen++;
	  }
	| date {
		pc->dates_seen++;
	  }
	| day {
		pc->days_seen++;
	  }
	| rel
	| number
	| hybrid
;

datetime:
	  iso_8601_datetime
;

iso_8601_datetime:
	  iso_8601_date 'T' iso_8601_time
;

time:
	  tUNUMBER tMERIDIAN {
		set_hhmmss (pc, $1.value, 0, 0, 0);
		pc->meridian = $2;
	  }
	| tUNUMBER ':' tUNUMBER tMERIDIAN {
		set_hhmmss (pc, $1.value, $3.value, 0, 0);
		pc->meridian = $4;
	  }
	| tUNUMBER ':' tUNUMBER ':' unsigned_seconds tMERIDIAN {
		set_hhmmss (pc, $1.value, $3.value, $5.tv_sec, $5.tv_nsec);
		pc->meridian = $6;
	  }
	| iso_8601_time
;

iso_8601_time:
	  tUNUMBER zone_offset {
		set_hhmmss (pc, $1.value, 0, 0, 0);
		pc->meridian = MER24;
	  }
	| tUNUMBER ':' tUNUMBER o_zone_offset {
		set_hhmmss (pc, $1.value, $3.value, 0, 0);
		pc->meridian = MER24;
	  }
	| tUNUMBER ':' tUNUMBER ':' unsigned_seconds o_zone_offset {
		set_hhmmss (pc, $1.value, $3.value, $5.tv_sec, $5.tv_nsec);
		pc->meridian = MER24;
	  }
;

o_zone_offset:
	/* empty */
	| zone_offset
;

zone_offset:
	  tSNUMBER o_colon_minutes {
		pc->zones_seen++;
		if (! time_zone_hhmm (pc, $1, $2)) YYABORT;
	  }
;

/**
 * Local zone strings only affect DST setting,
 * and only take affect if the current TZ setting is relevant.
 *
 * Example 1:
 * 'EEST' is parsed as tLOCAL_ZONE, as it relates to the effective TZ:
 *      TZ=Europe/Helsinki date -d '2016-12-30 EEST'
 *
 * Example 2:
 * 'EEST' is parsed as 'zone' (TZ=+03:00):
 *       TZ=Asia/Tokyo ./src/date --debug -d '2011-06-11 EEST'
 *
 * This is implemented by probing the next three calendar quarters
 * of the effective timezone and looking for DST changes -
 * if found, the timezone name (EEST) is inserted into
 * the lexical lookup table with type tLOCAL_ZONE.
 * (Search for 'quarter' comment in  'parse_date').
 */
local_zone:
	  tLOCAL_ZONE {
		pc->local_isdst = $1;
		pc->dsts_seen += (0 < $1);
	  }
	| tLOCAL_ZONE tDST {
		pc->local_isdst = 1;
		pc->dsts_seen += (0 < $1) + 1;
	  }
;

/**
 * Note 'T' is a special case, as it is used as the separator in ISO
 * 8601 date and time of day representation.
 */
zone:
	  tZONE {
		pc->time_zone = $1;
	  }
	| 'T' {
		pc->time_zone = HOUR(7);
	  }
	| tZONE relunit_snumber {
		pc->time_zone = $1;
		apply_relative_time (pc, $2, 1);
	  }
	| 'T' relunit_snumber {
		pc->time_zone = HOUR(7);
		apply_relative_time (pc, $2, 1);
	  }
	| tZONE tSNUMBER o_colon_minutes {
		if (! time_zone_hhmm (pc, $2, $3)) YYABORT;
		pc->time_zone += $1;
	  }
	| tDAYZONE {
		pc->time_zone = $1 + 60;
	  }
	| tZONE tDST {
		pc->time_zone = $1 + 60;
	  }
;

day:
	  tDAY {
		pc->day_ordinal = 0;
		pc->day_number = $1;
	  }
	| tDAY ',' {
		pc->day_ordinal = 0;
		pc->day_number = $1;
	  }
	| tORDINAL tDAY {
		pc->day_ordinal = $1;
		pc->day_number = $2;
	  }
	| tUNUMBER tDAY {
		pc->day_ordinal = $1.value;
		pc->day_number = $2;
	  }
;

date:
	  tUNUMBER '/' tUNUMBER {
		pc->month = $1.value;
		pc->day = $3.value;
	  }
	| tUNUMBER '/' tUNUMBER '/' tUNUMBER {
	/**
	 * Interpret as YYYY/MM/DD if the first value has 4 or more digits,
	 * otherwise as MM/DD/YY.
	 * The goal in recognizing YYYY/MM/DD is solely to support legacy
	 * machine-generated dates like those in an RCS log listing.  If
	 * you want portability, use the ISO 8601 format.
	 */
		if (4 <= $1.digits) {
			pc->year = $1;
			pc->month = $3.value;
			pc->day = $5.value;
		} else {
			pc->month = $1.value;
			pc->day = $3.value;
			pc->year = $5;
		}
	  }
	| tUNUMBER tMONTH tSNUMBER {
		/* e.g. 17-JUN-1992. */
		pc->day = $1.value;
		pc->month = $2;
		pc->year.value = -$3.value;
		pc->year.digits = $3.digits;
	  }
	| tMONTH tSNUMBER tSNUMBER {
		/* e.g. JUN-17-1992. */
		pc->month = $1;
		pc->day = -$2.value;
		pc->year.value = -$3.value;
		pc->year.digits = $3.digits;
	  }
	| tMONTH tUNUMBER {
		pc->month = $1;
		pc->day = $2.value;
	  }
	| tMONTH tUNUMBER ',' tUNUMBER {
		pc->month = $1;
		pc->day = $2.value;
		pc->year = $4;
	  }
	| tUNUMBER tMONTH {
		pc->day = $1.value;
		pc->month = $2;
	  }
	| tUNUMBER tMONTH tUNUMBER {
		pc->day = $1.value;
		pc->month = $2;
		pc->year = $3;
	  }
	| iso_8601_date
;

iso_8601_date:
	  tUNUMBER tSNUMBER tSNUMBER {
		/* ISO 8601 format.YYYY-MM-DD. */
		pc->year = $1;
		pc->month = -$2.value;
		pc->day = -$3.value;
	  }
;

rel:
	  relunit tAGO
		{ apply_relative_time (pc, $1, $2); }
	| relunit
		{ apply_relative_time (pc, $1, 1); }
	| dayshift
		{ apply_relative_time (pc, $1, 1); }
;

relunit:
	  tORDINAL tYEAR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.year = $1; }
	| tUNUMBER tYEAR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.year = $1.value; }
	| tYEAR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.year = 1; }
	| tORDINAL tMONTH_UNIT
		{ $$ = RELATIVE_TIME_0; $$.month = $1; }
	| tUNUMBER tMONTH_UNIT
		{ $$ = RELATIVE_TIME_0; $$.month = $1.value; }
	| tMONTH_UNIT
		{ $$ = RELATIVE_TIME_0; $$.month = 1; }
	| tORDINAL tDAY_UNIT
		{ $$ = RELATIVE_TIME_0; $$.day = $1 * $2; }
	| tUNUMBER tDAY_UNIT
		{ $$ = RELATIVE_TIME_0; $$.day = $1.value * $2; }
	| tDAY_UNIT
		{ $$ = RELATIVE_TIME_0; $$.day = $1; }
	| tORDINAL tHOUR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.hour = $1; }
	| tUNUMBER tHOUR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.hour = $1.value; }
	| tHOUR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.hour = 1; }
	| tORDINAL tMINUTE_UNIT
		{ $$ = RELATIVE_TIME_0; $$.minutes = $1; }
	| tUNUMBER tMINUTE_UNIT
		{ $$ = RELATIVE_TIME_0; $$.minutes = $1.value; }
	| tMINUTE_UNIT
		{ $$ = RELATIVE_TIME_0; $$.minutes = 1; }
	| tORDINAL tSEC_UNIT
		{ $$ = RELATIVE_TIME_0; $$.seconds = $1; }
	| tUNUMBER tSEC_UNIT
		{ $$ = RELATIVE_TIME_0; $$.seconds = $1.value; }
	| tSDECIMAL_NUMBER tSEC_UNIT {
		$$ = RELATIVE_TIME_0;
		$$.seconds = $1.tv_sec;
		$$.ns = $1.tv_nsec;
	  }
	| tUDECIMAL_NUMBER tSEC_UNIT {
		$$ = RELATIVE_TIME_0;
		$$.seconds = $1.tv_sec;
		$$.ns = $1.tv_nsec;
	  }
	| tSEC_UNIT
		{ $$ = RELATIVE_TIME_0; $$.seconds = 1; }
	| relunit_snumber
;

relunit_snumber:
	  tSNUMBER tYEAR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.year = $1.value; }
	| tSNUMBER tMONTH_UNIT
		{ $$ = RELATIVE_TIME_0; $$.month = $1.value; }
	| tSNUMBER tDAY_UNIT
		{ $$ = RELATIVE_TIME_0; $$.day = $1.value * $2; }
	| tSNUMBER tHOUR_UNIT
		{ $$ = RELATIVE_TIME_0; $$.hour = $1.value; }
	| tSNUMBER tMINUTE_UNIT
		{ $$ = RELATIVE_TIME_0; $$.minutes = $1.value; }
	| tSNUMBER tSEC_UNIT
		{ $$ = RELATIVE_TIME_0; $$.seconds = $1.value; }
;

dayshift:
	  tDAY_SHIFT
		{ $$ = RELATIVE_TIME_0; $$.day = $1; }
;

seconds: signed_seconds | unsigned_seconds;

signed_seconds:
	  tSDECIMAL_NUMBER
	| tSNUMBER
		{ $$.tv_sec = $1.value; $$.tv_nsec = 0; }
;

unsigned_seconds:
	  tUDECIMAL_NUMBER
	| tUNUMBER
		{ $$.tv_sec = $1.value; $$.tv_nsec = 0; }
;

number:
	  tUNUMBER
		{ digits_to_date_time (pc, $1); }
;

hybrid:
	  tUNUMBER relunit_snumber {
		/**
		 * Hybrid all-digit and relative offset, so that we accept e.g.,
		 * "YYYYMMDD +N days" as well as "YYYYMMDD N days".
		 */
		digits_to_date_time (pc, $1);
		apply_relative_time (pc, $2, 1);
	  }
;

o_colon_minutes:
	  /* empty */
		{ $$.value = $$.digits = 0; }
	| ':' tUNUMBER {
		$$ = $2;
	  }
;

%%

static table const meridian_table[] = {
	{ "AM",   tMERIDIAN, MERam },
	{ "A.M.", tMERIDIAN, MERam },
	{ "PM",   tMERIDIAN, MERpm },
	{ "P.M.", tMERIDIAN, MERpm },
	{ NULL, 0, 0 }
};

static table const dst_table[] = {
	{ "DST", tDST, 0 }
};

static table const month_and_day_table[] = {
	{ "JANUARY",  tMONTH,  1 },
	{ "FEBRUARY", tMONTH,  2 },
	{ "MARCH",    tMONTH,  3 },
	{ "APRIL",    tMONTH,  4 },
	{ "MAY",      tMONTH,  5 },
	{ "JUNE",     tMONTH,  6 },
	{ "JULY",     tMONTH,  7 },
	{ "AUGUST",   tMONTH,  8 },
	{ "SEPTEMBER",tMONTH,  9 },
	{ "SEPT",     tMONTH,  9 },
	{ "OCTOBER",  tMONTH, 10 },
	{ "NOVEMBER", tMONTH, 11 },
	{ "DECEMBER", tMONTH, 12 },
	{ "SUNDAY",   tDAY,    0 },
	{ "MONDAY",   tDAY,    1 },
	{ "TUESDAY",  tDAY,    2 },
	{ "TUES",     tDAY,    2 },
	{ "WEDNESDAY",tDAY,    3 },
	{ "WEDNES",   tDAY,    3 },
	{ "THURSDAY", tDAY,    4 },
	{ "THUR",     tDAY,    4 },
	{ "THURS",    tDAY,    4 },
	{ "FRIDAY",   tDAY,    5 },
	{ "SATURDAY", tDAY,    6 },
	{ NULL, 0, 0 }
};

static table const time_units_table[] = {
	{ "YEAR",     tYEAR_UNIT,      1 },
	{ "MONTH",    tMONTH_UNIT,     1 },
	{ "FORTNIGHT",tDAY_UNIT,      14 },
	{ "WEEK",     tDAY_UNIT,       7 },
	{ "DAY",      tDAY_UNIT,       1 },
	{ "HOUR",     tHOUR_UNIT,      1 },
	{ "MINUTE",   tMINUTE_UNIT,    1 },
	{ "MIN",      tMINUTE_UNIT,    1 },
	{ "SECOND",   tSEC_UNIT,       1 },
	{ "SEC",      tSEC_UNIT,       1 },
	{ NULL, 0, 0 }
};

/* Assorted relative-time words. */
static table const relative_time_table[] = {
	{ "TOMORROW", tDAY_SHIFT,      1 },
	{ "YESTERDAY",tDAY_SHIFT,     -1 },
	{ "TODAY",    tDAY_SHIFT,      0 },
	{ "NOW",      tDAY_SHIFT,      0 },
	{ "LAST",     tORDINAL,       -1 },
	{ "THIS",     tORDINAL,        0 },
	{ "NEXT",     tORDINAL,        1 },
	{ "FIRST",    tORDINAL,        1 },
      /*{ "SECOND",   tORDINAL,        2 }, */
	{ "THIRD",    tORDINAL,        3 },
	{ "FOURTH",   tORDINAL,        4 },
	{ "FIFTH",    tORDINAL,        5 },
	{ "SIXTH",    tORDINAL,        6 },
	{ "SEVENTH",  tORDINAL,        7 },
	{ "EIGHTH",   tORDINAL,        8 },
	{ "NINTH",    tORDINAL,        9 },
	{ "TENTH",    tORDINAL,       10 },
	{ "ELEVENTH", tORDINAL,       11 },
	{ "TWELFTH",  tORDINAL,       12 },
	{ "AGO",      tAGO,           -1 },
	{ "HENCE",    tAGO,            1 },
	{ NULL, 0, 0 }
};

/**
 * The universal time zone table.  These labels can be used even for
 * timestamps that would not otherwise be valid, e.g., GMT timestamps
 * in London during summer.
 */
static table const universal_time_zone_table[] = {
	{ "GMT",      tZONE,     HOUR ( 0) }, /* Greenwich Mean */
	{ "UT",       tZONE,     HOUR ( 0) }, /* Universal (Coordinated) */
	{ "UTC",      tZONE,     HOUR ( 0) },
	{ NULL, 0, 0 }
};

/**
 * The time zone table.  This table is necessarily incomplete, as time
 * zone abbreviations are ambiguous; e.g. Australians interpret "EST"
 * as Eastern time in Australia, not as US Eastern Standard Time.
 * You cannot rely on parse_date to handle arbitrary time zone
 * abbreviations; use numeric abbreviations like "-0500" instead.
 */
static table const time_zone_table[] = {
	{ "WET",      tZONE,     HOUR ( 0) }, /* Western European */
	{ "WEST",     tDAYZONE,  HOUR ( 0) }, /* Western European Summer */
	{ "BST",      tDAYZONE,  HOUR ( 0) }, /* British Summer */
	{ "ART",      tZONE,    -HOUR ( 3) }, /* Argentina */
	{ "BRT",      tZONE,    -HOUR ( 3) }, /* Brazil */
	{ "BRST",     tDAYZONE, -HOUR ( 3) }, /* Brazil Summer */
	{ "NST",      tZONE,   -(HOUR ( 3) + 30) },   /* Newfoundland Standard */
	{ "NDT",      tDAYZONE,-(HOUR ( 3) + 30) },   /* Newfoundland Daylight */
	{ "AST",      tZONE,    -HOUR ( 4) }, /* Atlantic Standard */
	{ "ADT",      tDAYZONE, -HOUR ( 4) }, /* Atlantic Daylight */
	{ "CLT",      tZONE,    -HOUR ( 4) }, /* Chile */
	{ "CLST",     tDAYZONE, -HOUR ( 4) }, /* Chile Summer */
	{ "EST",      tZONE,    -HOUR ( 5) }, /* Eastern Standard */
	{ "EDT",      tDAYZONE, -HOUR ( 5) }, /* Eastern Daylight */
	{ "CST",      tZONE,    -HOUR ( 6) }, /* Central Standard */
	{ "CDT",      tDAYZONE, -HOUR ( 6) }, /* Central Daylight */
	{ "MST",      tZONE,    -HOUR ( 7) }, /* Mountain Standard */
	{ "MDT",      tDAYZONE, -HOUR ( 7) }, /* Mountain Daylight */
	{ "PST",      tZONE,    -HOUR ( 8) }, /* Pacific Standard */
	{ "PDT",      tDAYZONE, -HOUR ( 8) }, /* Pacific Daylight */
	{ "AKST",     tZONE,    -HOUR ( 9) }, /* Alaska Standard */
	{ "AKDT",     tDAYZONE, -HOUR ( 9) }, /* Alaska Daylight */
	{ "HST",      tZONE,    -HOUR (10) }, /* Hawaii Standard */
	{ "HAST",     tZONE,    -HOUR (10) }, /* Hawaii-Aleutian Standard */
	{ "HADT",     tDAYZONE, -HOUR (10) }, /* Hawaii-Aleutian Daylight */
	{ "SST",      tZONE,    -HOUR (12) }, /* Samoa Standard */
	{ "WAT",      tZONE,     HOUR ( 1) }, /* West Africa */
	{ "CET",      tZONE,     HOUR ( 1) }, /* Central European */
	{ "CEST",     tDAYZONE,  HOUR ( 1) }, /* Central European Summer */
	{ "MET",      tZONE,     HOUR ( 1) }, /* Middle European */
	{ "MEZ",      tZONE,     HOUR ( 1) }, /* Middle European */
	{ "MEST",     tDAYZONE,  HOUR ( 1) }, /* Middle European Summer */
	{ "MESZ",     tDAYZONE,  HOUR ( 1) }, /* Middle European Summer */
	{ "EET",      tZONE,     HOUR ( 2) }, /* Eastern European */
	{ "EEST",     tDAYZONE,  HOUR ( 2) }, /* Eastern European Summer */
	{ "CAT",      tZONE,     HOUR ( 2) }, /* Central Africa */
	{ "SAST",     tZONE,     HOUR ( 2) }, /* South Africa Standard */
	{ "EAT",      tZONE,     HOUR ( 3) }, /* East Africa */
	{ "MSK",      tZONE,     HOUR ( 3) }, /* Moscow */
	{ "MSD",      tDAYZONE,  HOUR ( 3) }, /* Moscow Daylight */
	{ "IST",      tZONE,    (HOUR ( 5) + 30) },   /* India Standard */
	{ "SGT",      tZONE,     HOUR ( 8) }, /* Singapore */
	{ "KST",      tZONE,     HOUR ( 9) }, /* Korea Standard */
	{ "JST",      tZONE,     HOUR ( 9) }, /* Japan Standard */
	{ "GST",      tZONE,     HOUR (10) }, /* Guam Standard */
	{ "NZST",     tZONE,     HOUR (12) }, /* New Zealand Standard */
	{ "NZDT",     tDAYZONE,  HOUR (12) }, /* New Zealand Daylight */
	{ NULL, 0, 0 }
};

/**
 * Military time zone table.
 *
 * Note 'T' is a special case, as it is used as the separator in ISO
 * 8601 date and time of day representation.
 */
static table const military_table[] = {
	{ "A", tZONE, -HOUR ( 1) },
	{ "B", tZONE, -HOUR ( 2) },
	{ "C", tZONE, -HOUR ( 3) },
	{ "D", tZONE, -HOUR ( 4) },
	{ "E", tZONE, -HOUR ( 5) },
	{ "F", tZONE, -HOUR ( 6) },
	{ "G", tZONE, -HOUR ( 7) },
	{ "H", tZONE, -HOUR ( 8) },
	{ "I", tZONE, -HOUR ( 9) },
	{ "K", tZONE, -HOUR (10) },
	{ "L", tZONE, -HOUR (11) },
	{ "M", tZONE, -HOUR (12) },
	{ "N", tZONE,  HOUR ( 1) },
	{ "O", tZONE,  HOUR ( 2) },
	{ "P", tZONE,  HOUR ( 3) },
	{ "Q", tZONE,  HOUR ( 4) },
	{ "R", tZONE,  HOUR ( 5) },
	{ "S", tZONE,  HOUR ( 6) },
	{ "T", 'T',    0 },
	{ "U", tZONE,  HOUR ( 8) },
	{ "V", tZONE,  HOUR ( 9) },
	{ "W", tZONE,  HOUR (10) },
	{ "X", tZONE,  HOUR (11) },
	{ "Y", tZONE,  HOUR (12) },
	{ "Z", tZONE,  HOUR ( 0) },
	{ NULL, 0, 0 }
};

/**
 * Convert a time offset expressed as HH:MM or HHMM into an integer count of
 * minutes.  If hh is more than 2 digits then it is of the form HHMM and must be
 * delimited; in that case 'mm' is required to be absent.  Otherwise, hh and mm
 * are used ('mm' contains digits that were prefixed with a colon).
 *
 * POSIX TZ and ISO 8601 both define the maximum offset as 24:59. POSIX also
 * allows seconds, but currently the parser rejects them. Both require minutes
 * to be zero padded (2 digits). ISO requires hours to be zero padded, POSIX
 * does not, either is accepted; which means an invalid ISO offset could pass.
 */

static int time_zone_hhmm(parser_control *pc, textint hh, textint mm)
{
	int h, m;

	if (hh.digits > 2 && hh.digits < 5 && mm.digits == 0) {
		h = hh.value / 100;
		m = hh.value % 100;
	} else if (hh.digits < 3 && (mm.digits == 0 || mm.digits == 2)) {
		h = hh.value;
		m = hh.negative ? -mm.value : mm.value;
	} else
		return 0;

	if (abs(h) > 24 || abs(m) > 59)
		return 0;

	pc->time_zone =  h * 60 + m;
	return 1;
}

static int to_hour(intmax_t hours, int meridian)
{
	switch (meridian) {
	default: /* Pacify GCC. */
	case MER24:
		return 0 <= hours && hours < 24 ? hours : -1;
	case MERam:
		return 0 < hours && hours < 12 ? hours : hours == 12 ? 0 : -1;
	case MERpm:
		return 0 < hours && hours < 12 ? hours + 12 : hours == 12 ? 12 : -1;
	}
}

static long int to_year(textint textyear)
{
	intmax_t year = textyear.value;

	if (year < 0)
		year = -year;

	/**
	 * XPG4 suggests that years 00-68 map to 2000-2068, and
	 * years 69-99 map to 1969-1999.
	 */
	else if (textyear.digits == 2)
			year += year < 69 ? 2000 : 1900;

	return year;
}

static table const * lookup_zone(parser_control const *pc, char const *name)
{
	table const *tp;

	for (tp = universal_time_zone_table; tp->name; tp++)
		if (strcmp (name, tp->name) == 0)
			return tp;

	/**
	 * Try local zone abbreviations before those in time_zone_table, as
	 * the local ones are more likely to be right.
	 */
	for (tp = pc->local_time_zone_table; tp->name; tp++)
		if (strcmp (name, tp->name) == 0)
			return tp;

	for (tp = time_zone_table; tp->name; tp++)
		if (strcmp (name, tp->name) == 0)
			return tp;

	return NULL;
}

#if ! HAVE_TM_GMTOFF
/**
 * Yield the difference between *A and *B,
 * measured in seconds, ignoring leap seconds.
 * The body of this function is taken directly from the GNU C Library;
 * see src/strftime.c.
 */
static int tm_diff(struct tm const *a, struct tm const *b)
{
	/**
	 * Compute intervening leap days correctly even if year is negative.
	 * Take care to avoid int overflow in leap day calculations.
	 */
	int a4 = SHR (a->tm_year, 2) + SHR (TM_YEAR_BASE, 2) - ! (a->tm_year & 3);
	int b4 = SHR (b->tm_year, 2) + SHR (TM_YEAR_BASE, 2) - ! (b->tm_year & 3);
	int a100 = a4 / 25 - (a4 % 25 < 0);
	int b100 = b4 / 25 - (b4 % 25 < 0);
	int a400 = SHR (a100, 2);
	int b400 = SHR (b100, 2);
	int intervening_leap_days = (a4 - b4) - (a100 - b100) + (a400 - b400);
	int years = a->tm_year - b->tm_year;
	int days = (365 * years + intervening_leap_days
			 + (a->tm_yday - b->tm_yday));
	return (60 * (60 * (24 * days + (a->tm_hour - b->tm_hour))
		+ (a->tm_min - b->tm_min))
		+ (a->tm_sec - b->tm_sec));
}
#endif /* ! HAVE_TM_GMTOFF */

static table const * lookup_word(parser_control const *pc, char *word)
{
	char *p;
	char *q;
	size_t wordlen;
	table const *tp;
	int period_found;
	int abbrev;

	/* Make it uppercase. */
	for (p = word; *p; p++)
		*p = c_toupper (to_uchar (*p));

	for (tp = meridian_table; tp->name; tp++)
		if (strcmp (word, tp->name) == 0)
			return tp;

	/* See if we have an abbreviation for a month. */
	wordlen = strlen (word);
	abbrev = wordlen == 3 || (wordlen == 4 && word[3] == '.');

	for (tp = month_and_day_table; tp->name; tp++)
		if ((abbrev ? strncmp (word, tp->name, 3) :
		     strcmp (word, tp->name)) == 0)
			return tp;

	if ((tp = lookup_zone (pc, word)))
		return tp;

	if (strcmp (word, dst_table[0].name) == 0)
		return dst_table;

	for (tp = time_units_table; tp->name; tp++)
		if (strcmp (word, tp->name) == 0)
			return tp;

	/* Strip off any plural and try the units table again. */
	if (word[wordlen - 1] == 'S') {
		word[wordlen - 1] = '\0';
		for (tp = time_units_table; tp->name; tp++)
			if (strcmp (word, tp->name) == 0)
				return tp;
		word[wordlen - 1] = 'S'; /* For "this" in relative_time_table. */
	}

	for (tp = relative_time_table; tp->name; tp++)
		if (strcmp (word, tp->name) == 0)
			return tp;

	/* Military time zones. */
	if (wordlen == 1)
		for (tp = military_table; tp->name; tp++)
			if (word[0] == tp->name[0])
				return tp;

	/* Drop out any periods and try the time zone table again. */
	for (period_found = 0, p = q = word; (*p = *q); q++)
		if (*q == '.')
			period_found = 1;
		else
			p++;
	if (period_found && (tp = lookup_zone (pc, word)))
		return tp;

	return NULL;
}

static int yylex (union YYSTYPE *lvalp, parser_control *pc)
{
	unsigned char c;
	size_t count;

	for (;;) {
		while (c = *pc->input, c_isspace (c))
			pc->input++;

		if (c_isdigit (c) || c == '-' || c == '+') {
			char const *p;
			int sign;
			uintmax_t value;
			if (c == '-' || c == '+') {
				sign = c == '-' ? -1 : 1;
				while (c = *++pc->input, c_isspace (c))
					continue;
				if (! c_isdigit (c))
					/* skip the '-' sign */
					continue;
			} else
				sign = 0;
			p = pc->input;
			for (value = 0; ; value *= 10) {
				uintmax_t value1 = value + (c - '0');
				if (value1 < value)
					return '?';
				value = value1;
				c = *++p;
				if (! c_isdigit (c))
					break;
				if (UINTMAX_MAX / 10 < value)
					return '?';
			}
			if ((c == '.' || c == ',') && c_isdigit (p[1])) {
				time_t s;
				long ns;
				int digits;
				uintmax_t value1;

				/* Check for overflow when converting value to
				 * time_t.
				 */
				if (sign < 0) {
					s = - value;
					if (0 < s)
						return '?';
					value1 = -s;
				} else {
					s = value;
					if (s < 0)
						return '?';
					value1 = s;
				}
				if (value != value1)
					return '?';

				/* Accumulate fraction, to ns precision. */
				p++;
				ns = *p++ - '0';
				for (digits = 2;
				     digits <= LOG10_BILLION; digits++) {
					ns *= 10;
					if (c_isdigit (*p))
						ns += *p++ - '0';
				}

				/* Skip excess digits, truncating toward
				 * -Infinity.
				 */
				if (sign < 0)
					for (; c_isdigit (*p); p++)
						if (*p != '0') {
							ns++;
							break;
						}
				while (c_isdigit (*p))
					p++;

				/* Adjust to the timespec convention, which is
				 * that tv_nsec is always a positive offset even
				 * if tv_sec is negative.
				 */
				if (sign < 0 && ns) {
					s--;
					if (! (s < 0))
						return '?';
					ns = BILLION - ns;
				}

				lvalp->timespec.tv_sec = s;
				lvalp->timespec.tv_nsec = ns;
				pc->input = p;
				return
				  sign ? tSDECIMAL_NUMBER : tUDECIMAL_NUMBER;
			} else {
				lvalp->textintval.negative = sign < 0;
				if (sign < 0) {
					lvalp->textintval.value = - value;
					if (0 < lvalp->textintval.value)
						return '?';
				} else {
					lvalp->textintval.value = value;
					if (lvalp->textintval.value < 0)
						return '?';
				}
				lvalp->textintval.digits = p - pc->input;
				pc->input = p;
				return sign ? tSNUMBER : tUNUMBER;
			}
		}

		if (c_isalpha (c)) {
			char buff[20];
			char *p = buff;
			table const *tp;

			do {
				if (p < buff + sizeof buff - 1)
				*p++ = c;
				c = *++pc->input;
			}
			while (c_isalpha (c) || c == '.');

			*p = '\0';
			tp = lookup_word (pc, buff);
			if (! tp) {
				return '?';
			}
			lvalp->intval = tp->value;
			return tp->type;
		}

		if (c != '(')
			return to_uchar (*pc->input++);

		count = 0;
		do {
			c = *pc->input++;
			if (c == '\0')
				return c;
			if (c == '(')
				count++;
			else if (c == ')')
				count--;
		}
		while (count != 0);
	}
}

/* Do nothing if the parser reports an error. */
static int yyerror(parser_control const *pc __attribute__((__unused__)),
		   char const *s __attribute__((__unused__)))
{
	return 0;
}

/**
 * If *TM0 is the old and *TM1 is the new value of a struct tm after
 * passing it to mktime, return 1 if it's OK that mktime returned T.
 * It's not OK if *TM0 has out-of-range members.
 */

static int mktime_ok(struct tm const *tm0, struct tm const *tm1, time_t t)
{
	if (t == (time_t) -1) {
		/**
		 * Guard against falsely reporting an error when parsing a
		 * timestamp that happens to equal (time_t) -1, on a host that
		 * supports such a timestamp.
		 */
		tm1 = localtime (&t);
		if (!tm1)
			return 0;
	}

	return ! ((tm0->tm_sec ^ tm1->tm_sec)
		  | (tm0->tm_min ^ tm1->tm_min)
		  | (tm0->tm_hour ^ tm1->tm_hour)
		  | (tm0->tm_mday ^ tm1->tm_mday)
		  | (tm0->tm_mon ^ tm1->tm_mon)
		  | (tm0->tm_year ^ tm1->tm_year));
}

/**
 * A reasonable upper bound for the size of ordinary TZ strings.
 * Use heap allocation if TZ's length exceeds this.
 */
enum { TZBUFSIZE = 100 };

/**
 * Return a copy of TZ, stored in TZBUF if it fits, and heap-allocated
 * otherwise.
 */
static char * get_tz(char tzbuf[TZBUFSIZE])
{
	char *tz = getenv ("TZ");
	if (tz) {
		size_t tzsize = strlen (tz) + 1;
		tz = (tzsize <= TZBUFSIZE
		      ? memcpy (tzbuf, tz, tzsize)
		      : strdup (tz));
	}
	return tz;
}

/**
 * Parse a date/time string, storing the resulting time value into *result.
 * The string itself is pointed to by *p.  Return 1 if successful.
 * *p can be an incomplete or relative time specification; if so, use
 * *now as the basis for the returned time.
 */
int parse_date(struct timespec *result, char const *p,
		   struct timespec const *now)
{
	time_t Start;
	intmax_t Start_ns;
	struct tm const *tmp;
	struct tm tm;
	struct tm tm0;
	parser_control pc;
	struct timespec gettime_buffer;
	unsigned char c;
	int tz_was_altered = 0;
	char *tz0 = NULL;
	char tz0buf[TZBUFSIZE];
	int ok = 1;
	struct timeval tv;

	if (! now) {
		gettimeofday (&tv, NULL);
		gettime_buffer.tv_sec = tv.tv_sec;
		gettime_buffer.tv_nsec = tv.tv_usec * 1000;
		now = &gettime_buffer;
	}

	Start = now->tv_sec;
	Start_ns = now->tv_nsec;

	tmp = localtime (&now->tv_sec);
	if (! tmp)
		return 0;

	while (c = *p, c_isspace (c))
		p++;

	if (strncmp (p, "TZ=\"", 4) == 0) {
		char const *tzbase = p + 4;
		size_t tzsize = 1;
		char const *s;

		for (s = tzbase; *s; s++, tzsize++)
			if (*s == '\\') {
				s++;
				if (! (*s == '\\' || *s == '"'))
					break;
			} else if (*s == '"') {
				char *z;
				char *tz1 = NULL;
				char tz1buf[TZBUFSIZE] = { '\0' };
				int large_tz = TZBUFSIZE < tzsize;
				int setenv_ok;

				tz0 = get_tz (tz0buf);
				if (!tz0)
					goto fail;

				if (large_tz) {
					z = tz1 = malloc (tzsize);
					if (!tz1)
						goto fail;
				} else
					z = tz1 = tz1buf;

				for (s = tzbase; *s != '"'; s++)
					*z++ = *(s += *s == '\\');
				*z = '\0';
				setenv_ok = setenv ("TZ", tz1, 1) == 0;
				if (large_tz)
					free (tz1);
				if (!setenv_ok)
					goto fail;
				tz_was_altered = 1;

				p = s + 1;
				while (c = *p, c_isspace (c))
					p++;

				break;
			}
	}

	/**
	 * As documented, be careful to treat the empty string just like
	 * a date string of "0".  Without this, an empty string would be
	 * declared invalid when parsed during a DST transition.
	 */
	if (*p == '\0')
		p = "0";

	pc.input = p;
	pc.year.value = tmp->tm_year;
	pc.year.value += TM_YEAR_BASE;
	pc.year.digits = 0;
	pc.month = tmp->tm_mon + 1;
	pc.day = tmp->tm_mday;
	pc.hour = tmp->tm_hour;
	pc.minutes = tmp->tm_min;
	pc.seconds.tv_sec = tmp->tm_sec;
	pc.seconds.tv_nsec = Start_ns;
	tm.tm_isdst = tmp->tm_isdst;

	pc.meridian = MER24;
	pc.rel = RELATIVE_TIME_0;
	pc.timespec_seen = 0;
	pc.rels_seen = 0;
	pc.dates_seen = 0;
	pc.days_seen = 0;
	pc.times_seen = 0;
	pc.local_zones_seen = 0;
	pc.dsts_seen = 0;
	pc.zones_seen = 0;

#if HAVE_STRUCT_TM_TM_ZONE
	pc.local_time_zone_table[0].name = tmp->tm_zone;
	pc.local_time_zone_table[0].type = tLOCAL_ZONE;
	pc.local_time_zone_table[0].value = tmp->tm_isdst;
	pc.local_time_zone_table[1].name = NULL;

	/**
	 * Probe the names used in the next three calendar quarters, looking
	 * for a tm_isdst different from the one we already have.
	 */
	{
		int quarter;
		for (quarter = 1; quarter <= 3; quarter++) {
			time_t probe = Start + quarter * (90 * 24 * 60 * 60);
			struct tm const *probe_tm = localtime (&probe);
			if (probe_tm && probe_tm->tm_zone
				&& probe_tm->tm_isdst
				!= pc.local_time_zone_table[0].value) {
					{
					  pc.local_time_zone_table[1].name
					  = probe_tm->tm_zone;
					  pc.local_time_zone_table[1].type
					  = tLOCAL_ZONE;
					  pc.local_time_zone_table[1].value
					  = probe_tm->tm_isdst;
					  pc.local_time_zone_table[2].name
					  = NULL;
					}
				break;
			}
		}
	}
#else
#if HAVE_TZNAME
	{
# if !HAVE_DECL_TZNAME
		extern char *tzname[];
# endif
		int i;
		for (i = 0; i < 2; i++) {
			pc.local_time_zone_table[i].name = tzname[i];
			pc.local_time_zone_table[i].type = tLOCAL_ZONE;
			pc.local_time_zone_table[i].value = i;
		}
		pc.local_time_zone_table[i].name = NULL;
	}
#else
	pc.local_time_zone_table[0].name = NULL;
#endif
#endif

	if (pc.local_time_zone_table[0].name && pc.local_time_zone_table[1].name
	    && ! strcmp (pc.local_time_zone_table[0].name,
			 pc.local_time_zone_table[1].name)) {
		/**
		 * This locale uses the same abbreviation for standard and
		 * daylight times.  So if we see that abbreviation, we don't
		 * know whether it's daylight time.
		 */
		pc.local_time_zone_table[0].value = -1;
		pc.local_time_zone_table[1].name = NULL;
	}

	if (yyparse (&pc) != 0) {
		goto fail;
	}

	if (pc.timespec_seen)
		*result = pc.seconds;
	else {
		if (1 < (pc.times_seen | pc.dates_seen | pc.days_seen
			 | pc.dsts_seen
			 | (pc.local_zones_seen + pc.zones_seen))) {
			goto fail;
		}

		tm.tm_year = to_year (pc.year) - TM_YEAR_BASE;
		tm.tm_mon = pc.month - 1;
		tm.tm_mday = pc.day;
		if (pc.times_seen || (pc.rels_seen &&
				      ! pc.dates_seen && ! pc.days_seen)) {
			tm.tm_hour = to_hour (pc.hour, pc.meridian);
			if (tm.tm_hour < 0) {
				goto fail;
			}
			tm.tm_min = pc.minutes;
			tm.tm_sec = pc.seconds.tv_sec;
		} else {
			tm.tm_hour = tm.tm_min = tm.tm_sec = 0;
			pc.seconds.tv_nsec = 0;
		}

		/**
		 * Let mktime deduce tm_isdst if we have an absolute timestamp.
		 */
		if (pc.dates_seen | pc.days_seen | pc.times_seen)
			tm.tm_isdst = -1;

		/**
		 * But if the input explicitly specifies local time with or
		 * without DST, give mktime that information.
		 */
		if (pc.local_zones_seen)
			tm.tm_isdst = pc.local_isdst;

		tm0 = tm;

		Start = mktime (&tm);

		if (! mktime_ok (&tm0, &tm, Start)) {
			if (! pc.zones_seen) {
				goto fail;
			} else {
				/** Guard against falsely reporting errors near
				 * the time_t boundaries when parsing times in
				 * other time zones.  For example, suppose the
				 * input string "1969-12-31 23:00:00 -0100", the
				 * current time zone is 8 hours ahead of UTC,
				 * and the min time_t value is 1970-01-01
				 * 00:00:00 UTC.  Then the min localtime value
				 * is 1970-01-01 08:00:00, and mktime will
				 * therefore fail on 1969-12-31 23:00:00.  To
				 * work around the problem, set the time zone to
				 * 1 hour behind UTC temporarily by setting
				 * TZ="XXX1:00" and try mktime again.
				 */

				intmax_t time_zone = pc.time_zone;

				intmax_t abs_time_zone = time_zone < 0
					 ? - time_zone : time_zone;

				intmax_t abs_time_zone_hour
					 = abs_time_zone / 60;

				int abs_time_zone_min = abs_time_zone % 60;

				char tz1buf[sizeof "XXX+0:00"
					    + sizeof pc.time_zone
					    * CHAR_BIT / 3];

				if (!tz_was_altered)
					tz0 = get_tz (tz0buf);
				sprintf (tz1buf, "XXX%s%jd:%02d",
					 &"-"[time_zone < 0],
					 abs_time_zone_hour,
					 abs_time_zone_min);
				if (setenv ("TZ", tz1buf, 1) != 0) {
					goto fail;
				}
				tz_was_altered = 1;
				tm = tm0;
				Start = mktime (&tm);
				if (! mktime_ok (&tm0, &tm, Start)) {
					goto fail;
				}
			}
		}

		if (pc.days_seen && ! pc.dates_seen) {
			tm.tm_mday += ((pc.day_number - tm.tm_wday + 7) % 7 + 7
				       * (pc.day_ordinal
					  - (0 < pc.day_ordinal
					     && tm.tm_wday != pc.day_number)));
			tm.tm_isdst = -1;
			Start = mktime (&tm);
			if (Start == (time_t) -1) {
				goto fail;
			}
		}
		/* Add relative date. */
		if (pc.rel.year | pc.rel.month | pc.rel.day) {
			int year = tm.tm_year + pc.rel.year;
			int month = tm.tm_mon + pc.rel.month;
			int day = tm.tm_mday + pc.rel.day;
			if (((year < tm.tm_year) ^ (pc.rel.year < 0))
				| ((month < tm.tm_mon) ^ (pc.rel.month < 0))
				| ((day < tm.tm_mday) ^ (pc.rel.day < 0))) {
				goto fail;
			}
			tm.tm_year = year;
			tm.tm_mon = month;
			tm.tm_mday = day;
			tm.tm_hour = tm0.tm_hour;
			tm.tm_min = tm0.tm_min;
			tm.tm_sec = tm0.tm_sec;
			tm.tm_isdst = tm0.tm_isdst;
			Start = mktime (&tm);
			if (Start == (time_t) -1) {
				goto fail;
			}
		}

		/**
		 * The only "output" of this if-block is an updated Start value,
		 * so this block must follow others that clobber Start.
		 */
		if (pc.zones_seen) {
			intmax_t delta = pc.time_zone * 60;
			time_t t1;
#ifdef HAVE_TM_GMTOFF
			delta -= tm.tm_gmtoff;
#else
			time_t t = Start;
			struct tm const *gmt = gmtime (&t);
			if (! gmt) {
				goto fail;
			}
			delta -= tm_diff (&tm, gmt);
#endif
			t1 = Start - delta;
			if ((Start < t1) != (delta < 0)) {
				goto fail;  /* time_t overflow */
			}
			Start = t1;
		}

		/**
		 * Add relative hours, minutes, and seconds.  On hosts that
		 * support leap seconds, ignore the possibility of leap seconds;
		 * e.g., "+ 10 minutes" adds 600 seconds, even if one of them is
		 * a leap second.  Typically this is not what the user wants,
		 * but it's too hard to do it the other way, because the time
		 * zone indicator must be applied before relative times, and if
		 * mktime is applied again the time zone will be lost.
		 */
		intmax_t sum_ns = pc.seconds.tv_nsec + pc.rel.ns;
		intmax_t normalized_ns = (sum_ns % BILLION + BILLION) % BILLION;
		time_t t0 = Start;
		intmax_t d1 = 60 * 60 * pc.rel.hour;
		time_t t1 = t0 + d1;
		intmax_t d2 = 60 * pc.rel.minutes;
		time_t t2 = t1 + d2;
		time_t d3 = pc.rel.seconds;
		time_t t3 = t2 + d3;
		intmax_t d4 = (sum_ns - normalized_ns) / BILLION;
		time_t t4 = t3 + d4;
		time_t t5 = t4;

		if ((d1 / (60 * 60) ^ pc.rel.hour)
		    | (d2 / 60 ^ pc.rel.minutes)
		    | ((t1 < t0) ^ (d1 < 0))
		    | ((t2 < t1) ^ (d2 < 0))
		    | ((t3 < t2) ^ (d3 < 0))
		    | ((t4 < t3) ^ (d4 < 0))
		    | (t5 != t4)) {
			goto fail;
		}
		result->tv_sec = t5;
		result->tv_nsec = normalized_ns;
	}

	goto done;

	fail:
		ok = 0;
	done:
		if (tz_was_altered)
			ok &= (tz0 ? setenv ("TZ", tz0, 1)
				   : unsetenv ("TZ")) == 0;
		if (tz0 != tz0buf)
			free (tz0);
		return ok;
}
