
#pragma once

#include <string>
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
    Csv(std::string filename);
    Csv(const char* filename);
    ~Csv();

    inline status_t Open(std::string filename, const char* permission = "w")
    {
        Open(filename.c_str(), permission);
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

