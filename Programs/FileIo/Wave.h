#pragma once

#include <sys/types.h>

#include "types.h"
#include "status.h"
#include "Vector.h"

#define C4TOI(c0, c1, c2, c3) \
    ( ((c3 << 24) & 0xff000000) | \
      ((c2 << 16) & 0x00ff0000) | \
      ((c1 <<  8) & 0x0000ff00) | \
      ((c0 <<  0) & 0x000000ff) )

namespace mfio {

class Wave
{

public:

    enum WaveFormat
    {
        UNKNOWN = 0,
        LPCM = 1,
        IEEE_FLOAT = 3,
        PF_UNKNOWN = 0xffff
    };

    struct MetaData
    {
        WaveFormat format;
        int numChannels;
        int bitDepth;
        int samplingRate;
    };

    Wave();
    Wave(int samplingRate, int numChannels, int bitDepth);
    ~Wave();

    // Read
    status_t Read(const char* path, int16_t ** pBuffer, size_t* size);
    status_t Read(const char* path, mcon::Vector<int16_t>& buffer);

    // Write
    status_t Write(const char* path, int16_t* buffer, size_t size) const;
    status_t Write(const char* path, const mcon::Vector<int16_t>& buffer) const;

    // Meta Data
    const struct MetaData& GetMetaData(void) const;
    inline int GetSamplingRate(void) const
    {
        return m_MetaData.samplingRate;
    }
    inline int GetBitDepth(void) const
    {
        return m_MetaData.bitDepth;
    }
    inline int GetNumChannels(void) const
    {
        return m_MetaData.numChannels;
    }
    status_t GetMetaData(int* samplingRate, int* numChannels, int* bitDepth) const;
    status_t SetMetaData(struct MetaData& meta);
    status_t SetMetaData(int samplingRate, int numChannels, int bitDepth);
    double   GetDuration(void) const { m_Duration; }

    // TBD
    status_t Check(void);
private:
    enum ChunkID
    {
        CID_RIFF = C4TOI('R','I','F','F'),
        CID_FMT  = C4TOI('f','m','t',' '),
        CID_FACT = C4TOI('f','a','c','t'),
        CID_DATA = C4TOI('d','a','t','a')
    };

    void Initialize(int smaplingRate, int numChannels, int bitDepth, WaveFormat format);

    status_t ReadMetaData(FILE*& fd, int& dataPosition, size_t& dataSize);
    status_t WriteMetaData(FILE*& fd, size_t dataSize) const;

    struct MetaData m_MetaData;
    double  m_Duration;
};

} // namespace mfio
