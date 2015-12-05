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

#include "Csv.h"

namespace mfio {

namespace {
    char readBuffer[Csv::g_ReadBufferSize];
}

status_t Csv::Write(const char* fname, const double data[], int length)
{
    Csv csv(fname);
    return csv.Write(data, length);
}

status_t Csv::Write(const char* fname, const mcon::Vector<double>& vector)
{
    Csv csv(fname);
    return csv.Write(vector);
}

status_t Csv::Write(const char* fname, const mcon::Matrix<double>& matrix)
{
    Csv csv(fname);
    return csv.Write(matrix);
}

Csv::Csv()
    : m_Handle(NULL)
{}

Csv::Csv(const char* fname, const char* permission)
    : m_Handle(NULL)
{
    Open(fname, permission);
}

Csv::Csv(std::string fname, const char* permission)
    : m_Handle(NULL)
{
    Open(fname.c_str(), permission);
}

Csv::~Csv()
{
    Close();
}

status_t Csv::Open(const char* fname, const char* permission)
{
    m_Handle = fopen(fname, permission);
    if (NULL == m_Handle)
    {
        return -ERROR_ILLEGAL_PERMISSION;
    }
    return NO_ERROR;
}

status_t Csv::Close(void)
{
    if (NULL != m_Handle)
    {
        fclose(m_Handle);
        m_Handle = NULL;
    }
    return NO_ERROR;
}

status_t Csv::Crlf(void) const
{
    if (NULL == m_Handle)
    {
        return -ERROR_INVALID_HANDLE;
    }
    fprintf(m_Handle, "\n");
    return NO_ERROR;
}

status_t Csv::Write(const char* string) const
{
    if (NULL == m_Handle)
    {
        return -ERROR_INVALID_HANDLE;
    }
    fprintf(m_Handle, "%s", string);
    return NO_ERROR;
}

status_t Csv::Write(const double data[], int length) const
{
    if (NULL == m_Handle)
    {
        return -ERROR_INVALID_HANDLE;
    }
    for (int i = 0; i < length; ++i)
    {
        fprintf(m_Handle, "%d%c%g\n", i, g_Delimiter,  data[i]);
    }
    return NO_ERROR;
}

status_t Csv::Write(const mcon::Vector<double>& vector) const
{
    if (NULL == m_Handle)
    {
        return -ERROR_INVALID_HANDLE;
    }
    for (int i = 0; i < vector.GetLength(); ++i)
    {
        fprintf(m_Handle, "%d%c%g\n", i, g_Delimiter,  vector[i]);
    }
    return NO_ERROR;
}

status_t Csv::Write(const mcon::Matrix<double>& matrix) const
{
    if (NULL == m_Handle)
    {
        return -ERROR_INVALID_HANDLE;
    }
    for (int i = 0; i < matrix.GetColumnLength(); ++i)
    {
        fprintf(m_Handle, "%d%c%g", i, g_Delimiter,  matrix[0][i]);
        for (int k = 1; k < matrix.GetRowLength(); ++k)
        {
            fprintf(m_Handle, "%c%g", g_Delimiter,  matrix[k][i]);
        }
        fprintf(m_Handle, "\n");
    }
    return NO_ERROR;
}

/*----------------------------------------------------------------------*
 * Read
 *----------------------------------------------------------------------*/

status_t Csv::Read(const char* fname, mcon::Matrix<double>& matrix)
{
    Csv csv(fname, "r");
    return csv.Read(matrix);
}

int CountRow(char* line)
{
    int row = 0;
    const char sep[] = {Csv::g_Delimiter};
    char* s = line;
    s = strtok(s, sep);
    for ( ; s != NULL ; ++row)
    {
        s = strtok(NULL, sep);
    }
    return row;
}

inline bool IsAcceptableChar(char c)
{
    return isdigit(c) || c == '-' || c == '+' || c == '.';
}
    
void CountRowColumn(FILE* handle, int& row, int& column)
{
    row = 0 ;
    column = 0;
    fseek(handle, 0, SEEK_SET);
    while (NULL != fgets(readBuffer, Csv::g_ReadBufferSize - 1, handle))
    {
        if ( IsAcceptableChar(readBuffer[0]) )
        {
            ++column;
            const int r = CountRow(readBuffer);
            if ( r > row)
            {
                row = r;
            }
        }
    }
}

int ReadTokensAsDouble(mcon::Matrix<double>& matrix, char* line, int column, int rowMaximum)
{
    const char sep[] = {Csv::g_Delimiter};
    char* s = line;
    s = strtok(s, sep);
    for ( int r = 0 ; NULL != s && r < rowMaximum; ++r )
    {
        double v;
        sscanf(s, "%lf", &v);
        matrix[r][column] = v;
        s = strtok(NULL, sep);
    }
    return 0;
}

status_t Csv::Read(mcon::Matrix<double>& matrix) const
{
    if (NULL == m_Handle)
    {
        return -ERROR_INVALID_HANDLE;
    }

    int rowMaximum = 0;
    int columnMaximum = 0;
    CountRowColumn(m_Handle, rowMaximum, columnMaximum);
    if ( false == matrix.Resize(rowMaximum, columnMaximum) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    fseek(m_Handle, 0, SEEK_SET);
    int column = 0;
    while (NULL != fgets(readBuffer, Csv::g_ReadBufferSize - 1, m_Handle))
    {
        if ( IsAcceptableChar(readBuffer[0]) )
        {
            ReadTokensAsDouble(matrix, readBuffer, column, rowMaximum);
            ++column;
        }
    }
    return NO_ERROR;
}

} // namespace mfio {

