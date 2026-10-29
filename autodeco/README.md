# Auto Deco (Geode)

Select objects in the GD editor -> open pause menu -> press **Auto Deco**.
It scatters decoration around every selected object. Tweak everything in the mod's settings
(density, object IDs, scale range, rotation, no-touch).

## Build (easiest: GitHub Actions)
1. Create a GitHub repo and push this folder.
2. Go to Actions -> "Build Geode Mod" -> Run workflow.
3. Download the artifact, grab the `.geode` file, drop it in `Geometry Dash/geode/mods/`.

## Build locally
1. Install Geode SDK + CLI: https://docs.geode-sdk.org/getting-started/
2. `geode build`
3. The `.geode` file appears in `build/`.

Edit `id`/`developer` in `mod.json` before publishing.
