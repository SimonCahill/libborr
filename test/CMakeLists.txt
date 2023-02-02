cmake_minimum_required(VERSION 3.12)

project(borrtests LANGUAGES CXX VERSION 1.0.0 DESCRIPTION "Test suites for libborr")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT TARGET borr)
    message(FATAL_ERROR "Unit Tests must be built with main library!")
endif()

add_definitions(
    -DUNIT_TESTING=1
)

get_target_property(borr_INCLUDE_DIRS borr INCLUDE_DIRECTORIES)
include_directories(
    ${borr_INCLUDE_DIRS}
    include/
)

get_target_property(borr_SOURCES borr SOURCES)
file(GLOB_RECURSE FILES FOLLOW_SYMLINKS ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

find_package(GTest REQUIRED)

add_executable(
    ${PROJECT_NAME}

    ${FILES}
    ${borr_SOURCES}
)

target_link_libraries(
    ${PROJECT_NAME}

    ${GTEST_LIBRARIES}
)