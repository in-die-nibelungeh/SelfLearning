
#pragma once

#include <sys/time.h>

class BenchUtil
{
public:
    BenchUtil();
    ~BenchUtil();
    void Start(void);
    double Finish(void);
private:
    double m_Last;
    struct timeval m_Tv;
};

