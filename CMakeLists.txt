cmake_minimum_required(VERSION 3.28)
project(smbios)

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
)

add_executable(smbiosexe
  main.cpp
)

target_link_libraries(smbiosexe PRIVATE smbios)

install(TARGETS smbiosexe smbios)

install(FILES
  src/smbios.hpp
  src/bios_streambuf.hpp
  DESTINATION include
)
