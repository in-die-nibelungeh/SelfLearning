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

#include <stdlib.h>
#include <stdio.h>

#include "types.h"
#include "debug.h"

#include "mfio.h"

static void test_csv(void)
{
    const int length = 16;
    double darray[length];
    mcon::Vector<double> vector(length);
    mcon::Matrix<double> matrix(3, length);

    for ( int i = 0; i < length; ++i)
    {
        darray[i] = i + 1.0;
        vector[i] = i + 1.0;
        for ( int k = 0; k < matrix.GetRowLength(); ++k)
        {
            matrix[k][i] = 10*(k+1) + i + 1.0;
        }
    }
    {
        mfio::Csv::Write("static_darray.csv", darray, length);
        mfio::Csv::Write("static_vector.csv", vector);
        mfio::Csv::Write("static_matrix.csv", matrix);
    }
    {
        mfio::Csv csv("darray.csv");
        csv.Write("i,data\n");
        csv.Write(darray, length);
        csv.Write(darray, length);
        csv.Close();

        csv.Open("vector.csv");
        csv.Write("i,data\n");
        csv.Write(vector);
        csv.Crlf();
        csv.Write(vector);
        csv.Close();

        csv.Open("matrix.csv");
        csv.Write("i,data,data,data\n");
        csv.Write(matrix);
        csv.Crlf();
        csv.Write(matrix);
        csv.Close();
    }
    LOG("[Csv::Read]\n");
    {
        const std::string fname("matrix.csv");
        mcon::Matrix<double> m1;
        mfio::Csv::Read(fname, m1);
        int ec = 0;
        CHECK_VALUE(m1.GetRowLength()   , matrix.GetRowLength() + 1);
        CHECK_VALUE(m1.GetColumnLength(), matrix.GetColumnLength() * 2);
        for ( int c = 0; c < m1.GetColumnLength(); ++c )
        {
            const int _c = c % matrix.GetColumnLength();
            ec += m1[0][c] != _c ? 1 : 0;
            for ( int r = 0; r < m1.GetRowLength() - 1; ++r )
            {
                ec += m1[r+1][c] != matrix[r][_c] ? 1 : 0;
            }
        }
        CHECK_VALUE(ec, 0);
    }
    
}

int main(void)
{
    test_csv();
    return 0;
}
