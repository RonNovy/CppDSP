/* Plugin interfaces
 * Copyright (C) 2015
 * Ron S. Novy
 */
#pragma once
#include "configure.h"

#include <vector>
#include <string>
#include <fstream>

#include "dsp_containers.h"


// ********************************
// **** Be sure to update these accordingly.
#define AF_VER_MAJOR	1
#define AF_VER_MINOR	0
#define AF_VER_PATCH	0
#define AF_VER_BUILD	0
// ********************************


// ********************************
// **** dsp namespace
namespace dsp
{
	// ********************************
	// **** dsp::plugin class for plugin related classes and functions.
	namespace plugin
	{
		// ********************************
		// **** GUID
		typedef struct GUID
		{
			unsigned long	Data1;
			unsigned short	Data2;
			unsigned short	Data3;
			unsigned char	Data4[8];
		} GUID;
		// ********************************


		// ********************************
		// **** Version information.
		class version_t
		{
		public:
			int major, minor, patch, build;
			version_t(
				int _major = AF_VER_MAJOR,
				int _minor = AF_VER_MINOR,
				int _patch = AF_VER_PATCH,
				int _build = AF_VER_BUILD) :
				major(_major), minor(_minor),
				patch(_patch), build(_build) {}
			bool operator == (const version_t &rhs)
			{
				if ((major != rhs.major) || (minor != rhs.major))
					return false;
				return true;
			}
			bool operator != (const version_t &rhs)
			{
				if ((major != rhs.major) || (minor != rhs.major))
					return true;
				return false;
			}
		};
		// ********************************


		// ********************************
		// **** Compatibility card
		class compatibility_card
		{
		private:
			version_t	version;
			char		sizes[8];
			#ifdef _MSC_VER
				int msc_ver;
			#endif
		public:
			compatibility_card()
			{
				version.major = AF_VER_MAJOR;
				version.minor = AF_VER_MINOR;
				version.patch = AF_VER_PATCH;
				version.build = AF_VER_BUILD;
				sizes[0] = sizeof(int8_t);
				sizes[1] = sizeof(int16_t);
				sizes[2] = sizeof(int24_t);
				sizes[3] = sizeof(int32_t);
				sizes[4] = sizeof(int64_t);
				sizes[5] = sizeof(float);
				sizes[6] = sizeof(double);
				sizes[7] = sizeof(long double);

				#ifdef _MSC_VER
				msc_ver = _MSC_VER;
				#endif
			}
			~compatibility_card() {};
			bool operator== (const compatibility_card& rhs)
			{
				if (version != rhs.version)
					return false;
				for (int i = 0; i < sizeof(sizes); ++i)
				{
					if (sizes[i] != rhs.sizes[i])
						return false;
				}
				#ifdef _MSC_VER
				if (msc_ver != rhs.msc_ver)
					return false;
				#endif
				return true;
			}
		};
		// ********************************
		// ********************************


		// ********************************
		// **** range_t - Minimum and maximum limits of something.
		template <typename _Type>
		class range_t
		{
		public:
			_Type min, max;

			// ********************************
			// ****   Operator == will return true if 'rhs' is 
			// **** within the min and max limits.
			bool operator == (_Type rhs)
			{
				if (rhs >= min && rhs <= max)
					return true;
				return false;
			}
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Plugin information logging interface.
		// **** Host owns->Plugin calls
		class logging_functions
		{
		public:
			virtual ~logging_functions() {};
			virtual void information(const std::wstring &message) = 0;	// General information
			virtual void important(const std::wstring &message) = 0;	// Important information
			virtual void warning(const std::wstring &message) = 0;		// Warnings
			virtual void error(const std::wstring &message) = 0;		// Errors
			virtual void clear_information() = 0;	// Clear every message marked as information.
			virtual void clear_important() = 0;		// Clear every message marked important.
			virtual void clear_warning() = 0;		// Clear every message marked as warning.
			virtual void clear_error() = 0;			// Clear every error message.
			virtual void clear_all() = 0;			// Clear every message.
		};
		// ********************************


		// ********************************
		// **** Interface for opening files for reading/writing.
		// **** Plugin owns->Host calls
		class format_functions
		{
		public:
			virtual ~format_functions() {};
			virtual bool test(std::wstring name, int mode) = 0;
			virtual intptr_t open_existing(
				std::wstring name,
				std::fstream::openmode mode =
					std::fstream::in | std::fstream::binary) = 0;

			virtual intptr_t open_new(
				std::wstring name,
				std::fstream::openmode mode =
					std::fstream::out | std::fstream::binary) = 0;

			virtual bool is_open(intptr_t fn) = 0;
			virtual intptr_t close(intptr_t fn) = 0;

			virtual bool get_input_format(intptr_t fn, dspformat &fmt) = 0;
			virtual bool set_input_format(intptr_t fn, const dspformat &fmt) = 0;

			virtual size_t get_total_samples() = 0;
			virtual size_t get_total_frames() = 0;

			// ********************************
			// **** read_buffer - dspvector needs to be large enough to hold the frames.
			virtual size_t read_buffer(intptr_t fn, const dspvector<int8_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<int16_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<int24_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<int32_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<int64_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<uint8_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<uint16_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<uint32_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<uint64_t, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<float,  true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<double, true>& buf, size_t frames) = 0;
			virtual size_t read_buffer(intptr_t fn, const dspvector<long double, true>& buf, size_t frames) = 0;

			// ********************************
			// **** write_buffer - dspvector must be set to exact size to read.
			virtual size_t write_buffer(intptr_t fn, const dspvector<int8_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<int16_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<int24_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<int32_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<int64_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<uint8_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<uint16_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<uint32_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<uint64_t, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<float,  true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<double, true>& buf, size_t frames) = 0;
			virtual size_t write_buffer(intptr_t fn, const dspvector<long double, true>& buf, size_t frames) = 0;
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Information for given format supported by a plugin.
		// **** Plugin owns->Host copies/calls
		class format_information
		{
		public:
			bool	can_read;	// Format can read files.
			bool	can_write;	// Format can write files.
			GUID	guid;		// Unique identifier for this format.

			std::wstring	name;			// A name for the format.
			std::wstring	description;	// Long description.  Copyright, licensing for format etc.
			std::wstring	ui_tag;			// For display in Open/Save dialogs.

			std::vector<std::wstring>	extensions;	// List of extensions supported by this plugin.
			std::vector<range_t<int>>	channels;	// Min/Max channels supported.
			std::vector<range_t<int>>	bits;		// Min/Max bits.
			std::vector<range_t<int>>	rates;		// Min/Max sample rates.

			format_functions* finterface;
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Information for a given format supported by a plugin.
		// **** Plugin owns->Host copies/calls
		class plugin_information
		{
		public:
			GUID						guid;		// Unique identifier for this plugin.
			version_t					version;	// Version information.
			std::string					name;		// A name for the format.
			std::string					description;// Long description.  Copyright, licensing for format etc.
			std::string					ui_tag;		// For display in Open/Save dialogs.

			std::vector<format_information> formats;
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Host interface for plugin initialization
		// **** Host owns->Plugin calls
		class host_functions
		{
		public:
			virtual ~host_functions() {};
			virtual void get_host_information(int &major, int &minor) = 0;
			virtual void set_plugin_information(const plugin_information &info) = 0;
		};
		// ********************************
		// ********************************


		// ********************************
		// **** Host information for plugin initialization
		// **** Host owns->Plugin calls
		class host_information
		{
		public:
			compatibility_card	card;
			host_functions*		iface;
			logging_functions*	logging;
		};
		// ********************************
		// ********************************
	}
	// **** End namespace dsp::plugin_interface
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
