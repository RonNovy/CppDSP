/* DSP classes to handle arrays and channels inside of arrays.
 * Copyright (C) 2015
 * Ron S. Novy
 *
 *   This file is a collection of classes that handle arrays of samples
 * and channels within those arrays.  Operations and assignments can be
 * performed between arrays and channels of the same size only.  Although
 * the dspvector class is based on a vector it is tuned to work as a static
 * array would work, so it does not resize when operating on buffers of
 * different sizes.  The one and only exception to the no-resize rule is
 * the '=' assignment operators.
 *
 * Note: See sample.h for a better description of sample
 *
 * TODO: transpose class and in place transpose.
 * TODO?: Should we handle buffers of different sizes instead of assert.
 */
#pragma once

#include "configure.h"

#include <cstdint>
#include <vector>
#include <array>

#include "sample_traits.h"
#include "sample.h"

#ifdef _DEBUG
	#include <assert.h>
	#define DSPASSERT(x) assert(x)
#else
	#define DSPASSERT(x)
#endif


// ********************************
// **** Disable some annoying warnings.
// **** TODO: Find out how to disable warnings on clang/gcc if they pop-up here...
#ifdef _WIN32 // Disable warning 4244.
	#pragma warning( push )
	#pragma warning (disable : 4244 )
	#pragma warning (disable : 4307 )
	#define POP_WIN32 1
#endif
// ********************************


// ********************************
// **** dsp namespace for dsp classes and functions.
namespace dsp
{
	// ********************************
	// **** dsp::dspvector - Prototype for this class.  See below for the definition.
	template <typename _Type, bool _Native, class _Alloc>
	class dspvector;
	// ********************************


	// ********************************
	// **** dsp::channeldef - dsp version of slice for use with dsp style arrays as 'channels'.
	class channeldef
	{
	public:
		typedef int64_t	size_type;

	protected:
		size_type _start;	// Start offset.  Usually == channel number (0 to num_channels-1).
		size_type _size;	// Total number of samples in channel (number of frames in dspvector).
		size_type _stride;	// Offset between samples of this type.

	public:
		// Default construct with all zeros.
		channeldef() : _start(0), _size(0), _stride(0)
		{};

		//   Construct a channeldef using start as offset, with count samples and using
		// stride for the distance between samples.
		channeldef(size_type start, size_type count, size_type stride)
		: _start(start), _size(count), _stride(stride)
		{};

		//   Return start of channel in dspvector (usually the channel number
		// in the range 0 to num_channels - 1).
		size_type start() const
		{
			return _start;
		};

		//   Return total number of samples in the channeldef (is equal to
		// total number of frames in dspvector).
		size_type size() const
		{
			return _size;
		};

		//   Return the distance between two samples (equal to the total number
		// of channels in the dspvector).
		size_type stride() const
		{
			return _stride;
		};
	};
	// **** end dsp::channeldef
	// ********************************


	// ********************************
	// **** dsp::channel_iterator
	template <typename _Type = float, bool _Native = true>
	class channel_iterator : public std::iterator<std::random_access_iterator_tag, sample<_Type, _Native>>
	{
		typedef channel_iterator<_Type, _Native> my_channel_iterator;
		channeldef::size_type _stride;
		pointer p;

	public:
		channel_iterator(pointer x, channeldef::size_type stride) : p(x), _stride(stride) {}
		channel_iterator(const my_channel_iterator& x) : p(x.p), _stride(x._stride) {}

		my_channel_iterator& operator=  (my_channel_iterator& rhs)
		{ p = rhs.p; _stride = rhs._stride; return *this; }

		my_channel_iterator  operator+  (difference_type rhs)		{ return my_channel_iterator(p + (rhs * _stride), _stride); }
		my_channel_iterator  operator-  (difference_type rhs)		{ return my_channel_iterator(p - (rhs * _stride), _stride); }
		difference_type      operator+  (my_channel_iterator rhs)	{ return (p + rhs.p) / _stride; }
		difference_type      operator-  (my_channel_iterator rhs)	{ return (p - rhs.p) / _stride; }

		my_channel_iterator& operator++ () { p += _stride; return *this; }
		my_channel_iterator& operator-- () { p -= _stride; return *this; }

		my_channel_iterator  operator++ (int) { my_channel_iterator tmp(*this); p += _stride; return tmp; }
		my_channel_iterator  operator-- (int) { my_channel_iterator tmp(*this); p -= _stride; return tmp; }

		my_channel_iterator& operator+= (const difference_type n) { p += (n * _stride); }
		my_channel_iterator& operator-= (const difference_type n) { p -= (n * _stride); }
		bool operator <  (const my_channel_iterator& rhs) const { return p <  rhs.p; }
		bool operator >  (const my_channel_iterator& rhs) const { return p >  rhs.p; }
		bool operator <= (const my_channel_iterator& rhs) const { return p <= rhs.p; }
		bool operator >= (const my_channel_iterator& rhs) const { return p >= rhs.p; }
		bool operator == (const my_channel_iterator& rhs) const { return p == rhs.p; }
		bool operator != (const my_channel_iterator& rhs) const { return p != rhs.p; }

		unsigned int operator+() { return (unsigned int)p; }
		value_type& operator * () { return *p; }

		value_type& operator[](channeldef::size_type idx) { return *(p + (idx * _stride)); }
	};
	// **** End dsp::channel_iterator
	// ********************************


	// ********************************
	// **** dsp::channel_array
	template <typename _Type = float, bool _Native = true>
	class channel_array : public dsp::channeldef
	{
	public:
		typedef int64_t	size_type;
		typedef sample<_Type, _Native>			element_type;
		typedef channel_array<_Type, _Native>	channel_type;

		// Iterator types.
		typedef channel_iterator<_Type, _Native>	iterator;
		typedef std::reverse_iterator<iterator>		reverse_iterator;


		// ********************************
		// ********************************
		// **** MACRO, channel operation on this channel.
		// **** Note: For these operators to work, both buffers must be the same size.
		#pragma region channel_array_macros

#if 0
		#define CHANNELOP_CHANNEL(OPERATOR)		\
			DSPASSERT(_size == rhs.size());		\
			auto itrhs = rhs.begin();			\
			auto itlhs = begin();				\
			for (; itlhs != end();				\
				itrhs += rhs._stride,			\
				itlhs += _stride)				\
				*itlhs OPERATOR *itrhs;			\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, dspvector operation on this channel.
		#define CHANNELOP_DSPVECTOR(OPERATOR)	\
			DSPASSERT(_size == rhs.size());		\
			auto itrhs = rhs.begin();			\
			auto itlhs = begin();				\
			for ( ; itlhs != end();				\
				++itrhs, itlhs += _stride)		\
				*itlhs OPERATOR *itrhs;			\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, sample operation on this channel.
		#define CHANNELOP_SAMPLE_T(OPERATOR)	\
			sample<_Type, _Native> x = rhs;		\
			auto itlhs = begin();				\
			for (; itlhs != end();				\
				itlhs += _stride)				\
				*itlhs OPERATOR x;				\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, fundamental type operation on this channel.
		#define CHANNELOP_FUND(OPERATOR)		\
			sample<_Type, _Native> x = rhs;		\
			auto itlhs = begin();				\
			for (; itlhs != end();				\
				itlhs += _stride)				\
				*itlhs OPERATOR x;				\
			return *this
		// ********************************
#else
		#define CHANNELOP_CHANNEL(OPERATOR)		\
			DSPASSERT(_size == rhs._size);		\
			size_type i;						\
			PRAGMA_IF_OPENMP(omp for)			\
			for (i = 0; i < _size; ++i)			\
				*this[i] OPERATOR rhs[i];		\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, dspvector operation on this channel.
		#define CHANNELOP_DSPVECTOR(OPERATOR)		\
			DSPASSERT(_size == rhs.size());			\
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();\
			size_type i;							\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < _size; ++i)				\
				*this[i] OPERATOR rdat[i];			\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, dspvector operation on this channel.
		#define CHANNELOP_DSPARRAY(OPERATOR)		\
			DSPASSERT(_size == rhs.size());			\
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();\
			size_type i;							\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < _size; ++i)				\
				*this[i] OPERATOR rdat[i];			\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, sample operation on this channel.
		#define CHANNELOP_SAMPLE_T(OPERATOR)		\
			sample<_Type, _Native> x = rhs;			\
			size_type i;							\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < _size; ++i)				\
				*this[i].value OPERATOR x.value;	\
			return *this
		// ********************************

		// ********************************
		// **** MACRO, fundamental type operation on this channel.
		#define CHANNELOP_FUND(OPERATOR)			\
			sample<_Type, _Native> x(rhs);			\
			size_type i;							\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < _size; ++i)				\
				*this[i].value OPERATOR x.value;	\
			return *this
		// ********************************

#endif

		#pragma endregion channel_array_macros
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on this channel array using another channel array.
		#pragma region channel_array_channel_array

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator =
		(const channel_array<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_CHANNEL(= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator *=
		(const channel_array<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_CHANNEL(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator /=
		(const channel_array<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_CHANNEL(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator +=
		(const channel_array<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_CHANNEL(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator -=
		(const channel_array<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_CHANNEL(-= );
		};

		#pragma endregion channel_array_channel_array
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on this channel array using a dspvector.
		#pragma region channel_array_dspvector

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		channel_type&
		operator =
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			CHANNELOP_DSPVECTOR(= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		channel_type&
		operator *=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			CHANNELOP_DSPVECTOR(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		channel_type&
		operator /=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			CHANNELOP_DSPVECTOR(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		channel_type&
		operator +=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			CHANNELOP_DSPVECTOR(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		channel_type&
		operator -=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			CHANNELOP_DSPVECTOR(-= );
		};

		#pragma endregion channel_array_dspvector
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on this channel array using a dspvector.
		#pragma region channel_array_dsparray

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator =
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_DSPARRAY(= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator *=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_DSPARRAY(*= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator /=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_DSPARRAY(/= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator +=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_DSPARRAY(+= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator -=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_DSPARRAY(-= );
		};

		#pragma endregion channel_array_dsparray
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on this channel array using a sample type.
		#pragma region channel_array_sample_t

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator =
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_SAMPLE_T(= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator *=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_SAMPLE_T(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator /=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_SAMPLE_T(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator +=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_SAMPLE_T(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		channel_type&
		operator -=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			CHANNELOP_SAMPLE_T(-= );
		};

		#pragma endregion channel_array_sample_t
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on this channel array using a fundamental type.
		#pragma region channel_array_fund

		template <typename _TypeSrc>
		channel_type&
		operator =
		(const _TypeSrc & rhs)
		{
			CHANNELOP_FUND(= );
		};

		template <typename _TypeSrc>
		channel_type&
		operator *=
		(const _TypeSrc & rhs)
		{
			CHANNELOP_FUND(*= );
		};

		template <typename _TypeSrc>
		channel_type&
		operator /=
		(const _TypeSrc & rhs)
		{
			CHANNELOP_FUND(/= );
		};

		template <typename _TypeSrc>
		channel_type&
		operator +=
		(const _TypeSrc & rhs)
		{
			CHANNELOP_FUND(+= );
		};

		template <typename _TypeSrc>
		channel_type&
		operator -=
		(const _TypeSrc & rhs)
		{
			CHANNELOP_FUND(-= );
		};

		#pragma endregion channel_array_fund
		// ********************************
		// ********************************


		// ********************************
		// **** Return element at index 'idx' of this channel array.
		element_type & operator [] (size_t idx) const
		{
			DSPASSERT(idx < _size);
			return *(_Myptr + (idx * _stride) + _start);
		};

		element_type & at(size_t idx) const
		{
			DSPASSERT(idx < _size);
			return *(_Myptr + (idx * _stride) + _start);
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Get rid of public constructors.
#if defined (_MSC_VER) && (_MSC_VER >= 1800)
		channel_array() = delete;
		channel_array(const channel_array<_Type, _Native>& ch);// not defined
		channel_array& operator=(const channel_array& ch);	// not defined
#endif
		// ********************************
		// ********************************


		// ********************************
		// **** Iterator functions.
		#pragma region channel_array_iterators

		// ********************************
		// **** begin() for iterating through a channel_array.
		iterator begin() const
		{
			return iterator(_Myptr + _start, _stride);
		}
		// ********************************

		// ********************************
		// **** end() for iterating through a channel_array.
		iterator end() const
		{
			return iterator(_Myptr + (_start + (_size * _stride)), _stride);
		}
		// ********************************

		// ********************************
		// **** rbegin() for iterating through a channel_array in reverse.
		reverse_iterator rbegin() const
		{
			return reverse_iterator(iterator(_Myptr + (_start + (_size * _stride)), _stride));
		}
		// ********************************

		// ********************************
		// **** rend() for iterating through a channel_array in reverse.
		reverse_iterator rend() const
		{
			return reverse_iterator(iterator(_Myptr + _start, _stride));
		}
		#pragma endregion channel_array_iterators
		// ********************************
		// ********************************

	private:
		template <typename _Type2, bool _Native2, class _Alloc2>
		friend class dspvector;
		template <size_t _Size2, typename _Type2, bool _Native2>
		friend class dsparray;

		// ********************************
		// **** Construct channel_array and pointer to dspvector contents
		channel_array(const channeldef& _Channel, element_type *_dspVector)
		: channeldef(_Channel), _Myptr(_dspVector)
		{};

		element_type *_Myptr;	// Reference to dspvector class


		// ********************************
		// **** Clean up macros.
		#undef CHANNELOP_CHANNEL
		#undef CHANNELOP_DSPVECTOR
		#undef CHANNELOP_SAMPLE_T
		#undef CHANNELOP_FUND
		// ********************************
	};
	// **** End dsp::channel_array
	// ********************************


	// ********************************
	// ****   Class for handling basic summary information.
	// **** and providing a decent default.
	class dspformat
	{
	private:
		size_t	frames;			// Number of frames in each buffer.
		int		rate;			// Frame rate.
		int		num_channels;	// Number of channels.
		int		bits;			// Bits per sample.
		bool	floating_point;	// Are the samples floating-point?
		bool	interleaved;	// Are the channels interleaved?

		std::vector<channeldef> channels;
		inline void construct_channels()
		{
			// Construct the vector of channels.
			channels.clear();

			if (interleaved)
			{
				for (int i = 0; i < num_channels; ++i)
					channels.emplace_back(i, frames, num_channels);
			}
			else
			{
				for (int i = 0; i < num_channels; ++i)
					channels.emplace_back(i * frames, frames, 1);
			}
		}
	public:
		dspformat(
			uint64_t	_frames = 0,
			int			_rate = 0,
			int			_channels = 0,
			int			_bits = 0,
			bool		_floating_point = false,
			bool		_interleaved = true) :
			frames(_frames),
			rate(_rate),
			num_channels(_channels),
			bits(_bits),
			floating_point(_floating_point),
			interleaved(_interleaved)
		{
			construct_channels();
		};
		~dspformat() {};

		inline void set_frames(uint64_t _frames)		{ frames = _frames; }
		inline void set_rate(int _rate)					{ rate = _rate; }
		inline void set_channels(int num)				{ num_channels = num; construct_channels(); }
		inline void set_bits(int _bits)					{ bits = _bits; }
		inline void set_float(bool is_float)			{ floating_point = is_float; }
		inline void set_interleaved(bool _interleaved)	{ interleaved = _interleaved; construct_channels(); }

		inline uint64_t	get_frames()		const	{ return frames; }
		inline int		get_rate()			const	{ return rate; }
		inline int		get_channels()		const	{ return num_channels; }
		inline int		get_bits()			const	{ return bits; }
		inline bool		is_floats()			const	{ return floating_point; }
		inline bool		is_interleaved()	const	{ return interleaved; }

		inline const channeldef & get_channeldef(int idx)	const { return channels[idx]; }

		inline uint64_t get_total_samples()		const	{ return frames * num_channels; }
		inline uint64_t get_sizeof_sample()		const	{ return ((bits + 7) / 8); }
		inline uint64_t get_sizeof_frame()		const	{ return get_sizeof_sample() * num_channels; }
		inline uint64_t get_bytesper_second()	const	{ return get_sizeof_frame() * rate; }
		inline double	get_length_seconds()	const	{ return static_cast<double>(frames) / static_cast<double>(rate); }
	};
	// ********************************
	// ********************************


	// ********************************
	// **** dsp::dsparray - An array of 'sample<type, endianness>' samples ready for manipulation
	template <size_t _Size, typename _Type = float, bool _Native = true>
	class dsparray : public std::array<sample<_Type, _Native>, _Size>
	{
	private:
	public:
		typedef sample<_Type, _Native> element_type;
		typedef int64_t	size_type;
		typedef int64_t	difference_type;

		// ********************************
		// **** Zero out array by filling with the types real zero value.
		inline void zero()
		{
			sample<_Type, _Native> rhs = sample_traits<_Type>::zero();
			fill(rhs.value);
		};
		// ********************************


		// ********************************
		// **** operator[] taking a channeldef as a parameter and returning a channel_array.
		inline channel_array<_Type, _Native> operator[](const channeldef &ch)
		{
			return channel_array<_Type, _Native>(ch, /*const_cast<sample<_Type, _Native>*>*/(data()));
		}
		// ********************************


		// ********************************
		// **** Macros for all the operator functions.
		// **** Note: For these operators to work, both buffers must be the same size.
		#pragma region dsparray_macros

		#define DSPARRAYOP_DSPARRAY(OPERATOR)						\
			static_assert(_Size == _SizeSrc, "Size of arrays must be equal to perform this operation.");\
			size_type i;											\
			sample<_Type, _Native> *ldat = data();					\
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();	\
			PRAGMA_IF_OPENMP(omp for)								\
			for (i = 0; i < _Size; ++i)								\
				ldat[i] OPERATOR rdat[i];							\
			return *this

		#define DSPARRAYOP_DSPVECTOR(OPERATOR)						\
			DSPASSERT(_Size == rhs.size());							\
			size_type i;											\
			sample<_Type, _Native> *ldat = data();					\
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();	\
			PRAGMA_IF_OPENMP(omp for)								\
			for (i = 0; i < _Size; ++i)								\
				ldat[i] OPERATOR rdat[i];							\
			return *this

		#define DSPARRAYOP_DSPCHANNEL_ARRAY(OPERATOR)	\
			DSPASSERT(_Size == rhs.size());				\
			size_type i;								\
			sample<_Type, _Native> *ldat = data();		\
			PRAGMA_IF_OPENMP(omp for)					\
			for (i = 0; i < _Size; ++i)					\
				ldat[i] OPERATOR rhs[i];				\
			return *this

		#define DSPARRAYOP_SAMPLE_TYPE(OPERATOR)	\
			size_type i;							\
			sample<_Type, _Native> x = rhs;			\
			sample<_Type, _Native> *ldat = data();	\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < _Size; ++i)				\
				ldat[i].value OPERATOR x.value;		\
			return *this

		#define DSPARRAYOP_FUNDAMENTAL(OPERATOR)	\
			size_type i;							\
			sample<_Type, _Native> x(rhs);			\
			sample<_Type, _Native> *ldat = data();	\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < _Size; ++i)				\
				ldat[i].value OPERATOR x.value;		\
			return *this

		#pragma endregion dsparray_macros
		// ********************************
		// ********************************


		// ********************************
		// **** dsparray to dsparray operations
		#pragma region dsparray_dsparray

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator =
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPARRAY(= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator *=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPARRAY(*= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator /=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPARRAY(/= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator +=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPARRAY(+= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator -=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPARRAY(-= );
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		bool
		operator ==
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs) const
		{
			//static_assert(_Size == _SizeSrc, "Size of arrays must be equal to perform this operation.");
			if (_Size != rhs.size())
				return false;

			size_type i;
			const sample<_Type, _Native> *ldat = data();
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();
			//PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < _Size; ++i)
			{
				if (ldat[i] != rdat[i])
					return false;
			}
			return true;
		};

		#pragma endregion dsparray_dsparray
		// ********************************
		// ********************************


		// ********************************
		// **** dspvector to dsparray operations
		#pragma region dsparray_dspvector

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dsparray<_Size, _Type, _Native>&
		operator =
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs)
		{
			DSPARRAYOP_DSPVECTOR(= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dsparray<_Size, _Type, _Native>&
		operator *=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs)
		{
			DSPARRAYOP_DSPVECTOR(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dsparray<_Size, _Type, _Native>&
		operator /=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs)
		{
			DSPARRAYOP_DSPVECTOR(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dsparray<_Size, _Type, _Native>&
		operator +=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs)
		{
			DSPARRAYOP_DSPVECTOR(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dsparray<_Size, _Type, _Native>&
		operator -=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs)
		{
			DSPARRAYOP_DSPVECTOR(-= );
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		bool
		operator ==
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs) const
		{
			//static_assert(_Size == _SizeSrc, "Size of arrays must be equal to perform this operation.");
			if (_Size != rhs.size())
				return false;

			size_type i;
			const sample<_Type, _Native> *ldat = data();
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();
			//PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < _Size; ++i)
			{
				if (ldat[i] != rdat[i])
					return false;
			}
			return true;
		};

		#pragma endregion dsparray_dspvector
		// ********************************
		// ********************************


		// ********************************
		// **** channel_array to dsparray operations
		#pragma region dsparray_channel_array

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator =
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPCHANNEL_ARRAY(= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator *=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPCHANNEL_ARRAY(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator /=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPCHANNEL_ARRAY(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator +=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPCHANNEL_ARRAY(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator -=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_DSPCHANNEL_ARRAY(-= );
		};

		#pragma endregion dsparray_channel_array
		// ********************************
		// ********************************


		// ********************************
		// **** sample<> to dsparray operations
		#pragma region dsparray_sample_type

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator =
		(const sample<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_SAMPLE_TYPE(= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator *=
		(const sample<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_SAMPLE_TYPE(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator /=
		(const sample<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_SAMPLE_TYPE(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator +=
		(const sample<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_SAMPLE_TYPE(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dsparray<_Size, _Type, _Native>&
		operator -=
		(const sample<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPARRAYOP_SAMPLE_TYPE(-= );
		};

		#pragma endregion dsparray_sample_type
		// ********************************
		// ********************************


		// ********************************
		// **** Fundamental type to dsparray operations
		#pragma region dsparray_fundamental

		template <typename _TypeSrc>
		dsparray<_Size, _Type, _Native>&
		operator =
		(const _TypeSrc rhs)
		{
			DSPARRAYOP_FUNDAMENTAL(= );
		};

		template <typename _TypeSrc>
		dsparray<_Size, _Type, _Native>&
		operator *=
		(const _TypeSrc rhs)
		{
			DSPARRAYOP_FUNDAMENTAL(*= );
		};

		template <typename _TypeSrc>
		dsparray<_Size, _Type, _Native>&
		operator /=
		(const _TypeSrc rhs)
		{
			DSPARRAYOP_FUNDAMENTAL(/= );
		};

		template <typename _TypeSrc>
		dsparray<_Size, _Type, _Native>&
		operator +=
		(const _TypeSrc rhs)
		{
			DSPARRAYOP_FUNDAMENTAL(+= );
		};

		template <typename _TypeSrc>
		dsparray<_Size, _Type, _Native>&
		operator -=
		(const _TypeSrc rhs)
		{
			DSPARRAYOP_FUNDAMENTAL(-= );
		};

		#pragma endregion dsparray_fundamental
		// ********************************
		// ********************************


		// ********************************
		// **** Clean up macros
		#undef DSPARRAYOP_DSPARRAY
		#undef DSPARRAYOP_DSPVECTOR
		#undef DSPARRAYOP_DSPCHANNEL_ARRAY
		#undef DSPARRAYOP_SAMPLE_TYPE
		#undef DSPARRAYOP_FUNDAMENTAL
		// ********************************
	};
	// **** dsp::dsparray
	// ********************************


	// ********************************
	// **** dsp::dspvector - A vector of 'sample<type, endianness>' samples ready for manipulation
	template <typename _Type = float, bool _Native = true, class _Alloc = std::allocator< sample<_Type, _Native>> >
	class dspvector : public std::vector<sample<_Type, _Native>, _Alloc>
	{
	public:

		typedef sample<_Type, _Native>	element_type;
		typedef typename int64_t		size_type;
		typedef typename int64_t		difference_type;

		// ********************************
		// **** Constructor.
		// **** Note: We only accept construction with a size.
		dspvector(int num_samples = 0)
		{
			resize(num_samples);
		};
		~dspvector() {};
		// ********************************


		// ********************************
		// **** Zero out array using the types real zero value.
		// **** Note: This is different from clear() which destroys the data buffer.
		inline void zero()
		{
			sample<_Type, _Native> rhs = sample_traits<_Type>::zero();
			sample<_Type, _Native> *ldat = data();
			size_type i, lhssize = size();
			PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < lhssize; ++i)
				ldat[i].value = rhs.value;
		};
		// ********************************


		// ********************************
		// **** operator[] taking a channeldef as a parameter and returning a channel_array.
		inline channel_array<_Type, _Native> operator[](const channeldef &ch)
		{
			return channel_array<_Type, _Native>(ch, /*const_cast<sample<_Type, _Native>*>*/(data()));
		}
		// ********************************


		// ********************************
		// **** operator[] taking a channeldef as a parameter and returning a channel_array.
		inline element_type & operator[](size_type x)
		{
			return data()[x];
		};
		// ********************************


		// ********************************
		// **** Macros for all the operator functions.
		#pragma region dspvector_macros

		#define DSPVECTOROP_DSPVECTOR(OPERATOR, OPERATOR2)	\
			size_type lhssize = size();						\
			size_type rhssize = rhs.size();					\
			size_type i = 0, j = 0;							\
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();\
			if (lhssize < rhssize) {						\
				resize(rhssize);							\
				sample<_Type, _Native> *ldat = data();		\
				PRAGMA_IF_OPENMP(omp for)					\
				for (i = 0; i < lhssize; ++i)				\
					ldat[i] OPERATOR rdat[i];				\
				PRAGMA_IF_OPENMP(omp for)					\
				for (j = 0; j < (rhssize - lhssize); ++j)	\
					ldat[i + j] OPERATOR2;					\
			} else {										\
				if (lhssize != rhssize) resize(rhssize);	\
				sample<_Type, _Native> *ldat = data();		\
				PRAGMA_IF_OPENMP(omp for)					\
				for (i = 0; i < rhssize; ++i)				\
					ldat[i] OPERATOR rdat[i];				\
			} return *this

		#define DSPVECTOROP_DSPARRAY(OPERATOR, OPERATOR2)	\
			size_type lhssize = size();						\
			size_type rhssize = rhs.size();					\
			size_type i = 0, j = 0;							\
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data(); \
			if (lhssize < rhssize) {						\
				resize(rhssize);							\
				sample<_Type, _Native> *ldat = data();		\
				PRAGMA_IF_OPENMP(omp for)					\
				for (i = 0; i < lhssize; ++i)				\
					ldat[i] OPERATOR rdat[i];				\
				PRAGMA_IF_OPENMP(omp for)					\
				for (j = 0; j < (rhssize - lhssize); ++j)	\
					ldat[i + j] OPERATOR2;					\
			} else {										\
				if (lhssize != rhssize) resize(rhssize);	\
				sample<_Type, _Native> *ldat = data();		\
				PRAGMA_IF_OPENMP(omp for)					\
				for (i = 0; i < rhssize; ++i)				\
					ldat[i] OPERATOR rdat[i];				\
			} return *this

		#define DSPVECTOROP_CHANNEL(OPERATOR, OPERATOR2)	\
			size_type lhssize = size();						\
			size_type rhssize = rhs.size();					\
			size_type i = 0, j = 0;							\
			auto rdat = rhs.begin();						\
			if (lhssize < rhssize) {						\
				resize(rhssize);							\
				sample<_Type, _Native> *ldat = data();		\
				PRAGMA_IF_OPENMP(omp for)					\
				for (i = 0; i < lhssize; ++i)				\
					ldat[i] OPERATOR *rdat++;				\
				PRAGMA_IF_OPENMP(omp for)					\
				for (j = 0; j < (rhssize - lhssize); ++j)	\
					ldat[i + j] OPERATOR2;					\
			}												\
			else {											\
				if (lhssize != rhssize) resize(rhssize);	\
				sample<_Type, _Native> *ldat = data();		\
				PRAGMA_IF_OPENMP(omp for)					\
				for (i = 0; i < rhssize; ++i)				\
					ldat[i] OPERATOR *rdat++;				\
			} return *this

		#define DSPVECTOROP_SAMPLE_T(OPERATOR)		\
			sample<_TypeSrc, _NativeSrc> x = rhs;	\
			sample<_Type, _Native> *ldat = data();	\
			size_type i, lhs_size = size();			\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < lhs_size; ++i)			\
				ldat[i].value OPERATOR rhs.value;	\
			return *this

		#define DSPVECTOROP_FUND(OPERATOR)			\
			sample<_TypeSrc, true> x(rhs);			\
			sample<_Type, _Native> *ldat = data();	\
			size_type i, lhs_size = size();			\
			PRAGMA_IF_OPENMP(omp for)				\
			for (i = 0; i < lhs_size; ++i)			\
				ldat[i].value OPERATOR x.value;		\
			return *this

		#pragma endregion dspvector_macros
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on dspvector using another dspvector.
		#pragma region dspvector_dspvector

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator =
		(dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs)
		{
			DSPVECTOROP_DSPVECTOR(= , = rdat[i]);
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator *=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			DSPVECTOROP_DSPVECTOR(*= , = sample_traits<_Type>::zero());
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator /=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			DSPVECTOROP_DSPVECTOR(/= , = sample_traits<_Type>::zero());
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator +=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			DSPVECTOROP_DSPVECTOR(+= , = rdat[i]);
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator -=
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc>& rhs)
		{
			DSPVECTOROP_DSPVECTOR(-= , = -rdat[i]);
		};

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc>
		bool
		operator ==
		(const dspvector<_TypeSrc, _NativeSrc, _AllocSrc> &rhs) const
		{
			size_type i, lhssize = size();
			if (lhssize != rhs.size())
				return false;

			const sample<_Type, _Native> *ldat = data();
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();
			//PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < lhssize; ++i)
			{
				if (ldat[i] != rdat[i])
					return false;
			}
			return true;
		};

		#pragma endregion dspvector_dspvector
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on dspvector using a channel_array.
		#pragma region dspvector_dsparray

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator =
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_DSPARRAY(= , = rdat[i]);
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator *=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_DSPARRAY(*= , = sample_traits<_Type>::zero());
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator /=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_DSPARRAY(/= , = sample_traits<_Type>::zero());
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator +=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_DSPARRAY(+= , = rdat[i]);
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator -=
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_DSPARRAY(-= , = -rdat[i]);
		};

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc>
		bool
		operator ==
		(const dsparray<_SizeSrc, _TypeSrc, _NativeSrc> &rhs) const
		{
			size_type i, lhssize = size();
			if (lhssize != rhs.size())
				return false;

			const sample<_Type, _Native> *ldat = data();
			const sample<_TypeSrc, _NativeSrc> *rdat = rhs.data();
			//PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < lhssize; ++i)
			{
				if (ldat[i] != rdat[i])
					return false;
			}
			return true;
		};

		#pragma endregion dspvector_dsparray
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on dspvector using a channel_array.
		#pragma region dspvector_channel_array

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator =
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_CHANNEL(= , = *rdat++);
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator *=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_CHANNEL(*= , = sample_traits<_Type>::zero());
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator /=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_CHANNEL(/= , = sample_traits<_Type>::zero());
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator +=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_CHANNEL(+= , = *rdat++);
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator -=
		(const channel_array<_TypeSrc, _NativeSrc> &rhs)
		{
			DSPVECTOROP_CHANNEL(-= , = -*rdat++);
		};

		#pragma endregion dspvector_channel_array
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on dspvector with sample
		#pragma region dspvector_sample_t
		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator =
		(const sample<_TypeSrc, _NativeSrc> &rhs)
		{
			element_type x = rhs;
			size_type i, lhssize = size();
			PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < lhssize; ++i)
				*this[i].value = x.value;

			return *this;
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator *=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			DSPVECTOROP_SAMPLE_T(*= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator /=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			DSPVECTOROP_SAMPLE_T(/= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator +=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			DSPVECTOROP_SAMPLE_T(+= );
		};

		template <typename _TypeSrc, bool _NativeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator -=
		(const sample<_TypeSrc, _NativeSrc>& rhs)
		{
			DSPVECTOROP_SAMPLE_T(-= );
		};

		#pragma endregion dspvector_sample_t
		// ********************************
		// ********************************


		// ********************************
		// **** Operations on dspvector using a fundamental type.
		#pragma region dspvector_fund
		template <typename _TypeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator =
		(const _TypeSrc rhs)
		{
			static_assert(sample_traits<_TypeSrc>::is_dsp_type, "_TypeSrc must be a fundamental dsp type.");
			sample<_Type, _Native> x(rhs), *ldat = data();
			size_type i, lhssize = size();
			PRAGMA_IF_OPENMP(omp for)
			for (i = 0; i < lhssize; ++i)
				ldat[i].value = x.value;

			return *this;
		};

		template <typename _TypeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator *=
		(const _TypeSrc rhs)
		{
			DSPVECTOROP_FUND(*= );
		};

		template <typename _TypeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator /=
		(const _TypeSrc rhs)
		{
			DSPVECTOROP_FUND(/= );
		};

		template <typename _TypeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator +=
		(const _TypeSrc rhs)
		{
			DSPVECTOROP_FUND(+= );
		};

		template <typename _TypeSrc>
		dspvector<_Type, _Native, _Alloc>&
		operator -=
		(const _TypeSrc rhs)
		{
			DSPVECTOROP_FUND(-= );
		};

		#pragma endregion dspvector_fund
		// ********************************
		// ********************************


		// ********************************
		// **** TODO: vector<bool> operators???
		//vector<bool> operator!() const;//???
		// ********************************
		// ********************************

		// ********************************
		// **** Clean up
		#undef DSPVECTOROP_DSPVECTOR
		#undef DSPVECTOROP_DSPARRAY
		#undef DSPVECTOROP_CHANNEL
		#undef DSPVECTOROP_SAMPLE_T
		#undef DSPVECTOROP_FUND
		// ********************************
	};
	// **** End dsp::dspvector
	// ********************************
}
// **** End dsp namespace
// ********************************


// ********************************
#undef DSPASSERT
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
