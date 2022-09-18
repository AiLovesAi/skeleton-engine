@echo off
set BUILD_TYPE="RELEASE"

:: Compile shaders
for %%F in (shaders/*) do (
  glslc "shaders\%%F" -o "assets\shaders\%%F.spv"
)

:: Compile code and link libraries (-mwindows for no console)
cmake ^
 -G "MinGW Makefiles" ^
 -DCMAKE_CXX_COMPILER=g++ ^
 -DCMAKE_BUILD_TYPE="%BUILD_TYPE%" ^
 .
mingw32-make

PAUSE
