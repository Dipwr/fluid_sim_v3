cmake_minimum_required(VERSION 3.20)
project(fluid_sim_v3 CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
        SFML
        GIT_REPOSITORY https://github.com/SFML/SFML.git
        GIT_TAG        3.0.0
)
FetchContent_MakeAvailable(SFML)

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