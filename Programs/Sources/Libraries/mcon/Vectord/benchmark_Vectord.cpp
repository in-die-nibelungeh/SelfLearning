
#include "debug.h"

#include "mutl.h"
#include "Vectord.h"

static double _fabs(double v)
{
    return (v < 0) ? -v : v;
}

void benchmark_Vectord(void)
{
    static const int KiB = 1024;
    static const int MiB = 1024 * KiB;

    mutl::Stopwatch sw;
    const int samples[] = {
           1 * KiB,
           4 * KiB,
          16 * KiB,
          64 * KiB,
         256 * KiB,
           1 * MiB,
           4 * MiB,
          16 * MiB,
          64 * MiB,
    };
    // Maximum/Minimum
    LOG("* [GetMaximum]\n");
    {
        for ( unsigned int i = 0; i < sizeof(samples)/sizeof(int); ++i )
        {
            const int n = samples[i];
            printf("n=%d\n", n);
            mcon::Vectord vd(n);
            mcon::Vector<double> vc(n);
            double max = - __DBL_MAX__;
            double min = __DBL_MAX__;
            double maxAbs = 0;
            double minAbs = __DBL_MAX__;;
            double sum = 0;
            double dot = 0;

            for ( int k = 0; k < n; ++k )
            {
                const double v = k * (k & 1 ? 1 : -1);
                vd[k] = v;
                vc[k] = v;
                if (  max < v )
                {
                    max = v;
                }
                if (  min > v )
                {
                    min = v;
                }
                const double vAbs = _fabs(v);
                if (  maxAbs < vAbs )
                {
                    maxAbs = vAbs;
                }
                if (  minAbs > vAbs )
                {
                    minAbs = vAbs;
                }
                sum += v;
                dot += v*v;
            }
            //const double norm = sqrt(dot);
            //const double ave = sum / n;
            sw.Tick();
            CHECK_VALUE(vd.GetMaximum(), max);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetMaximum(), max);
            const double recordVector  = sw.Tick();
            LOG("GetMaximum:\t%g vs %g (%g)\n", recordVectord, recordVector, recordVector/recordVectord);
        }
    }
    LOG("END\n");
}
