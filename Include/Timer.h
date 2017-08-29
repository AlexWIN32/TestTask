/*******************************************************************************
    Author: Alexey Frolov (alexwin32@mail.ru)

    This software is distributed freely under the terms of the MIT License.
    See "LICENSE" or "http://copyfree.org/content/standard/licenses/mit/license.txt".
*******************************************************************************/

#pragma once
#include <windows.h>

namespace Time
{

class Timer final
{
private:
    UINT frameCounter = 0, fps = 0;
    DOUBLE timeDelta = 0.0, optimalFrameTickTime = 0.0;
    FLOAT timeFactor = 0.0f, eplasedTime = 0.0f;
    LONGLONG prevTimeStamp = 0, ticksPerSecond = 0;
public:
    Timer(){}
    UINT GetFps() const {return fps;}
    DOUBLE GetTimeFactor() const {return timeFactor;}
    DOUBLE GetEplasedTime() const {return eplasedTime;}
    void Init(INT OptimalFps);
    void Invalidate();
};

}