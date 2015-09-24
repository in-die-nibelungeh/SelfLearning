#include <math.h>

#include "types.h"
#include "status.h"
#include "Matrix.h"

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif // #ifndef M_PI

namespace Fft
{

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

status_t Ft(mcon::Matrix<double>& outFd, const mcon::Vector<double>& inTd)
{
    if ( outFd.GetRowLength() < 2 )
    {
        return -ERROR_ILLEGAL;
    }

    mcon::Vector<double>& real = outFd[0];
    mcon::Vector<double>& imag = outFd[1];

    for (int i = 0; i < outFd.GetColumnLength(); ++i)
    {
        int N = inTd.GetLength();
        double df = (double)i * g_Pi * 2 / N;
        real[i] = 0.0;
        imag[i] = 0.0;
        if (i < N)
        {
            for (int j = 0; j < N; ++j)
            {
                real[i] += (inTd[j] * cos(df * j));
                imag[i] -= (inTd[j] * sin(df * j));
            }
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

status_t Ift(mcon::Vector<double>& outTd, const mcon::Matrix<double>& inFd)
{
    if ( inFd.GetRowLength() < 2 )
    {
        return -ERROR_ILLEGAL;
    }
    const mcon::Vector<double>& real = inFd[0];
    const mcon::Vector<double>& imag = inFd[1];

    for (int i = 0; i < outTd.GetLength(); ++i)
    {
        const int N = inFd.GetColumnLength();
        double df = (double)i * g_Pi * 2 / N;
        outTd[i] = 0.0;
        if (i < N)
        {
            for (int j = 0; j < N; ++j)
            {
                outTd[i]  += (real[j] * cos(df * j) - imag[j] * sin(df * j));
            }
            outTd[i] /= N;
        }
    }
    return NO_ERROR;
}

#define POW2(v) ((v)*(v))

//status_t ConvertToRadAng(mcon::Matrix<double>& gainPhase, const mcon::Matrix<double>& complex)
status_t ConvertToGainPhase(mcon::Matrix<double>& gainPhase, const mcon::Matrix<double>& complex)
{
    if ( complex.GetRowLength() < 2 )
    {
        return -ERROR_ILLEGAL;
    }
    if (gainPhase.GetRowLength() < 2)
    {
        gainPhase.Resize(2, complex.GetColumnLength());
    }
    mcon::Vector<double>& gain  = gainPhase[0];
    mcon::Vector<double>& phase = gainPhase[1];
    const mcon::Vector<double>& real  = complex[0];
    const mcon::Vector<double>& imag  = complex[1];

    for (int i = 0; i < complex.GetColumnLength(); ++i)
    {
        gain[i] = sqrt(POW2(real[i]) + POW2(imag[i]));
        phase[i] = atan(imag[i]/real[i]);
    }
    return NO_ERROR;
}

}; // namespace Fft
