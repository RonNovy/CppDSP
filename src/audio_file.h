/* Main audio file reading class.
 * Copyright (C) 2015
 * Ron S. Novy
 */
#pragma once
#include "configure.h"

#include <vector>

#include "bstream.h"
#include "dsp_containers.h"


typedef enum STREAM_TYPE
{
	UNKNOWN_T = 0,
	INTEGER_T,
	FLOAT_T
} STREAM_TYPE;


typedef struct audio_format
{
	STREAM_TYPE type;
	int bits;
	int rate;
	int channels;
} audio_format;


enum {
	AF_READ = 1,
	AF_WRITE,
	AF_READ_WRITE
};


class audio_file
{
private:
	 dsp::bstream filestream;
	 audio_format format;
	 typedef enum AF_SEEK_e {
		 kSet = 0,
		 kCurrent,
		 kEnd
	 } AF_SEEK_e;

public:

	audio_file();
	audio_file(std::wstring file, std::fstream::openmode mode = std::fstream::in | std::fstream::binary, STREAM_TYPE _type = INTEGER_T, int _bits = 16, int _rate = 44100, int _channels = 2)
	{
		format.type = _type;
		format.bits = _bits;
		format.rate = _rate;
		format.channels = _channels;
		open(file, mode | std::fstream::binary, format.type, format.bits, format.rate, format.channels);
	};

	// ** Is the file open.
	inline bool is_open() { return filestream.is_open(); };

	// ** Virtual destructor...
	virtual ~audio_file() {};

	// Format can read/write?
	virtual bool can_read() = 0;
	virtual bool can_write() = 0;

	// Find the closest compatible format.
	virtual void get_compatible_format(audio_format &in, audio_format &out) = 0;

	// Open file
	virtual bool open(std::wstring file, std::fstream::openmode mode, STREAM_TYPE &_type, int &_bits, int &_rate, int &_channels) = 0;

	// verify(): Returns true if this file can be read by this class.
	virtual bool verify() = 0;

	// Give the name of this class, the name of the plug-in and the extensions supported.
	virtual void class_name(std::wstring &str) = 0;
	virtual void name(std::wstring &str) = 0;
	virtual void extensions(std::vector<std::wstring> &list) = 0;

	// Seek functions
	virtual bool can_seek() = 0;
	virtual bool read_seek(uint64_t pos, AF_SEEK_e r) = 0;
	virtual bool write_seek(uint64_t pos, AF_SEEK_e r) = 0;

	// Read functions
	//virtual bool set_read_parameters(uint64 buf_size, bool interleved = true) = 0;
	//virtual uint64 read(void *buff, uint64 num_samples) = 0;

	// Write functions


};

