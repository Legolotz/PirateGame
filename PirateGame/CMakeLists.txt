cmake_minimum_required(VERSION 3.29)
project(PirateGame)

set(CMAKE_CXX_STANDARD 20)

add_executable(PirateGame main.cpp
)


set(CMAKE_PREFIX_PATH SFML "C:/Users/schwa/OneDrive/Desktop/vcpkg/vcpkg/installed/x64-mingw-dynamic/share/sfml")
find_package(SFML COMPONENTS #[[system]] audio graphics window CONFIG REQUIRED)
find_package(OpenGL REQUIRED)
#[[find_package(GLU REQUIRED)]]
target_link_libraries(PirateGame PRIVATE #[[sfml-system sfml-network]] sfml-audio sfml-graphics sfml-window OpenGL::GL
        #[[GLU::GLU]])
