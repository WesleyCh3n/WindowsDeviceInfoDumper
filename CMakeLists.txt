cmake_minimum_required(VERSION 3.10)

project(GetHardwareID)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)
set(CMAKE_CXX_STANDARD 17)

add_executable(main
  main.cc
)
add_executable(devInfoDumper
  ./devInfoDumper.cc
)
