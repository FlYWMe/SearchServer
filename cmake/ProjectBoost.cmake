include(ExternalProject)
include(GNUInstallDirs)

set(BOOST_CXXFLAGS "cxxflags=-fPIC")
if (WIN32)
    set(BOOST_BOOTSTRAP_COMMAND bootstrap.bat)
    set(BOOST_BUILD_TOOL b2.exe)
    set(BOOST_LIBRARY_SUFFIX -vc140-mt-1_63.lib)
else()
    set(BOOST_BOOTSTRAP_COMMAND ./bootstrap.sh)
    set(BOOST_BUILD_TOOL ./b2)
    set(BOOST_LIBRARY_SUFFIX .a)
    # if (${BUILD_SHARED_LIBS})
    #     set(BOOST_CXXFLAGS "cxxflags=-fPIC")
    # endif()
endif()

ExternalProject_Add(boost
    PREFIX ${CMAKE_BINARY_DIR}/deps
    DOWNLOAD_DIR ${CMAKE_SOURCE_DIR}/deps
    DOWNLOAD_NO_PROGRESS 1
    URL https://dl.bintray.com/boostorg/release/1.65.1/source/boost_1_65_1.tar.gz
    URL_HASH SHA256=a13de2c8fbad635e6ba9c8f8714a0e6b4264b60a29b964b940a22554705b6b60
    BUILD_IN_SOURCE 1
    CONFIGURE_COMMAND ${BOOST_BOOTSTRAP_COMMAND}
    BUILD_COMMAND ${BOOST_BUILD_TOOL} install
        ${BOOST_CXXFLAGS}
        threading=multi
        link=static
        variant=release
        address-model=64
        --prefix=<INSTALL_DIR>
        --with-chrono
        --with-date_time
        --with-system
        --with-filesystem
        --with-random
        --with-regex
        --with-test
        --with-thread
        --with-serialization
    LOG_DOWNLOAD 1 
    LOG_CONFIGURE 1  
    LOG_BUILD 1
    LOG_INSTALL 1
    INSTALL_COMMAND ""
)

ExternalProject_Get_Property(boost SOURCE_DIR INSTALL_DIR)
set(BOOST_INCLUDE_DIR ${INSTALL_DIR}/include)
set(BOOST_LIB_DIR ${INSTALL_DIR}/lib)
unset(BUILD_DIR)

add_library(Boost::Chrono STATIC IMPORTED)
set_property(TARGET Boost::Chrono PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_chrono${BOOST_LIBRARY_SUFFIX})
add_dependencies(Boost::Chrono boost)

add_library(Boost::DataTime STATIC IMPORTED)
set_property(TARGET Boost::DataTime PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_date_time${BOOST_LIBRARY_SUFFIX})
add_dependencies(Boost::DataTime boost)

# add_library(Boost::Regex STATIC IMPORTED)
# set_property(TARGET Boost::Regex PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_regex${BOOST_LIBRARY_SUFFIX})
# add_dependencies(Boost::Regex boost)

add_library(Boost::System STATIC IMPORTED)
set_property(TARGET Boost::System PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_system${BOOST_LIBRARY_SUFFIX})
add_dependencies(Boost::System boost)

add_library(Boost::Filesystem STATIC IMPORTED)
set_property(TARGET Boost::Filesystem PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_filesystem${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::Filesystem PROPERTY INTERFACE_LINK_LIBRARIES Boost::System)
add_dependencies(Boost::Filesystem boost)

# add_library(Boost::Random STATIC IMPORTED)
# set_property(TARGET Boost::Random PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_random${BOOST_LIBRARY_SUFFIX})
# add_dependencies(Boost::Random boost)

# add_library(Boost::UnitTestFramework STATIC IMPORTED)
# set_property(TARGET Boost::UnitTestFramework PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_unit_test_framework${BOOST_LIBRARY_SUFFIX})
# add_dependencies(Boost::UnitTestFramework boost)

add_library(Boost::Thread STATIC IMPORTED)
set_property(TARGET Boost::Thread PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_thread${BOOST_LIBRARY_SUFFIX})
set_property(TARGET Boost::Thread PROPERTY INTERFACE_LINK_LIBRARIES Boost::Chrono Boost::DataTime)
# set_property(TARGET Boost::Thread PROPERTY INTERFACE_LINK_LIBRARIES Boost::Chrono Boost::DataTime Boost::Regex)
add_dependencies(Boost::Thread boost)

add_library(Boost::Serialization STATIC IMPORTED)
set_property(TARGET Boost::Serialization PROPERTY IMPORTED_LOCATION ${BOOST_LIB_DIR}/libboost_serialization${BOOST_LIBRARY_SUFFIX})
# set_property(TARGET Boost::Serialization PROPERTY INTERFACE_LINK_LIBRARIES)
add_dependencies(Boost::Serialization boost)

unset(INSTALL_DIR)
unset(SOURCE_DIR)
