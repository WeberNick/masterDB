#pragma once
//---------------------------------------------------------------------------
#include <sys/types.h>

#ifndef __WIN32__
#include <sys/resource.h>
#include <sys/time.h>

#else
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#endif

#include <ctime>

#include <iomanip>
#include <iostream>

//---------------------------------------------------------------------------
#define error_handle(x, y, z)

//---------------------------------------------------------------------------
#ifdef __svr4__
extern "C" {
int getrusage(int who, struct rusage* usage);
}
#endif
//---------------------------------------------------------------------------
class Measure {
  private:
    struct timeval startTime, stopTime;
#ifndef __WIN32__
    rusage result;
    long startCpuTime;
    long startCpuuTime;
    long stopCpuTime;
    long stopCpuuTime;
#else
    FILETIME startUserTime;
#endif
    bool started;

    time_t totalSecs, totalUSecs;
    long totalCpuTime;
    long totalCpuuTime;

  public:
    Measure() :
        startTime(), stopTime(),
#ifndef __WIN32__
        result(), startCpuTime(0), startCpuuTime(0), stopCpuTime(0), stopCpuuTime(0),
#endif
        started(false), totalSecs(0), totalUSecs(0), totalCpuTime(0), totalCpuuTime(0) {
    }

    void start() {
        if (started) {
          error_handle("Measure::start()", "", true);
        }
        totalCpuuTime = 0;
        cont();
    }

    void cont() {
        if (started) {
          error_handle("Measure::cont()", "", true);
        }
        gettimeofday(&startTime, 0);
#ifndef __WIN32__
        getrusage(RUSAGE_SELF, &result);
        startCpuTime = result.ru_utime.tv_sec + result.ru_stime.tv_sec;
        startCpuuTime = result.ru_utime.tv_usec + result.ru_stime.tv_usec;
#else
        FILETIME dummy;
        GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, &dummy, &startUserTime);
#endif
        started = true;
    }

    void stop() {
        if (!started) {
          error_handle("Measure::stop()", "", true);
        }
        started = false;
        gettimeofday(&stopTime, 0);
#ifndef __WIN32__
        getrusage(RUSAGE_SELF, &result);
        stopCpuTime = result.ru_utime.tv_sec + result.ru_stime.tv_sec;
        stopCpuuTime = result.ru_utime.tv_usec + result.ru_stime.tv_usec;
        totalCpuTime += stopCpuTime - startCpuTime;
        totalCpuuTime += stopCpuuTime - startCpuuTime;
#else
        FILETIME dummy, stopUserTime;
        GetProcessTimes(GetCurrentProcess(), &dummy, &dummy, &dummy, &stopUserTime);
        __int64 diff = *reinterpret_cast<__int64*>(&stopUserTime) - *reinterpret_cast<__int64*>(&startUserTime);
        totalCpuuTime += static_cast<int>((diff % 1000) * 100);
        totalCpuTime += static_cast<int>(diff / 1000);
#endif
        totalSecs = stopTime.tv_sec - startTime.tv_sec;
        totalUSecs = stopTime.tv_usec - startTime.tv_usec;

        if (totalCpuuTime < 0) {
            totalCpuuTime += 1000000;
            --totalCpuTime;
        }
        totalCpuTime += totalCpuuTime / 1000000;
        totalCpuuTime %= 1000000;
    }

    void end() {
      stop();
    }

    inline double mCpuTime() const { return (double)totalCpuTime + ((double)totalCpuuTime / 1000000); }
    inline double mTotalTime() const { return totalSecs + (double)(totalUSecs) / 1000000; }
    inline double mIoTime() const { return (mTotalTime() - mCpuTime()); }
    std::ostream& print(std::ostream& os) const {
        double cpuTime = (double)totalCpuTime + ((double)totalCpuuTime / 1000000);
        double outputTotal = totalSecs + (double)(totalUSecs) / 1000000;
        double ioTime = outputTotal - cpuTime;

        //     os << cpuTime <<endl;
        //     os << outputTotal <<endl;
        //     os << ioTime <<endl;

        os << "total : " << outputTotal;
        if (outputTotal) {
            std::ios::fmtflags currentFlags = os.flags();
            os.precision(2);
            os.flags(std::ios::fixed | std::ios::showpoint);
            os << " cpu : " << cpuTime << " (" << std::setw(5) << (cpuTime * 100) / outputTotal << "%)"
               << " io : " << ioTime << " (" << std::setw(5) << (ioTime * 100) / outputTotal << "%)";
            os.flags(currentFlags);
        }
        return os;
    }
};

inline std::ostream& operator<<(std::ostream& os, const Measure& me) {
  return me.print(os);
}
