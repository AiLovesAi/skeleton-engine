cmake_minimum_required(VERSION 3.24)

# set the project name
project(Minecraft CXX)

# get source files
file(GLOB_RECURSE CLIENT_FILES "src/client/*.cpp")
file(GLOB_RECURSE SERVER_FILES "src/server/*.cpp")
file(GLOB_RECURSE UTIL_FILES "src/util/*.cpp")
file(GLOB RESOURCE_FILE "resources.rc")

# add the executable
add_library(util SHARED "${UTIL_FILES}")
add_library(server SHARED "${SERVER_FILES}")
add_library(client SHARED "${CLIENT_FILES}")
add_executable(game "src/game.cpp")
add_executable(host "src/host.cpp")
add_executable(Minecraft "src/launcher.cpp")

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
	-mwindows
)

endif()

target_compile_options(client PRIVATE ${CMAKE_C_FLAGS})
target_link_options(client PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(server PRIVATE ${CMAKE_C_FLAGS})
target_link_options(server PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(util PRIVATE ${CMAKE_C_FLAGS})
target_link_options(util PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(game PRIVATE ${CMAKE_C_FLAGS})
target_link_options(game PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(host PRIVATE ${CMAKE_C_FLAGS})
target_compile_options(Minecraft PRIVATE ${CMAKE_C_FLAGS})
target_link_options(Minecraft PRIVATE ${CMAKE_L_FLAGS})

# set include paths
include_directories("lib/include/" "src/")
link_directories("lib/")
target_link_directories(client PRIVATE "lib/" "build/")
target_include_directories(server PRIVATE "lib/include/" "src/")
target_link_directories(server PRIVATE "lib/" "build/")
target_include_directories(util PRIVATE "lib/include/" "src/")
target_link_directories(util PRIVATE "lib/" "build/")
target_include_directories(game PRIVATE "lib/include/" "src/")
target_link_directories(game PRIVATE "lib/" "build/")
target_include_directories(host PRIVATE "lib/include/" "src/")
target_link_directories(host PRIVATE "lib/" "build/")
target_include_directories(Minecraft PRIVATE "lib/include/" "src/")
target_link_directories(Minecraft PRIVATE "lib/" "build/")

# link libraries
set(UTIL_LIBS
	-Wl,-Bdynamic -lgcc -lstdc++ -lpthread
)
set(SERVER_LIBS
	${UTIL_LIBS}
	-Wl,-Bdynamic -lutil
)
set(CLIENT_LIBS
	${SERVER_LIBS}
	-Wl,-Bstatic -l:vulkan-1.lib
	-Wl,-Bdynamic -lglfw3 -lsoft_oal -lsndfile -lserver
)
set(GAME_LIBS
	${UTIL_LIBS}
	-Wl,-Bdynamic -lutil -lclient
)
set(HOST_LIBS
	${SERVER_LIBS}
	-Wl,-Bdynamic -lserver
)
target_link_libraries(util PRIVATE ${UTIL_LIBS})
target_link_libraries(server PRIVATE ${SERVER_LIBS})
target_link_libraries(client PRIVATE ${CLIENT_LIBS})
target_link_libraries(game PRIVATE ${GAME_LIBS})
target_link_libraries(host PRIVATE ${HOST_LIBS})
target_link_libraries(Minecraft PRIVATE ${UTIL_LIBS})

# set resources
target_sources(Minecraft PRIVATE ${RESOURCE_FILE})
