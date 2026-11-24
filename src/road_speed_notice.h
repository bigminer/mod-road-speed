#pragma once

#include <cstdint>

struct RoadSpeedNotice
{
    bool Reported = false;
    std::uint32_t StableFor = 0;

    bool Update(bool active, std::uint32_t elapsed)
    {
        if (active == Reported)
        {
            StableFor = 0;
            return false;
        }

        if (elapsed < 2000 - StableFor)
        {
            StableFor += elapsed;
            return false;
        }

        Reported = active;
        StableFor = 0;
        return true;
    }
};
