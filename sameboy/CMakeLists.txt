cmake_minimum_required(VERSION 2.8)

set (CMAKE_C_COMPILER "clang")
set (CMAKE_CXX_COMPILER "clang++")


# additional compiler flags
# add_definitions(-Werror -Wall -Wno-strict-aliasing -Wno-unknown-warning -Wno-unknown-warning-option -Wno-multichar -Wno-int-in-bool-context -std=gnu11 -D_GNU_SOURCE -DVERSION="0.12.1" -I. -D_USE_MATH_DEFINES -Wno-deprecated-declarations -g -MT)

add_definitions(-Werror -Wall -Wno-strict-aliasing -Wno-unknown-warning -Wno-unknown-warning-option -Wno-multichar -Wno-int-in-bool-context -std=gnu11 -D_GNU_SOURCE -DVERSION="0.12.1" -I. -D_USE_MATH_DEFINES -Wno-deprecated-declarations -DGB_INTERNAL -g)
# add_definitions(-std=gnu11 -D_GNU_SOURCE -DVERSION="0.12.1" -I. -D_USE_MATH_DEFINES -Wno-deprecated-declarations -DGB_INTERNAL -g)

INCLUDE_DIRECTORIES(
   ${PROJECT_SOURCE_DIR}
)

file(GLOB_RECURSE core Core/*.c)

project(sameboy)
add_library(${PROJECT_NAME} ${core})
target_include_directories(${PROJECT_NAME} PRIVATE ${PROJECT_SOURCE_DIR})
TARGET_LINK_LIBRARIES(${PROJECT_NAME} -lm)
