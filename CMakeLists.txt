cmake_minimum_required(VERSION 3.6)

project(TinyRender)


set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

add_library(tinyRender 
    src/TinyRender.cpp
    src/TGAImage.cpp
    src/Model.cpp
    include/TinyRender.h
    include/TGAImage.h
    include/Model.h
    include/Geometry.h
    )


######## example
add_executable(example1 example/example1.cpp)
target_link_libraries(example1 tinyRender)