
// sfeTkError.h
//
// General header file for the SparkFun Toolkit
/*
The MIT License (MIT)

Copyright (c) 2023 SparkFun Electronics

Permission is hereby granted, free of charge, to any person obtaining a
copy of this software and associated documentation files (the "Software"),
to deal in the Software without restriction, including without limitation
the rights to use, copy, modify, merge, publish, distribute, sublicense,
and/or sell copies of the Software, and to permit persons to whom the
Software is furnished to do so, subject to the following conditions: The
above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software. THE SOFTWARE IS PROVIDED
"AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR IMPLIED, INCLUDING BUT
NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY, FITNESS FOR A PARTICULAR
PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT
HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN
ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN
CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.

*/

#pragma once


#include <stdint.h>

//
// General Concept
//    A SparkFun Toolkit error system. The goal is to keep this simple.
//
//    This mimics a vareity of systems, using an int type for error codes,
//    where:
//   		0   = okay
//         -1   = general failure
//         >0   = an informative error
//
//    Since *subsystems* in the toolkit can have their own errors,
//    A start range for these errors are defined. Values > than this value
//    define the errors for the set subsystem. These start ranges are set
//    in this file, with actual error values defined in the the respective
//    subsystem header files.
//
// Define our error codes/type
typedef int32_t sfeTkError_t;

// General errors

const sfeTkError_t kSTkErrFail = -1; // general fail
const sfeTkError_t kSTkErrOk = 0;    // success

// Base error number for IBus/Bus operations Bus errors are not less than this.
const sfeTkError_t kSTkErrBaseBus = 0x1000;
