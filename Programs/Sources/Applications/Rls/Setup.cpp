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

#include "mfio.h"

#include "Common.h"

namespace {
    status_t LoadSignalFromWav(const std::string& filepath, mcon::Matrixd& signal, uint* pSamplingRate = NULL)
    {
        mfio::Wave wave;
        mcon::Matrix<double> matrix;
        status_t status = wave.Read(filepath, matrix);

        if (NO_ERROR != status)
        {
            ERROR_LOG("An error occured during reading %s: error=%d\n", filepath.c_str(), status);
            return -ERROR_CANNOT_OPEN_FILE;
        }
        signal = matrix;

        if (NULL != pSamplingRate)
        {
            *pSamplingRate = wave.GetSamplingRate();
        }

        return NO_ERROR;
    }
}

status_t Setup(ProgramParameter* param)
{
    // Input
    ShowMessage msg(__func__);
    do
    {
        uint fs;
        const std::string& inputPath = param->inputFilepath;
        LOG("Loading signal from wav (%s) ... \n", inputPath.c_str());
        const status_t status = LoadSignalFromWav(std::string(inputPath), param->inputSignal, &fs);
        if (NO_ERROR != status)
        {
            ERROR_LOG("An error occured during loading %s: error=%d\n", inputPath.c_str(), status);
            return status;
        }
        if (param->inputLength > 0 && static_cast<uint>(param->inputSignal.GetColumnLength()) > param->inputLength)
        {
            const uint length = param->inputLength;
            const int ch = param->inputSignal.GetRowLength();
            const mcon::Matrixd _input(param->inputSignal);
            param->inputSignal.Resize(ch, length);
            for (int r = 0; r < ch; ++r)
            {
                const mcon::Vector<double> _in(_input[r]);
                const mcon::Vector<double> in = _in(0, length);
                param->inputSignal[r] = in;
            }
        }
        for (int r = 0; r < param->inputSignal.GetRowLength(); ++r)
        {
            const mcon::Vectord& d = param->inputSignal[r];
            LOG("    Energy-%d: %f\n", r, sqrt(d.Dot(d)));
        }
        msg.Log("Done\n");
        param->samplingRate = fs;

    } while (false);
#if 0
    if (true == cut)
    {
        LOG("Cut\n");
        const int base = 57;
        const int n = 170;
        const int w = n + (n-1);
        mcon::Vector<double> window(w);
        masp::window::Blackman(window);
        mcon::Matrix<double> saved(2, n);
        saved[0] = input(base, n);
        input = input(base, n);
        for (int i = 0; i < input.GetLength(); ++i)
        {
            input[i] *= window[i+n-1];
        }
        saved[1] = input;
        mfio::Csv::Write("inputs.csv", saved);
        tapps = n;
    }
#endif

    // Reference
    if ( !param->referenceFilepath.empty() )
    {
        uint fs;
        const std::string& referencePath = param->referenceFilepath;
        mcon::Matrixd referenceSignal;
        LOG("    Loading signal from wav (%s) ... \n", referencePath.c_str());
        const status_t status = LoadSignalFromWav(std::string(referencePath), referenceSignal, &fs);
        if (NO_ERROR != status)
        {
            ERROR_LOG("An error occured during loading %s: error=%d\n", referencePath.c_str(), status);
            return status;
        }
        if (fs != param->samplingRate)
        {
            ERROR_LOG("Not matched the sampling rates: %d (input) <=> %d (reference)\n", param->samplingRate, fs);
            return -ERROR_ILLEGAL;
        }

        const int ch = referenceSignal.GetRowLength();
        if ( ch > 1 )
        {
            LOG("%d-channel waveform was input.\n", ch);
            LOG("Only 1st channel is used for estimation.\n");
        }
        param->referenceSignal = referenceSignal[0];
        msg.Log("    Done\n");
    }
    else // ƒCƒ“ƒpƒ‹ƒX‰ž“š
    {
        const size_t N = param->inputSignal.GetColumnLength();
        param->referenceSignal.Resize(N);
        param->referenceSignal[0] = 1.0f;
    }

    if ( param->inputSignal.GetColumnLength() > param->referenceSignal.GetLength() )
    {
        LOG("\nWarning: input is longer than reference.\n");
        LOG("This may cause a wrong estimation.\n");
    }

    return NO_ERROR;
}
