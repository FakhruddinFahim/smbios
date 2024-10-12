cmake_minimum_required(VERSION 3.28)
project(smbios)

set(CMAKE_CXX_STANDARD 23)

if (WIN32)
  link_libraries(
    kernel32.lib
  )
endif ()


add_executable(smbios
  main.cpp
  src/smbios.cpp
  src/smbios.hpp
  src/bios_streambuf.cpp
  src/bios_streambuf.hpp
)
