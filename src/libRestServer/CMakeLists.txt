file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(restserver STATIC ${SRC_LIST} ${HEADERS})

target_include_directories(restserver PRIVATE ..)
target_link_libraries(restserver Pistache common search)
add_dependencies(restserver Pistache)
# include_directories(../common)