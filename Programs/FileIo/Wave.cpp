
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

Wave::Wave()
  : m_Duration(0.0)
{
    Initialize(0, 0, 0, UNKNOWN);
}

Wave::Wave(int fs, int ch, int bits)
  : m_Duration(0.0)
{
    Initialize(fs, ch, bits, LPCM);
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
        DEBUG_LOG("rb=%d, pos=%d, size=%d, eof=%d\n", rb, ftell(fd), chunk.size, feof(fd));

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

status_t Wave::Read(const char* path, mcon::Vector<int16_t>& buffer)
{
    FILE* fd = fopen(path, "r");

    if (NULL == fd)
    {
        return -ERROR_NOT_FOUND; // NOT_FOUND
    }

    size_t dataSize;
    int dataPosition;

    status_t ret = ReadMetaData(fd, dataPosition, dataSize);

    if (NO_ERROR == ret)
    {
        if (false == buffer.Resize(dataSize/sizeof(int16_t)))
        {
            ret = -ERROR_CANNOT_ALLOCATE_MEMORY;
        }
        else
        {
            fseek(fd, dataPosition, SEEK_SET);
            int ret = fread(buffer, sizeof(uint8_t), dataSize, fd);
        }
    }
    fclose(fd);

    return ret;
}


status_t Wave::Read(const char* path, int16_t** buffer, size_t* size)
{
    FILE* fd = fopen(path, "r");

    if (NULL == fd)
    {
        return -ERROR_NOT_FOUND; // NOT_FOUND
    }

    size_t dataSize;
    int dataPosition;

    status_t ret = ReadMetaData(fd, dataPosition, dataSize);

    if (NO_ERROR == ret)
    {
        *buffer = reinterpret_cast<int16_t*>(malloc(dataSize));
        *size = dataSize;
        if (*buffer == NULL)
        {
            ret = -ERROR_CANNOT_ALLOCATE_MEMORY;
        }
        else
        {
            fseek(fd, dataPosition, SEEK_SET);
            fread(*buffer, sizeof(uint8_t), dataSize, fd);
        }
    }
    fclose(fd);

    return ret;
}

status_t Wave::SetMetaData(int32_t fs, int32_t ch, int32_t depth)
{
    m_MetaData.samplingRate = fs;
    m_MetaData.numChannels = ch;
    m_MetaData.bitDepth = depth;
    return NO_ERROR;
}

status_t Wave::GetMetaData(int32_t* fs, int32_t* ch, int32_t* depth) const
{
    *fs = GetSamplingRate();
    *ch = GetNumChannels();
    *depth = GetBitDepth();
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
    //const int format = GetFormat();
    int BytesPerDt = bits / 8;
    FileWrite(fd, int32_t, CID_FMT);
    FileWrite(fd, int32_t, 0x10);
    FileWrite(fd, int16_t, LPCM); // PCM
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

status_t Wave::Write(const char* path, int16_t* buffer, size_t size) const
{
    if (NULL == buffer)
    {
        return -ERROR_NULL;
    }

    FILE *fd = fopen(path, "w");
    if (NULL == fd)
    {
        return -ERROR_ILLEGAL_PERMISSION;
    }
    WriteMetaData(fd, size);

    fwrite(buffer, 1, size, fd);

    fclose(fd);

    return NO_ERROR;
}

status_t Wave::Write(const char* path, const mcon::Vector<int16_t>& buffer) const
{
    if (NULL == buffer)
    {
        return -ERROR_NULL;
    }

    FILE *fd = fopen(path, "w");
    if (NULL == fd)
    {
        return -ERROR_ILLEGAL_PERMISSION;
    }
    size_t size = buffer.GetLength() * sizeof(int16_t);

    WriteMetaData(fd, size);

    fwrite(buffer, 1, size, fd);

    fclose(fd);

    return NO_ERROR;
}

} // namespace mfio {
