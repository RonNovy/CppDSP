/* Audio file plug-ing class.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#include "audio_file.h"

class audio_file_raw : public audio_file
{
private:
	bstream filestream;
	audio_format format;
public:

	audio_file_raw();
	audio_file_raw(std::wstring file, std::fstream::openmode mode = std::fstream::in | std::fstream::binary, STREAM_TYPE _type = INTEGER_T, int _bits = 16, int _rate = 44100, int _channels = 2)
	{
		format.type = _type;
		format.bits = _bits;
		format.rate = _rate;
		format.channels = _channels;
		open(file, mode | std::fstream::binary, format.type, format.bits, format.rate, format.channels);
	};

	// ** Virtual destructor...
	virtual ~audio_file_raw() {};

	// ** Is the file open.
	inline bool is_open() { return filestream.is_open(); };

	// Format can read/write?
	virtual bool can_read() { return true; };
	virtual bool can_write() { return true; };

	// Find the closest compatible format.
	virtual void get_compatible_format(audio_format &in, audio_format &out)
	{
		out = in;
	};

	// Open file
	virtual bool open(std::wstring file, std::fstream::openmode mode, STREAM_TYPE &_type, int &_bits, int &_rate, int &_channels)
	{
		filestream.open(file, mode | std::fstream::binary);
		{
			std::wstring ext = file.substr(file.rfind(__T('.')));
			app::strtolower(ext);
			verbose() << "Loaded " << ext << " file." << std::endl;
			if (ext == __T("dbl"))
			{
				_type = FLOAT_T;
				_bits = 64;
			}
			else if (ext == __T("sd2"))
			{
				_type = INTEGER_T;
				if (_bits > 24)
					_bits = 24;
			}
		}
		return filestream.is_open();
	};

	// verify(): Returns true if this file can be read by this class.
	virtual bool verify() { return true; };

	// Give the name of this class, the name of the plug-in and the extensions supported.
	virtual void class_name(std::wstring &str)	{ str = std::wstring(__T("audio_file")); };
	virtual void name(std::wstring &str)		{ str = std::wstring(__T("NULL")); };
	virtual void extensions(std::vector<std::wstring> &list)
	{
		list.clear();
		list.emplace_back(__T("raw"));		// Give all the supported extensions in lowercase.
		list.emplace_back(__T("sd2"));		// Sound Designer 2 default raw format.
		list.emplace_back(__T("dbl"));		// Double precision floating-point.
	};

	//

};

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
