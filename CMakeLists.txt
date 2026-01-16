cmake_minimum_required(VERSION 3.22)
project(PoliceModSZK LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include_directories(
    ${CMAKE_SOURCE_DIR}/include
    ${CMAKE_SOURCE_DIR}/aml
)

file(GLOB_RECURSE SRC_FILES
    src/*.cpp
    aml/*.cpp
)

add_library(PoliceModSZK SHARED ${SRC_FILES})

target_compile_options(PoliceModSZK PRIVATE
    -Wall
    -O2
    -fexceptions
    -frtti
)

target_compile_definitions(PoliceModSZK PRIVATE
    ANDROID
)

find_library(LOG_LIB log)

target_link_libraries(PoliceModSZK
    ${LOG_LIB}
)
