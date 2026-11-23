# Teldrassil road speed pilot

## Scope

Give players a 25% forward running-speed bonus while outdoors, on foot, and out of combat along the Shadowglen–Dolanaar route in Teldrassil. Roads, trails, and paved areas along the route are eligible. The module runs on AzerothCore 3.3.5a and was tested with the Playerbots fork. Players can use an unmodified 3.3.5a client.

## What we know

- The pilot was tested against Playerbots core revision `be923cbc45c3f1fab657f91d51fda999f6b81cc5`. It exposes `PlayerScript::OnPlayerUpdate`, and its movement code handles running-speed auras.
- AzerothCore's extracted terrain maps store area, height, liquids, and holes. Leeroy derives surface coverage from 3.3.5a client terrain and certain bridge models.
- A clean 3.3.5a client produced 220 Kalimdor coverage tiles. A strict cutoff left gaps along the main road. The pilot uses a cutoff of 32 out of 255, which connected the main approach to Dolanaar in an offline coverage check. Complete route continuity remains an in-game check.
- Leeroy writes 128 × 128 coverage bytes per terrain tile, about 4.2 yards per cell. The lookup needs to use the extractor's map, row, and column conventions across tile boundaries.

## Implementation and verification

1. **Validate the route data.** Regenerate coverage from the local clean client with the pinned Leeroy revision. Overlay the Shadowglen–Dolanaar subset on the minimap and verify alignment against in-game coordinates. Walk the main route and nearby trails and paving. Record any corrections needed where the surface mask misses a visible stretch or crosses a wall, hillside, or disconnected surface. Verify the corrected route in both directions before using it for the buff. Keep the extraction instructions and hand-authored corrections in Git. Store the client archives, extracted art, and generated local data outside Git.
2. **Build a small server module.** Use the Playerbots fork's player-update hook for a periodic position and eligibility check. Apply or remove a dedicated server-side aura only when the result changes. Use the core's normal speed calculation so a stronger run-speed buff can take precedence. Exclude mounted, swimming, flying, dead, or in-combat characters. Verify that the aura can run with stock clients and clears on logout or map change. Add a config switch that defaults to off.
3. **Test with Playerbots before deployment.** In an isolated build against the core and module revisions under test, measure normal forward running speed and confirm a 1.25× ratio on the same road segment. Check both directions, endpoints, corrected gaps, trails and paving, stepping off and back on, and entering and leaving combat. Test mounting, swimming, flight, stronger speed buffs and slows, teleporting, relogging, and whether party bots keep pace. Spot-check ordinary terrain and an unrelated zone for baseline speed. Confirm disabling the feature clears the bonus.
4. **Roll out after validation.** Keep the current worldserver image and config for rollback. Coordinate a worldserver restart, confirm the realm is healthy afterward, and repeat a short in-game route check. If there is a regression, disable the feature or restore the prior image and config.

Dolanaar toward Darnassus is the next candidate after the first route is proven.

## Staged results

The module was tested on an isolated realm with a copy of the world database and
disposable test characters. The test client read the server's running-speed
packets after moving between checkpoints.

| Check | Observed running speed |
| --- | --- |
| Off road | 7.00 yards/sec |
| Shadowglen approach and Dolanaar road | 8.75 yards/sec |
| Dolanaar trail or paved checkpoint | 8.75 yards/sec |
| Back off road or in combat | 7.00 yards/sec |
| Sprint on road | 10.50 yards/sec |
| Ground mount on road | 11.20 yards/sec |
| After Sprint or dismount | 8.75 yards/sec |
| Road after disabling and reloading config | 7.00 yards/sec |

A test Playerbot joined the group and followed the leader to within 16.8
yards at the next path checkpoint. The test used selected locations and a
headless 3.3.5a client. A complete on-foot playthrough with the graphical
client and a regular party was outside this staged test.

## Expanded coverage

- Extract road tiles from the Eastern Kingdoms, Kalimdor, Outland, and
  Northrend. Keep map IDs in tile keys so tiles at the same coordinates on
  separate continents remain independent.
- Treat areas flagged as capitals, city subareas, cities, or towns as
  covered, including indoors. Preserve the outdoor requirement for
  texture-based coverage. Exclude instanced maps and keep the existing
  combat and movement restrictions.
- Verify road-mask boundaries and coverage in each continent. On a test
  realm, check an outdoor road, an off-road point, and city thresholds.
  Include a neutral city such as Shattrath or Dalaran.

The expanded generator packed 736 tiles from 796 extracted tiles. The packed
mask matched every source tile at the selected cutoff. Both module translation
units compiled with `-DNOPCH=1`, and the expanded worldserver image built.

The isolated realm reported 8.75 yards/sec at all ten capital checkpoints
and at Booty Bay and Dolanaar. An outdoor road checkpoint outside the original
pilot also reported 8.75 yards/sec. Nearby uncovered terrain reported 7.00
yards/sec before and after that checkpoint.

## Contribution checks

Use the standalone module layout. Compile with `-DNOPCH=1` and verify both
automatic SQL discovery and the manual import path on a disposable world
database before publishing to the module catalogue.

## References

- [Leeroy extractor at the revision used for the first run](https://github.com/pthm/leeroy/blob/e3d7b70c3e14e65fafc5fa02c6f8e7a8d8a46d90/azsdk/clientdata/extract.go)
- [Playerbots player hooks at the tested revision](https://github.com/mod-playerbots/azerothcore-wotlk/blob/be923cbc45c3f1fab657f91d51fda999f6b81cc5/src/server/game/Scripting/ScriptDefines/PlayerScript.h)
- [Playerbots movement speed calculation](https://github.com/mod-playerbots/azerothcore-wotlk/blob/be923cbc45c3f1fab657f91d51fda999f6b81cc5/src/server/game/Entities/Unit/Unit.cpp)
- [AzerothCore module guide](https://www.azerothcore.org/wiki/create-a-module)
