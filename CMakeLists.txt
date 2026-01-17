cmake_minimum_required(VERSION 3.28)
project(smbios VERSION 2025.09.04)

if (NOT DEFINED CMAKE_CXX_STANDARD)
  set(CMAKE_CXX_STANDARD 23)
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

if (WIN32)
  target_link_libraries(smbios PRIVATE
    kernel32.lib
  )
endif ()

target_link_libraries(smbiosexe PRIVATE smbios)

install(TARGETS smbios
  EXPORT ${PROJECT_NAME}_targets
)

install(TARGETS smbiosexe)

install(FILES
  src/smbios_parser.hpp
  src/smbios.hpp
  src/bios_streambuf.hpp
  DESTINATION include
)

install(EXPORT ${PROJECT_NAME}_targets
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
