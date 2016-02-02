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

#include "masp.h"
#include "mfio.h"

#include "Common.h"

namespace {
    status_t SaveResults(const mcon::Matrixd& _signal, int samplingRate, const std::string& outbase)
    {
        status_t status = NO_ERROR;
        const std::string ecsv(".csv");
        const std::string ewav(".wav");
        const mcon::Matrix<double> signal(_signal);
        const size_t N = signal.GetColumnLength();
        ASSERT( N > 0 );

        LOG("Saving ... \n");
        {
            const double df = 1.0 / N;
            const int ch = signal.GetRowLength();
            mcon::Matrix<double> saved(1 + ch * 4, N);

            for (uint i = 0; i < N; ++i)
            {
                saved[0][i] = i * df * samplingRate;
            }

            for (int c = 0; c < ch; ++c)
            {
                mcon::Matrix<double> complex(2, N);
                mcon::Matrix<double> polar(2, N);
                masp::ft::Ft(complex, signal[c]);
                masp::ft::ConvertToPolarCoords(polar, complex);
                double squaredSum = 0;

                for (uint i = 0; i < N; ++i)
                {
                    squaredSum += signal[c][i] * signal[c][i];
                    saved[c * 4 + 3][i] = sqrt(squaredSum);
                }
                saved[c * 4 + 1] = polar[0];
                saved[c * 4 + 2] = polar[1];
                saved[c * 4 + 4] = signal[c];
            }

            // Amplitude/Phase/Energy/PulseSeries
            do
            {
                const std::string filepath = outbase + ecsv;
                mfio::Csv csv(filepath);
                LOG("    Output: %s\n", filepath.c_str());
                status = csv.Write(",Frequency");
                for (int c = 0; c < ch; ++c)
                {
                    status = csv.Write(",Amplitude,Argument,Energy,Impluse");
                }
                status = csv.Write("\n");
                if (NO_ERROR != status)
                {
                    ERROR_LOG("Failed in writing %s: error=%d\n", filepath.c_str(), status);
                    break;
                }
                status = csv.Write(saved);
                if (NO_ERROR != status)
                {
                    ERROR_LOG("Failed in writing %s: error=%d\n", filepath.c_str(), status);
                    break;
                }
                LOG("    Done\n");
                csv.Close();
            } while (false);
        }

        do
        {
            const std::string filepath = outbase + ewav;
            const int channelCount = signal.GetRowLength();
            const int SingleFloat = 32;
            mfio::Wave wave(samplingRate, channelCount, SingleFloat, mfio::Wave::IEEE_FLOAT);
            status = wave.Write(filepath, signal);
            LOG("    Output: %s\n", filepath.c_str());
            if (NO_ERROR != status)
            {
                ERROR_LOG("Failed in writing %s: error=%d\n", filepath.c_str(), status);
                break;
            }
            LOG("    Done\n");
        } while (false);

        return status;
    }
}

status_t Cleanup(const ProgramParameter* param)
{
    status_t status = SaveResults(
        param->inversedSignal,
        param->samplingRate,
        param->outputBase);

    if ( status != NO_ERROR )
    {
        ERROR_LOG("Failed in SaveResults(): error=%d\n", status);
        return status;
    }
#if 0
        // Rls logs
        {
            mcon::Matrix<double> logs(6, n);
            logs[0] = e;
            logs[1] = eta;
            logs[2] = J;
            logs[3] = K;
            logs[4] = U;
            logs[5] = E;
            std::string fname = outbase + std::string("_logs") + ecsv;
            mfio::Csv csv(fname);
            csv.Write("i,e,eta,J,|k|,|u|,Esum\n");
            csv.Write(logs);
            csv.Close();
            LOG("Output: %s\n", fname.c_str());
        }
#endif

    return NO_ERROR;
}
