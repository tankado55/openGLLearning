#pragma once

namespace Timem
{
    extern double deltaTime;
    extern double lastFrame;

    class TimeUpdater
    {
    public:
        static void UpdateDeltaTime();
    };
}
