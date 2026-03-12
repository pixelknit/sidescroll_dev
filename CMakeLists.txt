cmake_minimum_required(VERSION 3.11)
project(SideScroller)
set(CMAKE_CXX_STANDARD 17)

set(RAYLIB_DIR "C:/raylib/raylib")
set(RAYLIB_LIB_DIR "C:/raylib/raylib/build/raylib/Release")

include_directories(${RAYLIB_DIR}/src)
link_directories(${RAYLIB_LIB_DIR})

add_executable(game main.cpp)
target_link_libraries(game raylib opengl32 gdi32 winmm)