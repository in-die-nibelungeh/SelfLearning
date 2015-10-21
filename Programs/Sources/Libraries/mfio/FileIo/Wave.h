#pragma once

#include <string>
#include <sys/types.h>

#include "types.h"
#include "status.h"
#include "Matrix.h"

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
    Wave(int samplingRate, int numChannels, int bitDepth, WaveFormat format = LPCM);
    ~Wave();

    // Read
    inline status_t Read(std::string path, double ** pBuffer, int* length)
    {
        return Read(path.c_str(), pBuffer, length);
    }
    inline status_t Read(std::string path, mcon::Vector<double>& buffer)
    {
        return Read(path.c_str(), buffer);
    }
    inline status_t Read(std::string path, mcon::Matrix<double>& buffer)
    {
        return Read(path.c_str(), buffer);
    }
    status_t Read(const char* path, double ** pBuffer, int* length);
    status_t Read(const char* path, mcon::Vector<double>& buffer);
    status_t Read(const char* path, mcon::Matrix<double>& buffer);

    // Write
    inline status_t Write(std::string path, double* buffer, size_t size) const
    {
        return Write(path.c_str(), buffer, size);
    }
    inline status_t Write(std::string path, const mcon::Vector<double>& buffer) const
    {
        return Write(path.c_str(), buffer);
    }
    inline status_t Write(std::string path, const mcon::Matrix<double>& buffer) const
    {
        return Write(path.c_str(), buffer);
    }

    status_t Write(const char* path, double* buffer, size_t size) const;
    status_t Write(const char* path, const mcon::Vector<double>& buffer) const;
    status_t Write(const char* path, const mcon::Matrix<double>& buffer) const;

    // Meta Data
    const struct MetaData& GetMetaData(void) const;
    inline int GetSamplingRate(void) const { return m_MetaData.samplingRate; }
    inline int GetBitDepth(void) const { return m_MetaData.bitDepth; }
    inline int GetNumChannels(void) const { return m_MetaData.numChannels; }
    inline WaveFormat GetWaveFormat(void) const { return m_MetaData.format; }
    inline void SetSamplingRate(int samplingRate) { m_MetaData.samplingRate = samplingRate; }
    inline void SetBitDepth(int bitDepth) { m_MetaData.bitDepth = bitDepth; }
    inline void SetNumChannels(int numChannels) { m_MetaData.numChannels = numChannels; }
    inline void SetWaveFormat(WaveFormat format) { m_MetaData.format = format; }
    status_t GetMetaData(int* samplingRate, int* numChannels, int* bitDepth, int* format) const;
    status_t SetMetaData(const struct MetaData& metaData);
    status_t SetMetaData(int samplingRate, int numChannels, int bitDepth, WaveFormat format = LPCM);
    double   GetDuration(void) const { return m_Duration; }

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

    status_t IsValidMetaData(void) const;
    status_t ReadMetaData(FILE*& fd, int& dataPosition, size_t& dataSize);
    status_t WriteMetaData(FILE*& fd, size_t dataSize) const;

    struct MetaData m_MetaData;
    double  m_Duration;
};

} // namespace mfio
