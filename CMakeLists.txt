cmake_minimum_required(VERSION 3.28)
project(smbios VERSION 2026.02.12)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 23)
endif ()

option(ADD_ABI_TAG "Add abi tag in library name" OFF)
set(DEBUG_POSTFIX d CACHE STRING "Debug library postfix.")

add_library(smbios
  src/smbios.cpp
  src/smbios.hpp
  src/bios_streambuf.cpp
  src/bios_streambuf.hpp
  src/smbios_parser.cpp
  src/smbios_parser.hpp
)

add_executable(smbios_exe
  main.cpp
)

if (WIN32)
  target_link_libraries(smbios PRIVATE
    kernel32.lib
  )
endif ()

target_link_libraries(smbios_exe PRIVATE smbios)

set(OUTPUT_NAME_POSTFIX "")
set(EXPORT_NAME ${PROJECT_NAME}_targets)
set(ABI_TAG "")

if (MSVC AND ADD_ABI_TAG)
  if(NOT CMAKE_MSVC_RUNTIME_LIBRARY MATCHES "DLL")
    set(ABI_TAG "mt")
  else()
    set(ABI_TAG "md")
  endif()
  string(APPEND EXPORT_NAME "-${ABI_TAG}")
  string(APPEND OUTPUT_NAME_POSTFIX "-${ABI_TAG}")
endif()

if (NOT BUILD_SHARED_LIBS)
  string(APPEND EXPORT_NAME "-static")
endif ()

set_target_properties(smbios PROPERTIES
  DEBUG_POSTFIX ${DEBUG_POSTFIX}
  OUTPUT_NAME smbios${OUTPUT_NAME_POSTFIX}
)

set_target_properties(smbios_exe PROPERTIES
  OUTPUT_NAME ${PROJECT_NAME}
)

install(TARGETS smbios
  EXPORT ${EXPORT_NAME}
)

install(TARGETS smbios_exe)

install(FILES
  src/smbios_parser.hpp
  src/smbios.hpp
  src/bios_streambuf.hpp
  DESTINATION include
)

install(EXPORT ${EXPORT_NAME}
  NAMESPACE ${PROJECT_NAME}::
  DESTINATION lib/cmake/${PROJECT_NAME}
)

include(CMakePackageConfigHelpers)

configure_package_config_file(
  cmake/smbios-config.cmake.in
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  INSTALL_DESTINATION lib/cmake/${PROJECT_NAME}
)

write_basic_package_version_file(
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMajorVersion
)

install(FILES
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
  "${CMAKE_CURRENT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
  DESTINATION lib/cmake/${PROJECT_NAME}
)
