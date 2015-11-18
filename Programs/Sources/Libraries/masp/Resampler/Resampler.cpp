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

#include "status.h"
#include "masp/Window.h"
#include "masp/Fir.h"
#include "Resampler.h"

namespace masp {

namespace {
    static const int g_KaiserAlphaOffset = 2;
    static const float g_KaiserTransitionBand[] =
    {
         3.0/2,
         4.0/2,
         5.2/2,
         6.4/2,
         7.6/2,
         9.0/2,
        10.2/2,
        11.4/2,
        12.8/2
    };
    static const int g_KaiserStopbandDecay[] =
    {
        29,
        27,
        45,
        54,
        63,
        72,
        81,
        90,
        99
    };
    double* _Cast(void* ptr)
    {
        return reinterpret_cast<double*>(ptr);
    }
}

status_t Resampler::UpdateCoefficients(int windowType, double alpha)
{
    const double bandWidth = (m_StopBandFrequency - m_PassBandFrequency);
    DEBUG_LOG("nFp=%g, nFs=%g\n", m_PassBandFrequency, m_StopBandFrequency);
    int N = 0;
    switch(windowType)
    {
    case HANNING:
        {
            N = static_cast<int>(6.2 / 2 / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
            DEBUG_LOG("N=%d (Hanning)\n", N);
            m_Coefficients.Resize(N);
            masp::window::Hanning(m_Coefficients);
        }
        break;
    case HAMMING:
        {
            N = static_cast<int>(6.6 / 2 / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
            DEBUG_LOG("N=%d (Hamming)\n", N);
            m_Coefficients.Resize(N);
            masp::window::Hamming(m_Coefficients);
        }
        break;
    case BLACKMAN:
        {
            N = static_cast<int>(11.0 / 2 / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
            DEBUG_LOG("N=%d (Blackman)\n", N);
            m_Coefficients.Resize(N);
            masp::window::Blackman(m_Coefficients);
        }
        break;
    case KAISER:
        {
            ASSERT( 2.0 <= alpha && alpha <= 10.0 );
            const int index = static_cast<int>(alpha + 0.5) - masp::g_KaiserAlphaOffset;
            N = static_cast<int>( masp::g_KaiserTransitionBand[index] / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
            DEBUG_LOG("N=%d (Kaiser), alpha=%g\n", N, alpha);
            m_Coefficients.Resize(N);
            masp::window::Kaiser(m_Coefficients, alpha);
        }
        break;
    default:
        ASSERT ( 0 );
        break;
    }

    ASSERT( N != 0 );
    {
        const double cutoff = (m_StopBandFrequency + m_PassBandFrequency) / 2.0 / (m_L > m_M ? m_L : m_M);
        DEBUG_LOG("nFp=%g, nFs=%g\n", m_PassBandFrequency, m_StopBandFrequency);
        DEBUG_LOG("nFc=%g\n", cutoff);
        mcon::Vector<double> sinc(N);
        masp::fir::GetCoefficientsLpfSinc(sinc, cutoff);
        m_Coefficients *= sinc;
    }

    return NO_ERROR;
}

status_t Resampler::SetSamplingRates(int targetFs, int baseFs)
{
    if ( targetFs <= 0 || baseFs <= 0 )
    {
        return -ERROR_ILLEGAL;
    }
    m_TargetFs = targetFs;
    m_BaseFs = baseFs;

    {
#if defined(DEBUG)
        int LCM, GCD;
#endif // #if defined(DEBUG)
        int x = baseFs;
        int y = targetFs;
        int smaller = x > y ? y : x;

#if defined(DEBUG)
        LCM = 1;
#endif // #if defined(DEBUG)
        for (int i = 2; i < smaller/2; ++i)
        {
            if ( (x % i) == 0 && (y % i) == 0 )
            {
                x /= i;
                y /= i;
#if defined(DEBUG)
                LCM *= i;
#endif // #if defined(DEBUG)
                i = 1; // reset
            }
        }
        m_L = y;
        m_M = x;
#if defined(DEBUG)
        GCD = x * y * LCM;
#endif // #if defined(DEBUG)
        DEBUG_LOG("L=%d, M=%d\n", m_L, m_M);
        DEBUG_LOG("LCM=%d, GCD=%d\n", LCM, GCD);
    }

    return NO_ERROR;
}

status_t Resampler::SetFilterParams(double nFpass, double nFstop)
{
    if ( (nFpass <= 0.0 || 1.0 <= nFpass)
        || (nFstop <= 0.0 || 1.0 <= nFstop)
        || (nFstop < nFpass) )
    {
        return -ERROR_ILLEGAL;
    }
    m_PassBandFrequency = nFpass;
    m_StopBandFrequency = nFstop;

    return NO_ERROR;
}

status_t Resampler::Initialize(int targetFs, int baseFs, double nFpass, double nFstop)
{
    status_t status = SetSamplingRates(targetFs, baseFs);
    if ( NO_ERROR != status )
    {
        return status;
    }
    return SetFilterParams(nFpass, nFstop);
}

status_t Resampler::MakeFilterByWindowType(int windowType, double _alpha)
{
    if ( windowType <= 0 && NUM_WINDOW_TYPE <= windowType )
    {
        return -ERROR_ILLEGAL;
    }
    double alpha = _alpha;

    if ( KAISER == windowType )
    {
        if ( alpha < 2.0 )
        {
            alpha = 2.0;
        }
        else if ( alpha > 10.0 )
        {
            alpha = 10.0;
        }
    }
    return UpdateCoefficients(windowType, alpha);
}

status_t Resampler::MakeFilterBySpec(double pbRipple, double sbDecay)
{
    UNUSED(pbRipple);
    const int length = static_cast<int>(sizeof(masp::g_KaiserStopbandDecay)/sizeof(int));
    int index;
    for ( index = 0; index < length; ++index)
    {
        if ( masp::g_KaiserStopbandDecay[index] > sbDecay )
        {
            break;
        }
    }
    if ( index == length )
    {
        return -ERROR_NOT_FOUND;
    }
    return UpdateCoefficients(KAISER, static_cast<double>(index + masp::g_KaiserAlphaOffset));
}

double Convolution(const double* pData, const double* pCoefs, int N, int step)
{
    double ans = 0.0;
    for ( int i = 0; i < N; ++i )
    {
        ans += pData[N - 1 - i] * pCoefs[i * step];
    }
    return ans;
}

status_t Resampler::Convert(mcon::Vector<double>& output, const mcon::Vector<double>& input) const
{
    if ( m_Coefficients.IsNull() )
    {
        return -ERROR_ILLEGAL;
    }

    const int N = input.GetLength();
    output.Resize( (N * m_L + m_M - 1) / m_M );
    // L=2, M=1 の場合、純粋なアップサンプルなので、サンプル数は2 倍になる。
    // 入力バッファのサンプルの進みは 1/2 (M / L)、係数は 2N (N * L) である。
    // L=1, M=2 の場合、純粋なダウンサンプルなので、サンプル数は1/2 になる。
    // 入力バッファのサンプルの進みは 2 (M / L)、係数は N (N * L) である。

    const double* pInput = masp::_Cast(input);
    const double* pCoefficients = masp::_Cast(m_Coefficients);
    const int width = m_Coefficients.GetLength() / m_L;

    int acc = 0;
    for ( int i = 0; i < output.GetLength(); ++i )
    {
        acc += m_M;
        int index = acc / m_L;
        int amari = acc % m_L;
        const int M = (width - 1 - i > 0) ? i + 1 : width;
        DEBUG_LOG("i=%3d, M=%3d\n", i, M);
        output[i] = masp::Convolution(pInput + index, pCoefficients + amari, M, m_L);
    }
    return NO_ERROR;
}

status_t Resampler::GetCoefficients(mcon::Vector<double>& coefficients) const
{
    if ( m_Coefficients.IsNull() )
    {
        return -ERROR_NULL;
    }
    coefficients = m_Coefficients;

    return NO_ERROR;
}

} // namespace masp {
