#pragma once

#include <chrono>
#include <QDebug>


#define BENCHSTART( k )  Bench::ClockBegin();                       \
                    for( int i = 0; i< k; ++i ) {


#define BENCHEND(k)    } {double time = Bench::ClockEnd(); qDebug() << "Time : Total : " << time << " Unit : " << time / k; }


namespace Bench
{
    static  std::chrono::high_resolution_clock::time_point  sOriginClock;

    static void    ClockBegin()
    {
        sOriginClock = std::chrono::high_resolution_clock::now();
    }

    static double  ClockEnd()
    {
        std::chrono::duration< double > timeSpan = std::chrono::duration_cast<std::chrono::duration< double >>( std::chrono::high_resolution_clock::now() - sOriginClock );
        return  timeSpan.count();
    }
}
