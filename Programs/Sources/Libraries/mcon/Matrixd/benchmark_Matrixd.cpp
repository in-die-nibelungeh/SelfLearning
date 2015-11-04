
#include "debug.h"

#include "mutl.h"
#include "Matrix.h"
#include "Matrixd.h"

double g_Global;

void benchmark_Matrixd(void)
{
    static const int KiB = 1024;
    static const int MiB = 1024 * KiB;

    enum {
        ID_TRANSPOSE,
        ID_MULTIPLY,
        ID_DETERMINANT,
        ID_INVERSE,
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
        mcon::Matrixd md1(n);
        mcon::Matrix<double> mc1(n);

#define VALUE(i, k) ((i+1)*10 + (k+1))

        for ( int k = 0; k < n; ++k )
        {
            for ( int l = 0; l < n; ++l )
            {
                const double v = VALUE(k, l) * (k & 1 ? 1 : -1);
                md1[k][l] = v;
                mc1[k][l] = v;
            }
        }
        // Transpose
        int id = 0;
        {
            sw.Push();
            g_Global = md1.T()[0][0];
            const double recordMatrixd = sw.Tick();
            g_Global = mc1.T()[0][0];
            const double recordMatrix  = sw.Tick();
            scores[id][i] = recordMatrix/recordMatrixd;
            ++id;
        }
        // Multiply
        {
            sw.Push();
            g_Global = md1.Multiply(md1)[0][0];
            const double recordMatrixd = sw.Tick();
            g_Global = mc1.Multiply(mc1)[0][0];
            const double recordMatrix  = sw.Tick();
            scores[id][i] = recordMatrix/recordMatrixd;
            ++id;
        }
        // Determinant
        {
            sw.Push();
            g_Global = md1.D();
            const double recordMatrixd = sw.Tick();
            g_Global = mc1.D();
            const double recordMatrix  = sw.Tick();
            scores[id][i] = recordMatrix/recordMatrixd;
            ++id;
        }
        // Inverse
        {
            sw.Push();
            g_Global = md1.I()[0][0];
            const double recordMatrixd = sw.Tick();
            g_Global = mc1.I()[0][0];
            const double recordMatrix  = sw.Tick();
            scores[id][i] = recordMatrix/recordMatrixd;
            ++id;
        }
        LOG("Done\n");
    }
    const char* testNames[NUM_IDS] = {
        "Transpose",
        "Multiply",
        "Determinant",
        "Invserse"
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
