/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#include <Timer.h>

namespace Time
{

void Timer::Init(INT OptimalFps)
{
    QueryPerformanceFrequency(reinterpret_cast<LARGE_INTEGER*>(&ticksPerSecond));
    optimalFrameTickTime = (DOUBLE)(ticksPerSecond / OptimalFps);

    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&prevTimeStamp));
}

void Timer::Invalidate()
{
    frameCounter++;

    LONGLONG timestamp;
    QueryPerformanceCounter(reinterpret_cast<LARGE_INTEGER*>(&timestamp));

    LONGLONG eplTime = timestamp - prevTimeStamp;

    prevTimeStamp = timestamp;

    timeFactor = (FLOAT)((DOUBLE)eplTime / optimalFrameTickTime);
    eplasedTime = (FLOAT)((DOUBLE)eplTime / (DOUBLE)ticksPerSecond);

    timeDelta += eplasedTime;
    if(timeDelta >= 1.0){
        fps = frameCounter;
        timeDelta = 0.0;
        frameCounter = 0;
    }
}
}