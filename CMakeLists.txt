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
add_executable(Triangle example/Triangle.cpp)
target_link_libraries(Triangle tinyRender)

add_executable(EmptyFrame example/EmptyFrame.cpp)
target_link_libraries(EmptyFrame tinyRender)

add_executable(ColorTriangle example/ColorTriangle.cpp)
target_link_libraries(ColorTriangle tinyRender)