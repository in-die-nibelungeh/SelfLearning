/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in
 * all copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN
 * THE SOFTWARE.
 */

#pragma once

#include <string>

#include "status.h"

#include "mcon.h"

enum GainFormat
{
    GainFormat_Amplitude,
    GainFormat_10Log,
    GainFormat_20Log,
};

enum ArgFormat
{
    ArgFormat_Radian,
    ArgFormat_Degree,
};

enum WindowType
{
    WindowType_Rectangular,
    WindowType_Hanning,
    WindowType_Hamming,
    WindowType_Blackman,
    WindowType_BlackmanHarris,
};

typedef struct _ProgramParameter
{
    std::string inputFilepath;
    std::string outputBase;

    mcon::Matrix<double> signal;

    uint samplingRate;

    uint windowLength;
    WindowType windowType;

    uint sampleCount;

    bool isUsedOnlyFt;

    enum GainFormat gainFormat;
    enum ArgFormat argFormat;

} ProgramParameter;

bool IsFactorial(const uint value, const uint base); // in PreProcess

status_t Setup      (ProgramParameter* param);
status_t PreProcess (ProgramParameter* param);
status_t Process    (const ProgramParameter* param);
status_t PostProcess(ProgramParameter* param);
status_t Cleanup    (const ProgramParameter* param);

#define PRINT_RETURN_IF_FAILED(statement) \
    do {                                  \
        status_t status = statement;      \
        if (NO_ERROR != status)           \
        {                                 \
            ERROR_LOG("An error occued in " #statement ": error=%d\n", status); \
            return 0;                     \
        }                                 \
    } while (0)

#define RETURN_IF_FAILED(statement)  \
    do {                             \
        status_t status = statement; \
        if (NO_ERROR != status)      \
        {                            \
            return status;           \
        } \
    } while (0)
