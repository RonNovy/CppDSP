/* bstream class for binary file reading/writing.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#pragma once
#include "configure.h"
#include "machine.h"

#include <string>
#include <fstream>


// ********************************
// **** dsp namespace for dsp related classes and functions.
namespace dsp
{
	// ********************************
	// **** dsp::bstream - Binary file stream for simplified reading and writing of data.
	class bstream : public std::fstream
	{
	private:
		bool endian_swap_mode;

	public:
		bstream() { endian_swap_mode = false; };

		// ********************************
		// **** Set swap mode for functions that use it.
		inline void set_endian_swap_mode(bool swapq) { endian_swap_mode = swapq; };

		// ********************************
		// **** Read a sizeof(_T) bytes into vairable.
		template<typename _T>
		bool get_no_swap(_T &v)
		{
			return read(static_cast<char*> &v, sizeof(_T)).eof();
		};
		// ********************************

		// ********************************
		// **** Read a sizeof(_T) bytes into vairable and byte swap.
		template<typename _T>
		bool get_and_swap(_T &v)
		{
			bool ret = read(static_cast<char*> &v, sizeof(_T)).eof();
			v = machine::byte_swap(v);
			return ret;
		};
		// ********************************


		// ********************************
		// **** Read an integer from the stream that is in a specific endianness.
		#if (LITTLE_ENDIAN == 1) && (BIG_ENDIAN == 0)
			template<typename _T>
			inline bool get_little(_T &t)
			{
				return get_no_swap(t);
			};

			template<typename _T>
			inline bool get_big(_T &t)
			{
				return get_and_swap(t);
			};

		#elif (LITTLE_ENDIAN == 0) && (BIG_ENDIAN == 1)
			template<typename _T>
			inline bool get_little(_T &t)
			{
				return get_and_swap(t);
			};

			template<typename _T>
			inline bool get_big(_T &t)
			{
				return get_no_swap(t);
			};

		#else
			#error Endianness not properly defined in configure.h
		#endif


		// ********************************
		// **** Read an value from the stream in the set swap mode (big or little endian).
		template<typename _T>
		bool get_bymode(_T &t)
		{
			if (endian_swap_mode)
				return get_and_swap(t);

			return get_no_swap(t);
		};
		// ********************************

		// ********************************
		// **** Read a value from the stream in the opposite set swap mode (big or little endian).
		template<typename _T>
		bool get_bymode_swapped(_T &t)
		{
			if (endian_swap_mode)
				return get_no_swap(t);

			return get_and_swap(t);
		};
		// ********************************
		// ********************************
	};
	// **** End dsp::bstream
	// ********************************
}
// **** End dsp namespace
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
