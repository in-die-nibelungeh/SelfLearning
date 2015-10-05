
#pragma once

#include <stdio.h>

#include "status.h"
#include "types.h"
#include "Vector.h"
#include "Matrix.h"

namespace mfio {

class Csv
{
public:
    Csv();
    Csv(const char* filename);
    ~Csv();

    status_t Open(const char* filename, const char* permission = "w");
    status_t Close(void);

    status_t Write(const char* string) const;
    status_t Write(double data[], int length) const;
    status_t Write(mcon::Vector<double>& vector) const;
    status_t Write(mcon::Matrix<double>& matrix) const;
    status_t Crlf(void) const;

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

