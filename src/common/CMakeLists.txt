file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(common STATIC ${SRC_LIST} ${HEADERS})
