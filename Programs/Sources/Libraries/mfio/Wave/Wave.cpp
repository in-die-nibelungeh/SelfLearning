/*
 * The MIT License (MIT)
 *
 * Copyright (c) 2015-2016 Ryosuke Kanata
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

#include <stdint.h>
#include <string.h>

#include "debug.h"
#include "status.h"
#include "mfio/Wave.h"

namespace mfio {

namespace {

    struct WaveChunk
    {
        union
        {
            char c[4];
            int32_t i;
        } name;
        uint32_t size;
    };

}

Wave::Wave()
  : m_Duration(0.0)
{
    Initialize(0, 0, 0, UNKNOWN);
}

Wave::Wave(int fs, int ch, int bits, WaveFormat format)
  : m_Duration(0.0)
{
    Initialize(fs, ch, bits, format);
}

Wave::~Wave()
{
}

void Wave::Initialize(int fs, int ch, int bits, WaveFormat format)
{
    m_MetaData.format = format;
    m_MetaData.numChannels  = ch;
    m_MetaData.bitDepth = bits;
    m_MetaData.samplingRate = fs;
}

status_t Wave::Check(void)
{
    CHECK(8 == sizeof(WaveChunk));
    CHECK(4 == sizeof(int32_t));
    CHECK(2 == sizeof(int16_t));
    return NO_ERROR;
}

#define FileRead(p, u, c, fd)                  \
    {                                          \
        size_t readBlock = fread(p, u, c, fd); \
        if ( readBlock != c )                  \
        {                                      \
            return -ERROR_FILE_READ;           \
        }                                      \
    }

#define FileSeek(fd, o, b)                     \
    {                                          \
        int rtn = fseek(fd, o, b);             \
        if ( rtn != NO_ERROR )                 \
        {                                      \
            return -ERROR_FILE_SEEK;           \
        }                                      \
    }

#define FileWrite(fd, type, v)                         \
    {                                                  \
        type tmp = v;                                  \
        size_t wb = fwrite(&tmp, sizeof(type), 1, fd); \
        if ( wb != 1 )                                 \
        {                                              \
            return -ERROR_FILE_WRITE;                  \
        }                                              \
    }

status_t Wave::ReadMetaData(FILE*& fd, int& pos, size_t& size)
{
    int32_t length = 0;

    while(1)
    {
        WaveChunk chunk = {0, 0};
        int readBlock = fread(&chunk, sizeof(chunk), 1, fd);
        DEBUG_LOG("rb=%d, pos=%ld, size=%d, eof=%d\n", readBlock, ftell(fd), chunk.size, feof(fd));

        if ( feof(fd) || readBlock == 0 )
        {
            break;
        }
        switch(chunk.name.i)
        {
            case CID_RIFF:
            {
                int32_t name;
                FileRead(&name, sizeof(name), 1, fd);
                if (name != C4TOI('W', 'A', 'V', 'E'))
                {
                    ERROR_LOG("Unknown: %08x\n", name);
                    return -ERROR_UNSUPPORTED;
                }
                break;
            }
            case CID_FMT:
            {
                m_MetaData.format = UNKNOWN;
                m_MetaData.numChannels = 0;
                m_MetaData.bitDepth = 0;

                FileRead(&m_MetaData.format      , sizeof(int16_t), 1, fd);
                FileRead(&m_MetaData.numChannels , sizeof(int16_t), 1, fd);
                FileRead(&m_MetaData.samplingRate, sizeof(int32_t), 1, fd);
                FileSeek(fd, sizeof(int32_t)+sizeof(int16_t), SEEK_CUR);
                FileRead(&m_MetaData.bitDepth    , sizeof(int16_t), 1, fd);
                FileSeek(fd, chunk.size-16, SEEK_CUR);
                break;
            }
            case CID_DATA:
            {
                pos = ftell(fd);
                size = chunk.size;
                FileSeek(fd, size, SEEK_CUR);
                break;
            }
            case CID_FACT:
            {
                FileRead(&length, sizeof(int32_t), 1, fd);
                FileSeek(fd, chunk.size-sizeof(int32_t), SEEK_CUR);
                if (chunk.size > 4)
                {
                    ERROR_LOG("Unexpected size of fact: size=%d\n", chunk.size);
                }
                break;
            }
            default:
            {
                const char* c = chunk.name.c;
                ERROR_LOG("Unsupported chunk: %c%c%c%c\n", c[0], c[1], c[2], c[3]);
                FileSeek(fd, chunk.size, SEEK_CUR);
                // ret = ERROR_UNKNOWN;
                break;
            }
        }
    }
    if ( m_MetaData.format == LPCM || m_MetaData.format == IEEE_FLOAT )
    {
        length = size / (m_MetaData.bitDepth / 8) / m_MetaData.numChannels;
    }
    if (length != 0)
    {
        m_Duration = (double)length / GetSamplingRate();
    }

    return NO_ERROR;
}

status_t Wave::Read(const char* path, mcon::Vector<double>& buffer)
{
    FILE* fd = fopen(path, "rb");
    if (NULL == fd)
    {
        return -ERROR_NOT_FOUND; // NOT_FOUND
    }

    size_t dataSize;
    int dataPosition;
    status_t status = ReadMetaData(fd, dataPosition, dataSize);
    if (NO_ERROR != status)
    {
        fclose(fd);
        return status;
    }
    const int bytes = GetBitDepth() / 8;
    const int length = dataSize / bytes;
    if (false == buffer.Resize(length))
    {
        status = -ERROR_CANNOT_ALLOCATE_MEMORY;
        goto END;
    }

    FileSeek(fd, dataPosition, SEEK_SET);

    status = -ERROR_ILLEGAL;
    switch(GetWaveFormat())
    {
    case LPCM:
        {
            mcon::Vector<int16_t> tmp(length);
            if (dataSize == fread(tmp, sizeof(uint8_t), dataSize, fd))
            {
                buffer = tmp;
                status = NO_ERROR;
            }
        }
        break;
    case IEEE_FLOAT:
        {
            mcon::Vector<float> tmp(length);
            if (dataSize == fread(tmp, sizeof(uint8_t), dataSize, fd))
            {
                buffer = tmp;
                status = NO_ERROR;
            }
        }
        break;
    default:
        {
            status = -ERROR_UNSUPPORTED;
        }
        break;
    }
END:
    fclose(fd);
    return status;
}

status_t Wave::Read(const char* path, double** buffer, int* pLength)
{
    mcon::Vector<double> tmp;
    status_t status = Read(path, tmp);
    if (NO_ERROR != status)
    {
        return status;
    }

    const size_t length = tmp.GetLength();
    const size_t _size =  length * sizeof(double);
    *pLength = length;
    *buffer = new double[_size];
    if (*buffer == NULL)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    for (size_t i = 0; i < length; ++i)
    {
        (*buffer)[i] = tmp[i];
    }
    return NO_ERROR;
}

status_t Wave::Read(const char* path, mcon::Matrix<double>& buffer)
{
    mcon::Vector<double> tmp;
    status_t status = Read(path, tmp);

    if (NO_ERROR != status)
    {
        return status;
    }
    const size_t ch = GetNumChannels();
    const size_t length = tmp.GetLength() / ch;
    if ( false == buffer.Resize(ch, length) )
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    for (size_t i = 0; i < length; ++i)
    {
        for (size_t c = 0; c < ch; ++c)
        {
            buffer[c][i] = tmp[i*ch+c];
        }
    }
    return NO_ERROR;
}

status_t Wave::SetMetaData(int fs, int ch, int depth, WaveFormat format)
{
    m_MetaData.samplingRate = fs;
    m_MetaData.numChannels = ch;
    m_MetaData.bitDepth = depth;
    m_MetaData.format = format;
    return NO_ERROR;
}

status_t Wave::GetMetaData(int* fs, int* ch, int* depth, int* format) const
{
    *fs = GetSamplingRate();
    *ch = GetNumChannels();
    *depth = GetBitDepth();
    *format = GetWaveFormat();
    return NO_ERROR;
}

const struct Wave::MetaData& Wave::GetMetaData(void) const
{
    return m_MetaData;
}

status_t Wave::WriteMetaData(FILE*& fd, size_t size) const
{
    // 'RIFF'
    size_t riffSize = 4 + sizeof(WaveChunk) * 2 + 0x10 + size;
    FileWrite(fd, int32_t, CID_RIFF);
    FileWrite(fd, int32_t, riffSize);
    FileWrite(fd, int32_t, C4TOI('W', 'A', 'V', 'E'));

    // 'FMT '
    const int fs = GetSamplingRate();
    const int bits = GetBitDepth();
    const int ch = GetNumChannels();
    const int format = GetWaveFormat();
    const int BytesPerDt = bits / 8;
    FileWrite(fd, int32_t, CID_FMT);
    FileWrite(fd, int32_t, 0x10);
    FileWrite(fd, int16_t, format);
    FileWrite(fd, int16_t, ch);
    FileWrite(fd, int32_t, fs);
    FileWrite(fd, int32_t, fs * ch * BytesPerDt);
    FileWrite(fd, int16_t, ch * BytesPerDt); // BlockSize in Byte
    FileWrite(fd, int16_t, bits);

    // 'DATA'
    FileWrite(fd, int32_t, CID_DATA);
    FileWrite(fd, int32_t, size);

    return NO_ERROR;
}

status_t Wave::IsValidMetaData(void) const
{
    if (GetNumChannels()  == 0 ||
        GetSamplingRate() == 0 ||
        GetBitDepth()     == 0 ||
        GetWaveFormat() == UNKNOWN)
    {
        return false;
    }
    return true;
}

status_t Wave::Write(const char* path, const mcon::Vector<double>& buffer) const
{
    if (false == IsValidMetaData())
    {
        return -ERROR_ILLEGAL;
    }
    if (0 == buffer.GetLength())
    {
        return -ERROR_NULL;
    }

    FILE *fd = fopen(path, "wb");
    if (NULL == fd)
    {
        return -ERROR_ILLEGAL_PERMISSION;
    }

    const size_t length = buffer.GetLength();
    const size_t bytes = GetBitDepth() / 8;
    const size_t size = bytes * length;

    status_t status = WriteMetaData(fd, size);
    if (NO_ERROR != status)
    {
        fclose(fd);
        return status;
    }

    switch(GetWaveFormat())
    {
    case LPCM:
        {
            switch(bytes)
            {
            case sizeof(int16_t):
                {
                    mcon::Vector<int16_t> tmp(buffer);
                    if ( length > fwrite(tmp, bytes, length, fd) )
                    {
                        return -ERROR_FILE_WRITE;
                    }
                }
                break;
            case sizeof(int32_t):
                {
                    mcon::Vector<int32_t> tmp(buffer);
                    if ( length > fwrite(tmp, bytes, length, fd) )
                    {
                        return -ERROR_FILE_WRITE;
                    }
                }
                break;
            default:
                {
                    status = -ERROR_UNSUPPORTED;
                }
                break;
            }
        }
        break;
    case IEEE_FLOAT:
        {
            switch(bytes)
            {
            case sizeof(float):
                {
                    mcon::Vector<float> tmp(buffer);
                    if ( length > fwrite(tmp, bytes, length, fd) )
                    {
                        return -ERROR_FILE_WRITE;
                    }
                }
                break;
            default:
                {
                    status = -ERROR_UNSUPPORTED;
                }
                break;
            }
        }
        break;
    default:
        {
            status = -ERROR_UNSUPPORTED;
        }
        break;
    }
    fclose(fd);

    return status;
}

status_t Wave::Write(const char* path, double* buffer, size_t size) const
{
    mcon::Vector<double> tmp(size/sizeof(double));
    for (size_t i = 0; i < tmp.GetLength(); ++i)
    {
        tmp[i] = buffer[i];
    }
    return Write(path, tmp);
}

status_t Wave::Write(const char* path, const mcon::Matrix<double>& buffer) const
{
    if (0 == buffer.GetColumnLength()
        || 0 == buffer.GetRowLength())
    {
        return -ERROR_NULL;
    }
    const size_t ch = GetNumChannels();
    const size_t length = buffer.GetColumnLength();
    const int bits = GetBitDepth();
    UNUSED(bits);

    mcon::Vector<double> tmp(ch * length);

    for (size_t i = 0; i < length; ++i)
    {
        for (size_t c = 0; c < ch; ++c)
        {
            tmp[ch*i+c] = buffer[c][i];
        }
    }
    return Write(path, tmp);
}

} // namespace mfio {
