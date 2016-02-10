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
#include <stdio.h>

#include "status.h"
#include "types.h"
#include "mcon.h"

namespace mfio {

class Csv
{
public:
    Csv();
    Csv(std::string filename, const char* permission = "wb");
    Csv(const char* filename, const char* permission = "wb");
    ~Csv();

    inline status_t Open(std::string filename, const char* permission = "wb")
    {
        return Open(filename.c_str(), permission);
    }
    status_t Open(const char* filename, const char* permission = "wb");
    status_t Close(void);

    inline status_t Write(std::string str) const
    {
        return Write(str.c_str());
    }
    status_t Write(const char* string) const;
    status_t Write(const double data[], int length) const;
    status_t Write(const mcon::Vector<double>& vector) const;
    status_t Write(const mcon::Matrix<double>& matrix) const;
    status_t Crlf(void) const;

    inline static status_t Write(std::string filename, const double data[], int length)
    {
        return Write(filename.c_str(), data, length);
    }
    inline static status_t Write(std::string filename, const mcon::Vector<double>& vector)
    {
        return Write(filename.c_str(), vector);
    }
    inline static status_t Write(std::string filename, const mcon::Matrix<double>& matrix)
    {
        return Write(filename.c_str(), matrix);
    }

    static status_t Write(const char* filename, const double data[], int length);
    static status_t Write(const char* filename, const mcon::Vector<double>& vector);
    static status_t Write(const char* filename, const mcon::Matrix<double>& matrix);

    //status_t Read(mcon::Vector<double>& vector) const;

    //static status_t Read(const char* filename, double** data, int* length);
    //static status_t Read(const char* filename, mcon::Vector<double>& vector);
    static status_t Read(const char* filename, mcon::Matrix<double>& matrix);

    inline static status_t Read(std::string filename, mcon::Matrix<double>& matrix)
    {
        return Read(filename.c_str(), matrix);
    }

    static const char g_Delimiter = ',';
    static const size_t g_ReadBufferSize = 1024;

private:
    status_t Read(mcon::Matrix<double>& matrix) const;
    FILE* m_Handle;
};


} // namespace mfio {

