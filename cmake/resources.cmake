##################################################
##  Setup resources for build and compile-time  ##
##  expansions/checks/info.                     ##
##################################################

if (borr_BUILD_TESTS)
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/borr/resources.hpp)
else()
    configure_file(${CMAKE_CURRENT_SOURCE_DIR}/cmake/resources.hpp.in ${CMAKE_CURRENT_BINARY_DIR}/include/borr/resources.hpp)
endif()