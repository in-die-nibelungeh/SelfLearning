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
#include <stdio.h>

#include "status.h"
#include "types.h"
#include "mcon.h"

namespace mfio {

class Csv
{
public:
    Csv();
    Csv(std::string filename);
    Csv(const char* filename);
    ~Csv();

    inline status_t Open(std::string filename, const char* permission = "w")
    {
        return Open(filename.c_str(), permission);
    }
    status_t Open(const char* filename, const char* permission = "w");
    status_t Close(void);

    inline status_t Write(std::string str) const
    {
        return Write(str.c_str());
    }
    status_t Write(const char* string) const;
    status_t Write(double data[], int length) const;
    status_t Write(mcon::Vector<double>& vector) const;
    status_t Write(mcon::Matrix<double>& matrix) const;
    status_t Crlf(void) const;

    inline static status_t Write(std::string filename, double data[], int length)
    {
        return Write(filename.c_str(), data, length);
    }
    inline static status_t Write(std::string filename, mcon::Vector<double>& vector)
    {
        return Write(filename.c_str(), vector);
    }
    inline static status_t Write(std::string filename, mcon::Matrix<double>& matrix)
    {
        return Write(filename.c_str(), matrix);
    }

    static status_t Write(const char* filename, double data[], int length);
    static status_t Write(const char* filename, mcon::Vector<double>& vector);
    static status_t Write(const char* filename, mcon::Matrix<double>& matrix);

    //static status_t Read(const char* filename, double** data, int* length);
    //static status_t Read(const char* filename, mcon::Vector<double>& vector);
    //static status_t Read(const char* filename, mcon::Matrix<double>& matrix);
private:
    static const char g_Delimiter;
    FILE* m_Handle;
};


} // namespace mfio {

