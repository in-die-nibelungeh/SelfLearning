
#include "FileIo.h"
#include <stdio.h>

FileIo::FileIo()
  : 
    m_Format(DF_LPCM),
    m_SamplingRate(48000),
    m_NumChannels(1),
    m_BitDepth(16),
    m_Data(reinterpret_cast<void*>(0)),
    m_DataSize(0)
{
}

FileIo::~FileIo()
{
}

status_t FileIo::Test(void)
{
	if (8 != sizeof(WaveChunk))
	{
	}
	return NO_ERROR;
}

status_t FileIo::ReadHeader(const char* path)
{
	FILE* fd = fopen(path, "r");
	
	if (NULL == fd)
	{
		return -1; // NOT_FOUND
	}
	
	while(1)
	{
		WaveChunk chunk;
		
		fread(&chunk, sizeof(chunk), 1, fd);
		
		switch(chunk.m_Name.i)
		{
			CID_RIFF: 
			{
				char name[4];
				//fseek(fd, SEEK_CUR, 4);
				fread(name, sizeof(char), 4, fd);
				break;
			}
			CID_FMT: 
			{
				break;
			}
			CID_DATA: 
			{
				break;
			}
			CID_FACT: 
			{
				break;
			}
			default:
				break;
			
		}
	}
	
	fclose(fd);
	
	return NO_ERROR;
}

status_t FileIo::SetMetaData(s32 fs, s32 numChannels, s32 bitDepth)
{
	m_SamplingRate = fs;
	m_NumChannels = numChannels;
	m_BitDepth = bitDepth;
	return NO_ERROR;
}

#define ERROR_NULL_POINTER 0xe4

status_t FileIo::SetAudioData(void* buffer, size_t size)
{
	m_Data = buffer;
	m_DataSize = size;
	return NO_ERROR;
}

#define FileWrite(fd, type, v)             \
	{                                      \
		type tmp = v;                      \
		fwrite(&tmp, sizeof(type), 1, fd); \
	}

status_t FileIo::Write(const char* path)
{
	status_t ret = NO_ERROR;
	
	FILE *fd = fopen(path, "w");
	
	if (NULL == fd)
	{
		return -1;
	}
	
	// 'RIFF'
	size_t size = 4 + sizeof(WaveChunk) * 2 + 0x10 + m_DataSize;
	FileWrite(fd, s32, CID_RIFF);
	FileWrite(fd, s32, size);
	FileWrite(fd, s32, C4TOI('W', 'A', 'V', 'E'));
	
	// 'FMT '
	s32 BytesPerDt = m_BitDepth / 8;
	FileWrite(fd, s32, CID_FMT);
	FileWrite(fd, s32, 0x10);
	FileWrite(fd, s16, 0x1); // PCM
	FileWrite(fd, s16, m_NumChannels); 
	FileWrite(fd, s32, m_SamplingRate);
	FileWrite(fd, s32, m_SamplingRate * m_NumChannels * BytesPerDt);
	FileWrite(fd, s16, m_NumChannels * BytesPerDt); // BlockSize in Byte
	FileWrite(fd, s16, m_BitDepth);

	// 'DATA'
	FileWrite(fd, s32, CID_DATA);
	FileWrite(fd, s32, m_DataSize);
	
	if (NULL != m_Data)
	{
		fwrite(m_Data, 1, m_DataSize, fd);
	}
	else
	{
		ret = -ERROR_NULL_POINTER;
	}
	
	fclose(fd);
	
	return ret;
}

