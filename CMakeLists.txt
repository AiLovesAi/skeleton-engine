cmake_minimum_required(VERSION 3.24)

# set the project name
project(Minecraft CXX)

# get source files
file(GLOB_RECURSE CLIENT_FILES "src/client/*.cpp")
file(GLOB_RECURSE SERVER_FILES "src/server/*.cpp")
file(GLOB_RECURSE CORE_FILES "src/core/*.cpp")
file(GLOB RESOURCE_FILE "resources.rc")

# add the executable
add_library(core SHARED "${CORE_FILES}")
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

set(CMAKE_CPP_FLAGS
	-Wall -Werror -Wno-volatile
	-O0 -pipe -m64
	-Og -g -DDEBUG
	-std=c++23
)
set(CMAKE_L_FLAGS
	-Wl,--export-all-symbol
	-static-libgcc -static-libstdc++
)

else()

set(CMAKE_CPP_FLAGS
	-Wall -Werror -Wno-volatile
	-Ofast -pipe -m64
	-Os -fdata-sections -ffunction-sections
	-std=c++23
)
set(CMAKE_L_FLAGS
    -s -Wl,--gc-sections
)

if(WIN32)

set(CMAKE_L_FLAGS
    ${CMAKE_L_FLAGS}
	-mwindows
)	

endif()

endif()

target_compile_options(client PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(client PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(server PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(server PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(core PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(core PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(game PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(game PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(host PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(host PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(Minecraft PRIVATE ${CMAKE_CPP_FLAGS})
target_link_options(Minecraft PRIVATE ${CMAKE_L_FLAGS})

# set include paths
include_directories("lib/include/" "src/")
link_directories("lib/")
target_link_directories(client PRIVATE "lib/" "build/")
target_include_directories(server PRIVATE "lib/include/" "src/")
target_link_directories(server PRIVATE "lib/" "build/")
target_include_directories(core PRIVATE "lib/include/" "src/")
target_link_directories(core PRIVATE "lib/" "build/")
target_include_directories(game PRIVATE "lib/include/" "src/")
target_link_directories(game PRIVATE "lib/" "build/")
target_include_directories(host PRIVATE "lib/include/" "src/")
target_link_directories(host PRIVATE "lib/" "build/")
target_include_directories(Minecraft PRIVATE "lib/include/" "src/")
target_link_directories(Minecraft PRIVATE "lib/" "build/")

# link libraries
set(CORE_LIBS
	-Wl,-Bdynamic -lgcc -lstdc++ -lpthread
)
set(SERVER_LIBS
	${CORE_LIBS}
	-Wl,-Bdynamic -lcore
)
set(CLIENT_LIBS
	${SERVER_LIBS}
	-Wl,-Bstatic -l:vulkan-1.lib
	-Wl,-Bdynamic -lglfw3 -lsoft_oal -lsndfile -lserver
)
set(GAME_LIBS
	${CORE_LIBS}
	-Wl,-Bdynamic -lcore -lclient
)
set(HOST_LIBS
	${SERVER_LIBS}
	-Wl,-Bdynamic -lserver
)
target_link_libraries(core PRIVATE ${CORE_LIBS})
target_link_libraries(server PRIVATE ${SERVER_LIBS})
target_link_libraries(client PRIVATE ${CLIENT_LIBS})
target_link_libraries(game PRIVATE ${GAME_LIBS})
target_link_libraries(host PRIVATE ${HOST_LIBS})
target_link_libraries(Minecraft PRIVATE ${CORE_LIBS})

# set resources
target_sources(Minecraft PRIVATE ${RESOURCE_FILE})
