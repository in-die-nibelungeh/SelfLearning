
#include "status.h"
#include "Resampler.h"
#include "Window.h"
#include "Fir.h"

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
    int N = 0;

    switch(windowType)
    {
    case HANNING:
        {
            N = static_cast<int>(6.2 / 2 / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
            m_Coefficients.Resize(N);
            masp::window::Hanning(m_Coefficients);
        }
        break;
    case HAMMING:
        {
            N = static_cast<int>(6.6 / 2 / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
            m_Coefficients.Resize(N);
            masp::window::Hamming(m_Coefficients);
        }
        break;
    case BLACKMAN:
        {
            N = static_cast<int>(11.0 / 2 / bandWidth + 0.5);
            N += (N & 1 ? 0 : 1);
            N *= m_L;
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
        //int LCM, GCD;
        int x = baseFs;
        int y = targetFs;
        int smaller = x > y ? y : x;

        //LCM = 1;
        for (int i = 2; i < smaller/2; ++i)
        {
            if ( (x % i) == 0 && (y % i) == 0 )
            {
                x /= i;
                y /= i;
                //LCM *= i;
                i = 1; // reset
            }
        }
        m_L = y;
        m_M = x;
        //GCD = x * y * LCM;
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
#if 1
    return UpdateCoefficients(KAISER, static_cast<double>(index + masp::g_KaiserAlphaOffset));
#else
    double ratio = static_cast<double>(m_TargetFs) / m_BaseFs;
    if ( ratio < 1.0 )
    {
        ratio = 1 / ratio;
    }

    // Window (Kaiser)
    {
        const double bandWidth = (m_StopBandFrequency - m_PassBandFrequency);
        const int N = static_cast<int>(masp::g_KaiserTransitionBand[index] / bandWidth + 0.5) * m_L;
        const double alpha = static_cast<double>(index + masp::g_KaiserAlphaOffset);
        m_Coefficients.Resize(N);
        masp::window::Kaiser(m_Coefficients, alpha);
    }

    // Lpf (Sinc)
    {
        const int N = m_Coefficients.GetLength();
        const double cutoff = (m_StopBandFrequency + m_PassBandFrequency) / 2.0 / m_L;
        mcon::Vector<double> sinc(N);
        masp::fir::GetCoefficientsLpfSinc(sinc, cutoff);
        m_Coefficients *= sinc;
    }
#endif
}

double Convolution(const double* pData, const double* pCoefs, int N, int step)
{
    double ans = 0.0;
    for ( int i = 0; i < N; ++i )
    {
        ans += pData[i] * pCoefs[i * step];
    }
    return 0.0;
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
    for ( int i = 0; i < N; ++i )
    {
        acc += m_M;
        int index = acc / m_L;
        int amari = acc % m_L;
        output[i] = masp::Convolution(pInput + index, pCoefficients + amari, width, m_L);
    }
    return NO_ERROR;
}

} // namespace masp {
