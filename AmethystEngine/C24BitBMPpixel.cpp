#define _CRTDBG_MAP_ALLOC
#include <cstdlib>
#include <crtdbg.h>
#include <memory>

#ifdef _DEBUG
#define DEBUG_NEW new (_NORMAL_BLOCK , __FILE__ , __LINE__)
#else
#define DBG_NEW
#endif

// Written by Michael Feeney, Fanshawe College, 2006
// mfeeney@fanshawec.on.ca
// It may be distributed under the terms of the MIT License:
// http://opensource.org/licenses/MIT
// (Also, it would be Supah Cool if I heard this was remotely useful!)
// Use this code at your own risk. It is indented only as a learning aid.

#include "C24BitBMPpixel.hpp"

// Default constructor - all zeros
C24BitBMPpixel::C24BitBMPpixel()
: redPixel(0), greenPixel(0), bluePixel(0)
{
	return;
}

C24BitBMPpixel::C24BitBMPpixel(uchar red, uchar green, uchar blue)
: redPixel(red), greenPixel(green), bluePixel(blue)
{
	return;
}

C24BitBMPpixel::~C24BitBMPpixel()
{
	return;
}

// Default constructor - all zeros
C32BitBMPpixel::C32BitBMPpixel()
: redPixel(0), greenPixel(0), bluePixel(0), alphaPixel(1)
{
	return;
}

C32BitBMPpixel::C32BitBMPpixel(uchar red, uchar green, uchar blue)
: redPixel(red), greenPixel(green), bluePixel(blue), alphaPixel(1)
{
	return;
}

C32BitBMPpixel::C32BitBMPpixel(uchar red, uchar green, uchar blue, uchar aplha)
: redPixel(red), greenPixel(green), bluePixel(blue), alphaPixel(aplha)
{
	return;
}

C32BitBMPpixel::~C32BitBMPpixel()
{
	return;
}
