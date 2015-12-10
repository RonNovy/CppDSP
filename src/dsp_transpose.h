/* DSP classes to transpose data to/from interleaved to/from non-interleaved.
 * Copyright (C) 2015
 * Ron S. Novy
 *
 *  Class for transposing arrays or vecotrs.  Primarily used to interleave or
 * to de-interleave audio samples.
 */

#pragma once

#include "configure.h"
#include "dsp_containers.h"

#include <array>
#include <vector>


// ********************************
// **** dsp namespace for dsp classes and functions.
namespace dsp
{
	enum {
		deinterleave = true,
		interleave = false
	};

	class transpose_to
	{
	private:
		int rows, cols;
	public:
		// ********************************
		// **** Constructor - 'mode' should be either 'dsp::interleave' or 'dsp::deinterleave'
		transpose_to(int frames, int channels, bool mode)
		{
			if (mode)
			{
				rows = frames;
				cols = channels;
			}
			else
			{
				rows = channels;
				cols = frames;
			}
		}
		~transpose_to() {}
		// ********************************

		// ********************************
		// **** Process() functions.
		template <typename _Type>
		void operator()(_Type *A, _Type *B)
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < cols; ++c)
				{
					B[c * rows + r] = A[r * cols + c];
				}
			}
		}

		template <typename _TypeSrc, size_t _SizeSrc>
		inline void operator()(std::array<_TypeSrc, _SizeSrc> & A, std::array<_TypeSrc, _SizeSrc> & B)
		{
			this->operator()(A.data(), B.data());
		}

		template <typename _TypeSrc, class _AllocSrc>
		inline void operator()(std::vector<_TypeSrc, _AllocSrc> & A, std::vector<_TypeSrc, _AllocSrc> & B)
		{
			this->operator()(A.data(), B.data());
		}

		template <size_t _SizeSrc, typename _TypeSrc, bool _NativeSrc, size_t _SizeDst, typename _TypeDst, bool _NativeDst>
		inline void operator()(dsparray<_SizeSrc, _TypeSrc, _NativeSrc> & A, dsparray<_SizeDst, _TypeDst, _NativeDst> & B)
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < cols; ++c)
				{
					B[c * rows + r] = A[r * cols + c];
				}
			}
		}

		template <typename _TypeSrc, bool _NativeSrc, class _AllocSrc, typename _TypeDst, bool _NativeDst, class _AllocDst>
		inline void operator()(dspvector<_TypeSrc, _NativeSrc, _AllocSrc> & A, dspvector<_TypeDst, _NativeDst, _AllocDst> & B)
		{
			for (int r = 0; r < rows; ++r)
			{
				for (int c = 0; c < cols; ++c)
				{
					B[c * rows + r] = A[r * cols + c];
				}
			}
		}
		// **** End process() functions
		// ********************************
	};


	// ********************************
	// **** debug functions
	template <typename _Type>
	inline void print_mat(_Type *A, int frames, int channels, bool input_interleaved)
	{
		int rows, cols;
		if (input_interleaved)
		{
			rows = frames;
			cols = channels;
			printf("Interleaved matrix:\n");
		}
		else
		{
			rows = channels;
			cols = frames;
			printf("Non-interleaved matrix:\n");
		}
		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				printf("%02X ", A[c + (r * cols)]);
			}
			printf("\n");
		}
		printf("\n");
	}

	template <typename _Type, size_t _Size>
	inline void print_mat(std::array<_Type, _Size> &A, int frames, int channels, bool input_interleaved)
	{
		print_mat(A.data(), frames, channels, input_interleaved);
	}

	template <typename _Type, class _Alloc>
	inline void print_mat(std::vector<_Type, _Alloc> &A, int frames, int channels, bool input_interleaved)
	{
		print_mat(A.data(), frames, channels, input_interleaved);
	}


	template <typename _Type>
	inline void fill_mat(_Type *A, int frames, int channels, bool input_interleaved)
	{
		int rows, cols;
		if (input_interleaved)
		{
			rows = frames;
			cols = channels;
		}
		else
		{
			rows = channels;
			cols = frames;
		}
		for (int r = 0; r < rows; ++r)
		{
			for (int c = 0; c < cols; ++c)
			{
				A[c + (r * cols)] = c + (r * 0x10);
			}
		}
	}

	template <typename _Type, size_t _Size>
	inline void fill_mat(std::array<_Type, _Size> &A, int frames, int channels, bool input_interleaved)
	{
		fill_mat(A.data(), frames, channels, input_interleaved);
	}

	template <typename _Type, class _Alloc>
	inline void fill_mat(std::vector<_Type, _Alloc> &A, int frames, int channels, bool input_interleaved)
	{
		fill_mat(A.data(), frames, channels, input_interleaved);
	}
	// **** End debug functions
	// ********************************
}
// **** End dsp namespace
// ********************************

