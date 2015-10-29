/* Header for c++dsp.dll import/export stuff and basic functions for VB...
 * Copyright (C) 2015
 * Written by Ron S. Novy
 */

#ifndef _CPPDSP_DLL_H_
#define _CPPDSP_DLL_H_


#define VBEXTERN
//#define VBEXTERN extern "C"
#define VBCALL __stdcall

#ifdef CDSP_EXPORTS
	#define STRINGIFY(x) #x
	#define STRINGIFYEX(x) STRINGIFY(x)

	// EXPORT_ALIAS is used at the top of a function definition to define
	// an undecorated alias for the function.  This is needed to allow old
	// VBasic to link to DLL functions with by providing a de-mangled alias to
	// the mangled name function.  To use it add "EXPORT_ALIAS" inside
	// of the function you want exported to Borland Basic.
	#ifndef EXPORT_ALIAS
		#if _MSC_VER > 1200 // Later than Visual Studio 6.0
			//#define EXPORT_ALIAS _Pragma(STRINGIFY(comment(linker, "/EXPORT:"__FUNCTION__"="__FUNCDNAME__)))
			#define EXPORT_ALIAS comment(linker, "/EXPORT:"__FUNCTION__"="__FUNCDNAME__)
			#define EXPORT_ALIASX(x) comment(linker, "/EXPORT:" STRINGIFYEX(x) "=" __FUNCDNAME__)
		#else // Cannot use this way of exporting functions.
			#define EXPORT_ALIAS
		#endif // else need to use DEF file or __declspec(dllexport)
	#endif // ifndef EXPORT_ALIAS

	#define CPP_DSP_API __declspec(dllexport)
	//#define CPP_DSP_API_VB EXTERN_C CPP_DSP_API
	//#define CPP_DSP_API_VB VBCALL CPP_DSP_API

#else
	// Include the library automatically
	#pragma comment( lib, "c++dsp" )
	#define EXPORT_ALIAS
	#define CPP_DSP_API __declspec(dllimport)
	#define CPP_DSP_API_VB VBEXTERN CPP_DSP_API

#endif


// Some debugging stuff...
#ifdef _DEBUG
	#define FTRACE()	OutputDebugStringW(__FUNCTIONW__ L"\n")
	#define TRACEF(fmt, ...) { char t[1024]; sprintf_s(t, sizeof(t), fmt, __VA_ARGS__ ); OutputDebugStringA(t); }
#else
	#define FTRACE()
	#define TRACEF(fmt, ...) 
#endif


// ********************************
// **** DSPPTR is set to an integer that is the same size as a pointer for passing to VB.
#if defined(_WIN64)
	#define DSPPTR long long
#elif defined(_WIN32)
	#define DSPPTR int
#else
	#warning FIXME : Unknown platform.
	#define DSPPTR int
#endif

#define DSP_ERROR	0
#define DSP_OK		1
// ********************************


// ********************************
// **** Exports
class CPP_DSP_API dsp_sc_interface
{
public:
	virtual ~dsp_sc_interface() {};
	virtual int VBCALL start(DSPPTR &_this);
	virtual int VBCALL end(DSPPTR &_this);
	virtual int VBCALL clear(DSPPTR _this);
	virtual int VBCALL add_input(DSPPTR _this, const char *name, int &channels);
	virtual int VBCALL add_output(DSPPTR _this, const char *name, int fmtcodec, int rate);
	virtual int VBCALL get_error(DSPPTR _this, char *buf, int size);
	virtual int VBCALL do_split(DSPPTR _this);
	virtual int VBCALL do_combine(DSPPTR _this);
	virtual int VBCALL do_convert(DSPPTR _this);
};
// **** End exports
// ********************************


#if 0//ndef CDSP_EXPORTS
	CPP_DSP_API_VB int VBCALL dsp_sc_start(DSPPTR &_this);
	CPP_DSP_API_VB int VBCALL dsp_sc_end(DSPPTR &_this);
	CPP_DSP_API_VB int VBCALL dsp_sc_clear(DSPPTR &_this);
	CPP_DSP_API_VB int VBCALL dsp_sc_add_input(DSPPTR _this, const char *name, int &channels);
	CPP_DSP_API_VB int VBCALL dsp_sc_add_output(DSPPTR _this, const char *name, int fmtcodec, int rate);
	CPP_DSP_API_VB int VBCALL dsp_sc_get_error(DSPPTR _this, char *buf, int size);
	CPP_DSP_API_VB int VBCALL dsp_sc_do_split(DSPPTR _this);
	CPP_DSP_API_VB int VBCALL dsp_sc_do_combine(DSPPTR _this);
	CPP_DSP_API_VB int VBCALL dsp_sc_do_convert(DSPPTR _this);

#endif // if 0
#ifndef CDSP_EXPORTS
	CPP_DSP_API dsp_sc_interface sc_interface;
	#define dsp_sc_start		sc_interface.start
	#define dsp_sc_end			sc_interface.end
	#define dsp_sc_clear		sc_interface.clear
	#define dsp_sc_add_input	sc_interface.add_input
	#define dsp_sc_add_output	sc_interface.add_output
	#define dsp_sc_get_error	sc_interface.get_error
	#define dsp_sc_do_split		sc_interface.do_split
	#define dsp_sc_do_combine	sc_interface.do_combine
	#define dsp_sc_do_convert	sc_interface.do_convert
#endif

#endif // _CPPDSP_DLL_H_