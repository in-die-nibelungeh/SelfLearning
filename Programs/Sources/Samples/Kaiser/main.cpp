#include "masp.h"
#include "Vector.h"
#include "FileIo.h"

int main(void)
{
    const int N = 101;
    mcon::Vector<double> As(N);

    for ( int A = 0; A < N; ++A )
    {
        double alpha = 0.0;
        if ( A <= 21 )
        {
        }
        else if ( A < 50 )
        {
            alpha = 0.5842 * pow( A-21 , 0.4) + 0.07886 * (A-21);
        }
        else
        {
            alpha = 0.1102 * (A-8.7);
        }
        As[A] = alpha;
    }
    mfio::Csv::Write("Kaiser.csv", As);
    return 0;
}
