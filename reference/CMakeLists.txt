cmake_minimum_required(VERSION 3.12)

project(borr_reference LANGUAGES CXX VERSION 1.0.0 DESCRIPTION "THe reference implementation for libborr." HOMEPAGE_URL "https://github.com/SimonCahill/libborr")
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if (NOT TARGET borr)
    add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/../ ${CMAKE_CURRENT_BINARY_DIR}/libborr)
endif()

file(GLOB_RECURSE FILES FOLLOW_SYMLINKS ${CMAKE_CURRENT_SOURCE_DIR} src/*.cpp)

include_directories(
    include/
)

add_executable(${PROJECT_NAME} ${FILES})

target_link_libraries(
    ${PROJECT_NAME}

    borr
)