#pragma once

#include "types.h"

struct WaveChunk
{
	union
	{
		char c[4];
		s32 i;
	} m_Name;
	u32  m_Size;
};

#define C4TOI(c0, c1, c2, c3) ( ((c3 << 24) & 0xff000000) | ((c2 << 16) & 0x00ff0000) | ((c1 << 8) & 0x0000ff00) | (c0 & 0x000000ff))

class FileIo
{
	enum Format
	{
		DF_LPCM
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

	void* m_Data;
	size_t m_DataSize;
	
public:
	FileIo();
	~FileIo();
	
	// Read
	status_t ReadHeader(const char* path);
	status_t Read(const char* path);

	// Write
	status_t SetMetaData(s32 fs, s32 numChannels, s32 bitDepth);
	status_t SetAudioData(void* buffer, size_t size);
	status_t Write(const char* path);
	
	// TBD
	status_t Test(void);
};
