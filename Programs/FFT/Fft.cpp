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
    static f64 g_Pi(M_PI);
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

status_t Fft(f64 real[], f64 imag[], f64 td[], s32 n)
{
    if (Count1(n) != 1)
    {
        return -ERROR_ILLEGAL;
    }

    s32 iter = Ilog2(n);

    for (int i = 0; i < iter; ++i)
    {
        for (int j = 0; j < ((i+1) << 1); ++j)
        {
        }
    }
    return NO_ERROR;
}

status_t Ifft(f64 td[], f64 real[], f64 imag[], s32 n)
{
    return NO_ERROR;
}

status_t Ft(f64 real[], f64 imag[], const f64 td[], s32 n)
{
    for (int i = 0; i < n; ++i)
    {
        f64 df = (f64)i * g_Pi * 2/ n;
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

status_t Ft(Container::Matrix<f64>& fd, const Container::Vector<f64>& td)
{
    ASSERT(fd.GetNumOfArray() >= 2);
    ASSERT(td.GetNumOfData() == fd[0].GetNumOfData());
    ASSERT(td.GetNumOfData() == fd[1].GetNumOfData());

    Container::Vector<f64>& real = fd[0];
    Container::Vector<f64>& imag = fd[1];
    s32 n = td.GetNumOfData();
    for (int i = 0; i < n; ++i)
    {
        f64 df = (f64)i * g_Pi * 2/ n;
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

status_t Ift(f64 td[], const f64 real[], const f64 imag[], s32 n)
{
    for (int i = 0; i < n; ++i)
    {
        f64 df = (f64)i * g_Pi * 2/ n;
        td[i] = 0.0;
        for (int j = 0; j < n; ++j)
        {
            td[i]  += (real[j] * cos(df * j) - imag[j] * sin(df * j));
        }
        td[i] /= n;
    }
    return NO_ERROR;
}

status_t Ift(Container::Vector<f64>& td, const Container::Matrix<f64>& fd)
{
    ASSERT(fd.GetNumOfArray() >= 2);
    ASSERT(td.GetNumOfData() == fd[0].GetNumOfData());
    ASSERT(td.GetNumOfData() == fd[1].GetNumOfData());

    Container::Vector<f64>& real = fd[0];
    Container::Vector<f64>& imag = fd[1];
    const s32 n = fd.GetNumOfData();

    for (int i = 0; i < n; ++i)
    {
        f64 df = (f64)i * g_Pi * 2/ n;
        td[i] = 0.0;
        for (int j = 0; j < n; ++j)
        {
            td[i]  += (real[j] * cos(df * j) - imag[j] * sin(df * j));
        }
        td[i] /= n;
    }
    return NO_ERROR;
}

}; // namespace Fft
