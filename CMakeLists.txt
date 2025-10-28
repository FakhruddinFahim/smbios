cmake_minimum_required(VERSION 3.28)
project(smbios VERSION 2025.09.04)

set(CMAKE_CXX_STANDARD 23)

if (WIN32)
  link_libraries(
    kernel32.lib
  )
endif ()

add_library(smbios
  src/smbios.cpp
  src/smbios.hpp
  src/bios_streambuf.cpp
  src/bios_streambuf.hpp
  src/smbios_parser.cpp
  src/smbios_parser.hpp
)

add_executable(smbiosexe
  main.cpp
)

target_link_libraries(smbiosexe PRIVATE smbios)

install(TARGETS smbiosexe smbios)

install(FILES
  src/smbios_parser.hpp
  src/smbios.hpp
  src/bios_streambuf.hpp
  DESTINATION include
)
