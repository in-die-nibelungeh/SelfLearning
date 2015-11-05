#include <math.h>

#include "mcon.h"
#include "types.h"
#include "status.h"
#include "debug.h"

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
        for (unsigned int i = 0; i < sizeof(v) * 8; ++i)
        {
            ans += ((v >> i) & 0x1) ? 1 : 0;
        }
        return ans;
    }
    int BitReverse(int v, int w) // value, width
    {
        int x = 0;
        for (int i = 0; i < w; ++i)
        {
            x |= (((v >> i) & 0x1) << (w - i - 1));
        }
        return x;
    }
}

status_t Fft(mcon::Matrix<double>& complex, const mcon::Vector<double>& timeSeries)
{
    const int N = timeSeries.GetLength();
    if (Count1(N) != 1)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = complex.Resize(2, N);
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    mcon::Vector<double>& real = complex[0];
    mcon::Vector<double>& imag = complex[1];

    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable(N/2);
    mcon::Vector<double> cosTable(N/2);

    for (int i = 0; i < N/2; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }
    // Substitute beforehand
    real = timeSeries;
    imag = 0;

    int innerLoop = N/2;
    int outerLoop = 1;
    //DEBUG_LOG("N=%d\n", N);

    // Decimation in frequency.
    for ( ; 0 < innerLoop; innerLoop >>= 1, outerLoop <<= 1)
    {
        //DEBUG_LOG("inner=%d, outer=%d\n", innerLoop, outerLoop);
        for ( int outer = 0; outer < outerLoop; ++outer )
        {
            const int& step = innerLoop;
            const int ofs = outer * step * 2;
#if 0
            for ( int k = 0; k < innerLoop; ++k )
            {
                const double r1 = real[k+ofs];
                const double i1 = imag[k+ofs];
                const double r2 = real[k+step+ofs];
                const double i2 = imag[k+step+ofs];

                real[k+ofs] = r1 + r2;
                imag[k+ofs] = i1 + i2;
                const int idx = k * outerLoop;
                //DEBUG_LOG("step=%d, ofs=%d, k=%d, idx=%d\n", step, ofs, k, idx);
                real[k+step+ofs] =   (r1 - r2) * cosTable[idx] + (i1 - i2) * sinTable[idx];
                imag[k+step+ofs] = - (r1 - r2) * sinTable[idx] + (i1 - i2) * cosTable[idx];
            }
#else
            // Easier to look into.
            void* _pReal = real;
            void* _pImag = imag;
            double* pRealL = reinterpret_cast<double*>(_pReal) + ofs;
            double* pImagL = reinterpret_cast<double*>(_pImag) + ofs;
            double* pRealH = reinterpret_cast<double*>(_pReal) + ofs + step;
            double* pImagH = reinterpret_cast<double*>(_pImag) + ofs + step;
            for ( int k = 0; k < innerLoop; ++k )
            {
                const double r1 = pRealL[k];
                const double i1 = pImagL[k];
                const double r2 = pRealH[k];
                const double i2 = pImagH[k];

                const int idx = k * outerLoop;
                pRealL[k] = r1 + r2;
                pImagL[k] = i1 + i2;
                pRealH[k] =   (r1 - r2) * cosTable[idx] + (i1 - i2) * sinTable[idx];
                pImagH[k] = - (r1 - r2) * sinTable[idx] + (i1 - i2) * cosTable[idx];
                //DEBUG_LOG("step=%d, ofs=%d, k=%d, idx=%d\n", step, ofs, k, idx);
            }
#endif
        }
    }
    // Bit-reverse
    const int width = Ilog2(N);
    //DEBUG_LOG("width=%d\n", width);
    for ( int i = 0; i < N; ++i )
    {
        const int k = BitReverse(i, width);
        if (k == i || k < i)
        {
            continue;
        }
        const double real_temp = real[k];
        const double imag_temp = imag[k];
        real[k] = real[i];
        imag[k] = imag[i];
        real[i] = real_temp;
        imag[i] = imag_temp;
    }
    return NO_ERROR;
}

status_t Ifft(mcon::Vector<double>& timeSeries, const mcon::Matrix<double>& complex)
{
    const int N = complex.GetRowLength();
    if (Count1(N) != 1)
    {
        return -ERROR_ILLEGAL;
    }
    bool status = timeSeries.Resize(N);
    if (false == status)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
#if 0
    const mcon::Vector<double>& real = complex[0];
    const mcon::Vector<double>& imag = complex[1];

    const double df = 2.0 * g_Pi / N;

    mcon::Vector<double> sinTable(N/2);
    mcon::Vector<double> cosTable(N/2);

    for (int i = 0; i < N/2; ++i)
    {
        sinTable[i] = sin(df * i);
        cosTable[i] = cos(df * i);
    }
    // Substitute beforehand
    real = timeSeries;
    imag = 0;

    int innerLoop = N/2;
    int outerLoop = 1;
    DEBUG_LOG("N=%d\n", N);

    // Decimation in frequency.
    for ( ; 0 < innerLoop; innerLoop >>= 1, outerLoop <<= 1)
    {
        DEBUG_LOG("inner=%d, outer=%d\n", innerLoop, outerLoop);
        for ( int outer = 0; outer < outerLoop; ++outer )
        {
            const int& ofs = innerLoop;
            for ( int k = 0; k < innerLoop; ++k )
            {
                const int dofs = outer * ofs * 2;
                const double r1 = real[k+dofs];
                const double i1 = imag[k+dofs];
                const double r2 = real[k+ofs+dofs];
                const double i2 = imag[k+ofs+dofs];

                real[k+dofs] = r1 + r2;
                imag[k+dofs] = i1 + i2;
                const int idx = k * outerLoop;
                DEBUG_LOG("ofs=%d, dofs=%d, k=%d, idx=%d\n", ofs, dofs, k, idx);
                real[k+ofs+dofs] =   (r1 - r2) * cosTable[idx] + (i1 - i2) * sinTable[idx];
                imag[k+ofs+dofs] = - (r1 - r2) * sinTable[idx] + (i1 - i2) * cosTable[idx];
            }
        }
    }
    // Bit-reverse
    const int width = Ilog2(N);
    DEBUG_LOG("width=%d\n", width);
    for ( int i = 0; i < N; ++i )
    {
        const int k = BitReverse(i, width);
        if (k == i || k < i)
        {
            continue;
        }
        const double real_temp = real[k];
        const double imag_temp = imag[k];
        real[k] = real[i];
        imag[k] = imag[i];
        real[i] = real_temp;
        imag[i] = imag_temp;
    }
#endif
    return NO_ERROR;
}

status_t Fft(double real[], double imag[], double td[], int n)
{
    mcon::Vector<double> input;
    mcon::Matrix<double> complex;

    if ( false == input.Resize(n) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    if ( false == complex.Resize(2, n) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    memcpy(input, td, sizeof(double) * n);

    status_t status;
    status = Fft(complex, input);

    if ( NO_ERROR == status )
    {
        memcpy(real, complex[0], n * sizeof(double));
        memcpy(imag, complex[1], n * sizeof(double));
    }
    return status;
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

    mcon::Vector<double> sinTable;
    mcon::Vector<double> cosTable;

    if ( false == sinTable.Resize(N) ||
         false == cosTable.Resize(N) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }

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

