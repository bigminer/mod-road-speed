#include "../src/road_speed_notice.h"

#include <cassert>

int main()
{
    RoadSpeedNotice notice;
    assert(!notice.Update(false, 5000));
    assert(!notice.Update(true, 1000));
    assert(!notice.Update(false, 100));
    assert(!notice.Update(true, 1999));
    assert(notice.Update(true, 1));
    assert(!notice.Update(true, 5000));
    assert(!notice.Update(false, 1999));
    assert(notice.Update(false, 1));
    assert(!notice.Update(false, 5000));
}
