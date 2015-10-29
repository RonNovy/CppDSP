/* Configure.h  -  This file is used to configure some basic things for the dsp library.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#pragma once


// If OpenMP is enabled on the platform then this will help us take advantage of that.
#ifdef _OPENMP
	//   We automatically include the header if _OPENMP is defined.  This means that 
	// any code that uses OpenMP must either include the header itself or place an 
	// #ifdef _OPENMP around code that depends on this configure header to include it.
	#include <omp.h>
	#if defined(_WIN32) || defined(_WIN64)
		#define PRAGMA_IF_OPENMP(x) __pragma (x)
	#else
		#define PRAGMA_IF_OPENMP(x) _Pragma (x)
	#endif
#else	// #ifdef _OPENMP
	#define PRAGMA_IF_OPENMP(x)
#endif	// #ifdef _OPENMP



// ********************************
// **** Platform dependant things get defined here.
#if defined(_WIN32) || defined(_WIN64)

	// ************************************
	// **** **** Windows platform **** ****
	// ************************************

	// ********************************
	// ********************************
	// **** Set endianness of target machine.
	// **** Set one of these to 1 and the other to 0 to indicate endianness...
	#define BIG_ENDIAN 0
	#define LITTLE_ENDIAN 1
	// ********************************
	// ********************************

	#define uchar	unsigned char
	#define ushort	unsigned short
	#define uint	unsigned int
	#define ulong	unsigned long

	#define longlong	long long
	#define ulonglong	unsigned long long

	#ifndef _CRT_SECURE_NO_WARNINGS
		#define _CRT_SECURE_NO_WARNINGS
	#endif
	#ifndef NOMINMAX
		#define NOMINMAX
	#endif
	#ifndef UNICODE
		#define UNICODE
	#endif
	#ifndef STRICT
		#define STRICT
	#endif
	#if _MSC_VER < 1900
		#define snprintf sprintf_s
	#endif

	#define WIDEN(x)		__T(x)
	#define __WFILE__		WIDEN(__FILE__)
	#define __WFUNCTION__	WIDEN(__FUNCTION__)
	#define __WFILESEP__ '\\'

	#ifndef __T
	#include <tchar.h>
	#endif
	// ********************************
	// ********************************

#elif defined(__GNUC__)

	// **************************************
	// **** **** gcc or clang based **** ****
	// **************************************

	// ********************************
	// **** Set endianness of target machine.
	// **** TODO: Detect real platform and set these accordingly.
	// **** Set one of these to 1 and the other to 0 to indicate endianness...
	#define BIG_ENDIAN 0
	#define LITTLE_ENDIAN 1
	// ********************************
	// ********************************

	#define uchar	unsigned char
	#define ushort	unsigned short
	#define uint	unsigned int
	#define ulong	unsigned long

	#define longlong	long long
	#define ulonglong	unsigned long long

	#define _wcsdup wcsdup
	#define _wcslwr wcslwr

	#define _tmain wmain
	#define _TCHAR wchar_t
	#define __T(x) L ## x
	#define WIDEN(x) __T(x)
	#define __WFILE__ WIDEN(__FILE__)
	#define __WFUNCTION__ WIDEN(__WFILE__)
	#define __WFILESEP__ '/'


#else
	#define _wcsdup wcsdup
	#define _wcslwr wcslwr

	#define _tmain wmain
	#define _TCHAR wchar_t
	#define __T(x) L ## x
	#define WIDEN(x) __T(x)
	#define __WFILE__ WIDEN(__FILE__)
	#define __WFUNCTION__ WIDEN(__WFILE__)
	#define __WFILESEP__ '/'

	// **** **** Unknown platform **** ****
	#error Platform not defined for configuration.  Please add a new platform for this compiler.
#endif



// ********************************
// **** Sanity check the endianness defines...
#if ((BIG_ENDIAN) && (LITTLE_ENDIAN)) || (!(BIG_ENDIAN) && !(LITTLE_ENDIAN))
#error Endianness not set correctly (BIG_ENDIAN and LITTLE_ENDIAN).  Set one of these to 1 and the other to 0 to indicate endianness of target machine...
#endif
// ********************************
// ********************************


//*******************************************************************
// ********************************
// * Byte order determination at run-time.
// * Hopefully the compiler picks up on the fact that this is a constant and
// * removes any dead code.
#define IS_LITTLE_ENDIAN (*(const short *)"\x01\x02" == 0x0201)
#define IS_BIG_ENDIAN    (*(const short *)"\x01\x02" == 0x0102)
// ********************************
// ********************************

/*	▄▄▄▄▄▄▄ ▄▄     ▄▄  ▄▄ ▄▄▄▄▄▄▄
 *	█ ▄▄▄ █ ▄  ▄▄▄██  █ ▄ █ ▄▄▄ █
 *	█ ███ █ ██▄█ ▄  ▀█▄▄▀ █ ███ █
 *	█▄▄▄▄▄█ ▄▀▄ █ █ ▄▀█▀▄ █▄▄▄▄▄█
 *	▄▄▄▄  ▄ ▄▀ ▀ ██ ▄█▀▄▀▄  ▄▄▄ ▄
 *	██  ██▄█▀▀    ▄█▀▀█▀ ███▀▀▀▀▀
 *	█▄█ █ ▄ █▄ █▀▀▀▀ ▄ █▀▀  ▀ ▄ ▄
 *	▄▀ █ █▄▀▀ █▀▄▀▄  █▀█▀▄▀▄ █▄▄█
 *	█▀▀█ █▄▄▀▀▄▄▀▀  ▄ █ ▄ ▀▄█▀ ▄█
 *	▄▀▀▀ █▄▄███▄█▀ █▄█  ▄ ▄█▄▄█  
 *	▄▀▀█ ▄▄▄ █▄█▄  ▀█▄ ▄▄███▀█ █ 
 *	▄▄▄▄▄▄▄ ▀█▀▄██▀ ▀▀█▄█ ▄ █▀ ▄▀
 *	█ ▄▄▄ █   █ ▄ ▄▀ ▄▀ █▄▄▄█▄▄█▀
 *	█ ███ █ █▀ █▀▄▀▀ ██▀▄▀ ▄▀   █
 *	█▄▄▄▄▄█ ██ ▀▄ ██▄ █▄██▄▄▀▀▄█ 
 */
