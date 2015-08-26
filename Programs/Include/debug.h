#ifndef _DEBUG_H_
#define _DEBUG_H_

#include <stdio.h>

#define LOG(...) printf(__VA_ARGS__)
#define DEBUG_LOG(...) printf(__VA_ARGS__)

#define CHECK(cond)                \
	{                              \
		char r[3] = "OK";          \
		if (!(cond))               \
		{                          \
			r[0] = 'N';            \
			r[1] = 'G';            \
		}                          \
		printf(#cond ": %s\n", r); \
	}

#endif // #ifndef _DEBUG_H_
