#!/bin/bash
set -euo pipefail
cd "$(dirname "$0")"
if [[ "$(uname -s)" != Darwin ]]; then
  echo 'Bu derleme macOS ve Apple Command Line Tools gerektirir.' >&2
  exit 1
fi
xcrun --find clang++ >/dev/null
sdk="$(xcrun --sdk macosx --show-sdk-path)"
mkdir -p build
# A fresh output directory avoids overwriting an installed or previously built bundle.
out="$(mktemp -d "$PWD/build/mac-universal.XXXXXX")"
bundle="$out/PulseSweep.bundle"
mkdir -p "$bundle/Contents/MacOS"
flags=(-std=c++17 -O2 -isysroot "$sdk" -mmacosx-version-min=11.0 -DGL_SILENCE_DEPRECATION -Ivendor/ffgl/lib)
sources=(src/PulseSweep.cpp vendor/ffgl/lib/ffgl/*.cpp vendor/ffgl/lib/ffglex/*.cpp)
xcrun clang++ "${flags[@]}" -arch arm64 -arch x86_64 -bundle \
  "${sources[@]}" -framework OpenGL -o "$bundle/Contents/MacOS/PulseSweep"
cat > "$bundle/Contents/Info.plist" <<'PLIST'
<?xml version="1.0" encoding="UTF-8"?>
<!DOCTYPE plist PUBLIC "-//Apple//DTD PLIST 1.0//EN" "http://www.apple.com/DTDs/PropertyList-1.0.dtd">
<plist version="1.0"><dict>
<key>CFBundleExecutable</key><string>PulseSweep</string>
<key>CFBundleIdentifier</key><string>local.pulsesweep.ffgl</string>
<key>CFBundleName</key><string>PulseSweep</string>
<key>CFBundlePackageType</key><string>BNDL</string>
<key>CFBundleInfoDictionaryVersion</key><string>6.0</string>
<key>CFBundleShortVersionString</key><string>1.1</string>
<key>CFBundleVersion</key><string>1.1.0</string>
<key>LSMinimumSystemVersion</key><string>11.0</string>
</dict></plist>
PLIST
plutil -lint "$bundle/Contents/Info.plist"
lipo -verify_arch arm64 x86_64 "$bundle/Contents/MacOS/PulseSweep"
codesign --force --sign - "$bundle"
codesign --verify --strict "$bundle"
xcrun clang++ "${flags[@]}" tests/engine_test.cpp -o "$out/engine_test"
"$out/engine_test"
xcrun clang++ "${flags[@]}" tests/bundle_test.cpp -framework OpenGL -o "$out/bundle_test"
"$out/bundle_test" "$bundle/Contents/MacOS/PulseSweep"
ditto -c -k --keepParent "$bundle" "$out/PulseSweep-1.1-macOS-universal.zip"
echo "Hazir: $bundle"
echo 'Son adim: Resolume icinde goruntu ve renk secicisini test et.'
