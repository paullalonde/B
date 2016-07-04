/*
 *	File:		ansi.h
 *				©2000-2002 Metrowerks Corporation.  All rights reserved.
 *
 *	Content:	wchar_t overrides for OS X
 * 
 */


#ifndef	_MW_ANSI_H_
#define	_MW_ANSI_H_

#if defined (__ppc__)
	#include <ppc/ansi.h>
#elif defined (__i386__)
	#include <i386/ansi.h>
#else
	#error architecture not supported
#endif


#ifndef	_BSD_WCHAR_T_DEFINED_
	#define	_BSD_WCHAR_T_DEFINED_
	
	#if !__cplusplus || !__option(wchar_type) 
		typedef int wchar_t;
		#undef __WCHAR_TYPE__
		#define __WCHAR_TYPE__ int
	#else
		#undef __WCHAR_TYPE__
		#define __WCHAR_TYPE__ wchar_t
	#endif
	
	#undef _BSD_WCHAR_T_
	#define _BSD_WCHAR_T_	__WCHAR_TYPE__			/* wchar_t */
	
	#undef _BSD_RUNE_T_
	#define _BSD_RUNE_T_	__WCHAR_TYPE__			/* rune_t */
	
	#define WCHAR_MIN	((wchar_t) 0x80000000U)
	#if !((! defined(__cplusplus)) || defined(__STDC_LIMIT_MACROS))
		#define WCHAR_MAX	((wchar_t) 0x7FFFFFFFU)
	#endif
	
	typedef wchar_t	wint_t;
	typedef wchar_t	wctype_t;
	typedef int		mbstate_t;
	typedef wchar_t	Wint_t;
#endif


#ifndef _ANSI_SOURCE
	typedef _BSD_WCHAR_T_	rune_t;
#endif


#endif /* _MW_ANSI_H_ */