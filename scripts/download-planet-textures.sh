#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEST="$ROOT/assets/planet-textures"
BASE="https://www.solarsystemscope.com/textures/download"
mkdir -p "$DEST"

declare -A maps=(
  [sun.jpg]=2k_sun.jpg
  [mercury.jpg]=2k_mercury.jpg
  [venus.jpg]=2k_venus_surface.jpg
  [earth.jpg]=2k_earth_daymap.jpg
  [mars.jpg]=2k_mars.jpg
  [jupiter.jpg]=2k_jupiter.jpg
  [saturn.jpg]=2k_saturn.jpg
  [uranus.jpg]=2k_uranus.jpg
  [neptune.jpg]=2k_neptune.jpg
)

for target in "${!maps[@]}"; do
  source="${maps[$target]}"
  if [[ ! -s "$DEST/$target" ]]; then
    echo "Downloading $source as $target"
    curl --fail --location --retry 3 "$BASE/$source" -o "$DEST/$target"
  else
    echo "Keeping existing $target"
  fi
done

echo "Planet texture maps installed in $DEST"
