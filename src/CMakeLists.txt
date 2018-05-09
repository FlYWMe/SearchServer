
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${PISTACHE_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(libRestServer)
add_subdirectory(libSearch)

add_executable(queryServer main.cpp)
target_include_directories(queryServer PRIVATE ./)
target_link_libraries(queryServer restserver)

