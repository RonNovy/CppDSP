/* Machine specific information and functions
 * Copyright (C) 2015
 * Ron S. Novy
 */

#pragma once

#include "configure.h"
#include <cstdint>
#include <utility>	// C++11 std::swap

// Bodge for MSVC to force optimization to bswap instruction by using intrinsics.
#ifdef _MSC_VER
	#include <stdlib.h>
#endif

// ********************************
// **** dsp namespace for dsp based classes and functions.
namespace dsp
{
	// ********************************
	// **** dsp::machine namespace for machine specific classes and functions.
	namespace machine
	{
		// ********************************
		// **** Functions to help tell us what endianness we are in.
		inline bool is_little_endian(void) { return LITTLE_ENDIAN; }
		inline bool is_big_endian(void) { return BIG_ENDIAN; }
		// ********************************
		// ********************************


		// ********************************
		// **** machine::byte_swap() functions for changing endianness of different types.
		#pragma region machine_byte_swap
		template <typename _Type>
		inline _Type byte_swap(_Type val)
		{
			uint8 *x = (uint8*)&val;
			for (int s = 0, e = sizeof(_Type)-1; s < sizeof(_Type) / 2; ++s, --e)
				std::swap(x[s], x[e]);
			return val;
		};
		// ********************************

		// ********************************
		template <> inline uint8_t byte_swap<uint8_t>(uint8_t val)
		{
			return val;
		};
		// ********************************

		// ********************************
		template <> inline uint16_t byte_swap<uint16_t>(uint16_t val)
		{
			#ifdef _MSC_VER
				return _byteswap_ushort(val);
			#else
				return
					(val << 8) |
					((val >> 8) & 0x000000ffl);
			#endif
		};
		// ********************************

		// ********************************
		template <> inline uint32_t byte_swap<uint32_t>(uint32_t val)
		{
			#ifdef _MSC_VER
				return _byteswap_ulong(val);
			#else
				return
					(val << 24) |
					((val << 8) & 0x00ff0000l) |
					((val >> 8) & 0x0000ff00l) |
					((val >> 24) & 0x000000ffl);
			#endif
		};
		// ********************************

		// ********************************
		template <> inline uint64_t byte_swap<uint64_t>(uint64_t val)
		{
			#ifdef _MSC_VER
				return _byteswap_uint64(val);
			#else
				return
					(val << 56) |
					((val << 40) & 0x00ff000000000000ll) |
					((val << 24) & 0x0000ff0000000000ll) |
					((val << 8) & 0x000000ff00000000ll) |
					((val >> 8) & 0x00000000ff000000ll) |
					((val >> 24) & 0x0000000000ff0000ll) |
					((val >> 40) & 0x000000000000ff00ll) |
					((val >> 56) & 0x00000000000000ffll);
			#endif
		};
		// ********************************

		// ********************************
		template <> inline int8_t byte_swap<int8_t>(int8_t val)
		{
			return val;
		};
		// ********************************

		// ********************************
		template <> inline int16_t byte_swap<int16_t>(int16_t val)
		{
			#ifdef _MSC_VER
				return _byteswap_ushort(val);
			#else
				return
					(val << 8) |
					((val >> 8) & 0x000000ffl);
			#endif
		};
		// ********************************

		// ********************************
		template <> inline int32_t byte_swap<int32_t>(int32_t val)
		{
			#ifdef _MSC_VER
				return _byteswap_ulong(val);
			#else
				return
					(val << 24) |
					((val << 8) & 0x00ff0000l) |
					((val >> 8) & 0x0000ff00l) |
					((val >> 24) & 0x000000ffl);
			#endif
		};
		// ********************************

		// ********************************
		template <> inline int64_t byte_swap<int64_t>(int64_t val)
		{
			#ifdef _MSC_VER
				return _byteswap_uint64(val);
			#else
				return
					(val << 56) |
					((val << 40) & 0x00ff000000000000ll) |
					((val << 24) & 0x0000ff0000000000ll) |
					((val << 8) & 0x000000ff00000000ll) |
					((val >> 8) & 0x00000000ff000000ll) |
					((val >> 24) & 0x0000000000ff0000ll) |
					((val >> 40) & 0x000000000000ff00ll) |
					((val >> 56) & 0x00000000000000ffll);
			#endif
		};
		// ********************************

		// ********************************
		template <> inline float byte_swap<float>(float val)
		{
			union { int32_t x; float y; };
			y = val;
			#ifdef _MSC_VER
				x = _byteswap_ulong(x);
			#else
				x = (x << 24) |
					((x << 8) & 0x00ff0000l) |
					((x >> 8) & 0x0000ff00l) |
					((x >> 24) & 0x000000ffl);
			#endif
			return y;
		};
		// ********************************

		// ********************************
		template <> inline double byte_swap<double>(double val)
		{
			union { int64_t x; double y; };
			y = val;
			#ifdef _MSC_VER
				x = _byteswap_uint64(x);
			#else
				x = (x << 56) |
					((x << 40) & 0x00ff000000000000ll) |
					((x << 24) & 0x0000ff0000000000ll) |
					((x << 8) & 0x000000ff00000000ll) |
					((x >> 8) & 0x00000000ff000000ll) |
					((x >> 24) & 0x0000000000ff0000ll) |
					((x >> 40) & 0x000000000000ff00ll) |
					((x >> 56) & 0x00000000000000ffll);
			#endif
			return y;
		};
		// ********************************

		// ********************************
		template <> inline long double byte_swap<long double>(long double val)
		{
			uint8_t *x = (uint8_t*)&val;
			for (int s = 0, e = sizeof(long double)-1; s < sizeof(long double) / 2; ++s, --e)
				std::swap(x[s], x[e]);
			return val;
		};
		#pragma endregion machine_byte_swap
		// **** End dsp::machine::byte_swap functions
		// ********************************
	}
	// **** End dsp::machine namespace
	// ********************************
}
// **** End dsp namespace.
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
