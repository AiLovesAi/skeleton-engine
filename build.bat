:: Compile shaders
for %%F in (shaders/*) do (
  glslc "shaders\%%F" -o "assets\shaders\%%F.spv"
)

:: Compile binary resources for icon and info
windres resources.rc -o resources.o

:: Compile code and link libraries (-mwindows for no console)
g++^
 -Wall -Wno-volatile -Ofast -Os -m64 -std=c++23^
 -mwindows -static-libgcc -static-libstdc++^
 -DNDEBUG^
 src/*.cpp src/util/*.cpp src/graphics/systems/*.cpp src/graphics/window/*.cpp src/graphics/vulkan/*.cpp src/graphics/game/*.cpp src/game/sides/*.cpp src/game/world/*.cpp src/game/states/*.cpp src/game/components/*.cpp src/input/*.cpp src/sound/*.cpp^
 -o out.exe resources.o^
 -I lib/include -L lib/^
 -Wl,--export-all-symbol^
 -Wl,-Bstatic -lstdc++ -lpthread -l:libglfw3.a -l:vulkan-1.lib^
 -Wl,-Bdynamic -lgdi32 -lversion -lsoft_oal -lsndfile

PAUSE
