cmake_minimum_required(VERSION 3.16.0)

project(mzingacpp VERSION 0.9.8)

add_executable(mzingacpp)

target_compile_definitions(mzingacpp PRIVATE VERSION_STR="${PROJECT_VERSION}")

target_include_directories(mzingacpp PRIVATE include)

target_sources(mzingacpp PRIVATE
    src/main.cpp
    src/Board.cpp
    src/Engine.cpp
    src/Enums.cpp
    src/Move.cpp
    src/Position.cpp
)

set_property(TARGET mzingacpp PROPERTY CXX_STANDARD 17)
