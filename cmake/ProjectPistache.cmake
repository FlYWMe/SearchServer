include(ExternalProject)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(PISTACHE_CMAKE_COMMAND emcmake cmake)
else()
    set(PISTACHE_CMAKE_COMMAND ${CMAKE_COMMAND})
endif()

ExternalProject_Add(pistache
    PREFIX ${CMAKE_BINARY_DIR}/deps
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
    # SOURCE_DIR ${CMAKE_SOURCE_DIR}/deps/pistache    
    GIT_REPOSITORY https://github.com/bxq2011hust/pistache.git
    GIT_TAG master
    UPDATE_COMMAND ""
    CMAKE_COMMAND ${PISTACHE_CMAKE_COMMAND}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               # Build static lib but suitable to be included in a shared lib.
               -DBUILD_SHARED_LIBS=OFF
               -DCMAKE_POSITION_INDEPENDENT_CODE=${BUILD_SHARED_LIBS}
               -DCMAKE_BUILD_TYPE=Release               
    BUILD_COMMAND ""
    # INSTALL_COMMAND ""
    LOG_CONFIGURE 1
    LOG_BUILD 1  
    LOG_INSTALL 1
    LOG_DOWNLOAD 1
)

# Create Pistache imported library
ExternalProject_Get_Property(pistache INSTALL_DIR)
add_library(Pistache STATIC IMPORTED)
set(PISTACHE_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}pistache${CMAKE_STATIC_LIBRARY_SUFFIX})
set(PISTACHE_INCLUDE_DIR ${INSTALL_DIR}/include)
file(MAKE_DIRECTORY ${PISTACHE_INCLUDE_DIR})  # Must exist.
set_property(TARGET Pistache PROPERTY IMPORTED_LOCATION ${PISTACHE_LIBRARY})
set_property(TARGET Pistache PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${PISTACHE_INCLUDE_DIR})
add_dependencies(Pistache pistache)
unset(INSTALL_DIR)

