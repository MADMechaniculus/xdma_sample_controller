cmake_minimum_required(VERSION 3.14)

project(xdma_sample_controller LANGUAGES CXX)

set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

add_library(xdma_sample_controller SHARED
  xdma_sample_controller_global.h
  xdma_sample_controller.cpp
  xdma_sample_controller.h
  xdma_proxy.h
  xdma_proxy.cpp
  xdma_memorybase.h
  xdma_memorybase.cpp
  axi_register.h
  axi_register.cpp
)

target_compile_definitions(xdma_sample_controller PRIVATE XMDA_SAMPLE_CONTROLLER_LIBRARY)
