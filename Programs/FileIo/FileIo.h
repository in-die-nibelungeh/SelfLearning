#pragma once

#include <sys/types.h>

#include "types.h"
#include "status.h"
#include "Buffer.h"

#define C4TOI(c0, c1, c2, c3) \
    ( ((c3 << 24) & 0xff000000) | \
      ((c2 << 16) & 0x00ff0000) | \
      ((c1 <<  8) & 0x0000ff00) | \
      ((c0 <<  0) & 0x000000ff) )

class FileIo
{

public:

    enum PcmFormat
    {
        PF_LPCM = 1,
        PF_UNKNOWN = 0xffff
    };

    struct MetaData
    {
        PcmFormat format;
        int numChannels;
        int bitDepth;
        int samplingRate;
    };

    FileIo();
    FileIo(int samplingRate, int numChannels, int bitDepth);
    ~FileIo();

    // Read
    status_t Read(const char* path, int16_t ** pBuffer, size_t* size);
    status_t Read(const char* path, Container::Vector<int16_t>& buffer);

    // Write
    status_t Write(const char* path, int16_t* buffer, size_t size) const;
    status_t Write(const char* path, const Container::Vector<int16_t>& buffer) const;

    // Meta Data
    struct MetaData GetMetaData(void) const;
    status_t GetMetaData(int* samplingRate, int* numChannels, int* bitDepth) const;
    status_t SetMetaData(struct MetaData& meta);
    status_t SetMetaData(int samplingRate, int numChannels, int bitDepth);
    double   GetDuration(void) const { m_Duration; }

    // TBD
    status_t Test(void);
private:
    enum ChunkID
    {
        CID_RIFF = C4TOI('R','I','F','F'),
        CID_FMT  = C4TOI('f','m','t',' '),
        CID_FACT = C4TOI('f','a','c','t'),
        CID_DATA = C4TOI('d','a','t','a')
    };

    status_t ReadMetaData(FILE*& fd, int& dataPosition, size_t& dataSize);
    status_t WriteMetaData(FILE*& fd, size_t dataSize) const;

    PcmFormat m_Format;
    int32_t m_SamplingRate;
    int16_t m_NumChannels;
    int16_t m_BitDepth;

    double  m_Duration;
};
