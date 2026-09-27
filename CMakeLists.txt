cmake_minimum_required(VERSION 3.20)
project(fluid_sim_v3 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Search paths for macOS Homebrew & Linux DNF system libraries
list(APPEND CMAKE_PREFIX_PATH
        "/opt/homebrew"
        "/opt/homebrew/lib/cmake"
        "/usr/local"
        "/usr"
)

# Find SFML 3
find_package(SFML 3 COMPONENTS Graphics Window System REQUIRED)

include_directories(include)

add_executable(fluid_sim_v3
        src/main.cpp
        src/simulation.h
        src/simulation.cpp
)

target_link_libraries(fluid_sim_v3 PRIVATE
        SFML::Graphics
        SFML::Window
        SFML::System
)