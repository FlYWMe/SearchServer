if(BUILD_TEST)

include_directories(${CMAKE_SOURCE_DIR}/src)
file(GLOB testSrcs ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp)
    
foreach(source ${testSrcs})
    get_filename_component(name ${source} NAME_WE)

    # target
    add_executable(${name} ${source})
    target_link_libraries(${name} search common)

    # Install
    # install(TARGETS ${name} DESTINATION bin)

    # Unit test
    add_test("test_${name}" ${CMAKE_BINARY_DIR}/bin/${name})
endforeach(source)

endif(BUILD_TEST) 
