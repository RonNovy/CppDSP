/* Split/Combine code.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#include "dsp_file.h"
#include "dsp_transpose.h"

#include "cpp-dsp.h"

#include <filesystem>
namespace std { using namespace tr2; }


// ********************************
// **** dsp namespace for dsp classes and functions.
namespace dsp
{
	// ********************************
	// **** dsp_split_combine - Class to split a file.
	class dsp_split_combine
	{
	private:
		uint32_t sanity_check;

		class SF_STRINGS_T
		{
		public:
			std::string str;
			int id;
			SF_STRINGS_T(const char *s, int i) :str(s), id(i) {}
		};

		class file_description
		{
		public:
			std::sys::path	path;
			dsp::dspfile	file;
			dsp::dspformat	format;
			file_description() {}
			file_description(const char *_name) : path(_name) {}
			file_description(std::sys::path &_path) : path(_path) {}
			file_description(std::sys::path &_path, dsp::dspformat _format) : path(_path), format(_format) {}
			file_description(std::sys::path &_path, dsp::dspfile _file, dsp::dspformat _format) :
				path(_path), file(_file), format(_format) {}
		};

		bool format_override;
		dsp::dspformat out_format;
		int out_sf_format;

		std::vector<file_description> input;	// Input files
		std::vector<file_description> output;	// Output files

		// A wide string for passing error information back to a calling process.
		std::string error;

	public:
		dsp_split_combine();
		~dsp_split_combine();
		const char *get_error_str(void);

		// Clear inputs and outputs and reset the entire class.
		bool clear();

		// This function sets 'channels' to the number of channels detected in the input file.
		bool add_input(const char *name, int &channels);
		bool add_output_path(std::sys::path &path, int fmtcodec, int rate);	// Add full path and file name using filesystem>path.
		bool add_output(const char *name, int fmtcodec, int rate);			// Add full path and file name using a C string.

		// Functions to process files.
	private:
		template <typename _Type>
		unsigned int get_buffer_length();

		template <typename _Type>
		void split_template();

		template <typename _Type>
		void combine_template();

		template <typename _Type>
		void convert_template(int index);

		// Modifies a path to add " (chX)" into the name.
		std::sys::path name_output_split(std::sys::path p, int ch);

	public:
		bool do_split();
		bool do_combine();
		bool do_convert();
	};
	// **** End dsp_split_combine
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
