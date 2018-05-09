file(GLOB SRC_LIST "*.cpp")
file(GLOB HEADERS "*.h")

add_library(search STATIC ${SRC_LIST} ${HEADERS})
target_include_directories(search PRIVATE .. ${FAISS_INCLUDE_DIR})

if(NOT CUDA_FOUND)
    message(STATUS "Build ${CMAKE_PROJECT_NAME} without CUDA support.")
    include_directories(${FAISS_INCLUDE_DIR})
    target_link_libraries(search Faiss::CPU blas lapack)
elseif(CUDA_FOUND)
    add_definitions(-DCUDA_VERSION=${CUDA_VERSION_STRING})
    message(STATUS "Build ${CMAKE_PROJECT_NAME} with CUDA : " ${CUDA_VERSION})
    include_directories(${FAISS_INCLUDE_DIR} ${CUDA_INCLUDE_DIRS})
    target_link_libraries(search Faiss::GPU Faiss::CPU blas lapack ${CUDA_LIBRARIES} ${CUDA_CUBLAS_LIBRARIES})
endif()

add_dependencies(search Faiss::CPU)
