
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

    enum {
        ID_GET_MAXIMUM,
        ID_GET_MINIMUM,
        ID_GET_MAXIMUM_ABSOLUTE,
        ID_GET_MINIMUM_ABSOLUTE,
        ID_GET_SUM,
        ID_GET_NORM,
        ID_GET_DOT_PRODUCT,
        NUM_IDS
    };

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
    const unsigned int numPatterns = sizeof(samples) / sizeof(int);
    double scores[NUM_IDS][numPatterns];

    LOG("Benchmark started.\n");
    for ( unsigned int i = 0; i < numPatterns; ++i )
    {
        const int n = samples[i];
        LOG("Benchmark with the length of %d ... ", n);
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
        const double norm = sqrt(dot);
        // GetMaximum
        int id = 0;
        {
            sw.Push();
            CHECK_VALUE(vd.GetMaximum(), max);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetMaximum(), max);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        // GetMinimum
        {
            sw.Push();
            CHECK_VALUE(vd.GetMinimum(), min);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetMinimum(), min);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        // GetMaximumAbsolute
        {
            sw.Push();
            CHECK_VALUE(vd.GetMaximumAbsolute(), maxAbs);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetMaximumAbsolute(), maxAbs);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        // GetMinimumAbsolute
        {
            sw.Push();
            CHECK_VALUE(vd.GetMinimumAbsolute(), minAbs);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetMinimumAbsolute(), minAbs);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        // Sum
        {
            sw.Push();
            CHECK_VALUE(vd.GetSum(), sum);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetSum(), sum);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        // Norm
        {
            sw.Push();
            CHECK_VALUE(vd.GetNorm(), norm);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetNorm(), norm);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        // Dot
        {
            sw.Push();
            CHECK_VALUE(vd.GetDotProduct(vd), dot);
            const double recordVectord = sw.Tick();
            CHECK_VALUE(vc.GetDotProduct(vc), dot);
            const double recordVector  = sw.Tick();
            scores[id][i] = recordVector/recordVectord;
            ++id;
        }
        LOG("Done\n");
    }
    const char* testNames[NUM_IDS] = {
        "GetMaximum",
        "GetMinimum",
        "GetMaximumAbsolute",
        "GetMinimumAbsolute",
        "GetSum",
        "GetNorm",
        "GetDotProduct"
    };
    printf("Samples");
    for ( unsigned int k = 0; k < numPatterns; ++k )
    {
        printf(",%d", samples[k]);
    }
    printf("\n");
    for ( int id = 0; id < NUM_IDS; ++id )
    {
        printf("%s", testNames[id]);
        for ( unsigned int k = 0; k < numPatterns; ++k )
        {
            printf(",%g", scores[id][k]);
        }
        printf("\n");
    }
    LOG("END\n");
}
