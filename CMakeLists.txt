cmake_minimum_required(VERSION 2.8)
project( color_quantization )
find_package( OpenCV REQUIRED )
add_executable( color_quantization color_quantization.cpp )
target_link_libraries( color_quantization ${OpenCV_LIBS} )
