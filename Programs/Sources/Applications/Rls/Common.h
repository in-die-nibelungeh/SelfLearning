/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015 Ryosuke Kanata
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

typedef struct _ProgramParameter
{
    mcon::Matrixd inputSignal;
    mcon::Vectord referenceSignal;
    mcon::Matrixd inversedSignal;

    std::string inputFilepath;
    std::string referenceFilepath;
    std::string outputBase;

    uint tapps;
    uint inputLength;
    float upperValue;

    bool optimize;
    bool outputLog;

// Private
    uint samplingRate;
    int referenceOffset;

} ProgramParameter;

status_t Setup      (ProgramParameter* param);
status_t PreProcess (ProgramParameter* param);
status_t Process    (ProgramParameter* param);
status_t PostProcess(ProgramParameter* param);
status_t Cleanup    (const ProgramParameter* param);

status_t NormalEquation(mcon::Vector<double>& h, const mcon::Vectord& u, const mcon::Vectord& d, double* pError);

class ShowMessage
{
public:
    ShowMessage(const char* func, uint indent = 1)
        : m_Indent(indent)
        , m_IndentString()
    {
        LOG("%s ...\n", func);
        for (uint k = 0; k < m_Indent * 4; ++k)
        {
            m_IndentString.append(" ");
        }
    }
    ~ShowMessage()
    {
        LOG("Done\n\n");
    }
    void Log(const char* msg)
    {
        LOG("%s%s", m_IndentString.c_str(), msg);
    }
private:
    uint m_Indent;
    std::string m_IndentString;
};

