cmake_minimum_required(VERSION 3.29)
project(santagger_app_and_lib C)

set(CMAKE_C_STANDARD 11)

add_compile_definitions(HAVE_APP_VERSION_H=1)
add_compile_definitions(HAVE_LIB_VERSION_H=1)

add_library(santagger_includes INTERFACE)
add_subdirectory("src/includes")

add_library(santagger_lib SHARED)
target_include_directories(santagger_lib PRIVATE "${PROJECT_SOURCE_DIR}")
add_subdirectory("src/lib")

add_executable(santagger_app)
add_subdirectory("src/app")
target_link_libraries(santagger_app PRIVATE santagger_lib santagger_includes)

install(TARGETS santagger_lib santagger_includes FILE_SET HEADERS)
