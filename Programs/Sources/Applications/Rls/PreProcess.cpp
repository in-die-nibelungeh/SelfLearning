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

#include "Common.h"

// These are in Process.cpp
void NormalEquationPre(
    mcon::Matrixd& Inversed,
    mcon::Matrixd& Ut,
    const mcon::Vectord& u,
    const mcon::Matrixd& W);

void NormalEquationPost(
    mcon::Vector<double>& h,
    const mcon::Matrixd& Inversed,
    const mcon::Matrixd& Ut,
    const mcon::Vectord& d,
    const mcon::Matrixd& W,
    double* pError);

namespace {

template <typename Type>
Type Clamp(Type min, Type in, Type max)
{
    if (in < min)
    {
        return min;
    }
    if (in > max)
    {
        return max;
    }
    return in;
}

template <typename Type>
Type ClampLower(Type min, Type in)
{
    if (in < min)
    {
        return min;
    }
    return in;
}

template <typename Type>
Type ClampUpper(Type in, Type max)
{
    if (in > max)
    {
        return max;
    }
    return in;
}

double CalculateSquaredSum(const mcon::VectordBase& signal)
{
    const mcon::Vectord v(signal);
    return v.Dot(v);
}

int FindBaseFromFront(const mcon::VectordBase& signal, const double energy, const double threshold)
{
    const uint N = signal.GetLength();
    int start = -1;
    double tmp = 0;
    // �O����T��
    for (uint k = 0; k < N; ++k)
    {
        tmp += signal[k] * signal[k];
        if (tmp / energy > threshold)
        {
            start = k;
            break;
        }
    }
    return start;
}

int FindBaseFromBack(const mcon::VectordBase& signal, const double energy, const double threshold)
{
    const uint N = signal.GetLength();
    int end = -1;
    double tmp = 0;
    // ��납��T��
    for (uint k = N - 1; k >= 0; --k)
    {
        tmp += signal[k] * signal[k];
        if (tmp / energy > threshold)
        {
            end = k;
            break;
        }
    }
    return end;
}

void GetEnergySpotIndex(int* pStart, int* pEnd, const mcon::VectordBase& signal, double threshold = 0.95 /* TBD */)
{
    //const int minimumWidth = 8;            // TBD
    //const int maximumWidth = Clamp<int>(minimumWidth, tapps / 20, 64);  // TBD

    const double energy = CalculateSquaredSum(signal);

    *pStart = FindBaseFromBack (signal, energy, threshold);
    *pEnd   = FindBaseFromFront(signal, energy, threshold);
}

typedef struct _tag
{
    int start;
    int end;
} StartEnd;


StartEnd GetEnergySpotIndexOfInput(const mcon::Matrixd& input, const double threshold)
{
    const int N = input.GetColumnLength();
    int startIndex = N - 1;
    int endIndex = 0;
    for (int r = 0; r < input.GetRowLength(); ++r)
    {
        int startSpot = 0;
        int endSpot = 0;
        GetEnergySpotIndex(&startSpot, &endSpot, input[r], threshold);

        startIndex = std::min(startIndex, startSpot);
        endIndex = std::max(endIndex, endSpot);
    }
    StartEnd startEnd =
    {
        startIndex,
        endIndex
    };
    return startEnd;
}

StartEnd GetEnergySpotIndexOfReference(const mcon::Vectord& reference, const double threshold)
{
    int startIndex = 0;
    int endIndex = 0;
    GetEnergySpotIndex(&startIndex, &endIndex, reference, threshold);
    StartEnd startEnd =
    {
        startIndex,
        endIndex
    };
    return startEnd;
}

int FindLocalMinimum(const mcon::Vectord& v)
{
    // �l���ŏ��ƂȂ�ɏ��l��T��
    double minimum = v.GetMaximum();
    int index = -1;
    for (int k = 0; k < v.GetLength(); ++k)
    {
        if (v[k]==0)
        {
            continue;
        }
        LOG("%d,%g\n", k, log10(v[k]));
        if (0 == k || k == v.GetLength() - 1)
        {
            continue;
        }
        // �J�ł���A���]���l���ŏ��ƂȂ� Js ��I�ԁB
        if (v[k - 1] > v[k] && v[k] < v[k + 1])
        {
            if (minimum > v[k])
            {
                index = k;
                minimum = v[k];
            }
        }
    }
    return index;
}

// ���@
//   - ���͐M���ƎQ�ƐM���̈ʒu�֌W�� (���炵���) �덷���قȂ�B
//
// �v��
//   - �ł������ȒP�Ȍv�Z�ōŏ��덷�̏ꏊ����肵�����B

int Optimizer(
    int* pReferenceOffset,
    const mcon::Matrixd& input,
    const mcon::Vectord& reference,
    int tapps
)
{
    const mcon::Matrixd W;
    const int split = 16; // ������

    // ���͐M���̃G�l���M�������̈� (�J�n�_�ƏI���_) ���擾����B
    // �g���ĂȂ���...
    const StartEnd seInput = GetEnergySpotIndexOfInput(input, 0.999);
    DEBUG_LOG("Search: Start=%d, End=%d\n", seInput.start, seInput.end);
    UNUSED(seInput);

    // TBD:
    //   ���X�̃`�F�b�N�����{����K�v������ (��ɒ����Ɋւ���`�F�b�N)�B
    // �w��^�b�v���Ɠ��� or 1/2 (�v�Z�ʍ팸)
    //   1. ���炷�Ɛ��x�������A��10�T���v�����ꂽ��F���������B
    //   2. �������炷�Ȃ� 1/2�A���T���v�����x�̂��ꂾ�����B
    const int M = seInput.end - seInput.start + 1; // or tapps or tapps / 2;
    // �^�b�v����2�{���M�������A�Z����
    //const int N = std::min(M * 2, input.GetColumnLength());
    // 1. N == M �ɂ���Ƃ��܂������Ȃ�...
    // 2. N == 1.1 M ���炢�Ȃ�܂����܂��������AN == 1.05 M �ł̓_���B
    //    ���̂������ł��Ă��Ȃ��̂��c�O
    // �Ƃ肠�����A�^�b�v����1.5 �{�ɂ��Ă���
    const int N = std::min(static_cast<int>(M * 1.5), input.GetColumnLength());

    const StartEnd seReference = GetEnergySpotIndexOfReference(reference, 0.99);
    const int widthReference = seReference.end - seReference.start + 1;
    UNUSED(widthReference);
    DEBUG_LOG("Reference: %d=%d (width=%d)\n", seReference.start, seReference.end, widthReference);
    int offset = ClampLower<int>(0, seReference.end - M + 1); // �{���J�n�C���f�b�N�X
    int range = M; // �{���͈�
    DEBUG_LOG("M=%d, N=%d\n", M, N);

    while (1)
    {
        const int step = ClampLower<int>(0, (range + split - 1) / split);
        //const int end = ( (seReference.start - offset) + step - 1 ) / step;
        DEBUG_LOG("offset=%d, range=%d, step=%d\n", offset, range, step);
        mcon::Vectord Js(split);

        Js = 0;

        for (int r = 0; r < input.GetRowLength(); ++r)
        {
            const mcon::Vectord _signal(input[r]);
            const mcon::Vectord signal = _signal(0, N);
            mcon::Matrixd Ut(M, N);
            DEBUG_LOG("Ut: (%d, %d)\n", Ut.GetRowLength(), Ut.GetColumnLength());
            mcon::Matrixd inversed;
            DEBUG_LOG("r=%d: NormalEquationPre()\n", r);
            NormalEquationPre(inversed, Ut, signal, W);
            DEBUG_LOG("Inv: (%d, %d)\n", inversed.GetRowLength(), inversed.GetColumnLength());
            DEBUG_LOG("r=%d: Do, Optimizing ...\n", r);
            //for (int k = 0; k < N; k += step)
            for (int k = 0; k < split; ++k)
            {
                double J;
                mcon::Vector<double> h; // unused
                const mcon::Vectord d = reference(k * step + offset, N);
                ASSERT(d.GetLength() == N);
#if 0
            int startSpot = 0;
            int endSpot = 0;
            GetEnergySpotIndex(&startSpot, &endSpot, d, 0.99);
            DEBUG_LOG("Spot[%d]: Start=%d, End=%d, dlen=%d\n", k, startSpot, endSpot, d.GetLength());
#endif

                NormalEquationPost(h, inversed, Ut, d, W, &J);
                Js[ k ] += J;
            }
        }
        // �덷�z��ŋɏ��l���Ƃ�C���f�b�N�X���擾����B
        int lmIndex = FindLocalMinimum(Js);

        // ������Ȃ��ꍇ�͏�����ł��؂�~�߂� (TBD)�B
        if (lmIndex < 0)
        {
            break;
        }
        // Reference �M����̃C���f�b�N�X�ɒ����B
        lmIndex *= step;
        lmIndex += offset;
        LOG("    Found: index=%d\n", lmIndex);

        // Step �� 1 �Ȃ�I������B
        if (1 == step)
        {
            *pReferenceOffset = lmIndex;
            return 0;
        }
        // �����łȂ���Εϐ����X�V���Ď��̃X�e�[�W��
        // �ɏ��l������������ԂƂ��̑O��A���v3 �̋�Ԃ�Ώۂɂ���B
        // ��������ƁA�m���Ɏ����ɏ��l��������B
        // �Ȃ̂�...
        offset = ClampLower(0, lmIndex - step + 1); // �I�t�Z�b�g��1 ��ԑO�̐擪�ɒu��
        range = 3 * step; // �͈͂�3 ���
    }
    *pReferenceOffset = 0;
    return 0;
}

}

status_t PreProcess(ProgramParameter* param)
{
    if (!param->optimize)
    {
        return NO_ERROR;
    }
    return Optimizer(
        &param->referenceOffset,
        param->inputSignal,
        param->referenceSignal,
        param->tapps);
}
