
#pragma once

#include <string>
#include "ppointers.h"

#if defined(WIN32) || defined(_WIN32)
#include <Windows.h>
#else
#include <time.h>
#include <sys/time.h>
#endif

namespace PMA_NS
{
    class PTimer : protected PPointers
    {
    public:
        PTimer(class PMA *);
        ~PTimer();

        void reset();
        double elapsed();
        void print_elapsed();
        std::string DateAndTime();

    private:
#if defined(WIN32) || defined(_WIN32)
        LARGE_INTEGER time_ref;
        LARGE_INTEGER frequency;
#else
        timeval time_ref;
#endif
    };
}

