cmake_minimum_required(VERSION 3.12)

project(borr VERSION 1.0.1 LANGUAGES CXX DESCRIPTION "Borr; A simple cross-platform C++ language file parser" HOMEPAGE_URL "https://github.com/SimonCahill")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

include(cmake/resources.cmake)
include(cmake/format_support.cmake)

if (CMAKE_BUILD_TYPE STREQUAL "Debug")
    add_compile_options(
        -O0
        -ggdb
    )
else()
    add_compile_options(
        -O2
    )
endif()

include_directories(
    include/
    ${CMAKE_CURRENT_BINARY_DIR}/include
)

file(GLOB_RECURSE FILES FOLLOW_SYMLINKS ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

if (DEFINED libborr_BUILD_STATIC)
    add_library(${PROJECT_NAME} STATIC ${FILES})
else()
    add_library(${PROJECT_NAME} SHARED ${FILES})
endif()

target_link_libraries(
    ${PROJECT_NAME}

    # add potential dependencies here
    ${FMT_LIB_NAME}
)

target_include_directories(${PROJECT_NAME} PUBLIC include/ ${CMAKE_CURRENT_BINARY_DIR}/include)

###
# Docs target
###
add_custom_target("docs" COMMENT "Create Doxygen documentation")
add_custom_command(
    TARGET "docs"
    POST_BUILD
        COMMENT "Generate Doxygen documentation for publication or reading"
        COMMAND doxygen ${CMAKE_CURRENT_SOURCE_DIR}/Doxyfile
            WORKING_DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR}
)

if (borr_BUILD_TESTS)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/test ${CMAKE_CURRENT_BINARY_DIR}/borrtests)
endif()

if (borr_BUILD_REFERENCE)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/reference ${CMAKE_CURRENT_BINARY_DIR}/borrreference)
endif()