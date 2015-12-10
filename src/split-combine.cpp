/* Split/Combine code.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#include "configure.h"

#include <vector>

#include "split-combine.h"

 
 // ********************************
 // **** Import functions from sf_private
extern "C" {
	long long sfp_get_dataoffset(SNDFILE *sf);
	long long sfp_get_datalength(SNDFILE *sf);
	long long sfp_get_dataend(SNDFILE *sf);
}
// ********************************


#if 0//_MSC_VER >= 1900
	#define regular_file std::tr2::sys::file_type::regular
#else
	#define regular_file std::tr2::sys::file_type::regular_file
#endif


// ********************************
// **** dsp namespace for dsp classes and functions.
namespace dsp
{
	// ********************************
	// **** dsp_split_file - Class to split a file.
	dsp_split_combine::dsp_split_combine() { clear(); }
	dsp_split_combine::~dsp_split_combine() {}
	const char *dsp_split_combine::get_error_str(void) { return error.c_str(); }

	// ********************************
	// **** Clear inputs and outputs and reset the entire class.
	bool dsp_split_combine::clear()
	{
		input.clear();
		output.clear();
		error.clear();
		format_override = false;
		out_format = dsp::dspformat();
		out_sf_format = SF_FORMAT_WAV;
		return true;
	}


	// ********************************
	// **** This function adds an input file and returns data about the file.
	bool dsp_split_combine::add_input_ex(
		const char *name,			// Path to and name of file.
		int &Channels,				// Total number of channels.
		int &SampleSize,			// Bits per sample.
		int &FrameSize,				// Size of a single frame of audio (Channels * BytesPerSample).
		int &SampleRate,			// Sample rate.
		int &Float,					// True or false. Changed to Float from mFormat.
		int &ByteOrder,				// Endianness.
		int &dataOffset,			// File offset of sample data
		unsigned long &dataSize,	// Size of sample data in bytes
		int &HasBWF,				// 0 for no BWF and 1 for has BWF
		int &MediaType,				// 1 = wav, 2 = aif;
		SF_BROADCAST_INFO &bext
	)
	{
		// Create a path object.
		std::sys::path path(name);

		// Check that the file exists.
		if (!std::sys::exists(path))
		{
			error = "Could not find the file: " + path.string() + "\n";
			return false;
		}

		// Check that we have a path to a regular file and not some special file...
		if (std::sys::status(path).type() != regular_file)
		{
			error = "File does not appear to be a regular file: " + path.string() + "\n";
			return false;
		}

		// Open the file to check that we can read the file.
		dsp::dspfile tmp;
		tmp.open(path.string());
		if (!tmp.is_open())
		{
			error = "Could not open the file: " + path.string() + "\n";

			// Add the dspfile error string.
			error += tmp.get_error_string();
			error += "\n";
			return false;
		}

		// Get data calling function...
		dsp::dspformat fmt = tmp.get_dspformat();
		Channels	=	tmp.get_channels();		// Total number of channels.
		SampleSize	=	fmt.get_bits();			// Bits per sample.
		FrameSize	=	((SampleSize + 7) / 8) * Channels; // Size of a single frame of audio (Channels * BytesPerSample).
		SampleRate	=	tmp.get_samplerate();	// Sample rate.
		Float		=	fmt.is_floats();		// Changed to Float from mFormat,// ???

		ByteOrder = (tmp.get_format() & SF_FORMAT_ENDMASK) >> 28;		// Endianness.
		switch (ByteOrder)
		{
		case 0:
			if (tmp.command(SFC_RAW_DATA_NEEDS_ENDSWAP, 0, 0))
				ByteOrder = (LITTLE_ENDIAN ? 1 : 0);
			else
				ByteOrder = (LITTLE_ENDIAN ? 0 : 1);

			break;
			// Files endianness
		case 1:	ByteOrder = 0; break;	// Little-endian
		case 2: ByteOrder = 1; break;	// Big-endian
		case 3: ByteOrder = (LITTLE_ENDIAN ? 0 : 1); break;	// CPU endianness
		}

		//SF_BROADCAST_INFO bext;
		HasBWF = tmp.command(SFC_GET_BROADCAST_INFO, &bext, sizeof(bext));	// 0 for no BWF and 1 for has BWF
		MediaType = (tmp.get_format() & SF_FORMAT_TYPEMASK) >> 16;			// 1 = wav, 2 = aif;

		//int &dataOffset,	// ???
		dataOffset = dataSize = 0;
		tmp.seek(0, SF_SEEK_SET);
		dataOffset = (int)sfp_get_dataoffset(tmp.get_sndfile_ptr());
		dataSize = (unsigned long)sfp_get_datalength(tmp.get_sndfile_ptr());//((unsigned int)tmp.get_frames() * FrameSize);

		// If this is the first input we need to set the default output format.
		if (input.size() == 0)
		{
			out_format = fmt;
			out_sf_format = tmp.get_format();
		}

		// Add the file to the input list
		input.emplace_back(path, tmp, fmt);
		return true;
	}


	// ********************************
	// **** This function adds an input file and sets 'channels' to the
	// **** number of channels detected in the input file.
	bool dsp_split_combine::add_input(const char *name, int &Channels)
	{
		SF_BROADCAST_INFO bext;
		int
			//Channels,		// Total number of channels.
			SampleSize,		// Bits per sample.
			FrameSize,		// Size of a single frame of audio (Channels * BytesPerSample).
			SampleRate,		// Sample rate.
			Float,			// True or false.
			ByteOrder,		// Endianness.
			dataOffset,		// File offset of sample data
			HasBWF,			// 0 for no BWF and 1 for has BWF
			MediaType;		// 1 = wav, 2 = aif;
		unsigned long dataSize;	// Size of sample data in bytes
		return add_input_ex(name, Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, dataSize, HasBWF, MediaType, bext);
	}


	// ********************************
	// **** Add an output file name.
	bool dsp_split_combine::add_output_path(std::sys::path &path, int fmtcodec, int rate) // By path
	{
		dsp::dspformat fmt;
		// Handle fmtcodec
		if (fmtcodec)
			fmt = input[0].file.get_dspformat_from(fmtcodec, rate);
		fmt.set_rate(rate);
		output.emplace_back(path, fmt);
		return true;
	}
	bool dsp_split_combine::add_output(const char *name, int fmtcodec, int rate) // By C string
	{
		// Create a path object.
		std::sys::path path(name);
		return add_output_path(path, fmtcodec, rate);
	}
	// ********************************


	// ********************************
	// ********************************
	// **** This function will return the number of frames for a single buffer.
	template <typename _Type>
	unsigned int dsp_split_combine::get_buffer_length()
	{
		return (1*2*3*4*5*6*7*8);	// What is a good size for this???
	}


	// ********************************
	// **** A private template function used to run the actual split.
	template <typename _TypeSrc, typename _TypeDst>
	void dsp_split_combine::split_template()
	{
		// Get number of frames to read each round.  And number of channels.
		int frames = get_buffer_length<_TypeDst>();
		int channels = input[0].format.get_channels();

		// Main buffer.
		dsp::dspvector<_TypeSrc> inbuffer(frames * channels);
		dsp::dspvector<_TypeDst> outbuffer(frames * channels);
		std::vector<_TypeDst *>  chptr(channels);

		// Setup some pointers to each channel in non-interleaved space.
		for (int i = 0; i < channels; ++i)
			chptr[i] = (_TypeDst*)((unsigned char*)outbuffer.data() + (frames * i * sizeof(_TypeDst)));

		// Setup transposition process.
		dsp::transpose_to de_interleave(frames, channels, deinterleave);

		// Main loop:
		int rframes;
		while ((rframes = (int)input[0].file.read_frames<_TypeSrc>((_TypeSrc*)inbuffer.data(), frames)) == frames) // Read input.
		{
			// Transpose and convert type.
			de_interleave(inbuffer, outbuffer);

			// Write output.  FIXME: We should really log and report errors while writing.
			for (int i = 0; i < channels; ++i)
				output[i].file.write_frames<_TypeDst>(chptr[i], rframes);
		}

		// Handle leftovers...
		if (rframes > 0)
		{
			dsp::transpose_to de_interleave_leftovers(rframes, channels, deinterleave);
			de_interleave_leftovers(inbuffer, outbuffer);

			// Write output.  FIXME: We should really log and report errors while writing.
			for (int i = 0; i < channels; ++i)
				output[i].file.write_frames<_TypeDst>((_TypeDst*)((unsigned char*)outbuffer.data() + (rframes * i * sizeof(_TypeDst))), rframes);
		}
	}


	// ********************************
	// **** Template for the combine process.
	template <typename _TypeSrc, typename _TypeDst>
	void dsp_split_combine::combine_template()
	{
		// Get number of frames to read each round, number of channels etc...
		bool do_complex_method = false;
		bool done = false;
		int channels = 0;
		int i, cur_c, rframes, maxframes = 0;
		int frames = get_buffer_length<_TypeDst>();
		int num_inputs = input.size();

		// Calculate number of output channels and decide what method to use.
		for (i = 0; i < num_inputs; ++i)
		{
			int c = input[i].format.get_channels();
			channels += c;
			if (c != 1)
				do_complex_method = true;
		}

		if (!do_complex_method)
		{
			// Simple method...
			// Setup interleave functor.
			dsp::transpose_to interleave(frames, channels, dsp::interleave);

			// Create buffers.
			dsp::dspvector<_TypeSrc> inbuffer(frames * num_inputs);
			dsp::dspvector<_TypeDst> outbuffer(frames * num_inputs);

			// Run loop.
			while (!done)
			{
				done = true;
				maxframes = 0;

				for (i = 0; i < num_inputs; ++i)
				{
					// Read in buffer.
					if ((rframes = (int)input[i].file.read_frames<_TypeSrc>((_TypeSrc*)(inbuffer.data() + (frames * i)), frames)) == frames)
						done = false;

					// Zero out end of buffer if necessary.
					for (int x = rframes; x < frames; ++x)
						inbuffer[(frames * i) + x] = dsp::sample_traits<_TypeSrc>::zero();

					// Set max frames.
					if (rframes > maxframes)
						maxframes = rframes;

				} // for (i = 0; i < num_inputs; ++i)

				if (maxframes)
				{
					// Finally, transpose input buffer to output buffer.
					interleave(inbuffer, outbuffer);

					// And write to output file.
					output[0].file.write_frames<_TypeDst>((_TypeDst*)outbuffer.data(), maxframes);

				} // if (maxframes)
			} // while (!done)
		}
		else
		{
			// Complex method is below and the simple method is above.
			// Create input/mid buffers.
			std::vector<dsp::dspvector<_TypeDst>> inbuffers(num_inputs);
			std::vector<dsp::dspvector<_TypeDst>> midbuffers(num_inputs);

			// Calculate number of output channels
			for (i = 0; i < num_inputs; ++i)
			{
				int c = input[i].format.get_channels();
				inbuffers[i].resize(frames * c);
				midbuffers[i].resize(frames * c);
			}

			// Create output buffers.  We need two, one for building the deinterlaced.
			dsp::dspvector<_TypeDst> midoutbuffer(frames * channels);
			dsp::dspvector<_TypeDst> outbuffer(frames * channels);
			dsp::transpose_to interleave(frames, channels, dsp::interleave);

			// Setup some values.
			int cur_channels = 0, bufsize = frames * channels;

			// Do the confusing process of combining multiple audio channels/files into one.
			while (!done)
			{
				cur_c = 0;		// Current channel for output location calculation.
				done = true;	// Default to true for each pass.
				maxframes = 0;

				for (i = 0; i < num_inputs; ++i)
				{
					cur_channels = input[i].format.get_channels();

					// Read in frames.
					if ((rframes = (int)input[i].file.read_frames<_TypeDst>((_TypeDst*)inbuffers[i].data(), frames)) == frames)
						done = false;

					// If we read any frames...
					if (rframes > maxframes)
						maxframes = rframes;

					if (cur_channels == 1)
					{
						// Zero out extra samples.
						for (int x = rframes * cur_channels; x < frames * cur_channels; ++x)
							inbuffers[i][x] = dsp::sample_traits<_TypeDst>::zero();

						std::copy(inbuffers[i].cbegin(), inbuffers[i].cend(), midoutbuffer.begin() + (frames * cur_c));
						++cur_c;
					} // if (cur_channels == 1)
					else
					{
						dsp::transpose_to deinterleave(frames, cur_channels, dsp::deinterleave);

						// Zero out extra samples.
						for (int x = rframes * cur_channels; x < frames * cur_channels; ++x)
							inbuffers[i][x] = dsp::sample_traits<_TypeDst>::zero();

						// De-interleave this files samples.
						deinterleave(inbuffers[i], midbuffers[i]);

						// Copy the samples to the output buffer.
						std::copy(inbuffers[i].cbegin(), inbuffers[i].cend(), midoutbuffer.begin() + (frames * cur_c));
						cur_c += cur_channels;

					} // if (cur_channels == 1) else
				} // for (i = 0; i < num_inputs; ++i)

				if (maxframes)
				{
					// Finally, transpose input buffer to output buffer.
					interleave(midoutbuffer, outbuffer);

					// And write to output file.
					output[0].file.write_frames<_TypeDst>((_TypeDst*)outbuffer.data(), maxframes);

				} // if (maxframes)
			} // while (!done)
		} // if (do_complex_method)
	}


	// ********************************
	// **** Convert template function
	template <typename _TypeSrc, typename _TypeDst>
	void dsp_split_combine::convert_template(int index)
	{
		// Get number of frames to read each round.  And number of channels.
		int frames = get_buffer_length<_TypeDst>();
		int channels = input[index].format.get_channels();

		// Main buffer.
		dsp::dspvector<_TypeSrc> inbuffer(frames * channels);
		dsp::dspvector<_TypeDst> outbuffer(frames * channels);

		// Main loop:
		int rframes;
		while ((rframes = (int)input[index].file.read_frames<_TypeSrc>((_TypeSrc*)inbuffer.data(), frames)) == frames)
		{
			outbuffer = inbuffer;
			output[index].file.write_frames<_TypeDst>((_TypeDst*)outbuffer.data(), rframes);
		}

		// Handle leftovers...
		if (rframes > 0)
		{
			outbuffer = inbuffer;
			output[index].file.write_frames<_TypeDst>((_TypeDst*)outbuffer.data(), rframes);
		}
	}
	// ********************************


	// ********************************
	// **** Used to modifie a path to add " (chX)" into the name.
	std::sys::path dsp_split_combine::name_output_split(std::sys::path p, int ch)
	{
#if 0//_MSC_VER >= 1900
		std::string name = "\\" + p.stem().string();// p.basename();
		std::string ext = p.extension().string();
#else
		std::string name = "\\" + p.basename();
		std::string ext = p.extension();
#endif
		p.remove_filename();
		std::string nname = p.string();
		nname += name + " (ch" + std::to_string(ch) + ")" + ext;
		p = nname;
		return p;
	}
	// ********************************
	// ********************************


	// ********************************
	// **** Split process.
	bool dsp_split_combine::do_split()
	{
		// Sanity check.
		if (input.size() < 1)
		{
			error = "do_split(): Input file name not set.";
			return false;
		}

		// Open input.
		if (!input[0].file.is_open())
		{
			error = "do_split(): Input file not opened.  ";
			error += input[0].file.get_error_string();
			return false;
		}

		// Set input and output channels here instead of calling the function over and over.
		unsigned int in_channels = input[0].file.get_channels();
		out_format.set_channels(1);

		// Get bext chunk information.
		dsp::dspbwf bext;
		input[0].file.command(SFC_GET_BROADCAST_INFO, &bext, sizeof(SF_BROADCAST_INFO));

		// Get text information.
		std::vector<SF_STRINGS_T> strings;
		for (unsigned int i = SF_STR_FIRST; i < SF_STR_LAST; ++i)
		{
			const char *tmp = input[0].file.get_cstring(i);
			if (tmp)
				strings.emplace_back(tmp, i);
		}

		// Set up output file names.
		if (output.size() != in_channels)
		{
			// Start clean.
			output.clear();

			// Set base name, and extension.
			for (unsigned int i = 0; i < in_channels; ++i)
			{
				output.emplace_back(name_output_split(input[0].path, i + 1), out_format);
			}
		}

		// Open output files.
		for (unsigned int i = 0; i < output.size(); ++i)
		{
			if (output[i].format.get_rate() == 0)
				output[i].format.set_rate(input[0].format.get_rate());

			if (output[i].format.get_bits() == 0)
			{
				output[i].format.set_bits(input[0].format.get_bits());
				output[i].format.set_float(input[0].format.is_floats());
				output[i].format.set_interleaved(input[0].format.is_interleaved());
			}

			if (output[i].format.get_frames() == 0)
				output[i].format.set_frames(input[0].format.get_frames());
			output[i].format.set_channels(1);

#if 0//_MSC_VER >= 1900
			int oformat = output[i].file.get_good_sf_format(output[i].path.extension().string(), output[i].format);//, out_sf_format);
#else
			int oformat = output[i].file.get_good_sf_format(output[i].path.extension(), output[i].format);//, out_sf_format);
#endif
			output[i].file.open(
				output[i].path, SFM_WRITE,
				oformat, 1, output[i].format.get_rate());

			if (!output[i].file.is_open())
			{
				error = "do_split(): Could not open output file \"";
				error += output[i].path.string();
				error += "\".\n";
				error += output[i].file.get_error_str();
				return false;
			}
		}

		// Set information in output files.
		for (unsigned int i = 0; i < output.size(); ++i)
		{
			// Set bext chunk information for output files.
			output[i].file.command(SFC_SET_BROADCAST_INFO, &bext, sizeof(SF_BROADCAST_INFO));

			// Set text information.
			for (unsigned int j = 0; j < strings.size(); ++j)
				output[i].file.set_string(strings[j].id, strings[j].str.c_str());
		}

		// Do the process.
		switch ((input[0].format.get_bits() + 7) / 8)
		{
		case 1:
			if (!output[0].format.is_floats())
				split_template<int8_t, int8_t>();
			else
			{
				if (output[0].format.get_bits() <= 32)
					split_template<int8_t, float>();
				else
					split_template<int8_t, double>();
			}
			break;
		case 2:
			if (!output[0].format.is_floats())
				split_template<int16_t, int16_t>();
			else
			{
				if (output[0].format.get_bits() <= 32)
					split_template<int16_t, float>();
				else
					split_template<int16_t, double>();
			}
			break;
		case 3:
			if (!output[0].format.is_floats())
				split_template<int32_t, int32_t>();
			else
			{
				if (output[0].format.get_bits() <= 32)
					split_template<int32_t, float>();
				else
					split_template<int32_t, double>();
			}
			break;
		case 4:
			if (!input[0].format.is_floats())
			{
				if (!output[0].format.is_floats())
					split_template<int32_t, int32_t>();
				else
				{
					if (output[0].format.get_bits() <= 32)
						split_template<int32_t, float>();
					else
						split_template<int32_t, double>();
				}
			}
			else
			{
				if (output[0].format.get_bits() <= 32)
					split_template<float, float>();
				else
					split_template<float, double>();
			}
			break;
		case 8:
		default:
			if (!input[0].format.is_floats())
			{
				if (!output[0].format.is_floats())
					split_template<int64_t, int64_t>();
				else
				{
					if (output[0].format.get_bits() <= 32)
						split_template<int64_t, float>();
					else
						split_template<int64_t, double>();
				}
			}
			else
				split_template<double, double>();
			break;
		}

		// Default to success.
		return true;
	}
	// ********************************


	// ********************************
	// **** Do combine process.
	bool dsp_split_combine::do_combine()
	{
		// Sanity check inputs
		if (input.size() < 1)
		{
			error = "Error in do_combine(): Input file list is empty.\n";
			return false;
		}

		// Sanity check output
		if (output.size() != 1)
		{
			error = "Error in do_combine(): Output file list is empty.\n";
			return false;
		}

		// Open inputs and sanity check them but also count the total number of channels.
		int channels = 0;
		for (unsigned int i = 0; i < input.size(); ++i)
		{
			if (!input[i].file.is_open())
			{
				error = "do_combine(): File " + std::to_string(i + 1) + " in input list is not open \"";
				error += input[i].path.string();
				error += "\".\n";
				error += input[i].file.get_error_str();
				return false;
			}
			int c = input[i].format.get_channels();
			channels += c;
		}

		// Open output files.
//		for (unsigned int i = 0; i < output.size(); ++i)
		unsigned int i = 0;
		{
			if (output[i].format.get_rate() == 0)
				output[i].format.set_rate(input[0].format.get_rate());

			if (output[i].format.get_bits() == 0)
			{
				output[i].format.set_bits(input[0].format.get_bits());
				output[i].format.set_float(input[0].format.is_floats());
				output[i].format.set_interleaved(input[0].format.is_interleaved());
			}

			if (output[i].format.get_frames() == 0)
				output[i].format.set_frames(input[0].format.get_frames());

			output[i].format.set_channels(channels);

#if 0//_MSC_VER >= 1900
			int oformat =
				output[i].file.get_good_sf_format(output[i].path.extension().string(), output[i].format);//, out_sf_format);
#else
			int oformat =
				output[i].file.get_good_sf_format(output[i].path.extension(), output[i].format);//, out_sf_format);
#endif
			output[i].file.open(
				output[i].path, SFM_WRITE,
				oformat, channels, output[i].format.get_rate());

			if (!output[i].file.is_open())
			{
				error = "do_combine(): Could not open output file \"";
				error += output[i].path.string();
				error += "\".\n";
				error += output[i].file.get_error_str();
				return false;
			}
		}

		// Get bext chunk information.
		dsp::dspbwf bext;
		input[0].file.command(SFC_GET_BROADCAST_INFO, &bext, sizeof(SF_BROADCAST_INFO));

		// Get text information.
		std::vector<SF_STRINGS_T> strings;
		for (unsigned int i = SF_STR_FIRST; i < SF_STR_LAST; ++i)
		{
			const char *tmp = input[0].file.get_cstring(i);
			if (tmp)
				strings.emplace_back(tmp, i);
		}

		// Set information in output files.
		for (unsigned int i = 0; i < output.size(); ++i)
		{
			// Set bext chunk information for output files.
			output[i].file.command(SFC_SET_BROADCAST_INFO, &bext, sizeof(SF_BROADCAST_INFO));

			// Set text information.
			for (unsigned int j = 0; j < strings.size(); ++j)
				output[i].file.set_string(strings[j].id, strings[j].str.c_str());
		}

		// Do the process.
		switch ((input[0].format.get_bits() + 7) / 8)
		{
		case 1:
			combine_template<int8_t, int8_t>();
			break;
		case 2:
			combine_template<int16_t, int16_t>();
			break;
		case 3:
			combine_template<int32_t, int32_t>();
			break;
		case 4:
			if (!input[0].format.is_floats())
				combine_template<int32_t, int32_t>();
			else
				combine_template<float, float>();
			break;
		case 8:
		default:
			if (!input[0].format.is_floats())
				combine_template<int64_t, int64_t>();
			else
				combine_template<double, double>();
			break;
		}

		// Default to success.
		return true;
	}
	// ********************************


	// ********************************
	// **** Do conversion process.
	bool dsp_split_combine::do_convert()
	{
		// Setup some variables for the conversions.
		int num_files = input.size();
		int oformat;
		dsp::dspbwf bext;
		std::vector<SF_STRINGS_T> strings;
		error = "do_convert():\n";

		// Sanity check the input.
		// We need at least one input and the number of outputs must be equal to the number of inputs.
		if (input.size() <= 0)
		{
			error += "Error: No input files\n";
			return false;
		}
		if ((output.size() != input.size()))
		{
			error += "Error: Not enough output files for input files.\n";
			return false;
		}

		// loop through all files in the list and do the conversions.
		// FIXME: Test multi-threading here.  Can we do multiple processes at once?
		for (int i = 0; i < num_files; ++i)
		{
			// Check that input is open and can be read.
			if (!input[i].file.is_open())
			{
				error +=
					"Error with input file \"" + input[i].path.string() + "\".\n"
					+ input[i].file.get_error_string() + "\n";
				continue;
			}

			// Get bext chunk information.
			bext.clear();
			input[i].file.command(SFC_GET_BROADCAST_INFO, &bext, sizeof(SF_BROADCAST_INFO));

			// Get text information.
			strings.clear();
			for (unsigned int j = SF_STR_FIRST; j < SF_STR_LAST; ++j)
			{
				const char *tmp = input[i].file.get_cstring(j);
				if (tmp)
					strings.emplace_back(tmp, j);
			}

			// Setup the output format.
			out_format = input[i].file.get_dspformat();
			out_sf_format = input[i].file.get_format();

			output[i].format.set_channels(input[i].format.get_channels());

#if 0//_MSC_VER >= 1900
			oformat =
				output[i].file.get_good_sf_format(
					output[i].path.extension().string(), output[i].format);//, out_sf_format);
#else
			oformat =
				output[i].file.get_good_sf_format(
					output[i].path.extension(), output[i].format);//, out_sf_format);
#endif
			// Open the file.
			output[i].file.open(
				output[i].path, SFM_WRITE,
				oformat, out_format.get_channels(), out_format.get_rate());

			// Check that the output file is open.
			if (!output[i].file.is_open())
			{
				error += "Could not open output file \"";
				error += output[i].path.string();
				error += "\".\n";
				error += output[i].file.get_error_str();
				return false;
			}

			// Set bext chunk information for output file.
			output[i].file.command(SFC_SET_BROADCAST_INFO, &bext, sizeof(SF_BROADCAST_INFO));

			// Set text information.
			for (unsigned int j = 0; j < strings.size(); ++j)
				output[i].file.set_string(strings[j].id, strings[j].str.c_str());


			// Call convert template function.
			switch ((input[i].format.get_bits() + 7) / 8)
			{
			case 1:
				convert_template<int8_t, int8_t>(i);
				break;
			case 2:
				convert_template<int16_t, int16_t>(i);
				break;
			case 3:
				convert_template<int32_t, int32_t>(i);
				break;
			case 4:
				if (!input[i].format.is_floats())
					convert_template<int32_t, int32_t>(i);
				else
					convert_template<float, float>(i);
				break;
			case 8:
			default:
				if (!input[i].format.is_floats())
					convert_template<int64_t, int64_t>(i);
				else
					convert_template<double, double>(i);
				break;
			}
		}
		// End of loop. Be sure to wait here for active jobs if multi-threaded...

		// Exit with success.
		return true;
	}
	// ********************************
	// **** End dsp_split_file
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
