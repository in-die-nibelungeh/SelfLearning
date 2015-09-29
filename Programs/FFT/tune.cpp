#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include <string>

#include "Fft.h"
#include "FileIo.h"
#include "Vector.h"
#include "BenchUtil.h"

static void tune_ft(void)
{
    std::string fbody("sweep_440-3520_1s");
    int fs;

    mcon::Vector<double> sweep;
    {
        mcon::Vector<int16_t> sweep_int;
        std::string wfile = fbody + std::string(".wav");
        FileIo wave;
        wave.Read(wfile.c_str(), sweep_int);
        sweep = sweep_int;
        fs = wave.GetSamplingRate();
    }
    LOG("SamplingRate: %d\n", fs);
    LOG("Length: %d\n", sweep.GetLength());

    mcon::Matrix<double> complex(2, sweep.GetLength());
    mcon::Matrix<double> gp(2, sweep.GetLength());
    mbut::Stopwatch sw;
    Fft::Ft(complex, sweep);
    LOG("Time consumed for Ft: %f [sec]\n", sw.Push());
    Fft::ConvertToPolarCoords(gp, complex);
    LOG("Time consumed for Gp: %f [sec]\n", sw.Push());
    {
        std::string csv = fbody + std::string(".csv");

        FILE* fp = fopen(csv.c_str(), "w");

        if (NULL != fp)
        {
            for (int i = 0; i < complex.GetColumnLength(); ++i)
            {
                fprintf(fp, "%d,%g,%g\n", i, complex[0][i], complex[1][i]);
            }
            fprintf(fp, "\n");
            const double df = static_cast<double>(fs) / sweep.GetLength();
            for (int i = 0; i < gp.GetColumnLength(); ++i)
            {
                fprintf(fp, "%g,%g,%g\n", i*df, gp[0][i], gp[1][i]);
            }
            fclose(fp);
        }
    }
}

static void tunes(void)
{
    tune_ft();
}
