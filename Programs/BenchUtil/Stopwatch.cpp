#include "Stopwatch.h"

namespace mbut {

Stopwatch::Stopwatch()
  : m_Last(0.0)
{
    gettimeofday(&m_Tv, NULL);
}

Stopwatch::~Stopwatch() {}

double Stopwatch::Push(void)
{
    struct timeval ts;

    gettimeofday(&ts, NULL);

    m_Last =
        (ts.tv_sec - m_Tv.tv_sec) +
        (ts.tv_usec - m_Tv.tv_usec) / 1.0e+6;

    m_Tv = ts;

    return m_Last;
}

double Stopwatch::GetRecord(void) const
{
   return m_Last;
}

} // namespace mbut {
