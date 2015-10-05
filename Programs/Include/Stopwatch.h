
#pragma once

#include <sys/time.h>

namespace mbut {

class Stopwatch
{
public:
    Stopwatch();
    ~Stopwatch();
    double Tick(void);
    double GetRecord(void) const;
private:
    double m_Last;
    struct timeval m_Tv;
};

} // namespace mbut {
