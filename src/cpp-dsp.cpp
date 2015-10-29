/* DLL functions for processing audio.
 * Copyright (C) 2015
 * Ron S. Novy
 */

#include "cpp-dsp.h"
#include "split-combine.h"

// ******************************** ******************************** ********************************
// ********************************
// **** Windows specific things
#ifdef _WIN32
#include <Windows.h>

BOOL WINAPI DllMain(
	HINSTANCE hinstDLL,	// handle to DLL module
	DWORD fdwReason,	// reason for calling function
	LPVOID lpReserved)	// reserved
{
	// Perform actions based on the reason for calling.
	switch (fdwReason)
	{
	case DLL_PROCESS_ATTACH:
		// Initialize once for each new process.
		// Return FALSE to fail DLL load.
		break;

	case DLL_THREAD_ATTACH:
		// Do thread-specific initialization.
		break;

	case DLL_THREAD_DETACH:
		// Do thread-specific cleanup.
		break;

	case DLL_PROCESS_DETACH:
		// Perform any necessary cleanup.
		break;
	}
	return TRUE;  // Successful DLL_PROCESS_ATTACH.
}
#endif //ifdef _WIN32
// **** End Windows specific things
// ********************************
// ******************************** ******************************** ********************************



/*
// ********************************
// **** dsp_sc_
CPP_DSP_API_VB int VBCALL dsp_sc_(DSPPTR _this)
{
#pragma EXPORT_ALIAS
	bool ret = true;
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;

	return (ret) ? DSP_OK : DSP_ERROR;
}
// ********************************
*/


// ********************************
// **** dsp_sc_interface
//class dsp_sc_interface
//{
//public:
	// ********************************
	// **** dsp_sc_start - To allocate a special split/combine processor.
	int VBCALL dsp_sc_interface::start(DSPPTR &_this)
	{
		#pragma EXPORT_ALIASX(dsp_sc_start)
		dsp::dsp_split_combine *ret = new dsp::dsp_split_combine;
		_this = ((DSPPTR)ret);
		return (ret != nullptr) ? DSP_OK : DSP_ERROR;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_end - Destroy the information assigned to the split/combine processor.
	int VBCALL dsp_sc_interface::end(DSPPTR &_this)
	{
		#pragma EXPORT_ALIASX(dsp_sc_end)
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		if (_this)
		{
			delete sc_this;
			_this = 0;
			return DSP_OK;
		}
		return DSP_ERROR;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_clear - clear all input and output files to begin adding more.
	int VBCALL dsp_sc_interface::clear(DSPPTR _this)
	{
		#pragma EXPORT_ALIASX(dsp_sc_clear)
		bool ret = true;
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		ret = sc_this->clear();
		return (ret) ? DSP_OK : DSP_ERROR;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_add_input - add an input file to the input list.
	int VBCALL dsp_sc_interface::add_input(DSPPTR _this, const char *name, int &channels)
	{
		#pragma EXPORT_ALIASX(dsp_sc_add_input)
		bool ret = true;
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		ret = sc_this->add_input(name, channels);
		return (ret) ? DSP_OK : DSP_ERROR;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_add_output - add an output file to the output list.
	int VBCALL dsp_sc_interface::add_output(DSPPTR _this, const char *name, int fmtcodec, int rate)
	{
		#pragma EXPORT_ALIASX(dsp_sc_add_output)
		bool ret = true;
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		ret = sc_this->add_output(name, fmtcodec, rate);
		return (ret) ? DSP_OK : DSP_ERROR;
	}
	// ********************************


	// ********************************
	// ****  dsp_sc_get_error - Fill the string buffer pointer to by 'buf' (of 'size')
	// **** with the contents of the error string.
	int VBCALL dsp_sc_interface::get_error(DSPPTR _this, char *buf, int size)
	{
		#pragma EXPORT_ALIASX(dsp_sc_get_error)
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;

		const char *tmp = sc_this->get_error_str();
		int i;
		for (i = 0; (i < (size - 1)) && tmp[i] != 0; ++i)
			buf[i] = tmp[i];
		buf[i++] = 0;

		return DSP_OK;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_do_split
	int VBCALL dsp_sc_interface::do_split(DSPPTR _this)
	{
		#pragma EXPORT_ALIASX(dsp_sc_do_split)
		bool ret = true;
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		ret = sc_this->do_split();
		return (ret) ? DSP_OK : DSP_ERROR;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_do_combine
	int VBCALL dsp_sc_interface::do_combine(DSPPTR _this)
	{
		#pragma EXPORT_ALIASX(dsp_sc_do_combine)
		bool ret = true;
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		ret = sc_this->do_combine();
		return (ret) ? DSP_OK : DSP_ERROR;
	}
	// ********************************


	// ********************************
	// **** dsp_sc_do_convert
	int VBCALL dsp_sc_interface::do_convert(DSPPTR _this)
	{
		#pragma EXPORT_ALIASX(dsp_sc_do_convert)
		bool ret = true;
		dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
		ret = sc_this->do_convert();
		return (ret) ? DSP_OK : DSP_ERROR;
	}
	// ********************************
//};

CPP_DSP_API dsp_sc_interface sc_interface;

// **** dsp_sc_interface
// ********************************


#if 0
// ********************************
// **** dsp_sc_start - To allocate a special split/combine processor.
CPP_DSP_API_VB int VBCALL dsp_sc_start(DSPPTR &_this)
{
#pragma EXPORT_ALIAS
	dsp::dsp_split_combine *ret = new dsp::dsp_split_combine;
	_this = ((DSPPTR)ret);
	return (ret != nullptr) ? DSP_OK : DSP_ERROR;
}
// ********************************


// ********************************
// **** dsp_sc_end - Destroy the information assigned to the split/combine processor.
CPP_DSP_API_VB int VBCALL dsp_sc_end(DSPPTR &_this)
{
#pragma EXPORT_ALIAS
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
	if (_this)
	{
		delete sc_this;
		_this = 0;
		return DSP_OK;
	}
	return DSP_ERROR;
}
// ********************************


// ********************************
// **** dsp_sc_add_input - add an input file to the input list.
CPP_DSP_API_VB int VBCALL dsp_sc_add_input(DSPPTR _this, const char *name, int &channels)
{
#pragma EXPORT_ALIAS
	bool ret = true;
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
	ret = sc_this->add_input(name, channels);
	return (ret) ? DSP_OK : DSP_ERROR;
}
// ********************************


// ********************************
// **** dsp_sc_add_output - add an output file to the output list.
CPP_DSP_API_VB int VBCALL dsp_sc_add_output(DSPPTR _this, const char *name, int fmtcodec, int rate)
{
#pragma EXPORT_ALIAS
	bool ret = true;
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
	ret = sc_this->add_output(name, fmtcodec, rate);
	return (ret) ? DSP_OK : DSP_ERROR;
}
// ********************************


// ********************************
// ****  dsp_sc_get_error - Fill the string buffer pointer to by 'buf' (of 'size')
// **** with the contents of the error string.
CPP_DSP_API_VB int VBCALL dsp_sc_get_error(DSPPTR _this, char *buf, int size)
{
#pragma EXPORT_ALIAS
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
	
	const char *tmp = sc_this->get_error_str();
	int i;
	for (i = 0; (i < (size - 1)) && tmp[i] != 0; ++i)
		buf[i] = tmp[i];
	buf[i++] = 0;

	return DSP_OK;
}
// ********************************


// ********************************
// **** dsp_sc_do_combine
CPP_DSP_API_VB int VBCALL dsp_sc_do_combine(DSPPTR _this)
{
#pragma EXPORT_ALIAS
	bool ret = true;
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
	ret = sc_this->do_combine();
	return (ret) ? DSP_OK : DSP_ERROR;
}
// ********************************


// ********************************
// **** dsp_sc_do_split
CPP_DSP_API_VB int VBCALL dsp_sc_do_split(DSPPTR _this)
{
#pragma EXPORT_ALIAS
	bool ret = true;
	dsp::dsp_split_combine *sc_this = (dsp::dsp_split_combine *)_this;
	ret = sc_this->do_split();
	return (ret) ? DSP_OK : DSP_ERROR;
}
// ********************************
#endif // if 0

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
