/*******************************************************************************
**
**    Copyright (C) 1998-2018 Greg McGarragh <greg.mcgarragh@colostate.edu>
**
**    This source code is licensed under the GNU General Public License (GPL),
**    Version 3.  See the file COPYING for more details.
**
*******************************************************************************/

#ifndef GUTIL_H
#define GUTIL_H

#include <assert.h>
#ifdef __cplusplus
#include <complex>
#else
#include <complex.h>
#ifdef I
#undef I
#endif
#endif
#include <ctype.h>
#include <errno.h>
#include <float.h>
#include <limits.h>
#include <locale.h>
#include <math.h>
#include <setjmp.h>
#include <signal.h>
#include <stdarg.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifdef UNIX
#include <sys/stat.h>
#include <sys/types.h>
#endif

#ifdef __cplusplus
extern "C" {
#endif


/* Byte Order */
#define BYTE_ORDER_LE	0
#define BYTE_ORDER_BE	1

#define BYTE_ORDER_END	2


/* Native Size */
#define NATIVE_SIZE_16_BIT	1
#define NATIVE_SIZE_32_BIT	2
#define NATIVE_SIZE_64_BIT	3


/* Platforms */
#define AIX_CC		1
#define AIX_GCC		2
#define CYGWIN_GCC      3
#define DEC_UNIX_CC	4
#define DEC_UNIX_GCC	5
#define HP_CC		6
#define HP_GCC		7
#define IRIX_CC		8
#define IRIX_GCC	9
#define LINUX_GCC	10
#define LINUX_ICC	11
#define LINUX_PGCC	12
#define MACOSX_GCC	13
#define MACOSX_XLC	14
#define SOLARIS_CC	15
#define SOLARIS_GCC	16
#define WIN32_MSVC	17


/* Standard string sizes  */
#define NM	128
#define FN	256
#define LN	1024


/* Data types */

/*
Type		ILP64	LP64	LLP64
char		8	8	8
short		16	16	16
int		64	32	32
long		64	64	32
long long	64	64	64
pointer		64	64	64
*/

typedef unsigned char		uchar;
typedef unsigned short		ushort;
typedef unsigned int		uint;
typedef unsigned long		ulong;
typedef unsigned long long	ulonglong;


#if PLATFORM == WIN32_MSVC
#define LONG_LONG signed __int64
#define ULONG_LONG unsigned __int64
#else
#define LONG_LONG signed long long
#define ULONG_LONG unsigned long long
#endif


#ifndef __cplusplus
typedef float _Complex 		fcomplex;
typedef double _Complex 	dcomplex;
#else
typedef std::complex<float> 	fcomplex;
typedef std::complex<double> 	dcomplex;

#define _Complex_I		std::complex<double>(0., 1.)

#define cabs			abs
#define cexp			exp
#define cpow			pow
#define creal			real
#define cimag			imag
#define csqrt			sqrt
#endif


typedef signed char		int8_t;
typedef signed short		int16_t;
typedef unsigned char		uint8_t;
typedef unsigned short 		uint16_t;
#if   (NATIVE_SIZE == NATIVE_SIZE_16_BIT)
typedef signed long		int32_t;
typedef unsigned long		uint32_t;
#elif (NATIVE_SIZE == NATIVE_SIZE_32_BIT)
typedef signed int		int32_t;
typedef unsigned int		uint32_t;

#if   (PLATFORM    == WIN32_MSVC)
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
#else
typedef signed long long	int64_t;
typedef unsigned long long	uint64_t;
#endif

#else

typedef signed int		int32_t;
typedef unsigned int		uint32_t;
#if   (PLATFORM    == WIN32_MSVC)
typedef signed __int64		int64_t;
typedef unsigned __int64	uint64_t;
#else
typedef signed long		int64_t;
typedef unsigned long		uint64_t;
#endif

#endif

typedef float			float32_t;
typedef double			float64_t;


/* Data types ranges */

#ifndef INT8_MIN
#define INT8_MIN		CHAR_MIN
#define INT8_MAX		CHAR_MAX
#define UINT8_MAX		UCHAR_MAX
#endif

#ifndef INT16_MIN
#define INT16_MIN		SHRT_MIN
#define INT16_MAX		SHRT_MAX
#define UINT16_MAX		USHRT_MAX
#endif

#if   (NATIVE_SIZE == NATIVE_SIZE_16_BIT)
#ifndef INT32_MIN
#define INT32_MIN		LONG_MIN
#define INT32_MAX		LONG_MAX
#define UINT32_MAX		ULONG_MAX
#endif

#elif (NATIVE_SIZE == NATIVE_SIZE_32_BIT)
#ifndef INT32_MIN
#define INT32_MIN		INT_MIN
#define INT32_MAX		INT_MAX
#define UINT32_MAX		UINT_MAX
#endif

#if   (PLATFORM    == WIN32_MSVC)
#ifndef INT64_MIN
#define INT64_MAX		9223372036854775807i64
#define INT64_MIN		(-INT64_MAX - 1i64)
#define UINT64_MAX		18446744073709551615ui64
#endif
#else
#ifndef INT64_MIN
#define INT64_MAX		9223372036854775807LL
#define INT64_MIN		(-INT64_MAX - 1LL)
#define UINT64_MAX		18446744073709551615ULL
#endif
#endif

#else

#ifndef INT32_MIN
#define INT32_MIN		INT_MIN
#define INT32_MAX		INT_MAX
#define UINT32_MAX		UINT_MAX
#endif
#if   (PLATFORM    == WIN32_MSVC)
#ifndef INT64_MIN
#define INT64_MIN		_I64_MIN
#define INT64_MAX		_I64_MAX
#define UINT64_MAX		_UI64_MAX
#endif
#else
#ifndef INT64_MIN
#define INT64_MIN		LONG_MIN
#define INT64_MAX		LONG_MAX
#define UINT64_MAX		ULONG_MAX
#endif
#endif

#endif

#define FLOAT32_MIN		-FLT_MAX
#define FLOAT32_MAX		FLT_MAX
#define FLOAT64_MIN		-DBL_MAX
#define FLOAT64_MAX		DBL_MAX


/* Concatenation macros */
#ifdef CAT
#undef CAT
#endif
#define CAT(a, b) a##b

#ifdef XCAT
#undef XCAT
#endif
#define XCAT(x, y) CAT(x, y)


#ifdef ABS
#undef ABS
#endif
#define ABS(a) ((a) < (0) ? (-(a)) : (a))


/* Min/Max macros */
#ifdef MIN
#undef MIN
#endif
#define MIN(a,b) ((a) < (b) ? (a) : (b))

#ifdef MAX
#undef MAX
#endif
#define MAX(a,b) ((a) > (b) ? (a) : (b))


/* Array length macro */
#define LENGTH(NAME) (sizeof(NAME) / sizeof(NAME[0]))


#ifdef __cplusplus
}
#endif

#endif /* GUTIL_H */
