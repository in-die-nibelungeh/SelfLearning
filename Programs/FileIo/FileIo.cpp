
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "debug.h"
#include "status.h"
#include "types.h"

#include "FileIo.h"

struct WaveChunk
{
    union
    {
        char c[4];
        int32_t i;
    } m_Name;
    uint32_t  m_Size;
};


FileIo::FileIo()
{
    FileIo(0, 0, 0);
}

FileIo::FileIo(int fs, int ch, int depth)
  : m_Format(PF_LPCM),
    m_SamplingRate(fs),
    m_NumChannels(ch),
    m_BitDepth(depth),
    m_Duration(0.0)
{}

FileIo::~FileIo()
{
}

status_t FileIo::Test(void)
{
    CHECK(8 == sizeof(WaveChunk));
    CHECK(4 == sizeof(int32_t));
    CHECK(2 == sizeof(int16_t));
    return NO_ERROR;
}

status_t FileIo::ReadMetaData(FILE*& fd, int& pos, size_t& size)
{
    status_t ret = NO_ERROR;
    int32_t length = 0;

    while(1)
    {
        WaveChunk chunk = {0, 0};
        int rb = fread(&chunk, sizeof(chunk), 1, fd);
        DEBUG_LOG("rb=%d, pos=%d, size=%d, eof=%d\n", rb, ftell(fd), chunk.m_Size, feof(fd));

        if (feof(fd) || rb == 0)
        {
            break;
        }
        switch(chunk.m_Name.i)
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
                fread(&m_Format      , sizeof(int16_t), 1, fd);
                fread(&m_NumChannels , sizeof(int16_t), 1, fd);
                fread(&m_SamplingRate, sizeof(int32_t), 1, fd);
                fseek(fd, sizeof(int32_t)+sizeof(int16_t), SEEK_CUR);
                fread(&m_BitDepth    , sizeof(int16_t), 1, fd);
                fseek(fd, chunk.m_Size-16, SEEK_CUR);
                break;
            }
            case CID_DATA:
            {
                pos = ftell(fd);
                size = chunk.m_Size;
                fseek(fd, size, SEEK_CUR);
                break;
            }
            case CID_FACT:
            {
                fread(&length, sizeof(int32_t), 1, fd);
                fseek(fd, chunk.m_Size-sizeof(int32_t), SEEK_CUR);
                if (chunk.m_Size > 4)
                {
                    ERROR_LOG("Unexpected size of fact: size=%d\n", chunk.m_Size);
                    ret = -ERROR_UNKNOWN;
                }
                break;
            }
            default:
            {
                char * c = chunk.m_Name.c;
                LOG("Unexpeced chunk: %c%c%c%c\n", c[0], c[1], c[2], c[3]);
                fseek(fd, chunk.m_Size, SEEK_CUR);
                // ret = ERROR_UNKNOWN;
                break;
            }
        }
    }
    if (length != 0)
    {
        m_Duration = (double)length / m_SamplingRate;
    }

    return ret;
}

status_t FileIo::Read(const char* path, Container::Vector<int16_t>& buffer)
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


status_t FileIo::Read(const char* path, int16_t** buffer, size_t* size)
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

status_t FileIo::SetMetaData(int32_t fs, int32_t ch, int32_t depth)
{
    m_SamplingRate = fs;
    m_NumChannels = ch;
    m_BitDepth = depth;
    return NO_ERROR;
}

status_t FileIo::GetMetaData(int32_t* fs, int32_t* ch, int32_t* depth) const
{
    *fs = m_SamplingRate;
    *ch = m_NumChannels;
    *depth = m_BitDepth;
    return NO_ERROR;
}

struct FileIo::MetaData FileIo::GetMetaData(void) const
{
    struct FileIo::MetaData metaData =
    {
        m_Format,
        m_NumChannels,
        m_BitDepth,
        m_SamplingRate
    };

    return metaData;
}

#define FileWrite(fd, type, v)             \
    {                                      \
        type tmp = v;                      \
        fwrite(&tmp, sizeof(type), 1, fd); \
    }

status_t FileIo::WriteMetaData(FILE*& fd, size_t size) const
{
    // 'RIFF'
    size_t riffSize = 4 + sizeof(WaveChunk) * 2 + 0x10 + size;
    FileWrite(fd, int32_t, CID_RIFF);
    FileWrite(fd, int32_t, riffSize);
    FileWrite(fd, int32_t, C4TOI('W', 'A', 'V', 'E'));

    // 'FMT '
    int32_t BytesPerDt = m_BitDepth / 8;
    FileWrite(fd, int32_t, CID_FMT);
    FileWrite(fd, int32_t, 0x10);
    FileWrite(fd, int16_t, PF_LPCM); // PCM
    FileWrite(fd, int16_t, m_NumChannels);
    FileWrite(fd, int32_t, m_SamplingRate);
    FileWrite(fd, int32_t, m_SamplingRate * m_NumChannels * BytesPerDt);
    FileWrite(fd, int16_t, m_NumChannels * BytesPerDt); // BlockSize in Byte
    FileWrite(fd, int16_t, m_BitDepth);

    // 'DATA'
    FileWrite(fd, int32_t, CID_DATA);
    FileWrite(fd, int32_t, size);

    return NO_ERROR;
}

status_t FileIo::Write(const char* path, int16_t* buffer, size_t size) const
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

status_t FileIo::Write(const char* path, const Container::Vector<int16_t>& buffer) const
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
    size_t size = buffer.GetNumOfData() * sizeof(int16_t);

    WriteMetaData(fd, size);

    fwrite(buffer, 1, size, fd);

    fclose(fd);

    return NO_ERROR;
}

