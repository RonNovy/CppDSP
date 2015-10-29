/* Special class for int24_t 24-bit triplet
 * Copyright (C) 2015
 * Ron S. Novy
 */

#pragma once

#include "configure.h"
#include "machine.h"

#include <cstdint>
#include <algorithm>

// ********************************
// **** Define limits.
#ifndef INT24_MIN
#define INT24_MAX  (8388607)
#define INT24_MIN (-8388608)
#endif
// ********************************


// ********************************
// **** int24_t class for 24-bit sample operations.
#pragma pack(1)
class int24_t
{
private:
	uint8_t __bytes[3];

public:
	// ********************************
	// **** Constructors.
#if _MSC_VER >= 1900
	constexpr int24_t() : __bytes() { };
#if (LITTLE_ENDIAN == 1) && (BIG_ENDIAN == 0)
	constexpr int24_t(const int val) : __bytes{ static_cast<uint8_t>((val)), static_cast<uint8_t>((val>>8)), static_cast<uint8_t>((val>>16))} {};
#elif (LITTLE_ENDIAN == 0) && (BIG_ENDIAN == 1)
	constexpr int24_t(const int val) : __bytes{ static_cast<uint8_t>((val>>16)), static_cast<uint8_t>((val>>8)), static_cast<uint8_t>((val))} {};
#else
	#error Unknown Endianness.
#endif
#else
	inline int24_t() {};
	inline int24_t(const int val) { *this = val; };
#endif
	//inline int24_t(const int24_t& val) { std::copy(....); };
	// ********************************


	// ********************************
	// **** Special byte swap function
	inline int24_t& bswap()
	{
		std::swap(__bytes[0], __bytes[2]);
		return *this;
	}
	// ********************************


	// ********************************
	// **** Convert int24_t to a fundamental type.
	inline operator int() const
	{
#if LITTLE_ENDIAN == 1
		// sign extend the negative value.
		if (__bytes[2] & 0x80)
		{
			return (0xff000000) | (__bytes[2] << 16) | (__bytes[1] << 8) | (__bytes[0]);
		}
		else
		{
			return (__bytes[2] << 16) | (__bytes[1] << 8) | (__bytes[0]);
		}
#elif BIG_ENDIAN == 1
		// sign extend the negative value.
		if (__bytes[0] & 0x80)
		{
			return (0xff000000) | (__bytes[0] << 16) | (__bytes[1] << 8) | (__bytes[2]);
		}
		else
		{
			return (__bytes[0] << 16) | (__bytes[1] << 8) | (__bytes[2]);
		}
#else
	#error Unknown Endianness.
#endif
	}

	inline operator char()			const { return (char)this->operator int(); }
	inline operator signed char()	const { return (signed char)this->operator int(); } // Yes, this is different...
	inline operator short()			const { return (short)this->operator int(); }
	inline operator long()			const { return (long)this->operator int(); }
	inline operator long long()		const { return (long long)this->operator int(); }

	inline operator unsigned char()			const { return (unsigned char)this->operator int(); }
	inline operator unsigned short()		const { return (unsigned short)this->operator int(); }
	inline operator unsigned int()			const { return (unsigned int)this->operator int(); }
	inline operator unsigned long()			const { return (unsigned long)this->operator int(); }
	inline operator unsigned long long()	const { return (unsigned long long)this->operator int(); }

	inline operator float()			const { return (float)this->operator int(); }
	inline operator double()		const { return (double)this->operator int(); }
	inline operator long double()	const { return (long double)this->operator int(); }
	// ********************************
	// ********************************


	// ********************************
	// **** Assignment operators
	inline int24_t operator = (const int24_t rhs)
	{
		__bytes[0] = rhs.__bytes[0];
		__bytes[1] = rhs.__bytes[1];
		__bytes[2] = rhs.__bytes[2];
		return *this;
	}

	template <typename _Type>
	inline int24_t operator = (const _Type rhs)
	{
		*this = (int)rhs;
		return *this;
	}

	template <>
	inline int24_t operator = <int>(const int rhs)
	{
		__bytes[0] = ((const unsigned char*)&rhs)[0];
		__bytes[1] = ((const unsigned char*)&rhs)[1];
		__bytes[2] = ((const unsigned char*)&rhs)[2];
		return *this;
	}
	// ********************************
	// ********************************


	// ********************************
	// **** Arithmatic operators.
	inline int24_t operator + (const int24_t rhs) const { return int24_t((int)*this + (int)rhs); }
	inline int24_t operator - (const int24_t rhs) const { return int24_t((int)*this - (int)rhs); }
	inline int24_t operator * (const int24_t rhs) const { return int24_t((int)*this * (int)rhs); }
	inline int24_t operator / (const int24_t rhs) const { return int24_t((int)*this / (int)rhs); }

	// ********************************
	template <typename _Type> inline int24_t operator + (const _Type rhs) const { return int24_t((_Type)*this + rhs); }
	template <typename _Type> inline int24_t operator - (const _Type rhs) const { return int24_t((_Type)*this - rhs); }
	template <typename _Type> inline int24_t operator * (const _Type rhs) const { return int24_t((_Type)*this * rhs); }
	template <typename _Type> inline int24_t operator / (const _Type rhs) const { return int24_t((_Type)*this / rhs); }
	// ********************************
	// ********************************


	// ********************************
	// **** Arithmatic assignment operators.
	inline int24_t operator += (const int24_t rhs)
	{
		*this = *this + rhs;
		return *this;
	}

	inline int24_t operator -= (const int24_t rhs)
	{
		*this = *this - rhs;
		return *this;
	}

	inline int24_t operator *= (const int24_t rhs)
	{
		*this = *this * rhs;
		return *this;
	}

	inline int24_t operator /= (const int24_t rhs)
	{
		*this = *this / rhs;
		return *this;
	}
	// ********************************

	// ********************************
	template <typename _Type>
	inline int24_t operator += (const _Type rhs)
	{
		*this = ((_Type)*this + rhs);
		return *this;
	}

	template <typename _Type>
	inline int24_t operator -= (const _Type rhs)
	{
		*this = ((_Type)*this - rhs);
		return *this;
	}

	template <typename _Type>
	inline int24_t operator *= (const _Type rhs)
	{
		*this = ((_Type)*this * rhs);
		return *this;
	}

	template <typename _Type>
	inline int24_t operator /= (const _Type rhs)
	{
		*this = ((_Type)*this / rhs);
		return *this;
	}
	// ********************************
	// ********************************


	// ********************************
	// **** Shift operators.
	inline int24_t operator >> (const int val) const { return int24_t((int)*this >> val); }
	inline int24_t operator << (const int val) const { return int24_t((int)*this << val); }

	inline int24_t& operator >>= (const int val)
	{
		*this = *this >> val;
		return *this;
	}

	inline int24_t& operator <<= (const int val)
	{
		*this = *this << val;
		return *this;
	}
	// ********************************
	// ********************************


	// ********************************
	// **** Boolean and unary operations.
	inline operator bool() const	{ return (int)*this != 0; }
	inline bool operator !() const	{ return !((int)*this); }
	inline int24_t operator -()		{ return int24_t(-(int)*this); }
	inline int operator +()			{ return (int)*this; }
	// ********************************
	// ********************************


	// ********************************
	// **** Increment/Decrement operators.
	inline int24_t operator ++()
	{
		int x = *this;
		*this = ++x;
		return *this;
	};

	inline int24_t operator --()
	{
		int x = *this;
		*this = --x;
		return *this;
	};

	inline int24_t operator ++(int)
	{
		int24_t ret = *this;
		++*this;
		return ret;
	};

	inline int24_t operator --(int)
	{
		int24_t ret = *this;
		--*this;
		return ret;
	};
	// ********************************
	// ********************************


	// ********************************
	// **** Comparison operators
	inline bool operator == (const int24_t rhs) const { return (int)*this == (int)rhs; }
	inline bool operator != (const int24_t rhs) const { return (int)*this != (int)rhs; }
	inline bool operator >= (const int24_t rhs) const { return (int)*this >= (int)rhs; }
	inline bool operator <= (const int24_t rhs) const { return (int)*this <= (int)rhs; }
	inline bool operator >  (const int24_t rhs) const { return (int)*this >  (int)rhs; }
	inline bool operator <  (const int24_t rhs) const { return (int)*this <  (int)rhs; }

	template <typename _Type> inline bool operator == (const _Type rhs) const { return (_Type)*this == rhs; }
	template <typename _Type> inline bool operator != (const _Type rhs) const { return (_Type)*this != rhs; }
	template <typename _Type> inline bool operator >= (const _Type rhs) const { return (_Type)*this >= rhs; }
	template <typename _Type> inline bool operator <= (const _Type rhs) const { return (_Type)*this <= rhs; }
	template <typename _Type> inline bool operator >  (const _Type rhs) const { return (_Type)*this >  rhs; }
	template <typename _Type> inline bool operator <  (const _Type rhs) const { return (_Type)*this <  rhs; }
	// ********************************
	// ********************************
};
#pragma pack()
// **** End int24_t
// ********************************



// ********************************
// **** dsp namespace for dsp based classes and functions.
namespace dsp
{

	// ********************************
	// **** These are extensions to configure.h and machine.h
	namespace machine
	{
		template <> inline int24_t byte_swap<int24_t>(int24_t val)
		{
			return val.bswap();
		};
	}
	// **** End dsp::machine namespace.
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
