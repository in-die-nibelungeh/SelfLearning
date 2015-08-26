#pragma once

#include "types.h"

#define C4TOI(c0, c1, c2, c3) \
	( ((c3 << 24) & 0xff000000) | \
	  ((c2 << 16) & 0x00ff0000) | \
	  ((c1 <<  8) & 0x0000ff00) | \
	  ((c0 <<  0) & 0x000000ff) )

class FileIo
{
	enum Format
	{
		DF_LPCM = 1,
		DF_UNKNOWN = 0xffff
	};
	
	enum ChunkID
	{
		CID_RIFF = C4TOI('R','I','F','F'),
		CID_FMT  = C4TOI('f','m','t',' '),
		CID_FACT = C4TOI('f','a','c','t'),
		CID_DATA = C4TOI('d','a','t','a')
	};
	
	Format m_Format;
	
	s32 m_NumChannels;
	s32 m_BitDepth;
	s32 m_SamplingRate;
	f64 m_Duration;

	void*  m_Data;
	size_t m_DataSize;
	bool   m_Allocated;
public:
	FileIo();
	~FileIo();
	
	// Read
	//status_t ReadHeader(const char* path);
	status_t Read      (const char* path);
	status_t GetMetaData(s32* fs, s32* numChannels, s32* bitDepth) const ;
	status_t GetAudioDataSize(void) const { return m_DataSize; };
	status_t GetAudioData(void* buffer) const ;
	
	// Write
	status_t SetMetaData(s32 fs, s32 numChannels, s32 bitDepth);
	status_t SetAudioData(void* buffer, size_t size);
	status_t Write(const char* path);
	
	// TBD
	status_t Test(void);
};
