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

#include <string>

#include "masp.h"
#include "mfio.h"

#include "Common.h"

namespace {
    status_t Verify(mcon::Vector<double>& origin, const mcon::VectordBase& input, const mcon::VectordBase& estimated)
    {
        status_t status = NO_ERROR;
        const int N = input.GetLength();
        UNUSED(N);
        ASSERT( N > 0 );
        ASSERT( estimated.GetLength() > 0 );

        status = masp::fir::Convolution(origin, input, estimated);
        ASSERT( NO_ERROR == status );
        ASSERT( ! origin.IsNull() );
        return status;
    }
}

status_t PostProcess(ProgramParameter* param)
{
    status_t status = NO_ERROR;

    // ���x�����킹
    // 1. ���g���̈�ō��킹���
    //    �ǂ̎��g���ш�ō��킹�邩�����ɂȂ����B
    //    �ő�̑ш�Ń��x�����킹����ƁA�������ς���Ă���B
    //    �t���b�g�ȓ����ւ̕␳�ł���΂��Ղ����A�����ł͂Ȃ��Ȃ��Ă���B
    // 2. �G�l���M�[�ō��킹��
    //    ���ɑ�������H���g���ш�ō��킹��̂Ƃ͉����Ⴄ�H
    //    ��肽�����Ƃ� 3 �Ɠ������H
    // 3. ���ԗ̈�ō��킹�� (��)
    //    �W���̐�Βl�����Z�������ʂ� 1.0f �ȉ��ɂȂ�悤�ɍ��킹��B
    //    (���蓾�Ȃ��z�肾��) �S�^�b�v�ɂ킽��W���Ɠ��������� 32767 ��
    //    ���͂��ꂽ�ꍇ�ɁA16bit �̐��ő�l�𒴂��Ȃ��悤�ɂ��邽�߂̒����B
    LOG("Post processing ...\n");
    {
        const size_t ch = param->inversedSignal.GetRowLength();
        const size_t N = param->inversedSignal.GetColumnLength();

        mcon::Vectord sumups(ch);
        mcon::Vectord energies(ch);

        for (uint r = 0; r < ch; ++r)
        {
            // �`�����l�����̃��x������
            const mcon::VectordBase& signal = param->inversedSignal[r];
            energies[r] = sqrt(signal.Dot(signal));
            LOG("    Ch-%d Energy: %g\n", r, energies[r]);
            // �ώZ (��ݍ���) ���鎞�̂��߂̍ő�l���� (�S��)
            double sumup = 0;
            for (uint k = 0; k < N; ++k)
            {
                sumup += fabs(signal[k]);
            }
            sumups[r] = sumup;
        }
        LOG("    Ajusted Level:\n");
        // ���x������ (�グ�����)
        const double maxEnergy = energies.GetMaximum();
        for (uint r = 0; r < ch; ++r)
        {
            mcon::VectordBase& signal = param->inversedSignal[r];
            const double extendFactor =
                sqrt( maxEnergy / energies[r]  );
            signal *= extendFactor;
            LOG("    Ch-%d Energy: %g\n", r, sqrt(signal.Dot(signal)) );
        }
        param->inversedSignal *= param->upperValue / sumups.GetMaximum();
    }

    LOG("Verifying ... \n");
    {
        const int ch = param->inputSignal.GetRowLength();
        const int N = param->inputSignal.GetColumnLength();
        mcon::Matrix<double> saved(ch, N);
        for (int c = 0; c < ch; ++c)
        {
            const mcon::VectordBase& input(param->inputSignal[c]);
            const mcon::VectordBase& inversed(param->inversedSignal[c]);
            mcon::Vectord origin;
            status = Verify(
                origin,
                input,
                inversed);
            if ( NO_ERROR != status )
            {
                ERROR_LOG("Failed in Verify(): error=%d\n", status);
                return status;
            }
            saved[c] = origin;
        }
        const std::string ewav(".wav");
        const std::string filepath = param->outputBase + std::string("_iconv") + ewav;
        const int Pcm32Bit = 32;
        mfio::Wave wave(param->samplingRate, ch, Pcm32Bit, mfio::Wave::IEEE_FLOAT);
        status = wave.Write(filepath, saved);
        LOG("    Output: %s\n", filepath.c_str());
        if (NO_ERROR != status)
        {
            ERROR_LOG("Failed in writing %s: error=%d\n", filepath.c_str(), status);
        }
    }
    return status;
}
