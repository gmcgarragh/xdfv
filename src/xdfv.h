/*******************************************************************************
 *
 *    Copyright (C) 2015-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
 *
 *    This source code is licensed under the GNU General Public License (GPL),
 *    Version 3.  See the file COPYING for more details.
 *
 ******************************************************************************/

#ifndef XDFV_H
#define XDFV_H

#define LN 1024

typedef unsigned char      uchar;
typedef unsigned short     ushort;
typedef unsigned int       uint;
typedef unsigned long      ulong;
typedef unsigned long long ulonglong;

/* Min/Max macros */
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) ((a) > (b) ? (a) : (b))

extern const char *program_name;
extern const char *PROGRAM_NAME;

#endif /* XDFV_H */
