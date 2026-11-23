#include "RoadMask.generated.h"

#include "Config.h"
#include "DBCStores.h"
#include "Map.h"
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
constexpr uint32 SampleInterval = 250;
constexpr int CellsPerSide = RoadMask::TilesPerSide * RoadMask::CellsPerTile;
constexpr double CellSize = (1600.0 / 3.0) / RoadMask::CellsPerTile;

std::atomic<bool> Enabled{false};

struct RoadCheckTimer : DataMap::Base
{
    uint32 Remaining = 0;
};

bool IsCovered(uint32 map, float x, float y)
{
    int const row = static_cast<int>(std::floor(CellsPerSide / 2.0 - x / CellSize));
    int const column = static_cast<int>(std::floor(CellsPerSide / 2.0 - y / CellSize));
    if (row < 0 || row >= CellsPerSide || column < 0 || column >= CellsPerSide)
        return false;

    int const tileRow = row / RoadMask::CellsPerTile;
    int const tileColumn = column / RoadMask::CellsPerTile;
    uint32 const key = (map * RoadMask::TilesPerSide + tileRow) * RoadMask::TilesPerSide + tileColumn;
    auto const tile = std::lower_bound(std::begin(RoadMask::Tiles), std::end(RoadMask::Tiles), key,
        [](RoadMask::Tile const& entry, uint32 value) { return entry.Key < value; });
    if (tile == std::end(RoadMask::Tiles) || tile->Key != key)
        return false;

    int const cell = row % RoadMask::CellsPerTile * RoadMask::CellsPerTile + column % RoadMask::CellsPerTile;
    auto const end = RoadMask::Coverage + tile->End;
    auto const span = std::lower_bound(RoadMask::Coverage + tile->Begin, end, cell,
        [](RoadMask::Span const& entry, int value) { return entry.Last < value; });
    return span != end && span->First <= cell;
}

bool IsCityOrTown(uint32 areaId)
{
    constexpr uint32 flags = AREA_FLAG_CAPITAL | AREA_FLAG_CITY | AREA_FLAG_SLAVE_CAPITAL | AREA_FLAG_TOWN;
    AreaTableEntry const* area = sAreaTableStore.LookupEntry(areaId);
    return area && (area->flags & flags);
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
        if (!Enabled.load() || !player->IsInWorld() || player->GetMap()->Instanceable()
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

        bool const covered = IsCityOrTown(player->GetZoneId()) || IsCityOrTown(player->GetAreaId())
            || (IsCovered(player->GetMapId(), player->GetPositionX(), player->GetPositionY()) && player->IsOutdoors());
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
