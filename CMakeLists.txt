cmake_minimum_required(VERSION 3.6)

project(TinyRender)

set(ASSETS_PATH ${PROJECT_SOURCE_DIR}/assets)

# config file
configure_file(
    ${PROJECT_SOURCE_DIR}/config.h.in
    ${PROJECT_SOURCE_DIR}/config.h
)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_EXPORT_COMPILE_COMMANDS TRUE)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)
include_directories(${CMAKE_CURRENT_SOURCE_DIR})
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third_party/glm)

add_library(tinyRender 
    src/DDALine.h
    src/DDALine.cpp
    src/TinyRender.cpp
    src/TGAImage.cpp
    src/Model.cpp
    src/Geometry.cpp
    src/Color.cpp
    src/Bitmap.cpp
    src/device/BitmapDevice.cpp
    src/device/BitmapDevice.h
    src/device/Device.cpp
    src/RasterizerCells.cpp
    src/SWRaster.cpp
    include/TinyRender.h
    include/TGAImage.h
    include/Model.h
    include/Geometry.h
    include/Color.h
    include/Bitmap.h
    include/Device.h
    include/RasterizerCells.h
    include/SWRaster.h
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

add_executable(rasterizer_cell_example example/rasterizer_cell_example.cpp)
target_link_libraries(rasterizer_cell_example tinyRender)

add_executable(sw_raster_example example/sw_raster_example.cpp)
target_link_libraries(sw_raster_example tinyRender)
