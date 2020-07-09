cmake_minimum_required(VERSION 3.6)

project(TinyRender)


set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)

add_library(tinyRender
        src/RenderBuffer.cpp
        src/TGAImage.cpp
        src/Model.cpp
        include/RenderBuffer.h
        include/TGAImage.h
        include/Model.h
        include/Geometry.h
        include/geometry/Barycentric.hpp
        include/Canvas.hpp
        include/Paint.hpp
        include/Color.hpp
        src/Color.cpp
        src/Canvas.cpp
        src/software/SoftwareCanvas.cpp
        src/software/SoftwareCanvas.hpp)

target_include_directories(tinyRender PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

######## example
add_executable(example1 example/example1.cpp)
target_link_libraries(example1 tinyRender)

add_executable(example2 example/example2.cpp)
target_link_libraries(example2 tinyRender)

add_executable(example3 example/example3.cpp)
target_link_libraries(example3 tinyRender)