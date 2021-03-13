cmake_minimum_required(VERSION 3.6)

project(TinyRender)

set(ASSETS_PATH ${PROJECT_SOURCE_DIR}/assets)

# config file
configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_SOURCE_DIR}/config.h
)

set(CMAKE_CXX_STANDARD 14)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)

add_library(tinyRender 
    src/TinyRender.cpp
    src/TGAImage.cpp
    src/Model.cpp
    src/Geometry.cpp
    src/Color.cpp
    src/Bitmap.cpp
    src/device/BitmapDevice.cpp
    src/device/BitmapDevice.h
    src/device/Device.cpp
    include/TinyRender.h
    include/TGAImage.h
    include/Model.h
    include/Geometry.h
    include/Color.h
    include/Bitmap.h
    include/Device.h
    )

target_include_directories(tinyRender PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

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

add_executable(ModelTexture example/ModelTexture.cpp)
target_link_libraries(ModelTexture tinyRender)

add_executable(ViewMatrix example/ViewMatrix.cpp)
target_link_libraries(ViewMatrix tinyRender)

add_executable(PerspectiveMatrix example/PerspectiveMatrix.cpp)
target_link_libraries(PerspectiveMatrix tinyRender)