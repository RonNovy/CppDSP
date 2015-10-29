CppDSP Template Library

This library is in early alpha and is usable as it is now.  However, there are a number of huge changes that are planned for the future that will make the current code incompatible with future versions.

This is a C++ template library for doing very basic DSP operations on arrays or vectors of data.  It allows you to do basic operations like addition, subtraction, multiplication and division on two arrays or vectors as if they were fundamental types like int or float.

Example:

	dsp::dspvector<float>	a(8);	// Creates a vector of 8 floats.
	dsp::dsparray<8, float>	b;		// creates an array of 8 floats.
	a *= b;							// Multiplies vector a times array b and places the result in a.

There is no documentation for this at all but a small presentation was created using Google Docs that is available on request...

Ron (dot) Novy (at) Yahoo (dot) com
