
include_directories(${CMAKE_SOURCE_DIR}/src
                    ${PISTACHE_INCLUDE_DIR})

add_subdirectory(common)
add_subdirectory(libRestServer)
add_subdirectory(libSearch)

add_executable(searcher main.cpp)
target_include_directories(searcher PRIVATE ./)
target_link_libraries(searcher restserver)

