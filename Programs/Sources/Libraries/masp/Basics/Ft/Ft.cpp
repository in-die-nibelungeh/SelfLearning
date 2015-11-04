#include <math.h>

#include "types.h"
#include "status.h"
#include "mcon.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

namespace masp {
namespace ft {

namespace
{
    static double g_Pi(M_PI);
    int Ilog2(int v)
    {
        for (int i = sizeof(v) * 8 - 1; i >= 0 ; --i)
        {
            if ((v >> i) & 0x1)
            {
                return i;
            }
        }
        return -ERROR_ILLEGAL;
    }
    int Count1(int v)
    {
        int ans = 0;
        for (int i = 0; 0 < sizeof(v); ++i)
        {
            ans += ((v >> i) & 0x1) ? 1 : 0;
        }
        return ans;
    }
    int BitReverse(int v, int w)
    {
        int x = 0;
        for (int i = 0; i < w; ++i)
        {
            x |= (((v >> i) & 0x1) << (w - i - 1));
        }
        return x;
    }
}

status_t Fft(double real[], double imag[], double td[], int n)
{
    if (Count1(n) != 1)
    {
        return -ERROR_ILLEGAL;
    }

    int iter = Ilog2(n);

    for (int i = 0; i < iter; ++i)
    {
        for (int j = 0; j < ((i+1) << 1); ++j)
        {
        }
    }
    return NO_ERROR;
}

status_t Ifft(double td[], double real[], double imag[], int n)
{
    return NO_ERROR;
}

status_t Ft(double real[], double imag[], const double td[], int n)
{
    for (int i = 0; i < n; ++i)
    {
        double df = (double)i * g_Pi * 2/ n;
        real[i] = 0.0;
        imag[i] = 0.0;
        for (int j = 0; j < n; ++j)
        {
            real[i] += (td[j] * cos(df * j));
            imag[i] -= (td[j] * sin(df * j));
        }
    }
    return NO_ERROR;
}

status_t Ft(mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries)
{
    bool status = complex.Resize(2, timeSeries.GetLength());
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

    mcon::Vector<double>& real = complex[0];
    mcon::Vector<double>& imag = complex[1];

    const int N = timeSeries.GetLength();
    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable(N);
    mcon::Vector<double> cosTable(N);

    for (int i = 0; i < N; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }

    for (int i = 0; i < N; ++i)
    {
        real[i] = 0.0;
        imag[i] = 0.0;
        for (int j = 0; j < N; ++j)
        {
            int k = (i * j) % N;
            real[i] += (timeSeries[j] * cosTable[k]);
            imag[i] -= (timeSeries[j] * sinTable[k]);
        }
    }
    return NO_ERROR;
}

status_t Ift(double td[], const double real[], const double imag[], int N)
{
    for (int i = 0; i < N; ++i)
    {
        double df = (double)i * g_Pi * 2 / N;
        td[i] = 0.0;
        for (int j = 0; j < N; ++j)
        {
            td[i]  += (real[j] * cos(df * j) - imag[j] * sin(df * j));
        }
        td[i] /= N;
    }
    return NO_ERROR;
}

status_t Ift(mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex)
{
    if (complex.GetRowLength() < 2)
    {
        return -ERROR_ILLEGAL;
    }
    if ( false == timeSeries.Resize(complex.GetColumnLength()) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    const int N = complex.GetColumnLength();
    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable(N);
    mcon::Vector<double> cosTable(N);

    for (int i = 0; i < N; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }

    const mcon::Vector<double>& real = complex[0];
    const mcon::Vector<double>& imag = complex[1];

    for (int i = 0; i < timeSeries.GetLength(); ++i)
    {
        const int N = complex.GetColumnLength();
        timeSeries[i] = 0.0;
        for (int j = 0; j < N; ++j)
        {
            int k = (i * j) % N;
            timeSeries[i] += (real[j] * cosTable[k] - imag[j] * sinTable[k]);
        }
    }
    timeSeries /= N;
    return NO_ERROR;
}

#define POW2(v) ((v)*(v))

status_t ConvertToPolarCoords(mcon::Matrix<double>& polar, const mcon::Matrix<double>& complex)
{
    if (complex.GetRowLength() < 2)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = polar.Resize(2, complex.GetColumnLength());
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    mcon::Vector<double>& r  = polar[0];
    mcon::Vector<double>& arg = polar[1];
    const mcon::Vector<double>& real  = complex[0];
    const mcon::Vector<double>& imag  = complex[1];

    for (int i = 0; i < complex.GetColumnLength(); ++i)
    {
        r[i] = sqrt(POW2(real[i]) + POW2(imag[i]));
        arg[i] = atan(imag[i]/real[i]);
    }
    return NO_ERROR;
}

status_t ConvertToComplex(mcon::Matrix<double>& complex, const mcon::Matrix<double>& polar)
{
    if (polar.GetRowLength() < 2)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = complex.Resize(2, polar.GetColumnLength());
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    const mcon::Vector<double>& r = polar[0];
    const mcon::Vector<double>& arg = polar[1];
    mcon::Vector<double>& real  = complex[0];
    mcon::Vector<double>& imag  = complex[1];

    for (int i = 0; i < polar.GetColumnLength(); ++i)
    {
        const double v = r[i];
        real[i] = v * cos(arg[i]);
        imag[i] = v * sin(arg[i]);
    }
    return NO_ERROR;
}

} // namespace ft {
} // namespace masp {

