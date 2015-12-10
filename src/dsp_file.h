/* DSP classes to load information from files.
 * Copyright (C) 2015
 * Ron S. Novy
 *
 * TODO: Need to write an audio i/o plug-in API.
 *   Currently this code is dependant on libsndfile.  At some point I would
 * like this library to have its own audio i/o.
 */

#pragma once

// For this to work, libsndfile-1.lib must be in the linkers search paths
#pragma comment(lib, "libsndfile-1.lib")

#include "configure.h"

#include <ctime>
#include <cstdint>
#include <string>
#include <vector>
#include <array>
#include <memory>
#ifdef _WIN32
	#define ENABLE_SNDFILE_WINDOWS_PROTOTYPES 1
#endif

#include "sndfile.h"
#include "dsp_containers.h"
#include <array>

// ********************************
// **** dsp namespace for dsp classes and functions.
namespace dsp
{
	// ********************************
	// **** dspbwf class.
	class dspbwf
	{
	private:
		SF_BROADCAST_INFO info;

		std::string fixed_to_string(const char *cp, int size)
		{
			std::string ret;
			for (int i = 0; i < size; ++i)
			{
				if (cp[i] == 0)
					break;
				ret.push_back(cp[i]);
			}
			return ret;
		}

	public:
		dspbwf() { clear(); }
		~dspbwf() {}

		void clear() { std::memset(&info, 0, sizeof(SF_BROADCAST_INFO)); }
		SF_BROADCAST_INFO * data() { return &info; }
		int size() { return sizeof(SF_BROADCAST_INFO); }

		void default_fill()
		{
			char temp[64];
			time_t time_cur = std::time(NULL);
			tm *time_ptr = std::localtime(&time_cur);

			// OriginationReference
			std::strftime(
				info.originator_reference,
				sizeof(info.originator_reference),
				"AAT-AudioConvert;%H%M%S", time_ptr);

			// The random part of the Originator reference.
			std::srand((unsigned int)std::time(NULL));
			info.originator_reference[23] = (std::rand() % 10) + '0';
			info.originator_reference[24] = (std::rand() % 10) + '0';
			info.originator_reference[25] = (std::rand() % 10) + '0';
			info.originator_reference[26] = (std::rand() % 10) + '0';
			info.originator_reference[27] = (std::rand() % 10) + '0';
			info.originator_reference[28] = (std::rand() % 10) + '0';
			info.originator_reference[29] = (std::rand() % 10) + '0';
			info.originator_reference[30] = (std::rand() % 10) + '0';
			info.originator_reference[31] = (std::rand() % 10) + '0';

			// Origination Date
			std::memset(temp, 0, sizeof(temp));
			std::strftime(temp, sizeof(temp), "%Y-%m-%d", time_ptr);
			std::memcpy(info.origination_date, temp, sizeof(info.origination_date));

			// Origination Time
			std::memset(temp, 0, sizeof(temp));
			std::strftime(temp, sizeof(temp), "%H:%M:%S", time_ptr);
			std::memcpy(info.origination_time, temp, sizeof(info.origination_time));
		}

		std::string	get_description() { return fixed_to_string(info.description, sizeof(info.description)); }
		std::string	get_originator() { return fixed_to_string(info.originator, sizeof(info.originator)); }
		std::string	get_originator_reference() { return fixed_to_string(info.originator_reference, sizeof(info.originator_reference)); }
		std::string	get_origination_date() { return fixed_to_string(info.origination_date, sizeof(info.origination_date)); }
		std::string	get_origination_time() { return fixed_to_string(info.origination_time, sizeof(info.origination_time)); }
		uint64_t	get_time_reference() { return ((uint64_t)info.time_reference_high << 32) | (uint64_t)info.time_reference_low; }
		short       get_version() { return info.version; }
		std::string	get_umid() { return fixed_to_string(info.umid, sizeof(info.umid)); }
		std::string	get_reserved() { return fixed_to_string(info.reserved, sizeof(info.reserved)); }
		std::string	get_coding_history() { return fixed_to_string(info.coding_history, sizeof(info.coding_history)); }

#define MACRO(var, str)	\
	std::memset(var, 0, sizeof(var)); \
	std::strncpy(var, str.c_str(), sizeof(var))

		void set_description(const std::string & str) { MACRO(info.description, str); }
		void set_originator(const std::string & str) { MACRO(info.originator, str); }
		void set_originator_reference(const std::string & str) { MACRO(info.originator_reference, str); }
		void set_origination_date(const std::string & str) { MACRO(info.origination_date, str); }
		void set_origination_time(const std::string & str) { MACRO(info.origination_time, str); }
		void set_time_reference(uint64_t val) { info.time_reference_low = val & 0xffffffffu; info.time_reference_high = (val >> 32) & 0xffffffffu; }
		void set_version(short val) { info.version = val; }
		void set_umid(const std::string & str) { MACRO(info.umid, str); }
		void set_reserved(const std::string & str) { MACRO(info.reserved, str); }
		void set_coding_history(const std::string & str) { MACRO(info.coding_history, str); }

#undef MACRO
	};
	// **** End bext class.
	// ********************************


	// ********************************
	// **** dspfile class - using libsndfile...
	class dspfile
	{
	private:
		// ********************************
		// **** SNDFILE_ref class.
		class SNDFILE_ref
		{
		public:
			// ********************************
			// **** Default constructor.
			SNDFILE_ref() : sf(nullptr), sfinfo() {}
			// ********************************

			// ********************************
			// **** SNDFILE_ref destructor.
			~SNDFILE_ref()
			{
				sf_close(sf);
				sf = nullptr;
			}
			// ********************************

			// ********************************
			SNDFILE *sf;
			SF_INFO sfinfo;
			// ********************************
		};
		// **** End SNDFILE_ref class.
		// ********************************

		std::shared_ptr<SNDFILE_ref> p;

	public:
		// ********************************
		// **** Default constructor.
		dspfile() : p(nullptr) {};

		// ********************************
		dspfile(const char *path, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
			: p(nullptr)
		{
			open(path, mode, format, channels, samplerate);
		}

		dspfile(const wchar_t * wpath, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
			: p(nullptr)
		{
			open(wpath, mode, format, channels, samplerate);
		}

		dspfile(const std::string & path, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
			: p(nullptr)
		{
			open(path, mode, format, channels, samplerate);
		}

		dspfile(const std::wstring & wpath, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
			: p(nullptr)
		{
			open(wpath, mode, format, channels, samplerate);
		}

		dspfile(int fd, bool close_desc, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
			: p(nullptr)
		{
			open(fd, close_desc, mode, format, channels, samplerate);
		}

		dspfile(SF_VIRTUAL_IO &sfvirtual, void *user_data, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
			: p(nullptr)
		{
			open(sfvirtual, user_data, mode, format, channels, samplerate);
		}
		// ********************************

		// ********************************
		~dspfile(void) {}
		// ********************************

		// ********************************
		SNDFILE *get_sndfile_ptr() { return p->sf; }
		// ********************************

		// ********************************
		// ********************************
		void open(const char *path, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
		{
			p = std::make_shared<SNDFILE_ref>(SNDFILE_ref());

			if (p != nullptr)
			{
				p->sfinfo.frames = 0;
				p->sfinfo.channels = channels;
				p->sfinfo.format = format;
				p->sfinfo.samplerate = samplerate;
				p->sfinfo.sections = 0;
				p->sfinfo.seekable = 0;

				p->sf = sf_open(path, mode, &p->sfinfo);
			}
		}

		void open(const wchar_t * wpath, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
		{
			p = std::make_shared<SNDFILE_ref>(SNDFILE_ref());

			if (p != nullptr)
			{
				p->sfinfo.frames = 0;
				p->sfinfo.channels = channels;
				p->sfinfo.format = format;
				p->sfinfo.samplerate = samplerate;
				p->sfinfo.sections = 0;
				p->sfinfo.seekable = 0;

				p->sf = sf_wchar_open(wpath, mode, &p->sfinfo);
			}
		}

		void open(const std::string & path, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
		{
			p = std::make_shared<SNDFILE_ref>(SNDFILE_ref());

			if (p != nullptr)
			{
				p->sfinfo.frames = 0;
				p->sfinfo.channels = channels;
				p->sfinfo.format = format;
				p->sfinfo.samplerate = samplerate;
				p->sfinfo.sections = 0;
				p->sfinfo.seekable = 0;

				p->sf = sf_open(path.c_str(), mode, &p->sfinfo);
			}
		}

		void open(const std::wstring & wpath, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
		{
			p = std::make_shared<SNDFILE_ref>(SNDFILE_ref());

			if (p != nullptr)
			{
				p->sfinfo.frames = 0;
				p->sfinfo.channels = channels;
				p->sfinfo.format = format;
				p->sfinfo.samplerate = samplerate;
				p->sfinfo.sections = 0;
				p->sfinfo.seekable = 0;

				p->sf = sf_wchar_open(wpath.c_str(), mode, &p->sfinfo);
			}
		}

		void open(int fd, bool close_desc, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
		{
			if (fd < 0)
				return;

			p = std::make_shared<SNDFILE_ref>(SNDFILE_ref());

			if (p != nullptr)
			{
				p->sfinfo.frames = 0;
				p->sfinfo.channels = channels;
				p->sfinfo.format = format;
				p->sfinfo.samplerate = samplerate;
				p->sfinfo.sections = 0;
				p->sfinfo.seekable = 0;

				p->sf = sf_open_fd(fd, mode, &p->sfinfo, close_desc);
			}
		}

		void open(SF_VIRTUAL_IO &sfvirtual, void *user_data, int mode = SFM_READ, int format = 0, int channels = 0, int samplerate = 0)
		{
			p = std::make_shared<SNDFILE_ref>(SNDFILE_ref());

			if (p != nullptr)
			{
				p->sfinfo.frames = 0;
				p->sfinfo.channels = channels;
				p->sfinfo.format = format;
				p->sfinfo.samplerate = samplerate;
				p->sfinfo.sections = 0;
				p->sfinfo.seekable = 0;

				p->sf = sf_open_virtual(&sfvirtual, mode, &p->sfinfo, user_data);
			}
		}
		// ********************************
		// ********************************


		// ********************************
		bool is_open() const { return (p != nullptr) ? (p->sf != nullptr) : false; }
		//operator bool() const { return (p != nullptr); }
		bool operator == (const dspfile &rhs) const { return (p == rhs.p); }
		// ********************************

		// ********************************
		int64_t	get_frames(void) const		{ return (p != nullptr) ? p->sfinfo.frames : 0; }
		int		get_format(void) const		{ return (p != nullptr) ? p->sfinfo.format : 0; }
		int		get_channels(void) const	{ return (p != nullptr) ? p->sfinfo.channels : 0; }
		int		get_samplerate(void) const	{ return (p != nullptr) ? p->sfinfo.samplerate : 0; }
		// ********************************

		// ********************************
		int error(void) const
		{
			return sf_error(p->sf);
		}

		const char * get_error_str(void) const
		{
			return sf_strerror(p->sf);
		}

		std::string get_error_string(void) const
		{
			std::string ret(sf_strerror(p->sf));
			//const char * tmp = sf_strerror(p->sf);
			//int tmp_size = strlen(tmp);
			//ret.resize(tmp_size);
			//memcpy((void*)ret.data(), tmp, tmp_size);
			return ret;
		}
		// ********************************

		// ********************************
		int command(int cmd, void *data, int datasize)
		{
			return sf_command(p->sf, cmd, data, datasize);
		}
		// ********************************

		// ********************************
		// **** Set string metadata.
		// * Valid strings are:
		// *	SF_STR_TITLE
		// *	SF_STR_COPYRIGHT
		// *	SF_STR_SOFTWARE
		// *	SF_STR_ARTIST
		// *	SF_STR_COMMENT
		// *	SF_STR_DATE
		// *	SF_STR_ALBUM
		// *	SF_STR_LICENSE
		// *	SF_STR_TRACKNUMBER
		// *	SF_STR_GENRE
		// * Also, there are SF_STR_FIRST and SF_STR_LAST
		int set_string(int str_type, const std::string & str)
		{
			return sf_set_string(p->sf, str_type, str.c_str());
		}

		int set_string(int str_type, const char *str)
		{
			return sf_set_string(p->sf, str_type, str);
		}

		std::string get_string(int str_type) const
		{
			return std::string(sf_get_string(p->sf, str_type));
		}

		const char *get_cstring(int str_type) const
		{
			return sf_get_string(p->sf, str_type);
		}
		// ********************************


		// ********************************
		dsp::dspformat get_dspformat_from(int sf_fmt, int rate) const
		{
			dsp::dspformat ret;
			if (rate)
				ret.set_rate(rate);

			switch (sf_fmt & SF_FORMAT_SUBMASK)
			{
			case SF_FORMAT_PCM_S8:		//0x0001,       /* Signed 8 bit data */
			case SF_FORMAT_PCM_U8:		//0x0005,       /* Unsigned 8 bit data (WAV and RAW only) */
			case SF_FORMAT_DPCM_8:		//0x0050,       /* 8 bit differential PCM (XI only) */
				ret.set_bits(8);
				ret.set_float(false);
				break;

			case SF_FORMAT_PCM_16:		//0x0002,       /* Signed 16 bit data */
			case SF_FORMAT_DPCM_16:		//0x0051,       /* 16 bit differential PCM (XI only) */
			case SF_FORMAT_DWVW_12:		//0x0040,       /* 12 bit Delta Width Variable Word encoding. */
			case SF_FORMAT_DWVW_16:		//0x0041,       /* 16 bit Delta Width Variable Word encoding. */
			case SF_FORMAT_ALAC_16:		//0x0070,		/* Apple Lossless Audio Codec (16 bit). */
				ret.set_bits(16);
				ret.set_float(false);
				break;

			case SF_FORMAT_PCM_24:		//0x0003,       /* Signed 24 bit data */
			case SF_FORMAT_DWVW_24:		//0x0042,       /* 24 bit Delta Width Variable Word encoding. */
			case SF_FORMAT_ALAC_20:		//0x0071,		/* Apple Lossless Audio Codec (20 bit). */
			case SF_FORMAT_ALAC_24:		//0x0072,		/* Apple Lossless Audio Codec (24 bit). */
				ret.set_bits(24);
				ret.set_float(false);
				break;

			case SF_FORMAT_PCM_32:		//0x0004,       /* Signed 32 bit data */
			case SF_FORMAT_ALAC_32:		//0x0073,		/* Apple Lossless Audio Codec (32 bit). */
				ret.set_bits(32);
				ret.set_float(false);
				break;

			case SF_FORMAT_FLOAT:		//0x0006,       /* 32 bit float data */
				ret.set_bits(32);
				ret.set_float(true);
				break;

			case SF_FORMAT_DOUBLE:		//0x0007,       /* 64 bit float data */
			case SF_FORMAT_DWVW_N:		//0x0043,       /* N bit Delta Width Variable Word encoding. */
				ret.set_bits(64);
				ret.set_float(true);
				break;

			// ***TODO: Get more info on the folowing formats so we can convert them appropriately.
			case SF_FORMAT_ULAW:		//0x0010,       /* U-Law encoded. */
			case SF_FORMAT_ALAW:		//0x0011,       /* A-Law encoded. */
			case SF_FORMAT_IMA_ADPCM:	//0x0012,       /* IMA ADPCM. */
			case SF_FORMAT_MS_ADPCM:	//0x0013,       /* Microsoft ADPCM. */
			case SF_FORMAT_GSM610:		//0x0020,       /* GSM 6.10 encoding. */
			case SF_FORMAT_VOX_ADPCM:	//0x0021,       /* Oki Dialogic ADPCM encoding. */
			case SF_FORMAT_G721_32:		//0x0030,       /* 32kbs G721 ADPCM encoding. */
			case SF_FORMAT_G723_24:		//0x0031,       /* 24kbs G723 ADPCM encoding. */
			case SF_FORMAT_G723_40:		//0x0032,       /* 40kbs G723 ADPCM encoding. */
			case SF_FORMAT_VORBIS:		//0x0060,       /* Xiph Vorbis encoding. */
			default:
				ret.set_bits(32);
				ret.set_float(true);
				break;
			} // switch ()

			return ret;
		}

		dsp::dspformat get_dspformat() const
		{
			if (p != nullptr)
			{
				dsp::dspformat ret(p->sfinfo.frames, p->sfinfo.samplerate, p->sfinfo.channels);
				ret.set_interleaved(true);

				switch (p->sfinfo.format & SF_FORMAT_SUBMASK)
				{
				case SF_FORMAT_PCM_S8:		//0x0001,       /* Signed 8 bit data */
				case SF_FORMAT_PCM_U8:		//0x0005,       /* Unsigned 8 bit data (WAV and RAW only) */
				case SF_FORMAT_DPCM_8:		//0x0050,       /* 8 bit differential PCM (XI only) */
					ret.set_bits(8);
					ret.set_float(false);
					break;

				case SF_FORMAT_PCM_16:		//0x0002,       /* Signed 16 bit data */
				case SF_FORMAT_DPCM_16:		//0x0051,       /* 16 bit differential PCM (XI only) */
				case SF_FORMAT_DWVW_12:		//0x0040,       /* 12 bit Delta Width Variable Word encoding. */
				case SF_FORMAT_DWVW_16:		//0x0041,       /* 16 bit Delta Width Variable Word encoding. */
				case SF_FORMAT_ALAC_16:		//0x0070,		/* Apple Lossless Audio Codec (16 bit). */
					ret.set_bits(16);
					ret.set_float(false);
					break;

				case SF_FORMAT_PCM_24:		//0x0003,       /* Signed 24 bit data */
				case SF_FORMAT_DWVW_24:		//0x0042,       /* 24 bit Delta Width Variable Word encoding. */
				case SF_FORMAT_ALAC_20:		//0x0071,		/* Apple Lossless Audio Codec (20 bit). */
				case SF_FORMAT_ALAC_24:		//0x0072,		/* Apple Lossless Audio Codec (24 bit). */
					ret.set_bits(24);
					ret.set_float(false);
					break;

				case SF_FORMAT_PCM_32:		//0x0004,       /* Signed 32 bit data */
				case SF_FORMAT_ALAC_32:		//0x0073,		/* Apple Lossless Audio Codec (32 bit). */
					ret.set_bits(32);
					ret.set_float(false);
					break;

				case SF_FORMAT_FLOAT:		//0x0006,       /* 32 bit float data */
					ret.set_bits(32);
					ret.set_float(true);
					break;

				case SF_FORMAT_DOUBLE:		//0x0007,       /* 64 bit float data */
				case SF_FORMAT_DWVW_N:		//0x0043,       /* N bit Delta Width Variable Word encoding. */
					ret.set_bits(64);
					ret.set_float(true);
					break;

				// ***TODO: Get more info on the folowing formats so we can convert them appropriately.
				case SF_FORMAT_ULAW:		//0x0010,       /* U-Law encoded. */
				case SF_FORMAT_ALAW:		//0x0011,       /* A-Law encoded. */
				case SF_FORMAT_IMA_ADPCM:	//0x0012,       /* IMA ADPCM. */
				case SF_FORMAT_MS_ADPCM:	//0x0013,       /* Microsoft ADPCM. */
				case SF_FORMAT_GSM610:		//0x0020,       /* GSM 6.10 encoding. */
				case SF_FORMAT_VOX_ADPCM:	//0x0021,       /* Oki Dialogic ADPCM encoding. */
				case SF_FORMAT_G721_32:		//0x0030,       /* 32kbs G721 ADPCM encoding. */
				case SF_FORMAT_G723_24:		//0x0031,       /* 24kbs G723 ADPCM encoding. */
				case SF_FORMAT_G723_40:		//0x0032,       /* 40kbs G723 ADPCM encoding. */
				case SF_FORMAT_VORBIS:		//0x0060,       /* Xiph Vorbis encoding. */
				default:
					ret.set_bits(32);
					ret.set_float(true);
					break;
				} // switch ()
				return ret;
			} // if (p)
			return dsp::dspformat(); // Return default.
		}
		// ********************************

		// ********************************
		static int format_check(int format, int channels, int samplerate)
		{
			SF_INFO sfinfo;

			sfinfo.frames = 0;
			sfinfo.channels = channels;
			sfinfo.format = format;
			sfinfo.samplerate = samplerate;
			sfinfo.sections = 0;
			sfinfo.seekable = 0;

			return sf_format_check(&sfinfo);
		}
		// ********************************

		// ********************************
		int get_good_sf_format(std::string ext, dsp::dspformat fmt, int desired_sf = -1)
		{
			std::wstring tmp;
			int size = (int)ext.size();
			tmp.resize(size);
			for (int i = 0; i < size; ++i)
				tmp[i] = ext[i];

			return get_good_sf_formatw(tmp, fmt, desired_sf);
		}
		// ********************************

		// ********************************
		int get_good_sf_formatw(std::wstring ext, dsp::dspformat fmt, int desired_sf = -1)
		{
			typedef struct SF_FMT_LIST
			{
				const wchar_t *ext;
				int fmt;
			} SF_FMT_LIST;

			// Get sf format from extension
			static const SF_FMT_LIST fmts[] =
			{
				{ L".wav",	SF_FORMAT_WAV	}, // Microsoft WAV format (little endian).
				{ L".wve",	SF_FORMAT_WAVEX }, // MS WAVE with WAVEFORMATEX.
				{ L".w64",	SF_FORMAT_W64	}, // Sonic Foundry's 64 bit RIFF/WAV.
				{ L".rf64",	SF_FORMAT_RF64	}, // RF64 WAV file.
				{ L".aif",	SF_FORMAT_AIFF	}, // Apple/SGI AIFF format (big endian).
				{ L".aiff",	SF_FORMAT_AIFF	}, // Apple/SGI AIFF format (big endian).
				{ L".aifc",	SF_FORMAT_AIFF	}, // Apple/SGI AIFF format (big endian).
				{ L".au",	SF_FORMAT_AU	}, // Sun/NeXT AU format (big endian).
				{ L".paf",	SF_FORMAT_PAF	}, // Ensoniq PARIS file format.
				{ L".svx",	SF_FORMAT_SVX	}, // Amiga IFF / SVX8 / SV16 format.
				{ L".nist",	SF_FORMAT_NIST	}, // Sphere NIST format.
				{ L".sph",	SF_FORMAT_NIST	}, // Sphere NIST format.
				{ L".voc",	SF_FORMAT_VOC	}, // VOC files.
				{ L".sf",	SF_FORMAT_IRCAM	}, // Berkeley/IRCAM/CARL.
				{ L".mat",	SF_FORMAT_MAT4	}, // Matlab (tm) V4.2 / GNU Octave 2.0.
				{ L".mat",	SF_FORMAT_MAT5	}, // Matlab (tm) V5.0 / GNU Octave 2.1.
				{ L".pvf",	SF_FORMAT_PVF	}, // Portable Voice Format.
				{ L".xi",	SF_FORMAT_XI	}, // Fasttracker 2 Extended Instrument.
				{ L".htk",	SF_FORMAT_HTK	}, // HMM Tool Kit format.
				{ L".sds",	SF_FORMAT_SDS	}, // Midi Sample Dump Standard.
				{ L".avr",	SF_FORMAT_AVR	}, // Audio Visual Research.
				{ L".sd2",	SF_FORMAT_SD2	}, // Sound Designer 2.
				{ L".flac",	SF_FORMAT_FLAC	}, // FLAC lossless file format.
				{ L".caf",	SF_FORMAT_CAF	}, // Core Audio File format.
				{ L".wve",	SF_FORMAT_WVE	}, // Psion WVE format.
				{ L".ogg",	SF_FORMAT_OGG	}, // Xiph OGG container.
				{ L".oga",	SF_FORMAT_OGG	}, // Xiph OGG container.
				{ L".mpc",	SF_FORMAT_MPC2K	}, // Akai MPC 2000 sampler.
				{ L".raw",	SF_FORMAT_RAW	}, // RAW PCM data.
				{ L".pcm",	SF_FORMAT_RAW	}, // RAW PCM data.
				{ L".dbl",	SF_FORMAT_RAW	}, // RAW PCM data.
				{ L".",		SF_FORMAT_RAW	}, // RAW PCM data.
				{ L"",		SF_FORMAT_WAV	}, // Default.
				{ nullptr,	0				}  // Terminate...
			};

			// make sure that the extension is all lowercase.
			std::transform(ext.begin(), ext.end(), ext.begin(), ::tolower);

			// Select format container by extension.
			// The list includes a format with an extension of a single dot '.' to force a raw format.
			// Default to WAV.
			int ret = SF_FORMAT_WAV;
			for (int i = 0; fmts[i].ext != nullptr; ++i)
			{
				if (ext == fmts[i].ext)
				{
					ret = fmts[i].fmt;
					break;
				}
			}

			// Setup macro
			#define TEST_SF_FORMAT(FNUM)	\
				if (format_check(ret | FNUM, fmt.get_channels(), fmt.get_rate())) { \
					ret |= FNUM; break; }

			// Test our default to value first.
			if ((desired_sf != -1))
			{
				desired_sf = (desired_sf & SF_FORMAT_SUBMASK) | ret;
				if (format_check(desired_sf, fmt.get_channels(), fmt.get_rate()))
					return desired_sf;
			}

			// Select codec type by querying libsndfile.
			switch ((fmt.get_bits() + 7) / 8)
			{
			case 1:
				// Check for support for a signed 8-bit format first.
				TEST_SF_FORMAT(SF_FORMAT_PCM_S8);

				// Check for support for a unsigned 8-bit since the signed form is not supported.
				TEST_SF_FORMAT(SF_FORMAT_PCM_U8);
				TEST_SF_FORMAT(SF_FORMAT_DPCM_8);

				// Fall-through.
			case 2:
				TEST_SF_FORMAT(SF_FORMAT_ALAC_16);	// Apple Lossless Audio Codec (16 bit).
				TEST_SF_FORMAT(SF_FORMAT_ALAC_20);	// Apple Lossless Audio Codec (20 bit).
				TEST_SF_FORMAT(SF_FORMAT_PCM_16);
				TEST_SF_FORMAT(SF_FORMAT_DPCM_16);
				TEST_SF_FORMAT(SF_FORMAT_DWVW_16);	// 16 bit Delta Width Variable Word encoding.
				TEST_SF_FORMAT(SF_FORMAT_DWVW_12);	// 12 bit Delta Width Variable Word encoding.

				// Fall-through.
			case 3:
				if (fmt.get_bits() <= 20)
				{
					TEST_SF_FORMAT(SF_FORMAT_ALAC_20);	// Apple Lossless Audio Codec (20 bit).
				}

				// 24-bit should default to 24-bit integer.
				TEST_SF_FORMAT(SF_FORMAT_ALAC_24);	// Apple Lossless Audio Codec (24 bit).
				TEST_SF_FORMAT(SF_FORMAT_PCM_24);

				// If 24-bit integers are not supported then check for 32-bit floating-point support
				TEST_SF_FORMAT(SF_FORMAT_FLOAT);
				TEST_SF_FORMAT(SF_FORMAT_DWVW_24);		// 24 bit Delta Width Variable Word encoding.

				// Fall-through.
			case 4:
				TEST_SF_FORMAT(SF_FORMAT_ALAC_32);	// Apple Lossless Audio Codec (32 bit).
				if (fmt.is_floats())
				{
					TEST_SF_FORMAT(SF_FORMAT_FLOAT);

					// 24-bit should otherwise default to 24-bit integer.
					TEST_SF_FORMAT(SF_FORMAT_PCM_24);
				}

				// If it is not floating-point or it is not supported then check for 32-bit integer support.
				TEST_SF_FORMAT(SF_FORMAT_PCM_32);

				// Fall-through.
			case 8:
				TEST_SF_FORMAT(SF_FORMAT_DOUBLE);

				// Fall-through.
			default:
				// Check all formats while going for less and less bit resolution...
				TEST_SF_FORMAT(SF_FORMAT_PCM_32);
				TEST_SF_FORMAT(SF_FORMAT_ALAC_32);	// Apple Lossless Audio Codec (24 bit).
				TEST_SF_FORMAT(SF_FORMAT_FLOAT);
				TEST_SF_FORMAT(SF_FORMAT_ALAC_24);	// Apple Lossless Audio Codec (24 bit).
				TEST_SF_FORMAT(SF_FORMAT_PCM_24);
				TEST_SF_FORMAT(SF_FORMAT_ALAC_20);	// Apple Lossless Audio Codec (20 bit).
				TEST_SF_FORMAT(SF_FORMAT_ALAC_16);	// Apple Lossless Audio Codec (16 bit).
				TEST_SF_FORMAT(SF_FORMAT_VORBIS);	// Xiph Vorbis encoding.
				TEST_SF_FORMAT(SF_FORMAT_PCM_16);
				TEST_SF_FORMAT(SF_FORMAT_DPCM_16);
				TEST_SF_FORMAT(SF_FORMAT_DWVW_16);	// 16 bit Delta Width Variable Word encoding.


				TEST_SF_FORMAT(SF_FORMAT_DWVW_N);	// N bit Delta Width Variable Word encoding.
				TEST_SF_FORMAT(SF_FORMAT_DWVW_12);	// 12 bit Delta Width Variable Word encoding.
				TEST_SF_FORMAT(SF_FORMAT_PCM_S8);
				TEST_SF_FORMAT(SF_FORMAT_PCM_U8);
				TEST_SF_FORMAT(SF_FORMAT_DPCM_8);

				TEST_SF_FORMAT(SF_FORMAT_G723_40);	// 40kbs G723 ADPCM encoding.
				TEST_SF_FORMAT(SF_FORMAT_G721_32);	// 32kbs G721 ADPCM encoding.
				TEST_SF_FORMAT(SF_FORMAT_G723_24);	// 24kbs G723 ADPCM encoding.

				TEST_SF_FORMAT(SF_FORMAT_ULAW);		// U-Law encoded.
				TEST_SF_FORMAT(SF_FORMAT_ALAW);		// A-Law encoded.

				TEST_SF_FORMAT(SF_FORMAT_IMA_ADPCM);// IMA ADPCM.
				TEST_SF_FORMAT(SF_FORMAT_MS_ADPCM);	// Microsoft ADPCM.

				TEST_SF_FORMAT(SF_FORMAT_GSM610);	// GSM 6.10 encoding.
				TEST_SF_FORMAT(SF_FORMAT_VOX_ADPCM);// OKI / Dialogix ADPCM
			}
			return ret;
		}
		#undef TEST_SF_FORMAT
		// ********************************
		// ********************************


		// ********************************
		int64_t seek(int64_t frame, int whence)
		{
			return sf_seek(p->sf, frame, whence);
		}
		// ********************************

		// ********************************
		void writeSync(void)
		{
			sf_write_sync(p->sf);
		}
		// ********************************

		// ********************************
		// **** Read functions.
		template <typename _Type>
		inline int64_t read(_Type *ptr, int64_t items)
		{
			static_assert(true, "read<_Type>() template function does not know of _Type."); return 0;
		}

		template <>
		inline int64_t read<uint8_t>(uint8_t *ptr, int64_t items)
		{
			return sf_read_raw(p->sf, ptr, items);
		}

		template <>
		inline int64_t read<int8_t>(int8_t *ptr, int64_t items)
		{
			return sf_read_raw(p->sf, ptr, items);
		}

		template <>
		inline int64_t read<int16_t>(int16_t *ptr, int64_t items)
		{
			return sf_read_short(p->sf, ptr, items);
		}

		template <>
		inline int64_t read<int24_t>(int24_t *ptr, int64_t items)
		{
			return sf_read_raw(p->sf, ptr, items * 3) / 3;
		}

		template <>
		inline int64_t read<int32_t>(int32_t *ptr, int64_t items)
		{
			return sf_read_int(p->sf, ptr, items);
		}

		template <>
		inline int64_t read<float>(float *ptr, int64_t items)
		{
			return sf_read_float(p->sf, ptr, items);
		}

		template <>
		inline int64_t read<double>(double *ptr, int64_t items)
		{
			return sf_read_double(p->sf, ptr, items);
		}
		// ********************************
		// ********************************


		// ********************************
		// **** Write functions.
		template <typename _Type>
		inline int64_t write(const _Type *ptr, int64_t items)
		{
			static_assert(true, "write<_Type>() template function does not know of _Type."); return 0;
		}

		template <>
		inline int64_t write<uint8_t>(const uint8_t *ptr, int64_t items)
		{
			return sf_write_raw(p->sf, ptr, items);
		}

		template <>
		inline int64_t write<int8_t>(const int8_t *ptr, int64_t items)
		{
			return sf_write_raw(p->sf, ptr, items);
		}

		template <>
		inline int64_t write<int16_t>(const int16_t *ptr, int64_t items)
		{
			return sf_write_short(p->sf, ptr, items);
		}

		template <>
		inline int64_t write<int24_t>(const int24_t *ptr, int64_t items)
		{
			return sf_write_raw(p->sf, ptr, items * 3) / 3;
		}

		template <>
		inline int64_t write<int32_t>(const int32_t *ptr, int64_t items)
		{
			return sf_write_int(p->sf, ptr, items);
		}

		template <>
		inline int64_t write<float>(const float *ptr, int64_t items)
		{
			return sf_write_float(p->sf, ptr, items);
		}

		template <>
		inline int64_t write<double>(const double *ptr, int64_t items)
		{
			return sf_write_double(p->sf, ptr, items);
		}
		// ********************************
		// ********************************


		// ********************************
		// **** Read frames functions.
		template <typename _Type>
		inline int64_t read_frames(_Type *ptr, int64_t frame_count)
		{
			static_assert(true, "read_frames<_Type>() template function does not know of _Type."); return 0;
		}

		template <>
		inline int64_t read_frames<uint8_t>(uint8_t *ptr, int64_t frame_count)
		{
			return sf_read_raw(p->sf, ptr, frame_count * p->sfinfo.channels) / p->sfinfo.channels;
		}

		template <>
		inline int64_t read_frames<int8_t>(int8_t *ptr, int64_t frame_count)
		{
			return sf_read_raw(p->sf, ptr, frame_count * p->sfinfo.channels) / p->sfinfo.channels;
		}

		template <>
		inline int64_t read_frames<int16_t>(int16_t *ptr, int64_t frame_count)
		{
			return sf_readf_short(p->sf, ptr, frame_count);
		}

		template <>
		inline int64_t read_frames<int24_t>(int24_t *ptr, int64_t frame_count)
		{
			int64_t m = p->sfinfo.channels * 3;
			return sf_read_raw(p->sf, ptr, frame_count * m) / m;
		}

		template <>
		inline int64_t read_frames<int32_t>(int32_t *ptr, int64_t frame_count)
		{
			return sf_readf_int(p->sf, ptr, frame_count);
		}

		template <>
		inline int64_t read_frames<float>(float *ptr, int64_t frame_count)
		{
			return sf_readf_float(p->sf, ptr, frame_count);
		}

		template <>
		inline int64_t read_frames<double>(double *ptr, int64_t frame_count)
		{
			return sf_readf_double(p->sf, ptr, frame_count);
		}
		// ********************************
		// ********************************


		// ********************************
		// **** Write frames functions.
		template <typename _Type>
		inline int64_t write_frames(const _Type *ptr, int64_t frame_count)
		{
			static_assert(true, "write_frames<_Type>() template function does not know of _Type."); return 0;
		}

		template <>
		inline int64_t write_frames<uint8_t>(const uint8_t *ptr, int64_t frame_count)
		{
			return sf_write_raw(p->sf, ptr, frame_count * p->sfinfo.channels) / p->sfinfo.channels;
		}

		template <>
		inline int64_t write_frames<int8_t>(const int8_t *ptr, int64_t frame_count)
		{
			return sf_write_raw(p->sf, ptr, frame_count * p->sfinfo.channels) / p->sfinfo.channels;
		}

		template <>
		inline int64_t write_frames<int16_t>(const int16_t *ptr, int64_t frame_count)
		{
			return sf_writef_short(p->sf, ptr, frame_count);
		}

		template <>
		inline int64_t write_frames<int24_t>(const int24_t *ptr, int64_t frame_count)
		{
			int64_t m = p->sfinfo.channels * 3;
			return sf_write_raw(p->sf, ptr, frame_count * m) / m;
		}

		template <>
		inline int64_t write_frames<int32_t>(const int32_t *ptr, int64_t frame_count)
		{
			return sf_writef_int(p->sf, ptr, frame_count);
		}

		template <>
		inline int64_t write_frames<float>(const float *ptr, int64_t frame_count)
		{
			return sf_writef_float(p->sf, ptr, frame_count);
		}

		template <>
		inline int64_t write_frames<double>(const double *ptr, int64_t frame_count)
		{
			return sf_writef_double(p->sf, ptr, frame_count);
		}
		// ********************************
		// ********************************


		// ********************************
		// **** Raw read/write functions.
		inline int64_t read_raw(void *ptr, int64_t bytes)
		{
			return sf_read_raw(p->sf, ptr, bytes);
		}

		inline int64_t write_raw(const void *ptr, int64_t bytes)
		{
			return sf_write_raw(p->sf, ptr, bytes);
		}
		// ********************************
		// ********************************


		// ********************************
		// **** C++ Read/Write functions.
		template <typename _Type, class _Alloc>
		inline int64_t read(std::vector<_Type, _Alloc> &buf)
		{
			return read((_Type *)buf.data(), buf.size());
		}

		template <typename _Type, class _Alloc>
		inline int64_t write(const std::vector<_Type, _Alloc> &buf)
		{
			return write((_Type *)buf.data(), buf.size());
		}

		template <typename _Type, size_t _Size>
		inline int64_t read(std::array<_Type, _Size> &buf)
		{
			return read((_Type *)buf.data(), buf.size());
		}

		template <typename _Type, size_t _Size>
		inline int64_t write(const std::array<_Type, _Size> &buf)
		{
			return write((_Type *)buf.data(), buf.size());
		}

		template <size_t _Size, typename _Type, bool _Native>
		inline int64_t read(dsp::dsparray<_Size, _Type, _Native> &buf)
		{
			return read<_Type>((_Type *)buf.data(), buf.size());
		}

		template <size_t _Size, typename _Type, bool _Native>
		inline int64_t write(const dsp::dsparray<_Size, _Type, _Native> &buf)
		{
			return write<_Type>((_Type *)buf.data(), buf.size());
		}

		template <typename _Type, bool _Native, class _Alloc>
		inline int64_t read(dsp::dspvector<_Type, _Native, _Alloc> &buf)
		{
			return read<_Type>((_Type *)buf.data(), buf.size());
		}

		template <typename _Type, bool _Native, class _Alloc>
		inline int64_t write(const dsp::dspvector<_Type, _Native, _Alloc> &buf)
		{
			return write<_Type>((_Type *)buf.data(), buf.size());
		}
		// ********************************
		// ********************************
	};
	// **** End dspfile
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
