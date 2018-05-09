cmake_minimum_required (VERSION 3.0.2)
project (QueryServer)

# The project version number.
set(VERSION_MAJOR   0   CACHE STRING "Project major version number.")
set(VERSION_MINOR   1   CACHE STRING "Project minor version number.")
set(VERSION_PATCH   0   CACHE STRING "Project patch version number.")
mark_as_advanced(VERSION_MAJOR VERSION_MINOR VERSION_PATCH)
message(STATUS "Build ${CMAKE_PROJECT_NAME} ${VERSION_MAJOR}.${VERSION_MINOR}.${VERSION_PATCH}")

add_definitions(-DVERSION_MAJOR=${VERSION_MAJOR})
add_definitions(-DVERSION_MINOR=${VERSION_MINOR})
add_definitions(-DVERSION_PATCH=${VERSION_PATCH})

# Cmake scripts directory
set(VERIFIER_CMAKE_DIR "${CMAKE_CURRENT_LIST_DIR}/cmake" CACHE PATH "The path to the cmake directory")
list(APPEND CMAKE_MODULE_PATH ${VERIFIER_CMAKE_DIR})

# Options
OPTION(BUILD_TEST "Build Tests." ON)
OPTION(BUILD_DOCS "Build documentation" ON)
enable_testing()

# Find cuda
find_package(CUDA QUIET)
include_directories("${CUDA_INCLUDE_DIRS}")

# Include cmake scripts
include(CompilerSettings)
include(ProjectPistache)
include(ProjectFaiss)

# Set binary path
set(EXECUTABLE_OUTPUT_PATH ${PROJECT_BINARY_DIR}/bin)
add_subdirectory (src)
add_subdirectory (test)
add_subdirectory (docs)

# Install
install(PROGRAMS ${CMAKE_BINARY_DIR}/bin/queryServer DESTINATION bin)
# install(DIRECTORY model DESTINATION /etc/queryServer)
# install(FILES config.json DESTINATION /etc/queryServer)

# cpack config, make package
set(CPACK_PACKAGE_VERSION_MAJOR ${VERSION_MAJOR})
set(CPACK_PACKAGE_VERSION_MINOR ${VERSION_MINOR})
set(CPACK_PACKAGE_VERSION_PATCH ${VERSION_PATCH})
set(CPACK_GENERATOR "DEB")
set(CPACK_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_DEBIAN_PACKAGE_NAME ${CMAKE_PROJECT_NAME})
set(CPACK_DEBIAN_PACKAGE_ARCHITECTURE "amd64")
set(CPACK_PACKAGE_CONTACT "support@xxx.cn")
set(CPACK_DEBIAN_PACKAGE_DEPENDS "libc6, libstdc++6, libopencv-dev(>=2.4.0), libopencv-dev(<=2.4.13)")
include(CPack)
