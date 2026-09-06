cmake_minimum_required(VERSION 4.3)

set(CMAKE_EXPERIMENTAL_CXX_IMPORT_STD "451f2fe2-a8a2-47c3-bc32-94786d8fc91b")

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -stdlib=libc++")

project(raytracer CXX)

set(CMAKE_CXX_STANDARD 26)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_CXX_MODULE_STD ON)

add_executable(raytracer src/main.cpp)

target_sources(raytracer
        PUBLIC
        FILE_SET CXX_MODULES
        FILES
        src/ppm.ixx
        src/ray.ixx
        src/vec.ixx
)

target_link_options(raytracer PRIVATE -stdlib=libc++)
