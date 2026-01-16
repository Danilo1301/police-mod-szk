@echo off
set NDK=C:\android-ndk-r27d

echo =========================
echo Building armeabi-v7a
echo =========================

cmake -S . -B build/armeabi-v7a -G Ninja ^
  -DCMAKE_TOOLCHAIN_FILE=%NDK%\build\cmake\android.toolchain.cmake ^
  -DANDROID_ABI=armeabi-v7a ^
  -DANDROID_PLATFORM=android-21 ^
  -DANDROID_STL=c++_static ^
  -DCMAKE_EXPORT_COMPILE_COMMANDS=ON

cmake --build build/armeabi-v7a --config Release

copy build\armeabi-v7a\compile_commands.json compile_commands.json >nul