#!/usr/bin/env bash
set -euo pipefail

ROOT="$(cd "$(dirname "${BASH_SOURCE[0]}")/.." && pwd)"
DEST="$ROOT/assets/planet-textures"
BASE="https://www.solarsystemscope.com/textures/download"
mkdir -p "$DEST"

files=(
  2k_sun.jpg
  2k_mercury.jpg
  2k_venus_surface.jpg
  2k_earth_daymap.jpg
  2k_mars.jpg
  2k_jupiter.jpg
  2k_saturn.jpg
  2k_uranus.jpg
  2k_neptune.jpg
)

for file in "${files[@]}"; do
  if [[ ! -s "$DEST/$file" ]]; then
    echo "Downloading $file"
    curl --fail --location --retry 3 "$BASE/$file" -o "$DEST/$file"
  else
    echo "Keeping existing $file"
  fi
done

echo "Planet texture maps installed in $DEST"
