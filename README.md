# Road Speed for AzerothCore

A server-side AzerothCore module that gives players **25% faster running speed**
on covered outdoor roads, trails, and paved areas in the Eastern Kingdoms,
Kalimdor, Outland, and Northrend. Cities and towns identified by the game's
area flags also qualify, including their indoor areas. The bonus applies
on foot and out of combat. Players can use their existing WoW 3.3.5a clients.
Players receive a private chat message when the bonus has been active or inactive
for two seconds, reducing repeated messages at road boundaries.

## Requirements

- A WoW 3.3.5a client installation to generate the road mask locally.
- Go and Python 3 to run the extraction tools.
- An AzerothCore Playerbots build environment. The pilot was built and tested
  against [Playerbots core revision be923cb](https://github.com/mod-playerbots/azerothcore-wotlk/tree/be923cbc45c3f1fab657f91d51fda999f6b81cc5).

## Generate the road mask

From the `azsdk` directory of [Leeroy at e3d7b70](https://github.com/pthm/leeroy/tree/e3d7b70c3e14e65fafc5fa02c6f8e7a8d8a46d90/azsdk),
run the Go command below. Then run the Python command from this repository.
Replace the paths in angle brackets with paths on your machine.

```text
go run "<this repo>/tools/extract_roads.go" -client "<clean client>" -out "<coverage output>"
python tools/build_mask.py "<coverage output>" src/RoadMask.generated.h
```

Leeroy derives road coverage from the game's terrain textures and bridge
models. The generator packs tiles from all four overworld maps and includes
cells with a score of at least 32 out of 255. Road coverage depends on how
the terrain was painted. Client archives, extracted coverage, and the
generated C++ header are local inputs.

## Install and enable

1. Place the project root at `<azerothcore>/modules/mod-road-speed`. Keep that
   directory name so the core finds the matching script loader. Generate the
   mask in that copy before configuring and building the worldserver.
2. Confirm spell ID `91025` is free in both the client spell data and the
   world database's `spell_dbc` table. AzerothCore's database updater discovers
   [the module SQL](data/sql/db-world/road_speed_spell.sql) under
   `data/sql/db-world/`. If the world database updater is disabled, import
   the SQL manually before starting the worldserver. The spell belongs in the
   existing world database.
3. Set `RoadSpeed.Enable = 1` in the installed module config
   `mod_road_speed.conf` and restart the worldserver. Use the
   [configuration template](conf/mod_road_speed.conf.dist) if you need a local
   copy. To disable the bonus, restore the default `0` and reload the server
   config.

## Verification

During the Teldrassil pilot, server packets showed **7.00 → 8.75 → 7.00 yards/sec**
as the test character went from off-road terrain to a covered path and back.
Entering combat suppressed the bonus. Sprint and mounting used their expected
speeds, and a Playerbot followed the leader to the next checkpoint.

See [PLAN.md](PLAN.md) for the pilot scope, test results, and rollout guidance.

## Contributing

See [AGENTS.md](AGENTS.md) for project conventions and privacy rules.

This module follows AzerothCore's standalone module layout. Its loader lives
in `src/`, its default config in `conf/`, and its world SQL in
`data/sql/db-world/`. It uses existing hooks. When checking a contribution,
include a worldserver build with `-DNOPCH=1` to catch missing includes.

See the [module guide](https://www.azerothcore.org/wiki/create-a-module) and
[skeleton module](https://github.com/azerothcore/skeleton-module). The
[catalogue](https://www.azerothcore.org/catalogue.html#/how-to) uses the
GitHub topic `azerothcore-module`.
