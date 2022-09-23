cmake_minimum_required(VERSION 3.24)

# set the project name
project(Minecraft CXX)

# get source files
file(GLOB_RECURSE CLIENT_FILES "src/client/*.cpp")
file(GLOB_RECURSE SERVER_FILES "src/server/*.cpp")
file(GLOB EXE_FILES "src/*.cpp")
file(GLOB RESOURCE_FILE "resources.rc")

# add the executable
#add_library(Client SHARED "${CLIENT_FILES}")
add_library(Server SHARED "${SERVER_FILES}")
#add_executable(Minecraft "${EXE_FILES}")

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

#target_compile_options(Client PRIVATE ${CMAKE_C_FLAGS})
#target_link_options(Client PRIVATE ${CMAKE_L_FLAGS})
target_compile_options(Server PRIVATE ${CMAKE_C_FLAGS})
target_link_options(Server PRIVATE ${CMAKE_L_FLAGS})
#target_compile_options(Minecraft PRIVATE ${CMAKE_C_FLAGS})
#target_link_options(Minecraft PRIVATE ${CMAKE_L_FLAGS})

# set include paths
#target_include_directories(Client PRIVATE "lib/include/")
#target_link_directories(Client PRIVATE "lib/")
target_include_directories(Server PRIVATE "lib/include/")
target_link_directories(Server PRIVATE "lib/")
#target_include_directories(Minecraft PRIVATE "lib/include/")
#target_link_directories(Minecraft PRIVATE "lib/")

# link libraries
set(CLIENT_LIBS
	-Wl,-Bstatic -lstdc++ -lpthread -l:libglfw3.a -l:vulkan-1.lib
	-Wl,-Bdynamic -lsoft_oal -lsndfile
)
set(Server_LIBS
	-Wl,-Bstatic -lstdc++ -lpthread
)
#target_link_libraries(Client PRIVATE ${CLIENT_LIBS})
target_link_libraries(Server PRIVATE ${SERVER_LIBS})
#target_link_libraries(Minecraft PRIVATE ${CLIENT_LIBS})

# set resources
#target_sources(Minecraft PRIVATE ${RESOURCE_FILE})
