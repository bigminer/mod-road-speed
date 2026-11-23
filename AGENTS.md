# AGENTS.md

## Module conventions

- Follow the [AzerothCore module guide](https://www.azerothcore.org/wiki/create-a-module)
  and the [official skeleton](https://github.com/azerothcore/skeleton-module).
- Keep this repository installable as `modules/mod-road-speed`. Place C++
  in `src/`, distributed config templates in `conf/`, and world SQL in
  `data/sql/db-world/`.
- Keep the dedicated loader entry point `Addmod_road_speedScripts()` in
  `src/road_speed_loader.cpp`. Use existing AzerothCore hooks for module
  logic and keep core changes separate.
- Keep the distributed config disabled by default. Keep `.conf.dist` files
  LF-terminated and document any new settings in the README.
- Check for spell ID collisions before testing SQL. Use a disposable world
  database to verify SQL changes and document automatic and manual
  installation paths as applicable.

## Privacy and portability

- Keep all tracked files suitable for community distribution. Do not include
  the maintainer's name, account or character names, server or realm names,
  hostnames, IP addresses, personal domains, credentials, private deployment
  details, local paths, or values specific to any private installation.
  This applies to code, comments, docs, examples, test fixtures, config
  defaults, and build instructions.
- Use placeholders such as `<azerothcore>` and `<clean client>` in examples.
  Read deployment-specific values from local configuration or arguments.
  Public game identifiers and feature defaults may be included when
  needed for the module.
- Keep client archives, generated road tiles, the generated mask header,
  and local configuration outside Git. Check staged content for private
  or deployment-specific details before committing or publishing.

## Verification

- When changing road extraction or coverage, regenerate the mask locally
  and verify the output and lookup coordinates in the affected area.
- Before a module release, build with `-DNOPCH=1` on the target core and
  verify installation and the speed change on a test realm.
- Exercise the affected behavior and report what was actually tested.
  Obtain explicit authorization before changing a live realm.
