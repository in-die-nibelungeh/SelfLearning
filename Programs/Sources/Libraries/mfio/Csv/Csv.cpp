
#include "Csv.h"

namespace mfio {

const char Csv::g_Delimiter = ',';

status_t Csv::Write(const char* fname, double data[], int length)
{
    Csv csv(fname);
    return csv.Write(data, length);
}

status_t Csv::Write(const char* fname, mcon::Vector<double>& vector)
{
    Csv csv(fname);
    return csv.Write(vector);
}

status_t Csv::Write(const char* fname, mcon::Matrix<double>& matrix)
{
    Csv csv(fname);
    return csv.Write(matrix);
}

Csv::Csv()
    : m_Handle(NULL)
{}

Csv::Csv(const char* fname)
    : m_Handle(NULL)
{
    Open(fname);
}

Csv::Csv(std::string fname)
    : m_Handle(NULL)
{
    Open(fname.c_str());
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

status_t Csv::Write(double data[], int length) const
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

status_t Csv::Write(mcon::Vector<double>& vector) const
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

status_t Csv::Write(mcon::Matrix<double>& matrix) const
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

} // namespace mfio {

