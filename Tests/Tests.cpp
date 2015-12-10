// Tests.cpp : Defines the entry point for the console application.
//

#include "stdafx.h"

#include "cpp-dsp.h"

// ********************************
// **** Class stop_watch - Used for timing operations and processes.
#include <chrono>
#include <ctime>

class stop_watch
{
private:
	typedef std::chrono::time_point<std::chrono::system_clock> watch_t;
	watch_t _start, _end;

public:
	watch_t start() { return _start = std::chrono::system_clock::now(); }
	watch_t end()   { return _end = std::chrono::system_clock::now(); }

	template <typename _Type>
	std::chrono::duration<_Type> elapsed_seconds(){ return _end - _start; }

	std::time_t end_time() { return std::chrono::system_clock::to_time_t(_end); }
};
// **** End Class stop_watch
// ********************************


// ********************************
// **** Test file splitting
int test_split(char * input, char * output, int outfmt = 0)
{
	// ********************************
	// **** Initialize stop watch and display splash.
	stop_watch t;
	std::cout << "Test for splitting process:\n";
	// ********************************


	// ********************************
	// **** Start a new split/combine process.
	DSPPTR handle;
	std::cout << "Calling dsp_sc_start(handle)...";
	if (dsp_sc_start(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << std::hex << handle << "\n";
	}
	else
	{
		std::cout << "error. Couldn't start...\n";
		// We can't use dsp_sc_get_error here because we don't have a valid handle.
		return DSP_ERROR; // Return false on error.
	}
	// ********************************


	// ********************************
	// **** Add input file
	char * name = input;
	std::cout << "Calling dsp_sc_add_input(handle, \"" << name << "\", channels)...";
	SF_BROADCAST_INFO bext;
	int Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, HasBWF, MediaType;
	unsigned long dataSize;
	if (dsp_sc_add_input_ex(handle, name, Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, dataSize, HasBWF, MediaType, bext) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << std::hex << handle
			<< "," << std::dec << Channels
			<< "," << std::dec << SampleSize
			<< "," << std::dec << FrameSize
			<< "," << std::dec << SampleRate
			<< "," << (Float ? "Float": "Integer")
			<< "," << (ByteOrder ? "Big": "Little")
			<< "," << std::hex << dataOffset
			<< "," << std::hex << dataSize
			<< "," << (HasBWF ? "BWF" : "NoBWF")
			<< "," << std::hex << MediaType
			<< "\n";
	}
	else
	{
		std::cout << "Error.  Couldn't add input file \"" << name << "\"...\n";
		char buf[1024];
		dsp_sc_get_error(handle, buf, sizeof(buf));
		std::cout << buf << "\n";
		goto exit_early;
	}
	// ********************************


	// ********************************
	// **** Add output files?
	if (output != nullptr)
	{
		char outname[1024];
		for (int i = 0; i < Channels; ++i)
		{
			sprintf_s(
				outname, sizeof(outname),
				output,
				i + 1
			);
			std::cout << "Calling dsp_sc_add_output(handle, \"" << outname << "\", channels)...";
			if (dsp_sc_add_output(handle, outname, outfmt, 0) == DSP_OK)
			{
				std::cout << "ok. handle = 0x" << handle << ", channel = " << i << "\n";
			}
			else
			{
				std::cout << "Error.  Couldn't add output file \"" << outname << "\"...\n";
				char buf[2048];
				dsp_sc_get_error(handle, buf, sizeof(buf));
				std::cout << buf << "\n";
				goto exit_early;
			}
		}
	}
	// ********************************


	// ********************************
	// **** Setup some timers to time the process.
	std::cout << "\nStarting timer section\n{\n";
	t.start();
	// ********************************


	// ********************************
	// **** Do split
	std::cout << "Calling dsp_sc_do_split(handle)...";
	if (dsp_sc_do_split(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		std::cout << "Error.  Could not split...\n";
		char buf[1024];
		dsp_sc_get_error(handle, buf, sizeof(buf));
		std::cout << buf << "\n";
		return DSP_ERROR; // Return false on error.
	}
	// ********************************


	// ********************************
	// **** Stop timers.
	t.end();
	std::cout << "}\nStopped timer.  Elapsed time: " << t.elapsed_seconds<double>().count() << "s\n";
	// ********************************


	// ********************************
	// **** End the split/combine process.
exit_early:
	std::cout << "Calling dsp_sc_end(handle)...";
	if (dsp_sc_end(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		// This should never happen unless we overwrote the handle variable or didn't get a valid handle.
		std::cout << "Error.  Couldn't start...\n";
		return DSP_ERROR; // Return false on error.
	}
	// ********************************

	return DSP_OK;
}


// ********************************
// **** test_combine
int test_combine(char * input[8], char * output)
{
	// ********************************
	// **** Initialize stop watch and display splash.
	stop_watch t;
	std::cout << "Test for combine process:\n" << std::hex;
	// ********************************


	// ********************************
	// **** Start a new split/combine process.
	DSPPTR handle;
	std::cout << "Calling dsp_sc_start(handle)...";
	if (dsp_sc_start(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		std::cout << "error. Couldn't start...\n";
		// We can't use dsp_sc_get_error here because we don't have a valid handle.
		return DSP_ERROR; // Return false on error.
	}
	// ********************************


	// ********************************
	// **** Add input files
	//int numc = 0, 
	int count = 0;
	SF_BROADCAST_INFO bext;
	int Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, HasBWF, MediaType;
	unsigned long dataSize;
	for (int i = 0; (i < 8) && (input[i] != nullptr); ++i)
	{
		std::cout << "Calling dsp_sc_add_input(handle, \"" << input[i] << "\", channels, num)...";
		if (dsp_sc_add_input_ex(handle, input[i], Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, dataSize, HasBWF, MediaType, bext) == DSP_OK)
		{
			count += Channels;
			std::cout << "handle = 0x [" << i <<"]" << std::hex << handle
				<< "," << std::dec << Channels
				<< "," << std::dec << SampleSize
				<< "," << std::dec << FrameSize
				<< "," << std::dec << SampleRate
				<< "," << (Float ? "Float" : "Integer")
				<< "," << (ByteOrder ? "Big" : "Little")
				<< "," << std::hex << dataOffset
				<< "," << std::hex << dataSize
				<< "," << (HasBWF ? "BWF" : "NoBWF")
				<< "," << std::hex << MediaType
				<< "\n";
		}
		else
		{
			std::cout << "Error.  Couldn't add output file \"" << input[i] << "\"...\n";
			char buf[2048];
			dsp_sc_get_error(handle, buf, sizeof(buf));
			std::cout << buf << "\n";
			goto exit_early;
		}
	}
	std::cout << "total = " << count << "\n";
	// ********************************


	// ********************************
	// **** Add output file
	std::cout << "Calling dsp_sc_add_output(handle, \"" << output << "\", channels, 0, 0)...";
	if (dsp_sc_add_output(handle, output, 0, 0) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << ", channels = " << count << "\n";
	}
	else
	{
		std::cout << "Error.  Couldn't add input file \"" << output << "\"...\n";
		char buf[1024];
		dsp_sc_get_error(handle, buf, sizeof(buf));
		std::cout << buf << "\n";
		goto exit_early;
	}
	// ********************************


	// ********************************
	// **** Setup some timers to time the process.
	std::cout << "\nStarting timer section\n{\n";
	t.start();
	// ********************************


	// ********************************
	// **** Do combine
	std::cout << "Calling dsp_sc_do_combine(handle)...";
	if (dsp_sc_do_combine(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		std::cout << "Error.  Could not combine...\n";
		char buf[1024];
		dsp_sc_get_error(handle, buf, sizeof(buf));
		std::cout << buf << "\n";
		return DSP_ERROR; // Return false on error.
	}
	// ********************************


	// ********************************
	// **** Stop timers.
	t.end();
	std::cout << "}\nStopped timer.  Elapsed time: " << t.elapsed_seconds<double>().count() << "s\n";
	// ********************************

	// ********************************
	// **** End the split/combine process.
exit_early:
	std::cout << "Calling dsp_sc_end(handle)...";
	if (dsp_sc_end(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		// This should never happen unless we overwrote the handle variable or didn't get a valid handle.
		std::cout << "Error.  Couldn't start...\n";
		return DSP_ERROR; // Return false on error.
	}
	// ********************************

	return DSP_OK;
}
// ********************************


// ********************************
// **** test conversion process.
int test_convert(char * inputs[4], char * outputs[4])
{
	// ********************************
	// **** Initialize stop watch and display splash.
	stop_watch t;
	std::cout << "Test for convert process:\n" << std::hex;
	// ********************************


	// ********************************
	// **** Start a new split/combine process.
	DSPPTR handle;
	std::cout << "Calling dsp_sc_start(handle)...";
	if (dsp_sc_start(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		std::cout << "error. Couldn't start...\n";
		// We can't use dsp_sc_get_error here because we don't have a valid handle.
		return DSP_ERROR; // Return false on error.
	}
	// ********************************


	// ********************************
	// **** Add input files
	//int numc = 0, 
	int count = 0;
	SF_BROADCAST_INFO bext;
	int Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, HasBWF, MediaType;
	unsigned long dataSize;
	for (int i = 0; (i < 4) && (inputs[i] != nullptr); ++i)
	{
		std::cout << "Calling dsp_sc_add_input(handle, \"" << inputs[i] << "\", channels, num)...";
		if (dsp_sc_add_input_ex(handle, inputs[i], Channels, SampleSize, FrameSize, SampleRate, Float, ByteOrder, dataOffset, dataSize, HasBWF, MediaType, bext) == DSP_OK)
		{
			count += Channels;
			std::cout
				<< "ok. handle = 0x" << std::hex << handle
				<< ",[" << i << "] "
				<< "," << std::dec << Channels
				<< "," << std::dec << SampleSize
				<< "," << std::dec << FrameSize
				<< "," << std::dec << SampleRate
				<< "," << (Float ? "Float" : "Integer")
				<< "," << (ByteOrder ? "Big" : "Little")
				<< "," << std::hex << dataOffset
				<< "," << std::hex << dataSize
				<< "," << (HasBWF ? "BWF" : "NoBWF")
				<< "," << std::hex << MediaType
				<< "\n";
		}
		else
		{
			std::cout << "Error.  Couldn't add output file \"" << inputs[i] << "\"...\n";
			char buf[2048];
			dsp_sc_get_error(handle, buf, sizeof(buf));
			std::cout << buf << "\n";
			goto exit_early;
		}
	}
	std::cout << "total = " << count << "\n";
	// ********************************


	// ********************************
	// **** Add output file
	for (int i = 0; (i < 4) && (outputs[i] != nullptr); ++i)
	{
		std::cout << "Calling dsp_sc_add_output(handle, \"" << outputs[i] << "\", 0, 0)...";
		if (dsp_sc_add_output(handle, outputs[i], 0, 0) == DSP_OK)
		{
			std::cout << "ok. handle = 0x" << handle << ", channels[" << i << "]\n";
		}
		else
		{
			std::cout << "Error.  Couldn't add output file \"" << outputs[i] << "\"...\n";
			char buf[2048];
			dsp_sc_get_error(handle, buf, sizeof(buf));
			std::cout << buf << "\n";
			goto exit_early;
		}
	}
	std::cout << "total = " << count << "\n";
	// ********************************


	// ********************************
	// **** Setup some timers to time the process.
	std::cout << "\nStarting timer section\n{\n";
	t.start();
	// ********************************


	// ********************************
	// **** Do combine
	std::cout << "Calling dsp_sc_do_convert(handle)...";
	if (dsp_sc_do_convert(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		std::cout << "Error.  Could not dsp_sc_do_convert...\n";
		char buf[1024];
		dsp_sc_get_error(handle, buf, sizeof(buf));
		std::cout << buf << "\n";
		return DSP_ERROR; // Return false on error.
	}
	// ********************************


	// ********************************
	// **** Stop timers.
	t.end();
	std::cout << "}\nStopped timer.  Elapsed time: " << t.elapsed_seconds<double>().count() << "s\n";
	// ********************************

	// ********************************
	// **** End the split/combine process.
exit_early:
	std::cout << "Calling dsp_sc_end(handle)...";
	if (dsp_sc_end(handle) == DSP_OK)
	{
		std::cout << "ok. handle = 0x" << handle << "\n";
	}
	else
	{
		// This should never happen unless we overwrote the handle variable or didn't get a valid handle.
		std::cout << "Error.  Couldn't start...\n";
		return DSP_ERROR; // Return false on error.
	}
	// ********************************

	return DSP_OK;
}
// ********************************


// ********************************
// **** Main
int _tmain(int argc, _TCHAR* argv[])
{
	// Split test 0
	if (!test_split(
		"X:\\Projects\\test_data\\Media\\MSRT09.WAV",
		"X:\\Projects\\test_data\\Media\\out\\MSRT09 (ch%d).WAV",
		0x010000 + 0x0006))
		return 1;

	// Split test 1
	if (!test_split(
		"X:\\Projects\\test_data\\Media\\002143.wav",
		"X:\\Projects\\test_data\\Media\\out\\002143 (ch%d).aif"))
		return 1;

	// Split test 2
	if (!test_split(
		"X:\\Projects\\test_data\\Media\\26_489_T2_SR028009.WAV",
		"X:\\Projects\\test_data\\Media\\out\\26_489_T2_SR028009 (ch%d).aif"))
		return 1;

	// Combine test 1
	{
		char * test_inputs[8] =
		{
			"X:\\Projects\\test_data\\Media\\out\\002143 (ch1).aif",
			"X:\\Projects\\test_data\\Media\\out\\002143 (ch2).aif",
			"X:\\Projects\\test_data\\Media\\out\\002143 (ch3).aif",
			"X:\\Projects\\test_data\\Media\\out\\002143 (ch4).aif",
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};
		if (!test_combine(
			test_inputs,
			"X:\\Projects\\test_data\\Media\\out\\002143.WAV"))
			return 1;
	}

	// Combine test 2
	{
		char * test_inputs[8] =
		{
			"X:\\Projects\\test_data\\Media\\out\\26_489_T2_SR028009 (ch1).aif",
			"X:\\Projects\\test_data\\Media\\out\\26_489_T2_SR028009 (ch2).aif",
			"X:\\Projects\\test_data\\Media\\out\\26_489_T2_SR028009 (ch3).aif",
			nullptr,
			nullptr,
			nullptr,
			nullptr,
			nullptr
		};
		if (!test_combine(
			test_inputs,
			"X:\\Projects\\test_data\\Media\\out\\26_489_T2_SR028009 out.WAV"))
			return 1;
	}

	// Test converter.
	{
		char * test_inputs[4] =
		{
			"X:\\Projects\\test_data\\Media\\002143.wav",
			"X:\\Projects\\test_data\\Media\\26_489_T2_SR028009.WAV",
			"X:\\Projects\\test_data\\Media\\0001f3.wav",
			"X:\\Projects\\test_data\\Media\\mvi_1738x.wav",
		};
		char * test_outputs[4] =
		{
			"X:\\Projects\\test_data\\Media\\out\\002143.caf",
			"X:\\Projects\\test_data\\Media\\out\\26_489_T2_SR028009.caf",
			"X:\\Projects\\test_data\\Media\\out\\0001f3.aif",
			"X:\\Projects\\test_data\\Media\\out\\mvi_1738x.aif",
		};
		test_convert(test_inputs, test_outputs);
	}

	return 0;
}
// **** End Main
// ********************************

