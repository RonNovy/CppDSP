/* An endianness aware type for DSP buffers.
 * Copyright (C) 2015
 * Ron S. Novy
 *
 *  Here we define a DSP type for use with many different fundamental C++ types.
 * Floating-point types are stored as a range between +1.0 and -1.0 and integer
 * types are stored between their respective upper and lower type limits. The 
 * main purpose of this class is to offer a way to manipulate DSP data, using 
 * any fundamental type, in a way that every operation will give results that 
 * are equivilent to the same operation in another type.  So, with this class,
 * the following would be true:
 *
 *  sample<int>   a(some_integer), b(some_number2);
 *  sample<float> x(a), y(b);
 *  true = ((a + b) == (x + y));
 *  true = ((a - b) == (x - y));
 *  true = ((a * b) == (x * y));
 *  true = ((a / b) == (x / y));
 *  etc...
 *
 *  Fundamental tyes from 8-bit signed/unsigned integers to long double
 * floating-point are usable with this class, including a special signed
 * 24-bit integer.
 *
 *  Valid sample types are:
 *    Floating-point:
 *        float, double, long double
 *
 *    Signed integers:
 *        int8_t, int16_t, int24_t, int32_t, int64_t
 *
 *    Unigned integers:
 *        uint8_t, uint16_t, uint32_t, uint64_t
 *
 */

#pragma once

#include "configure.h"

#include <cstdint>

#include "int24_t.h"
#include "sample_traits.h"


// ********************************
// **** Disable some annoying warnings.
// **** TODO: Find out how to disable warnings on clang/gcc if they pop-up here...
#ifdef _WIN32 // Disable warning 4244.
	#pragma warning( push )
	#pragma warning (disable : 4244 )
	#pragma warning (disable : 4307 )
	#pragma warning (disable : 4309 )
	#define POP_WIN32 1
#endif
// ********************************


// ********************************
// **** dsp namespace for dsp classes and functions.
namespace dsp
{

	// ********************************
	// **** dsp::internal namepsace.
	namespace internal
	{
		// ********************************
		// **** Helper function to convert to/from native endianness.
		template <typename _Type, bool _Native>
		inline
		_Type dsp_endianness_to_native(_Type src)
		{
				if (!_Native) src = machine::byte_swap(src);
				return src;
			};
		// ********************************
		// ********************************


		// ********************************
		// **** float to float converion
		template <typename _TypeDst, typename _TypeSrc> inline _TypeDst dsp_float_to_float(_TypeDst dst, _TypeSrc src)	{ return dst; };
		template <> inline float		dsp_float_to_float<float, float>(float dst, float src)							{ return (float)src; };
		template <> inline float		dsp_float_to_float<float, double>(float dst, double src)						{ return (float)src; };
		template <> inline float		dsp_float_to_float<float, long double>(float dst, long double src)				{ return (float)src; };
		template <> inline double		dsp_float_to_float<double, float>(double dst, float src)						{ return (double)src; };
		template <> inline double		dsp_float_to_float<double, double>(double dst, double src)						{ return (double)src; };
		template <> inline double		dsp_float_to_float<double, long double>(double dst, long double src)			{ return (double)src; };
		template <> inline long double	dsp_float_to_float<long double, float>(long double dst, float src)				{ return (long double)src; };
		template <> inline long double	dsp_float_to_float<long double, double>(long double dst, double src)			{ return (long double)src; };
		template <> inline long double	dsp_float_to_float<long double, long double>(long double dst, long double src)	{ return (long double)src; };
		// ********************************


		// ********************************
		// **** shift helper functions to eliminate warnings.
		template <typename _Type>
		inline _Type dsp_shift_left(_Type src, int n) { if (n < 0 || n > 63) n &= 0x3f; return src << n; };
		template <> inline float		dsp_shift_left(float src, int n)		{ return src; };
		template <> inline double		dsp_shift_left(double src, int n)		{ return src; };
		template <> inline long double	dsp_shift_left(long double src, int n)	{ return src; };

		template <typename _Type>
		inline _Type dsp_shift_right(_Type src, int n) { if (n < 0 || n > 63) n &= 0x3f; return src >> n; };
		template <> inline float		dsp_shift_right(float src, int n)		{ return src; };
		template <> inline double		dsp_shift_right(double src, int n)		{ return src; };
		template <> inline long double	dsp_shift_right(long double src, int n)	{ return src; };
		// ********************************


		// ********************************
		// **** intX to intX converion
		template <typename _TypeDst, typename _TypeSrc>
		inline
			_TypeDst dsp_int_to_int(_TypeDst dst, _TypeSrc src)
		{
				if (dsp::sample_traits<_TypeDst>::is_unsigned != dsp::sample_traits<_TypeSrc>::is_unsigned)
					src += dsp_shift_left<_TypeSrc>(1, ((sizeof(_TypeSrc)* 8) - 1));

				if (sizeof(_TypeDst) > sizeof(_TypeSrc))
					dst = dsp_shift_left<_TypeDst>((_TypeDst)src, (sizeof(_TypeDst)-sizeof(_TypeSrc)) * 8);
				else if (sizeof(_TypeDst) < sizeof(_TypeSrc))
					dst = (_TypeDst)dsp_shift_right<_TypeSrc>(src, (sizeof(_TypeSrc)-sizeof(_TypeDst)) * 8);
				else
					dst = (_TypeDst)src;
				return dst;
			};
		// ********************************


		// ********************************
		// **** intX to float converion
		template <typename _TypeDst, typename _TypeSrc>
		inline
			_TypeDst dsp_int_to_float(_TypeDst dst, _TypeSrc src)
		{
				dst = (_TypeDst)src;
				dst *= dsp::sample_traits<_TypeSrc>::reciprocal();

				if (dsp::sample_traits<_TypeSrc>::is_unsigned)
					dst -= 1.0;
				return dst;
			};
		// ********************************


		// ********************************
		// **** float to intX converion
		template <typename _TypeDst, typename _TypeSrc>
		inline
		_TypeDst dsp_float_to_int(_TypeDst dst, _TypeSrc src)
		{
				// Clamp value to range of destination type.
				if ((long double)src < dsp::sample_traits<_TypeDst>::min())
					src = dsp::sample_traits<_TypeDst>::min();
				if ((long double)src > dsp::sample_traits<_TypeDst>::max())
					src = dsp::sample_traits<_TypeDst>::max();

				// Adjust for unsigned types.
				if (dsp::sample_traits<_TypeDst>::is_unsigned)
					src += 1.0;

				// Apply multiplier.
				src *= dsp::sample_traits<_TypeDst>::multiplier();
				dst = (_TypeDst)src;
				return dst;
			};
		// ********************************
	}
	// **** End dsp::internal namepsace.
	// ********************************


	// ********************************
	// **** dsp::sample - An endianness aware type for DSP operations.
	template <typename _Type = float, bool _Native = true>
	class sample
	{
	private:
		// ********************************
		// **** The actual raw value.
		_Type value;
		// ********************************


		// ********************************
		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native>
		convert_fundamental(_TypeIn src)
		{
			// Sanity check.  This function operates only on fundamental types. Everything else must assert.
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");
			static_assert(sample_traits<_TypeIn>::is_dsp_type, "_TypeIn must be a fundamental dsp type.");

			// Byte swap the source sample if the source sample is not in our native endianness.
			src = dsp::internal::dsp_endianness_to_native<_TypeIn, _NativeIn>(src);

			// Is the source and destination formats both floating-point?
			if (!dsp::sample_traits<_Type>::is_integral && !dsp::sample_traits<_TypeIn>::is_integral)
			{
				// Source and destination are both floating-point.
				// Floating-point formats are in +1 to -1 scalar form.
				value = dsp::internal::dsp_float_to_float<_Type, _TypeIn>(value, src);
			}
			else if (dsp::sample_traits<_Type>::is_integral && dsp::sample_traits<_TypeIn>::is_integral)
			{
				// Source and destination are both integer types.
				value = dsp::internal::dsp_int_to_int<_Type, _TypeIn>(value, src);
			}
			else if (!dsp::sample_traits<_Type>::is_integral && dsp::sample_traits<_TypeIn>::is_integral)
			{
				// Destination is a floating-point value but the source is an integral value.
				value = dsp::internal::dsp_int_to_float<_Type, _TypeIn>(value, src);
			}
			else if (dsp::sample_traits<_Type>::is_integral && !dsp::sample_traits<_TypeIn>::is_integral)
			{
				// Source is floating-point but the destination is an integral value.  MAYBE LOSSY
				value = dsp::internal::dsp_float_to_int<_Type, _TypeIn>(value, src);
			}

			// Byte swap the destination sample if the destination sample is not in our native endianness.
			value = dsp::internal::dsp_endianness_to_native<_Type, _Native>(value);

			return *this;
		}
		// ********************************
		// ********************************


	public:
		// ********************************
		// **** Setup friend classes
		template <typename _Type2, bool _Native2>
		friend class sample;

		template <size_t _Size2, typename _Type2, bool _Native2>
		friend class dsparray;

		template <typename _Type2, bool _Native2, class _Alloc2>
		friend class dspvector;
		// ********************************

		// ********************************
		// **** Constructors.
		sample()
		{
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");
			// Do absolutely nothing to construct this by default.
		};
		// ********************************

		// ********************************
		// **** Construct from sample<>
		template <typename _TypeIn, bool _NativeIn>
		inline
		sample(const sample<_TypeIn, _NativeIn> rhs)
		{
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");
			convert_fundamental<_TypeIn, _NativeIn>(rhs.value);
		};
		// ********************************


		// ********************************
		// **** Note: can use dsp::sample_traits<_Type>::zero()
		template <typename _TypeIn>
		inline
		sample(const _TypeIn rhs, bool native = true)
		{
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");
			if (native)
				convert_fundamental<_TypeIn, true>(rhs);
			else
				convert_fundamental<_TypeIn, false>(rhs);
		};
		// ********************************


		// ********************************
		// **** Get raw value as native endianness
		inline _Type native() const
		{
			sample<_Type> ret = *this;
			return ret.value;
		}
		// ********************************


		// ********************************
		// **** Convert any type to any other type.
		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native>
		operator =
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			return convert_fundamental<_TypeIn, _NativeIn>(rhs.value);
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Convert from fundamental type.
		template <typename _TypeIn>
		inline
		sample<_Type, _Native>
		operator =
		(const _TypeIn rhs)
		{
			return convert_fundamental<_TypeIn, true>(rhs);
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Transform value to a fundamental type.
		#pragma region sample_t_fund_types

		inline operator char() const
		{
			sample<char> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator signed char() const
		{
			sample<signed char> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator unsigned char() const
		{
			sample<unsigned char> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator short() const
		{
			sample<short> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator unsigned short() const
		{
			sample<unsigned short> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator int() const
		{
			sample<int> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator unsigned int() const
		{
			sample<unsigned int> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator long() const
		{
			sample<long> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator unsigned long() const
		{
			sample<unsigned long> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator long long() const
		{
			sample<long long> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator unsigned long long() const
		{
			sample<unsigned long long> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator float() const
		{
			sample<float> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator double() const
		{
			sample<double> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator long double() const
		{
			sample<long double> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		inline operator int24_t() const
		{
			sample<int24_t> ret;
			ret.convert_fundamental<_Type, _Native>(value);
			return ret.value;
		}

		#pragma endregion sample_t_fund_types
		// ********************************
		// ********************************


		// ********************************
		// **** Macro for function that returns a sample type by value after 
		// **** performing an operation with a reference to a sample type.
		#pragma region sample_t_macros

		#define VAL_OP_REF(OPERATOR)								\
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");		\
			static_assert(sample_traits<_TypeIn>::is_dsp_type, "_TypeIn must be a fundamental dsp type.");	\
			sample<_Type, _Native> ret;								\
			if (std::is_floating_point<_Type>::value) {				\
				if (sizeof(_Type) > 8) {							\
					long double td, ts;								\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					ret.convert_fundamental<long double, true>(td);	\
				} else if (sizeof(_Type) > 3) {						\
					double td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					ret.convert_fundamental<double, true>(td);		\
				} else {											\
					float td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					ret.convert_fundamental<float, true>(td);		\
				}													\
			} else {												\
				if (sizeof(_Type) > 4) {							\
					long double td, ts;								\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					ret.convert_fundamental<long double, true>(td); \
				} else if (sizeof(_Type) > 3) {						\
					double td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					ret.convert_fundamental<double, true>(td);		\
				} else {											\
					float td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					ret.convert_fundamental<float, true>(td);		\
				}													\
			}														\
			return ret
		// ********************************

		// ********************************
		// **** Macro for a function that returns by reference a sample type 
		// **** after performing an operation on a reference to a sample type.
		#define REF_OP_REF(OPERATOR)																		\
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");			\
			static_assert(sample_traits<_TypeIn>::is_dsp_type, "_TypeIn must be a fundamental dsp type.");		\
			if (std::is_floating_point<_Type>::value) {				\
				if (sizeof(_Type) > 8) {							\
					sample<long double> td, ts;						\
					td = *this;										\
					ts = rhs;										\
					td.value OPERATOR ts.value;						\
					convert_fundamental<long double, true>(td);		\
				} else if (sizeof(_Type) > 3) {						\
					double td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					convert_fundamental<double, true>(td);			\
				} else {											\
					float td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					convert_fundamental<float, true>(td);			\
				}													\
			} else {												\
				if (sizeof(_Type) > 4) {							\
					long double td, ts;								\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					convert_fundamental<long double, true>(td);		\
				} else if (sizeof(_Type) > 3) {						\
					double td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					convert_fundamental<double, true>(td);			\
				} else {											\
					float td, ts;									\
					td = *this;										\
					ts = rhs;										\
					td OPERATOR ts;									\
					convert_fundamental<float, true>(td);			\
			}	}													\
			return *this
		// ********************************

		// ********************************
		// **** Macro for a function that returns a sample type by value after 
		// **** performing an operation with a fundamental type.
		#define TYP_OP_FUN(OPERATOR)																	\
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");		\
			static_assert(sample_traits<_TypeIn>::is_dsp_type, "_TypeIn must be a fundamental dsp type.");	\
			sample<_Type, _Native> ret;										\
			if (std::is_floating_point<_Type>::value) {						\
				if (sizeof(_Type) > 8) {									\
					sample<long double> td, ts;								\
					td = *this;												\
					ts = rhs;												\
					td.value OPERATOR ts.value;								\
					ret.convert_fundamental<long double, true>(td.value);	\
				} else if (sizeof(_Type) > 3) {								\
					sample<double> td, ts;									\
					td = *this;												\
					ts = rhs;												\
					td.value OPERATOR ts.value;								\
					ret.convert_fundamental<double, true>(td.value);		\
				} else {													\
					sample<float> td, ts;									\
					td = *this;												\
					ts = rhs;												\
					td.value OPERATOR ts.value;								\
					ret.convert_fundamental<float, true>(td.value);			\
				}															\
			} else {														\
				if (sizeof(_Type) > 4) {									\
					sample<long double> td, ts;								\
					td = *this;												\
					ts = rhs;												\
					td.value OPERATOR ts.value;								\
					ret.convert_fundamental<long double, true>(td.value);	\
				} else if (sizeof(_Type) > 3) {								\
					sample<double> td, ts;									\
					td = *this;												\
					ts = rhs;												\
					td.value OPERATOR ts.value;								\
					ret.convert_fundamental<double, true>(td.value);		\
				} else {													\
					sample<float> td, ts;									\
					td = *this;												\
					ts = rhs;												\
					td.value OPERATOR ts.value;								\
					ret.convert_fundamental<float, true>(td.value);			\
			}	}															\
			return ret
		// ********************************

		// ********************************
		// **** Macro for a function that returns by reference a sample type 
		// **** after performing an operation on a const fundamental type.
		#define REF_OP_CTYP(OPERATOR)																	\
			static_assert(sample_traits<_Type>::is_dsp_type, "_Type must be a fundamental dsp type.");		\
			static_assert(sample_traits<_TypeIn>::is_dsp_type, "_TypeIn must be a fundamental dsp type.");	\
			if (std::is_floating_point<_Type>::value) {					\
				if (sizeof(_Type) > 8) {								\
					sample<long double> td, ts;							\
					td = *this;											\
					ts = rhs;											\
					td.value OPERATOR ts.value;							\
					convert_fundamental<long double, true>(td.value);	\
				} else if (sizeof(_Type) > 3) {							\
					sample<double> td, ts;								\
					td = *this;											\
					ts = rhs;											\
					td.value OPERATOR ts.value;							\
					convert_fundamental<double, true>(td.value);		\
				} else {												\
					sample<float> td, ts;								\
					td = *this;											\
					ts = rhs;											\
					td.value OPERATOR ts.value;							\
					convert_fundamental<float, true>(td.value);			\
				}														\
			} else {													\
				if (sizeof(_Type) > 4) {								\
					sample<long double> td, ts;							\
					td = *this;											\
					ts = rhs;											\
					td.value OPERATOR ts.value;							\
					convert_fundamental<long double, true>(td.value);	\
				} else if (sizeof(_Type) > 3) {							\
					sample<double> td, ts;								\
					td = *this;											\
					ts = rhs;											\
					td.value OPERATOR ts.value;							\
					convert_fundamental<double, true>(td.value);		\
				} else {												\
					sample<float> td, ts;								\
					td = *this;											\
					ts = rhs;											\
					td.value OPERATOR ts.value;							\
					convert_fundamental<float, true>(td.value);			\
			}	}														\
			return *this

		#pragma endregion sample_t_macros
		// ********************************
		// ********************************


		// ********************************
		// **** sample operations on sample.
		#pragma region sample_t_sample_t

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native> operator +
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			VAL_OP_REF(+= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native> operator -
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			VAL_OP_REF(-= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native> operator *
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			VAL_OP_REF(*= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native> operator /
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			VAL_OP_REF(/= );
		};
		// ********************************

		// ********************************
		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native>& operator +=
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			REF_OP_REF(+= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native>& operator -=
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			REF_OP_REF(-= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native>& operator *=
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			REF_OP_REF(*= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		sample<_Type, _Native>& operator /=
		(const sample<_TypeIn, _NativeIn> rhs)
		{
			REF_OP_REF(/= );
		};

		#pragma endregion sample_t_sample_t
		// ********************************
		// ********************************


		// ********************************
		// **** operations on a sample using a fundamental type.
		#pragma region sample_t_fundamental

		template <typename _TypeIn>
		inline
		sample<_Type, _Native> operator +
		(const _TypeIn rhs) const
		{
			TYP_OP_FUN(+=);
		};

		template <typename _TypeIn>
		inline
		sample<_Type, _Native> operator -
		(const _TypeIn rhs) const
		{
			TYP_OP_FUN(-=);
		};

		template <typename _TypeIn>
		inline
		sample<_Type, _Native> operator *
		(const _TypeIn rhs) const
		{
			TYP_OP_FUN(*=);
		};

		template <typename _TypeIn>
		inline
		sample<_Type, _Native> operator /
		(const _TypeIn rhs) const
		{
			TYP_OP_FUN(/=);
		};
		// ********************************

		// ********************************
		template <typename _TypeIn>
		inline
		sample<_Type, _Native>& operator +=
		(const _TypeIn rhs)
		{
			REF_OP_CTYP(+= );
		};

		template <typename _TypeIn>
		inline
		sample<_Type, _Native>& operator -=
		(const _TypeIn rhs)
		{
			REF_OP_CTYP(-= );
		};

		template <typename _TypeIn>
		inline
		sample<_Type, _Native>& operator *=
		(const _TypeIn rhs)
		{
			REF_OP_CTYP(*= );
		};

		template <typename _TypeIn>
		inline
		sample<_Type, _Native>& operator /=
		(const _TypeIn rhs)
		{
			REF_OP_CTYP(/= );
		};

		#pragma endregion sample_t_fundamental
		// ********************************
		// ********************************


		// ********************************
		// **** Unary minus and unary plus operators.
		inline sample<_Type, _Native> operator -()
		{
			_Type x = value;
			if (!_Native) x = machine::byte_swap(x);
			return sample<_Type, _Native>(-x);
		}

		inline int operator +()
		{
			return (int)*this;
		}
		// ********************************
		// ********************************


		// ********************************
		// **** Boolean operator not and convert to bool.
		inline operator bool() const   { return value != 0; }
		inline bool operator !() const { return !(value); }
		// ********************************
		// ********************************


		// ********************************
		// **** Macro for a function that returns a bool and compares this sample type
		// **** with VAL.  For comparison operators we convert to an int type and then 
		// **** compare.  This eliminates special cases for floating-point types and 
		// **** allows us to get reasonably accurate comparison results.
		#pragma region sample_t_macro_comparator

		#define BOOL_CMP_VAL(OPERATOR)																\
			if (std::is_floating_point<_Type>::value && std::is_floating_point<_TypeIn>::value) {	\
				if (sizeof(_Type) < 8 || sizeof(_TypeIn) < 8) {										\
					sample<int24_t> l, r;															\
					l = *this;																		\
					r = rhs;																		\
					return l.value OPERATOR r.value;												\
				} else if (sizeof(_Type) == 8 || sizeof(_TypeIn) == 8) {							\
					sample<int32_t> l, r;															\
					l = *this;																		\
					r = rhs;																		\
					return l.value OPERATOR r.value;												\
				} else {																			\
					sample<int64_t> l, r;															\
					l = *this;																		\
					r = rhs;																		\
					return l.value OPERATOR r.value;												\
				}																					\
			} else if (sizeof(_Type) < 2 || sizeof(_TypeIn) < 2) {									\
				sample<int8_t> l, r;																\
				l = *this;																			\
				r = rhs;																			\
				return l.value OPERATOR r.value;													\
			} else if (sizeof(_Type) < 3 || sizeof(_TypeIn) < 3) {									\
				sample<int16_t> l, r;																\
				l = *this;																			\
				r = rhs;																			\
				return l.value OPERATOR r.value;													\
			} else if (sizeof(_Type) < 4 || sizeof(_TypeIn) < 4) {									\
				sample<int24_t> l, r;																\
				l = *this;																			\
				r = rhs;																			\
				return l.value OPERATOR r.value;													\
			} else if (sizeof(_Type) < 8 || sizeof(_TypeIn) < 8) {									\
				sample<int32_t> l, r;																\
				l = *this;																			\
				r = rhs;																			\
				return l.value OPERATOR r.value;													\
			} else {																				\
				sample<int64_t> l, r;																\
				l = *this;																			\
				r = rhs;																			\
				return l.value OPERATOR r.value;													\
			}

		#pragma endregion sample_t_macro_comparator
		// ********************************
		// ********************************


		// ********************************
		// **** Comparison operators on sample
		#pragma region sample_t_comparator_operators

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator ==
		(const sample<_TypeIn, _NativeIn>& rhs) const
		{
			BOOL_CMP_VAL(== );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator !=
		(const sample<_TypeIn, _NativeIn>& rhs) const
		{
			BOOL_CMP_VAL(!= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator >=
		(const sample<_TypeIn, _NativeIn>& rhs) const
		{
			BOOL_CMP_VAL(>= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator <=
		(const sample<_TypeIn, _NativeIn>& rhs) const
		{
			BOOL_CMP_VAL(<= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator >
		(const sample<_TypeIn, _NativeIn>& rhs) const
		{
			BOOL_CMP_VAL(>);
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator <
		(const sample<_TypeIn, _NativeIn>& rhs) const
		{
			BOOL_CMP_VAL(<);
		};
		// ********************************

		// ********************************
		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator ==
		(const _TypeIn rhs) const
		{
			BOOL_CMP_VAL(== );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator !=
		(const _TypeIn rhs) const
		{
			BOOL_CMP_VAL(!= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator >=
		(const _TypeIn rhs) const
		{
			BOOL_CMP_VAL(>= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator <=
		(const _TypeIn rhs) const
		{
			BOOL_CMP_VAL(<= );
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator >
		(const _TypeIn rhs) const
		{
			BOOL_CMP_VAL(>);
		};

		template <typename _TypeIn, bool _NativeIn>
		inline
		bool operator <
		(const _TypeIn rhs) const
		{
			BOOL_CMP_VAL(<);
		};

		#pragma endregion sample_t_comparator_operators
		// ********************************
		// ********************************


		// ********************************
		// **** Clean up macros.
		#undef VAL_OP_REF
		#undef TYP_OP_FUN
		#undef REF_OP_REF
		#undef REF_OP_FREF
		#undef BOOL_CMP_VAL
		// ********************************
		// ********************************
	};
	// **** End dsp::sample
	// ********************************
};
// **** End dsp namespace
// ********************************


// ********************************
// **** Restore previous warning state.
#ifdef POP_WIN32
	#undef POP_WIN32
	#pragma warning( pop )
#endif
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
