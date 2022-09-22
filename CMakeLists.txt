cmake_minimum_required(VERSION 3.24)

# set the project name
project(Minecraft CXX)

# get source files
file(GLOB_RECURSE SOURCE_FILES "src/*.cpp")
file(GLOB RESOURCE_FILE "resources.rc")

# add the executable
add_executable(Minecraft "${SOURCE_FILES}")

# specify the C++ standard
set(CMAKE_CXX_STANDARD 23)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# set compiler flags
if(CMAKE_BUILD_TYPE EQUAL DEBUG)

set(CMAKE_C_FLAGS
	-std=c++23
	-Wall -Werror -Wno-volatile
	-O0 -pipe -m64 -march=native
	-g -DDEBUG
)
set(CMAKE_L_FLAGS
	-Wl,--export-all-symbol
	-static-libgcc -static-libstdc++
)

else()

set(CMAKE_C_FLAGS
	-std=c++23
	-Wall -Werror -Wno-volatile
	-Ofast -Os -pipe -m64 -march=native
)
set(CMAKE_L_FLAGS
	-static-libgcc -static-libstdc++
	-mwindows
)

endif()

target_compile_options(Minecraft PRIVATE ${CMAKE_C_FLAGS})
target_link_options(Minecraft PRIVATE ${CMAKE_L_FLAGS})

# set include paths
target_include_directories(Minecraft PRIVATE "lib/include/")
target_link_directories(Minecraft PRIVATE "lib/")

# link libraries
set(LIBRARIES
	-Wl,-Bstatic -lstdc++ -lpthread -l:libglfw3.a -l:vulkan-1.lib
	-Wl,-Bdynamic -lgdi32 -lversion -lsoft_oal -lsndfile
)
target_link_libraries(Minecraft PRIVATE ${LIBRARIES})

# set resources
target_sources(Minecraft PRIVATE ${RESOURCE_FILE})
