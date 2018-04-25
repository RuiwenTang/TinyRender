cmake_minimum_required(VERSION 3.6)

project(TinyRender)

set(ASSETS_PATH ${PROJECT_SOURCE_DIR}/assets)

# config file
configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_SOURCE_DIR}/config.h
)

set(CMAKE_CXX_STANDARD 11)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})

add_library(tinyRender 
    src/TinyRender.cpp
    src/TGAImage.cpp
    src/Model.cpp
    src/Geometry.cpp
    include/TinyRender.h
    include/TGAImage.h
    include/Model.h
    include/Geometry.h
    )


######## example #########
add_executable(Triangle example/Triangle.cpp)
target_link_libraries(Triangle tinyRender)

add_executable(EmptyFrame example/EmptyFrame.cpp)
target_link_libraries(EmptyFrame tinyRender)

add_executable(ColorTriangle example/ColorTriangle.cpp)
target_link_libraries(ColorTriangle tinyRender)

add_executable(Wireframe example/Wireframe.cpp)
target_link_libraries(Wireframe tinyRender)

add_executable(RandomColorModel example/RandomColorModel.cpp)
target_link_libraries(RandomColorModel tinyRender)

add_executable(LightAndFaceCut example/LightAndFaceCut.cpp)
target_link_libraries(LightAndFaceCut tinyRender)