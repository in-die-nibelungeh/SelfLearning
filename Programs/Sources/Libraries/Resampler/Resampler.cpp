
#include "status.h"
#include "Resampler.h"
#include "Window.h"
#include "Fir.h"

namespace masp {

namespace {
    const double g_KaiserTransitionBand[] =
    {
        3.0,
        4.0,
        5.2,
        6.4,
        7.6,
        9.0,
        10.2,
        11.4,
        12.8
    };
}

status_t Resampler::Initialize(int targetFs, int baseFs, int windowType)
{
    SetSamplingRates(targetFs, baseFs);
    SetWindowType(windowType);
    return NO_ERROR;
}

status_t Resampler::UpdateCoefficients(void)
{
    int MinimumXXX;
    int MaximumXXX;

    {
        int x = m_BaseFs;
        int y = m_TargetFs;
        int smaller = x > y ? y : x;

        MinimumXXX = 1;
        for (int i = 2; i < smaller/2; ++i)
        {
            if ( (x % i) == 0 && (y % i) == 0 )
            {
                x /= i;
                y /= i;
                MinimumXXX *= i;
                i = 1; // reset
            }
        }
        m_L = y;
        m_M = x;
        MaximumXXX = x * y * MinimumXXX;
    }
    const double fcNormalized = 1.0 / static_cast<double>(m_L > m_M ? m_L : m_M);
    const double transitionBandNormalized = 0.1;
    int N = 0;

    switch(m_WindowType)
    {
    case HANNING:
        {
            N = static_cast<int>(6.2 / transitionBandNormalized + 0.5);
            m_Coefficients.Resize(N);
            masp::window::Hanning(m_Coefficients);
        }
        break;
    case HAMMING:
        {
            N = static_cast<int>(6.6 / transitionBandNormalized + 0.5);
            m_Coefficients.Resize(N);
            masp::window::Hamming(m_Coefficients);
        }
        break;
    case BLACKMAN:
        {
            N = static_cast<int>(11.0 / transitionBandNormalized + 0.5);
            m_Coefficients.Resize(N);
            masp::window::Blackman(m_Coefficients);
        }
        break;
    case KAISER:
        {
            ASSERT( 2.0 <= m_WindowArgument && m_WindowArgument <= 10.0 );
            const int index = static_cast<int>(m_WindowArgument + 0.5) - 2;
            N = static_cast<int>( masp::g_KaiserTransitionBand[index] / transitionBandNormalized + 0.5);
            m_Coefficients.Resize(N);
            masp::window::Kaiser(m_Coefficients, 1.0);
        }
        break;
    default:
        ASSERT ( 0 );
        break;
    }
    mcon::Vector<double> sinc(N);
    masp::fir::GetCoefficientsLpfSinc(sinc, fcNormalized);
    m_Coefficients *= sinc;

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
    m_Ratio = static_cast<double>(targetFs) / baseFs;
    m_Length = static_cast<int>( m_Ratio + 0.5);

    UpdateCoefficients();
    return NO_ERROR;
}

status_t Resampler::SetWindowType(int windowType, double alpha)
{
    if ( windowType <= 0 && NUM_WINDOW_TYPE <= windowType)
    {
        return -ERROR_ILLEGAL;
    }
    // “¯‚¶Œ^‚È‚çˆ—‚µ‚È‚¢B
    if ( m_WindowType == windowType )
    {
        return NO_ERROR;
    }
    m_WindowType = windowType;
    m_WindowArgument = alpha;

    if ( KAISER == windowType )
    {
        if ( alpha < 2.0 )
        {
            m_WindowArgument = 2.0;
        }
        else if ( alpha > 10.0 )
        {
            m_WindowArgument = 10.0;
        }
    }

    UpdateCoefficients();

    return NO_ERROR;
}

status_t Resampler::Convert(mcon::Vector<double>& output) const
{
    return NO_ERROR;
}

double Resampler::Get(int i) const
{
    return 0.0;
}

} // namespace masp {
