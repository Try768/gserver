cmake_minimum_required(VERSION 3.20)

project(MyProject LANGUAGES CXX)

# Standar C++
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Ambil semua source dari src/
file(GLOB_RECURSE SOURCES
     CONFIGURE_DEPENDS
     src/*.cpp
)

add_executable(MyProject ${SOURCES})

# Include path → include/
target_include_directories(MyProject
    PRIVATE
        ${CMAKE_SOURCE_DIR}/include
)

# (Opsional) Folder output bin/
set_target_properties(MyProject PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/bin
)
