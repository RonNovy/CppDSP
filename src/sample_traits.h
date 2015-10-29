/* sample_traits  -  Information on dsp types.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#pragma once
#include "configure.h"

#include <cstdint>
#include "int24_t.h"



/*********************************
 * ********************************
 *   I have debated on whether to use std::numeric_limits<_T>::max() or to use
 * std::numeric_limits<_T>::max() + 1 as the scalar/denominator for conversions
 * to or from floating-point formats.  I have done some very minimal tests on 
 * each and it seems that there are less rounding errors when using the + 1
 * format with std::numeric_limits<_T>::max() + 1.  Without the + 1 you get
 * inconsistencies when comparing results on equivilant operations using different
 * types which could mean that (int a + b) is not quite equal to (float a + b).
 * My guess on why this happens would be because a power of 2 is more easily 
 * stored and operated on when converted to or from float then a power of 2 - 1.
 * So my conclusion is to use the + 1 format...
 */

// ********************************
// **** dsp namespace for dsp related functions and classes.
namespace dsp
{
	// ********************************
	// sample_traits holds dsp type properties for conversions.
	//
	// bool is_dsp_type;         - Indicates whether this is a valid dsp type.
	// bool is_integral;		 - Indicates whether this is integral or not.
	// bool is_unsigned;		 - Indicates whether the type is unsigned or signed.
	// _T zero();                - Value that represents 0 for the given sample type.
	// long double min();		 - Minimum value for type when in floating-point form.
	// long double max();		 - Maximum value for type when in floating-point form.
	// long double multiplier(); - Multiplier for converting floats to integral types.
	// long double reciprocal(); - Recipricol of multiplier for converting integral types to floats.
	// 

	#ifdef min
		#undef min
	#endif
	#ifdef max
		#undef max
	#endif

#if _MSC_VER < 1900
#define constexpr const
#endif

	// ********************************
	// **** dsp::sample_traits
	template <typename _T> class sample_traits
	{
	public:
		static constexpr bool is_dsp_type = false;
		static constexpr bool is_integral = false;
		static constexpr bool is_unsigned = false;
		static constexpr int zero() { return 0; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return  1.0l; };
		static constexpr long double multiplier() { return 1.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<int8_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = false;
		static constexpr int8_t zero() { return 0i8; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 127.0l / 128.0l; };
		static constexpr long double multiplier() { return 128.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<int16_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = false;
		static constexpr int16_t zero() { return 0i16; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 32767.0l / 32768.0l; };
		static constexpr long double multiplier() { return 32768.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<int24_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = false;
		static constexpr int24_t zero() { return 0i32; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 8388607.0l / 8388608.0l; };
		static constexpr long double multiplier() { return 8388608.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<int32_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = false;
		static constexpr int32_t zero() { return 0i32; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 2147483647.0l / 2147483648.0l; };
		static constexpr long double multiplier() { return 2147483648.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<int64_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = false;
		static constexpr int64_t zero() { return 0i64; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 9223372036854775807.0l / 9223372036854775808.0l; };
		static constexpr long double multiplier() { return 9223372036854775808.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<uint8_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = true;
		static constexpr uint8_t zero() { return 128ui8; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 127.0l / 128.0l; };
		static constexpr long double multiplier() { return 128.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<uint16_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = true;
		static constexpr uint16_t zero() { return 32768ui16; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 32767.0l / 32768.0l; };
		static constexpr long double multiplier() { return 32768.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<uint32_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = true;
		static constexpr uint32_t zero() { return 2147483648ui32; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 2147483647.0l / 2147483648.0l; };
		static constexpr long double multiplier() { return 2147483648.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<uint64_t>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = true;
		static constexpr bool is_unsigned = true;
		static constexpr uint64_t zero() { return 9223372036854775808ui64; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return 9223372036854775807.0l / 9223372036854775808.0l; };
		static constexpr long double multiplier() { return 9223372036854775808.0l; };
		static constexpr long double reciprocal() { return 1.0l / multiplier(); };
	};

	template <> class sample_traits<float>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = false;
		static constexpr bool is_unsigned = false;
		static constexpr float zero() { return 0.0f; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return  1.0l; };
		static constexpr long double multiplier() { return 1.0l; };
		static constexpr long double reciprocal() { return 1.0l; };
	};

	template <> class sample_traits<double>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = false;
		static constexpr bool is_unsigned = false;
		static constexpr double zero() { return 0.0; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return  1.0l; };
		static constexpr long double multiplier() { return 1.0l; };
		static constexpr long double reciprocal() { return 1.0l; };
	};

	template <> class sample_traits<long double>
	{
	public:
		static constexpr bool is_dsp_type = true;
		static constexpr bool is_integral = false;
		static constexpr bool is_unsigned = false;
		static constexpr long double zero() { return 0.0l; };
		static constexpr long double min() { return -1.0l; };
		static constexpr long double max() { return  1.0l; };
		static constexpr long double multiplier() { return 1.0l; };
		static constexpr long double reciprocal() { return 1.0l; };
	};
	// **** End sample_traits
	// ********************************

#if _MSC_VER < 1900
#undef constexpr
#endif

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
