
#include "debug.h"
#include "status.h"
#include "FileIo.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

struct WaveChunk
{
	union
	{
		char c[4];
		s32 i;
	} m_Name;
	u32  m_Size;
};


FileIo::FileIo()
  : 
    m_Format(DF_LPCM),
    m_SamplingRate(48000),
    m_NumChannels(1),
    m_BitDepth(16),
    m_Data(NULL),
    m_DataSize(0),
    m_Allocated(false),
    m_Duration(0.0)
{
}

FileIo::~FileIo()
{
	if (true == m_Allocated &&
		NULL != m_Data)
	{
		free(m_Data);
	}
}

status_t FileIo::Test(void)
{
	CHECK(8 == sizeof(WaveChunk));
	CHECK(4 == sizeof(s32));
	CHECK(2 == sizeof(s16));
	return NO_ERROR;
}

status_t FileIo::Read(const char* path)
{
	FILE* fd = fopen(path, "r");
	
	if (NULL == fd)
	{
		return -ERROR_NOT_FOUND; // NOT_FOUND
	}
	
	status_t ret = NO_ERROR;
	s32 length = 0;
	
	while(1)
	{
		WaveChunk chunk = {0, 0};
		int rb = fread(&chunk, sizeof(chunk), 1, fd);
		//printf("rb=%d, pos=%d, size=%d, eof=%d\n", rb, ftell(fd), chunk.m_Size, feof(fd));
		
		if (feof(fd) || rb == 0)
		{
			break;
		}
		switch(chunk.m_Name.i)
		{
			case CID_RIFF: 
			{
				s32 name;
				fread(&name, sizeof(name), 1, fd);
				if (name != C4TOI('W', 'A', 'V', 'E'))
				{
					fprintf(stderr, "Unknown: %08x\n", name);
					//ret = ERROR_UNKNOWN;
				}
				break;
			}
			case CID_FMT: 
			{
				fread(&m_Format      , sizeof(s16), 1, fd);
				fread(&m_NumChannels , sizeof(s16), 1, fd);
				fread(&m_SamplingRate, sizeof(s32), 1, fd);
				fseek(fd, sizeof(s32)+sizeof(s16), SEEK_CUR);
				fread(&m_BitDepth    , sizeof(s16), 1, fd);
				fseek(fd, chunk.m_Size-16, SEEK_CUR);
				break;
			}
			case CID_DATA: 
			{
				m_Allocated = true;
				m_Data = malloc(chunk.m_Size);
				fread(m_Data, sizeof(u8), chunk.m_Size, fd);
				break;
			}
			case CID_FACT: 
			{
				fread(&length, sizeof(s32), 1, fd);
				fseek(fd, chunk.m_Size-sizeof(s32), SEEK_CUR);
				if (chunk.m_Size > 4)
				{
					fprintf(stderr, "Unexpected size of fact: size=%d\n", chunk.m_Size);
					ret = ERROR_UNKNOWN;
				}
				break;
			}
			default:
			{
				char * c = chunk.m_Name.c;
				printf("Unexpeced chunk: %c%c%c%c\n", c[0], c[1], c[2], c[3]);
				fseek(fd, chunk.m_Size, SEEK_CUR);
				// ret = ERROR_UNKNOWN;
				break;
			}
		}
	}
	if (length != 0)
	{
		m_Duration = (f64)length / m_SamplingRate;
	}
	fclose(fd);
	
	return ret;
}

status_t FileIo::SetMetaData(s32 fs, s32 numChannels, s32 bitDepth)
{
	m_SamplingRate = fs;
	m_NumChannels = numChannels;
	m_BitDepth = bitDepth;
	return NO_ERROR;
}

status_t FileIo::GetMetaData(s32* fs, s32* chnumChannels, s32* bitDepth) const
{
	*fs = m_SamplingRate;
	*chnumChannels = m_NumChannels;
	*bitDepth = m_BitDepth;
	return NO_ERROR;
}

status_t FileIo::GetAudioData(void* buffer) const 
{
	memcpy(buffer, m_Data, m_DataSize);
	return NO_ERROR;
}

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
		return -ERROR_NOT_FOUND;
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
	FileWrite(fd, s16, DF_LPCM); // PCM
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
		ret = -ERROR_NULL;
	}
	
	fclose(fd);
	
	return ret;
}

