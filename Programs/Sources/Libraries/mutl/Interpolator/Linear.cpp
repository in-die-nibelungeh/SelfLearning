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

#define DEBUG_LOG(...)

#include "status.h"
#include "types.h"
#include "debug.h"
#include "Linear.h"

namespace mutl {
namespace interp {

status_t Linear::Interpolate(mcon::Vector<double>& output, const mcon::Vector<double>& input, int sampleCount)
{
    if ( sampleCount <= 0 )
    {
        return -ERROR_INVALID_ARGUMENT;
    }
    if ( false == output.Resize(sampleCount) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    const int N = input.GetLength();
    // �Ԋu�̐��͂��ꂼ��̒������� 1 �����������l�ɂȂ�B
    // ���̒l�ŃX�P�[������B
    const double step = static_cast<double>(N - 1) / (sampleCount - 1);
    DEBUG_LOG("step=%g\n", step);

    // ���[�̒l�̓��[�v�O�ő�����Ă����B
    // �I�[�̒l�̓��[�v�O�ŏ������悤�Ƃ���ƁA�͈͊O�A�N�Z�X�𐶂���̂Œ��ӁB
    output[0] = input[0];
    output[sampleCount - 1] = input[N - 1];
    for ( int k = 1; k < sampleCount - 1; ++k )
    {
        const double position = k * step;             // ���Z�����ʒu (����)
        const int index = static_cast<int>(position); // ���͔z��C���f�b�N�X (����)
        const double frac = position - index;         // ������
        DEBUG_LOG("k=%d, pos=%g, index=%d\n", k, position, index);
        output[k] = (input[index + 1] - input[index]) * frac + input[index];
    }

    return NO_ERROR;
}

} // namespace interp {
} // namespace mutl {
