:: Remove old logs
del /s /q products\logs\*

:: Compile shaders
for %%F in (shaders/*) do (
  glslc "shaders\%%F" -o "products\assets\shaders\%%F.spv"
)

:: Compile binary resources for icon and info
windres resources.rc -o resources.o

:: Compile code and link libraries (-mwindows for no console)
g++^
 -Wall -Wno-volatile -Ofast -Os -m64 -std=c++23^
 -mwindows -static-libgcc -static-libstdc++^
 -DNDEBUG^
 src/*.cpp src/util/*.cpp src/graphics/*.cpp src/graphics/systems/*.cpp src/game/*.cpp src/input/*.cpp src/sound/*.cpp^
 -o products/out.exe resources.o^
 -I lib/include -L lib/^
 -Wl,--export-all-symbol^
 -Wl,-Bstatic -lstdc++ -lpthread -l:libglfw3.a -l:vulkan-1.lib^
 -Wl,-Bdynamic -lgdi32 -lversion -lsoft_oal -lsndfile

PAUSE
