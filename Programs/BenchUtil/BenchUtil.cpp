#include "BenchUtil.h"

BenchUtil::BenchUtil()
  : m_Last(0.0)
{
    gettimeofday(&m_Tv, NULL);
}

BenchUtil::~BenchUtil() {}

void BenchUtil::Start(void)
{
    gettimeofday(&m_Tv, NULL);
}

double BenchUtil::Finish(void)
{
    struct timeval end;

    gettimeofday(&end, NULL);

    m_Last =
        (end.tv_sec - m_Tv.tv_sec) +
        (end.tv_usec - m_Tv.tv_usec) / 1.0e+6;

   return m_Last;
}
