#include <math.h>

#include "types.h"
#include "status.h"
#include "Buffer.h"

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

status_t Ft(Container::Matrix<double>& outFd, const Container::Vector<double>& inTd)
{
    if ( outFd.GetNumOfArray() < 2 )
    {
        return -ERROR_ILLEGAL;
    }

    Container::Vector<double>& real = outFd[0];
    Container::Vector<double>& imag = outFd[1];

    for (int i = 0; i < outFd.GetNumOfData(); ++i)
    {
        int N = inTd.GetNumOfData();
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

status_t Ift(Container::Vector<double>& outTd, const Container::Matrix<double>& inFd)
{
    if ( inFd.GetNumOfArray() < 2 )
    {
        return -ERROR_ILLEGAL;
    }
    Container::Vector<double>& real = inFd[0];
    Container::Vector<double>& imag = inFd[1];

    for (int i = 0; i < outTd.GetNumOfData(); ++i)
    {
        const int N = inFd.GetNumOfData();
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

status_t ConvertToGainPhase(Container::Matrix<double>& gainPhase, const Container::Matrix<double>& complex)
{
    if ( gainPhase.GetNumOfArray() < 2
         || complex.GetNumOfArray() < 2 )
    {
        return -ERROR_ILLEGAL;
    }
    Container::Vector<double>& gain  = gainPhase[0];
    Container::Vector<double>& phase = gainPhase[1];
    Container::Vector<double>& real  = complex[0];
    Container::Vector<double>& imag  = complex[1];

    for (int i = 0; i < complex.GetNumOfData()/2; ++i)
    {
        if (i < gainPhase.GetNumOfData())
        {
            gain[i] = 10 * log10(POW2(real[i]) + POW2(imag[i]));
            phase[i] = atan(real[i]/imag[i]);
        }
    }
    return NO_ERROR;
}

}; // namespace Fft
