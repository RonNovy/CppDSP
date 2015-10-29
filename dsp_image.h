/* dsp_image.h - Template classes and functions for digital signal processing with images.
 * Copyright (C) 2015
 * Written by Ron S. Novy
 */

#pragma once

#include <assert.h>
#include <utility>


//template <typename _Type, int _NumElements = 4> class pixel;
//template <typename _Type> class rgb_t : public pixel<_Type, 3>;
//template <typename _Type> class rgba_t : public pixel<_Type, 4>;
//template <typename _Type> class bgr_t : public pixel<_Type, 3>;
//template <typename _Type> class bgra_t : public pixel<_Type, 4>;


// Default pixel has 4 elements.
template <typename _Type, int _NumElements = 4>
class pixel
{
private:
	_Type element[_NumElements];

public:
	static const int red = 0;
	static const int green = 1;
	static const int blue = 2;
	static const int alpha = 3;
	static const int cyan = 0;
	static const int magenta = 1;
	static const int yellow = 2;
	static const int black = 3;

	template <typename _Type2, int _NumElements2> friend class pixel;
	template <typename _TypeX> friend class rgb_t;
	template <typename _TypeX> friend class rgba_t;
	template <typename _TypeX> friend class bgr_t;
	template <typename _TypeX> friend class bgra_t;


	inline _Type get(int i)
	{
		assert(i < _NumElements);
		return element[i];
	}

	template <int _Index>
	inline _Type get()
	{
		static_assert(_Index < _NumElements, "_Index must be less than _NumElements.");
		return element[_Index];
	}
	template <int _Index>
	inline void set(_Type x)
	{
		static_assert(_Index < _NumElements, "_Index must be less than _NumElements.");
		element[_Index] = x;
	}
	template <int _Index1, int _Index2>
	inline _Type swap_elements()
	{
		static_assert(_Index1 != _Index2, "_Index1 and _Index2 cannot be equal.");
		static_assert(_Index1 < _NumElements, "_Index1 must be less than _NumElements.");
		static_assert(_Index2 < _NumElements, "_Index2 must be less than _NumElements.");
		return std::swap(element[_Index1], element[_Index2]);
	}

	inline _Type & r() { return this->element[red]; }
	inline _Type & g() { return this->element[green]; }
	inline _Type & b() { return this->element[blue]; }
	inline _Type & a() { return this->element[alpha]; }
	inline _Type & c() { return this->element[cyan]; }
	inline _Type & m() { return this->element[magenta]; }
	inline _Type & y() { return this->element[yellow]; }
	inline _Type & k() { return this->element[black]; }
};

template <typename _Type>
class rgb_t : public pixel<_Type, 3>
{
public:
	static const int red = 0;
	static const int green = 1;
	static const int blue = 2;

	inline _Type & r() { return this->element[red]; }
	inline _Type & g() { return this->element[green]; }
	inline _Type & b() { return this->element[blue]; }

};

template <typename _Type>
class rgba_t : public pixel<_Type, 4>
{
public:
	static const int red = 0;
	static const int green = 1;
	static const int blue = 2;
	static const int alpha = 3;

	inline _Type & r() { return this->element[red]; }
	inline _Type & g() { return this->element[green]; }
	inline _Type & b() { return this->element[blue]; }
	inline _Type & a() { return this->element[alpha]; }
};

template <typename _Type>
class bgr_t : public pixel<_Type, 3>
{
public:
	static const int red = 2;
	static const int green = 1;
	static const int blue = 0;

	inline _Type & r() { return this->element[red]; }
	inline _Type & g() { return this->element[green]; }
	inline _Type & b() { return this->element[blue]; }
};

template <typename _Type>
class bgra_t : public pixel<_Type, 4>
{
public:
	static const int red = 2;
	static const int green = 1;
	static const int blue = 0;
	static const int alpha = 3;

	inline _Type & r() { return this->element[red]; }
	inline _Type & g() { return this->element[green]; }
	inline _Type & b() { return this->element[blue]; }
	inline _Type & a() { return this->element[alpha]; }
};

using rgb = rgb_t<char>;
using rgba = rgba_t<char>;
using bgr = bgr_t<char>;
using bgra = bgra_t<char>;



#ifdef TEST_DSP_IMAGE

#include <iostream>

const char LUT_8x8_to_64[64] =
{
	0,  1,  8, 16,  9,  2,  3, 10,
	17, 24, 32, 25, 18, 11,  4,  5,
	12, 19, 26, 33, 40, 48, 41, 34,
	27, 20, 13,  6,  7, 14, 21, 28,
	35, 42, 49, 56, 57, 50, 43, 36,
	29, 22, 15, 23, 30, 37, 44, 51,
	58, 59, 52, 45, 38, 31, 39, 46,
	53, 60, 61, 54, 47, 55, 62, 63
};

const char LUT_16x16_to_256[256] =
{
	  0,   1,  16,  32,  17,   2,   3,  18,  33,  48,  64,  49,  34,  19,   4,   5,
	 20,  35,  50,  65,  80,  96,  81,  66,  51,  36,  21,   6,   7,  22,  37,  52,
	 67,  82,  97, 112, 128, 113,  98,  83,  68,  53,  38,  23,   8,   9,  24,  39,
	 54,  69,  84,  99, 114, 129, 144, 160, 145, 130, 115, 100,  85,  70,  55,  40,
	 25,  10,  11,  26,  41,  56,  71,  86, 101, 116, 131, 146, 161, 176, 192, 177,
	162, 147, 132, 117, 102,  87,  72,  57,  42,  27,  12,  13,  28,  43,  58,  73,
	 88, 103, 118, 133, 148, 163, 178, 193, 208, 224, 209, 194, 179, 164, 149, 134,
	119, 104,  89,  74,  59,  44,  29,  14,  15,  30,  45,  60,  75,  90, 105, 120,
	135, 150, 165, 180, 195, 210, 225, 240, 241, 226, 211, 196, 181, 166, 151, 136,
	121, 106,  91,  76,  61,  46,  31,  47,  62,  77,  92, 107, 122, 137, 152, 167,
	182, 197, 212, 227, 242, 243, 228, 213, 198, 183, 168, 153, 138, 123, 108,  93,
	 78,  63,  79,  94, 109, 124, 139, 154, 169, 184, 199, 214, 229, 244, 245, 230,
	215, 200, 185, 170, 155, 140, 125, 110,  95, 111, 126, 141, 156, 171, 186, 201,
	216, 231, 246, 247, 232, 217, 202, 187, 172, 157, 142, 127, 143, 158, 173, 188,
	203, 218, 233, 248, 249, 234, 219, 204, 189, 174, 159, 175, 190, 205, 220, 235,
	250, 251, 236, 221, 206, 191, 207, 222, 237, 252, 253, 238, 223, 239, 254, 255
};


int main()
{
	std::cout << "Testing pixel<>\n\n";

	//
	rgba_t<float> p;
	p.set<rgb::red>(1.23f);

	std::cout << p.get<rgb::red>() << "\n\n";
	std::cout << p.r() << "\n\n";
	std::cout << p.c() << "\n\n";

	std::cout << "End\n\n";
}
#endif // ifdef TEST_DSP_IMAGE

