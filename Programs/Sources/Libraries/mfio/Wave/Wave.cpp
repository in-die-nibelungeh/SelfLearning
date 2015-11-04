
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "status.h"
#include "types.h"
#include "Wave.h"

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

/*
#if 8 != sizeof(WaveChunk)
#error Unexpected value: sizeof(WaveChunk)
#endif
*/


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

status_t Wave::ReadMetaData(FILE*& fd, int& pos, size_t& size)
{
    status_t ret = NO_ERROR;
    int32_t length = 0;

    while(1)
    {
        WaveChunk chunk = {0, 0};
        int rb = fread(&chunk, sizeof(chunk), 1, fd);
        DEBUG_LOG("rb=%d, pos=%ld, size=%d, eof=%d\n", rb, ftell(fd), chunk.size, feof(fd));

        if (feof(fd) || rb == 0)
        {
            break;
        }
        switch(chunk.name.i)
        {
            case CID_RIFF:
            {
                int32_t name;
                fread(&name, sizeof(name), 1, fd);
                if (name != C4TOI('W', 'A', 'V', 'E'))
                {
                    ERROR_LOG("Unknown: %08x\n", name);
                    //ret = ERROR_UNKNOWN;
                }
                break;
            }
            case CID_FMT:
            {
                m_MetaData.format = UNKNOWN;
                m_MetaData.numChannels = 0;
                m_MetaData.bitDepth = 0;

                fread(&m_MetaData.format      , sizeof(int16_t), 1, fd);
                fread(&m_MetaData.numChannels , sizeof(int16_t), 1, fd);
                fread(&m_MetaData.samplingRate, sizeof(int32_t), 1, fd);
                fseek(fd, sizeof(int32_t)+sizeof(int16_t), SEEK_CUR);
                fread(&m_MetaData.bitDepth    , sizeof(int16_t), 1, fd);
                fseek(fd, chunk.size-16, SEEK_CUR);
                break;
            }
            case CID_DATA:
            {
                pos = ftell(fd);
                size = chunk.size;
                fseek(fd, size, SEEK_CUR);
                break;
            }
            case CID_FACT:
            {
                fread(&length, sizeof(int32_t), 1, fd);
                fseek(fd, chunk.size-sizeof(int32_t), SEEK_CUR);
                if (chunk.size > 4)
                {
                    ERROR_LOG("Unexpected size of fact: size=%d\n", chunk.size);
                    ret = -ERROR_UNKNOWN;
                }
                break;
            }
            default:
            {
                char * c = chunk.name.c;
                LOG("Unexpeced chunk: %c%c%c%c\n", c[0], c[1], c[2], c[3]);
                fseek(fd, chunk.size, SEEK_CUR);
                // ret = ERROR_UNKNOWN;
                break;
            }
        }
    }
    if (length != 0)
    {
        m_Duration = (double)length / GetSamplingRate();
    }

    return ret;
}

status_t Wave::Read(const char* path, mcon::Vector<double>& buffer)
{
    FILE* fd = fopen(path, "r");
    if (NULL == fd)
    {
        return -ERROR_NOT_FOUND; // NOT_FOUND
    }

    size_t dataSize;
    int dataPosition;
    status_t status;

    status = ReadMetaData(fd, dataPosition, dataSize);
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

    fseek(fd, dataPosition, SEEK_SET);

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
        break;
    }
END:
    fclose(fd);
    return status;
}

status_t Wave::Read(const char* path, double** buffer, int* length)
{
    mcon::Vector<double> tmp;
    status_t status = Read(path, tmp);
    if (NO_ERROR != status)
    {
        return status;
    }

    const int _length = tmp.GetLength();
    const size_t _size =  _length * sizeof(double);
    *length = _length;
    *buffer = reinterpret_cast<double*>(malloc(_size));
    if (*buffer == NULL)
    {
        return -ERROR_CANNOT_ALLOCATE_MEMORY;
    }
    for (int i = 0; i < _length; ++i)
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
    const int ch = GetNumChannels();
    const int length = tmp.GetLength() / ch;
    buffer.Resize(ch, length);
    for (int i = 0; i < length; ++i)
    {
        for (int c = 0; c < ch; ++c)
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

#define FileWrite(fd, type, v)             \
    {                                      \
        type tmp = v;                      \
        fwrite(&tmp, sizeof(type), 1, fd); \
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

    FILE *fd = fopen(path, "w");
    if (NULL == fd)
    {
        return -ERROR_ILLEGAL_PERMISSION;
    }

    const int length = buffer.GetLength();
    const int bytes = GetBitDepth() / 8;
    const size_t size = bytes * length;

    WriteMetaData(fd, size);

    status_t status = NO_ERROR;

    switch(GetWaveFormat())
    {
    case LPCM:
        {
            switch(bytes)
            {
            case sizeof(int16_t):
                {
                    mcon::Vector<int16_t> tmp(buffer);
                    fwrite(tmp, bytes, length, fd);
                }
                break;
            case sizeof(int32_t):
                {
                    mcon::Vector<int32_t> tmp(buffer);
                    fwrite(tmp, bytes, length, fd);
                }
                break;
            default:
                {
                    status = -ERROR_ILLEGAL;
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
                    fwrite(tmp, bytes, length, fd);
                }
                break;
            default:
                {
                    status = -ERROR_ILLEGAL;
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
    for (int i = 0; i < tmp.GetLength(); ++i)
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
    const int ch = GetNumChannels();
    const int length = buffer.GetColumnLength();
    const int bits = GetBitDepth();
    UNUSED(bits);

    mcon::Vector<double> tmp(ch * length);

    for (int i = 0; i < length; ++i)
    {
        if (i < 20)
            LOG("%d,%f\n", i, buffer[0][i]);
        for (int c = 0; c < ch; ++c)
        {
            tmp[ch*i+c] = buffer[c][i];
        }
    }
    return Write(path, tmp);
}

} // namespace mfio {
