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

#include "debug.h"

#include "mcon.h"
#include "masp.h"

#include "Common.h"

void NormalEquationPre(mcon::Matrixd& Inversed, mcon::Matrixd& Ut, const mcon::Vectord& u, const mcon::Matrixd& W)
{
    const int N = Ut.GetColumnLength();
    const int M = Ut.GetRowLength();

    ASSERT( !Ut.IsNull() );
    ASSERT( N != 0 );
    ASSERT( M != 0 );
    UNUSED( W );

    Ut = 0;

    // Ut の方がメモリアクセスの観点から有利なはず。
    for (int m = 0; m < M; ++m)
    {
        for (int n = 0; n + m < N; ++n)
        {
            //LOG("m=%d(%d), n=%d(%d)\n", m, M, n, N);
            Ut[m][m + n] = u[n];
        }
    }
    if (W.IsNull())
    {
        Inversed = Ut.Multiply( Ut.T() ).I();
    }
    else
    {
        Inversed = Ut.Multiply( W ).Multiply( Ut.T() ).I();
    }
}

void NormalEquationPost(
    mcon::Vector<double>& h,
    const mcon::Matrixd& Inversed,
    const mcon::Matrixd& Ut,
    const mcon::Vectord& d,
    const mcon::Matrixd& W,
    double* pError)
{
    const mcon::Matrixd dm(d, true); // d = (1, n) ==> dm = (n, 1);
    const mcon::Matrixd hm = W.IsNull() ? Inversed.Multiply(Ut).Multiply(dm)
        : Inversed.Multiply(Ut).Multiply(W).Multiply(dm);
    h = hm.T()[0];
    if (NULL != pError)
    {
        const mcon::Matrixd diff = dm - Ut.T().Multiply(hm);
        *pError = (diff * diff).T()[0].GetSum();
    }
}

status_t NormalEquation(mcon::Vector<double>& h, const mcon::Vectord& u, const mcon::Vectord& d, double* pError)
{
    const int N = d.GetLength();
    const int M = h.GetLength();
    if ( h.IsNull() || M > N )
    {
        return -ERROR_ILLEGAL;
    }
    //   h   = (  Ut  *   W   *   U  )^(-1) *   Ut  *   W   *   d
    // [Mx1] = ([MxN] * [NxN] * [NxM])      * [MxN] * [NxN] * [Nx1]
    mcon::Matrixd Ut(M, N);
    mcon::Matrixd Inv;
    mcon::Matrixd W;

    NormalEquationPre(Inv, Ut, u, W);

    NormalEquationPost(h, Inv, Ut, d, W, pError);

    return NO_ERROR;
}

status_t Rls(mcon::Vector<double>& h, const mcon::Vectord& u, const mcon::Vectord& d)
{
    LOG("\n");
    LOG("Initializing variables ... ");
    const int N = u.GetLength();
    const int M = h.GetLength();

    double c = 0.001; // an appropriately small number
    mcon::Matrixd P = mcon::Matrixd::E(M);
    mcon::Matrixd hm(M, 1);
    mcon::Vectord uv(M);

    mcon::Matrix<double> logs(6, N);
    mcon::VectordBase& e   = logs[0];
    mcon::VectordBase& eta = logs[1];
    mcon::VectordBase& J   = logs[2];
    mcon::VectordBase& K   = logs[3];
    mcon::VectordBase& U   = logs[4];
    mcon::VectordBase& E   = logs[5];

    hm = 0;
    P /= c;
    uv = 0;

    for (int i = 0; i < N; ++i)
    {
        uv.Unshift(u[i]);
        U[i] = uv.GetNorm(); // logs
        const mcon::Matrixd um(uv, true); // Transposed
        mcon::Matrixd k(P.Multiply(um)); // numerator
        const mcon::Matrixd& denominator = um.Transpose().Multiply(P).Multiply(um);
        ASSERT(denominator.GetRowLength() == 1 && denominator.GetColumnLength() == 1);
        const double denom = denominator[0][0] + 1;
        k /= denom;
        K[i] = k.Transpose()[0].GetNorm(); // logs
        const mcon::Matrixd& m = um.Transpose().Multiply(hm);
        ASSERT(m.GetRowLength() == 1 && m.GetColumnLength() == 1);
        eta[i] = d[i] - m[0][0]; // logs
        hm += k * eta[i];

        e[i] = d[i] - (um.Transpose().Multiply(hm))[0][0]; // logs
        if ( i > 0 )
        {
            J[i] = J[i-1] + e[i] * eta[i]; // logs
            E[i] = E[i-1] + d[i] * d[i]; // logs
        }
        else
        {
            J[i] = e[i] * eta[i]; // logs
            E[i] = d[i] * d[i]; // logs
        }
        P -= k.Multiply(um.Transpose()).Multiply(P);
        if ( (i % 10) == 0 )
        {
            LOG("%4.1f [%%]: %d/%d\r", i*100.0/N, i, N);
        }
    }
    h = hm.Transpose()[0];

    return NO_ERROR;
}

#if 0
status_t RlsFromTwoWaveforms(mcon::Vector<double>& resp, const mcon::Vectord& input, const mcon::Vectord& d, int tapps, mcon::Matrix<double>* pMatrix)
{
    LOG("\n");
    LOG("Initializing variables ... ");
    const int n = input.GetLength() > d.GetLength() ? d.GetLength() : input.GetLength();
    const int M = tapps;
    double c = 0.001; // an appropriately small number
    mcon::Matrixd P = mcon::Matrixd::E(M);
    mcon::Matrixd h(M, 1);
    mcon::Vectord uv(M);

    mcon::Matrix<double> logs(6, n);
    mcon::Vector<double>& e   = logs[0];
    mcon::Vector<double>& eta = logs[1];
    mcon::Vector<double>& J   = logs[2];
    mcon::Vector<double>& K   = logs[3];
    mcon::Vector<double>& U   = logs[4];
    mcon::Vector<double>& E   = logs[5];

    h = 0;
    P /= c;
    uv = 0;

    LOG("Done\n");
    LOG("Now starting RLS with %d samples.\n", n);
    for (int i = 0; i < n; ++i)
    {
        uv.Unshift(input[i]);
        U[i] = uv.GetNorm(); // logs
        const mcon::Matrixd u(uv, true); // Transposed
        mcon::Matrixd k(P.Multiply(u)); // numerator
        const mcon::Matrixd& denominator = u.Transpose().Multiply(P).Multiply(u);
        ASSERT(denominator.GetRowLength() == 1 && denominator.GetColumnLength() == 1);
        const double denom = denominator[0][0] + 1;
        k /= denom;
        K[i] = k.Transpose()[0].GetNorm(); // logs
        const mcon::Matrixd& m = u.Transpose().Multiply(h);
        ASSERT(m.GetRowLength() == 1 && m.GetColumnLength() == 1);
        eta[i] = d[i] - m[0][0]; // logs
        h += k * eta[i];

        e[i] = d[i] - (u.Transpose().Multiply(h))[0][0]; // logs
        if ( i > 0 )
        {
            J[i] = J[i-1] + e[i] * eta[i]; // logs
            E[i] = E[i-1] + d[i] * d[i]; // logs
        }
        else
        {
            J[i] = e[i] * eta[i]; // logs
            E[i] = d[i] * d[i]; // logs
        }
        P -= k.Multiply(u.Transpose()).Multiply(P);
        if ( (i % 10) == 0 )
        {
            LOG("%4.1f [%%]: %d/%d\r", i*100.0/n, i, n);
        }
    }
    resp = h.Transpose()[0] / scale;

    if ( outfile.empty() )
    {
        char _fbody[128];
        mutl::NodePath _inputPath(inputFile);
        mutl::NodePath _referencePath(referenceFile);

        sprintf(_fbody, "Af_%dtapps_", tapps);
        fbody = std::string(_fbody);
        fbody += _inputPath.GetBasename();
        fbody += std::string("_");
        fbody += _referencePath.GetBasename();
    }
    const std::string outbase = outdir + fbody;
    LOG("\n");
    {
        const int length = resp.GetLength();
        const std::string ecsv(".csv");
        const std::string ewav(".wav");
        mcon::Matrix<double> gp(2, length);
        {
            mcon::Matrix<double> complex(2, length);
            masp::ft::Ft(complex, resp);
            masp::ft::ConvertToPolarCoords(gp, complex);
        }
        // Gain/Phase/PulseSeries
        {
            mcon::Matrix<double> saved(4, length);
            const double df = 1.0 / length;
            for (int i = 0; i < length; ++i)
            {
                saved[0][i] = i*df*fs;
            }
            saved[1] = gp[0];
            saved[2] = gp[1];
            saved[3] = resp;

            std::string fname = outbase + ecsv;
            mfio::Csv csv(fname);
            csv.Write(",Frequency,Amplitude,Argument,Impluse\n");
            csv.Write(saved);
            csv.Close();
            LOG("Output: %s\n", fname.c_str());
        }
        {
            mfio::Wave wave(fs, 1, 32, mfio::Wave::IEEE_FLOAT);
            if ( NO_ERROR == wave.Write(outbase + ewav, resp) )
            {
                LOG("Output: %s\n", (outbase + ewav).c_str());
            }
            else
            {
                LOG("Failed in writing %s\n", (outbase + ewav).c_str());
            }
        }

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
    }
    LOG("\n");
    {
        LOG("Verifying ... ");
        const int n = input.GetLength();
        mcon::Vector<double> origin(n);
        Convolution(origin, input, resp);
        LOG("Done\n");
        origin *= 32767.0/origin.GetMaximumAbsolute();
        const std::string ewav(".wav");
        std::string fname = outbase + std::string("_iconv") + ewav;
        LOG("Saving as %s\n", fname.c_str());
        mfio::Wave wave;
        wave.SetNumChannels(1);
        wave.SetBitDepth(16);
        wave.SetSamplingRate(fs);
        wave.SetWaveFormat(mfio::Wave::LPCM);
        status_t status = wave.Write(fname, origin);
        if ( NO_ERROR != status )
        {
            ERROR_LOG("Failed in writing %s: error=%d\n", fname.c_str(), status);
        }
        LOG("Done.\n");
    }
    return NO_ERROR;
}
#endif

status_t Estimater(
    mcon::Matrixd& estimated,
    const mcon::Matrixd& input,
    const mcon::Vectord& reference,
    const int tapps,
    const mcon::Vectord& referenceOffsets
    )
{
    const int M = tapps;
    const int referenceOffsetMaximum = referenceOffsets.IsNull() ? 0 : referenceOffsets.GetMaximum();
    const int N = std::min(input.GetColumnLength(), reference.GetLength() - referenceOffsetMaximum + 1);
    // Can't continue processing...
    if (M > N)
    {
        return -ERROR_ILLEGAL;
    }
    if (false == estimated.Resize(input.GetRowLength() , M))
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    status_t status = NO_ERROR;
    for (uint r = 0; r < input.GetRowLength(); ++r)
    {
        const uint referenceOffset = referenceOffsets.IsNull() ? 0 : referenceOffsets[r];
        if (!referenceOffsets.IsNull())
        {
            LOG("    Ch-%d: ReferenceOffset=%d\n", r, referenceOffset);
        }
        const mcon::Vectord d = reference(referenceOffset, N);
        const mcon::Vectord _u = input[r];
        const mcon::Vectord u = _u(0, N);
        mcon::Vector<double> h(M);
        NormalEquation(h, u, d, NULL);
        if (NO_ERROR != status)
        {
            break;
        }
        estimated[r] = h;
    }
    return status;
}

status_t Process(ProgramParameter* param)
{
    return Estimater(
        param->inversedSignal,
        param->inputSignal,
        param->referenceSignal,
        param->tapps,
        param->referenceOffset
    );
}
