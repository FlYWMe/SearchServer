include(ExternalProject)

if (${CMAKE_SYSTEM_NAME} STREQUAL "Emscripten")
    set(FAISS_CMAKE_COMMAND emcmake cmake)
else()
    set(FAISS_CMAKE_COMMAND ${CMAKE_COMMAND})
endif()

ExternalProject_Add(faiss
    PREFIX ${CMAKE_BINARY_DIR}/deps
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS 1
    DOWNLOAD_NAME faiss-1.0.tar.gz
    URL https://github.com/facebookresearch/faiss/archive/v1.2.1.tar.gz
    URL_HASH SHA256=0a8d629f86ee728c9c9cd72527027c09fc390963dd3cbdd9675eb577873a2695
    BUILD_IN_SOURCE 1
    # GIT_REPOSITORY https://github.com/facebookresearch/faiss.git
    # GIT_TAG master
    # UPDATE_COMMAND ""
    CMAKE_COMMAND ${FAISS_CMAKE_COMMAND}
    CMAKE_ARGS -DCMAKE_INSTALL_PREFIX=<INSTALL_DIR>
               -DBUILD_TUTORIAL=OFF
               -DBUILD_TEST=OFF
               -DBUILD_WITH_GPU=${CUDA_FOUND}
               -DCMAKE_BUILD_TYPE=Release  
    LOG_DOWNLOAD 1 
    LOG_CONFIGURE 1
    LOG_BUILD 1  
    LOG_INSTALL 1
    # BUILD_COMMAND ""
    INSTALL_COMMAND cmake -E copy_directory lib <INSTALL_DIR>/lib
)

ExternalProject_Get_Property(faiss INSTALL_DIR SOURCE_DIR)


add_library(Faiss::CPU STATIC IMPORTED)
set(FAISS_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}faiss${CMAKE_STATIC_LIBRARY_SUFFIX})
set(FAISS_INCLUDE_DIR ${INSTALL_DIR}/include)
set_property(TARGET Faiss::CPU PROPERTY IMPORTED_LOCATION ${FAISS_LIBRARY})
set_property(TARGET Faiss::CPU PROPERTY INTERFACE_INCLUDE_DIRECTORIES ${FAISS_INCLUDE_DIR})
add_dependencies(Faiss::CPU faiss)

add_library(Faiss::GPU STATIC IMPORTED)
set(FAISS_GPU_LIBRARY ${INSTALL_DIR}/lib/${CMAKE_STATIC_LIBRARY_PREFIX}gpufaiss${CMAKE_STATIC_LIBRARY_SUFFIX})
set_property(TARGET Faiss::GPU PROPERTY IMPORTED_LOCATION ${FAISS_GPU_LIBRARY})
add_dependencies(Faiss::GPU faiss)

file(MAKE_DIRECTORY ${FAISS_INCLUDE_DIR}/faiss ${FAISS_INCLUDE_DIR}/faiss/gpu)  # Must exist.
ADD_CUSTOM_TARGET(install_header ALL COMMAND ${CMAKE_COMMAND} -E copy ${SOURCE_DIR}/*.h ${INSTALL_DIR}/include/faiss
                    COMMAND ${CMAKE_COMMAND} -E copy_directory ${SOURCE_DIR}/gpu ${INSTALL_DIR}/include/faiss/gpu)
add_dependencies(install_header faiss)
unset(INSTALL_DIR)
unset(SOURCE_DIR)

