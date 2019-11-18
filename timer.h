/*
 * @file timer.h
 * @author m.shebanow
 * @date 11/09/2019
 * @brief model system timer.
 */
#pragma once
#ifndef _TIMER_H_
#define _TIMER_H_
#include <stdio.h>
#include <sys/time.h>
#include <string>

class Timer {
public:
    Timer() { timerValue = 0.0; }
    
    void start() { timerValue = getTime(); }                    ///< Starts a "stopwatch"
    void stop() { timerValue = getTime() - timerValue; }        ///< Stops a "stopwatch"
    inline double operator* () const { return timerValue; }     ///< Return stopwatch value as a double
    std::string operator() (const int prec = 3) const {         ///< Return timer value as a string; single int argument is the precision to use for prining floating point.
        char buffer[64];
        if (timerValue < 1.0e-3)
            sprintf(buffer, "%1.*f %s", prec, (float) timerValue * 1.0e6f, "usec");
        else if (timerValue < 1.0)
            sprintf(buffer, "%1.*f %s", prec, (float) timerValue * 1.0e3f, "msec");
        else 
            sprintf(buffer, "%1.*f %s", prec, (float) timerValue, "sec");
        return std::string(buffer);
    }

    static unsigned getSeed() {
        struct timeval tv;
        if (int err = gettimeofday(&tv, NULL))
            fprintf(stderr, "gettimeofday returned error %d\n", err);
        return (unsigned) tv.tv_usec;
    }
    
private:
    double timerValue;
    
    // routine to read time
    double getTime() const {
        struct timeval tv;
        if (int err = gettimeofday(&tv, NULL))
            fprintf(stderr, "gettimeofday returned error %d\n", err);
        return (double) tv.tv_sec + ((double) tv.tv_usec / 1.0e6);
    }
};

#endif /* _TIMER_H_ */