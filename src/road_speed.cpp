#include "RoadMask.generated.h"

#include "Config.h"
#include "Player.h"
#include "PlayerScript.h"
#include "WorldScript.h"

#include <algorithm>
#include <atomic>
#include <cmath>
#include <iterator>

namespace
{
constexpr uint32 RoadSpeedSpell = 91025;
constexpr uint32 Kalimdor = 1;
constexpr uint32 Teldrassil = 141;
constexpr uint32 SampleInterval = 250;
constexpr double CellSize = (1600.0 / 3.0) / 128.0;

std::atomic<bool> Enabled{false};

struct RoadCheckTimer : DataMap::Base
{
    uint32 Remaining = 0;
};

bool IsCovered(float x, float y)
{
    int const row = static_cast<int>(std::floor(4096.0 - x / CellSize)) - RoadMask::OriginRow;
    int const column = static_cast<int>(std::floor(4096.0 - y / CellSize)) - RoadMask::OriginColumn;
    if (row < 0 || row >= RoadMask::Rows || column < 0 || column >= RoadMask::Columns)
        return false;

    int const cell = row * RoadMask::Columns + column;
    auto const it = std::lower_bound(std::begin(RoadMask::Coverage), std::end(RoadMask::Coverage), cell,
        [](RoadMask::Span const& span, int value) { return span.Last < value; });
    return it != std::end(RoadMask::Coverage) && it->First <= cell;
}

class RoadSpeedConfig final : public WorldScript
{
public:
    RoadSpeedConfig() : WorldScript("RoadSpeedConfig", {WORLDHOOK_ON_AFTER_CONFIG_LOAD}) { }

    void OnAfterConfigLoad(bool /*reload*/) override
    {
        Enabled.store(sConfigMgr->GetOption<bool>("RoadSpeed.Enable", false));
    }
};

class RoadSpeedPlayer final : public PlayerScript
{
public:
    RoadSpeedPlayer() : PlayerScript("RoadSpeedPlayer", {PLAYERHOOK_ON_UPDATE}) { }

    void OnPlayerUpdate(Player* player, uint32 elapsed) override
    {
        if (player->GetMapId() != Kalimdor || player->GetZoneId() != Teldrassil || !Enabled.load()
            || !player->IsAlive() || player->IsInCombat() || player->IsMounted() || player->IsInWater()
            || player->IsFlying() || player->IsInFlight() || player->GetVehicle() || player->IsBeingTeleported())
        {
            if (player->HasAura(RoadSpeedSpell))
                player->RemoveAura(RoadSpeedSpell);
            return;
        }

        auto* timer = player->CustomData.GetDefault<RoadCheckTimer>("RoadSpeed.CheckTimer");
        if (timer->Remaining > elapsed)
        {
            timer->Remaining -= elapsed;
            return;
        }
        timer->Remaining = SampleInterval;

        bool const covered = IsCovered(player->GetPositionX(), player->GetPositionY()) && player->IsOutdoors();
        if (covered && !player->HasAura(RoadSpeedSpell))
            player->AddAura(RoadSpeedSpell, player);
        else if (!covered && player->HasAura(RoadSpeedSpell))
            player->RemoveAura(RoadSpeedSpell);
    }
};
}

void AddRoadSpeedScripts()
{
    new RoadSpeedConfig();
    new RoadSpeedPlayer();
}
