cmake_minimum_required(VERSION 3.5)
project(esphome-hitachi-ac)

set(CMAKE_CXX_STANDARD 11)

# Include all source files in the hitachi_ac component.
file(GLOB_RECURSE SOURCES "components/hitachi_ac/*.cpp" "components/hitachi_ac/*.h")
add_library(${PROJECT_NAME} ${SOURCES})

# Make sure the compiler can find your header files.
target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/components/hitachi_ac)

