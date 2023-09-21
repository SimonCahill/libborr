######################################################
##  This CMake script tests for std::format support ##
##  in the current environment.                     ##
##  If std::format is not found, then it will       ##
##  include libfmt into the project automatically.  ##
##                                                  ##
##  Copyright ©️ Simon Cahill                        ##
######################################################

include(CheckIncludeFileCXX)
check_include_file_cxx("format" CXX_FORMAT_SUPPORT)

if (DEFINED CXX_FORMAT_SUPPORT AND "${CXX_FORMAT_SUPPORT}" STREQUAL "1")
    message("std::format found! Lucky you!")
    set(FMT_LIB_NAME "")
else()

    if (NOT TARGET fmt-header-only)
        include(FetchContent)
        FetchContent_Declare(
            fmt-header-only
            GIT_REPOSITORY https://github.com/fmtlib/fmt.git
            GIT_TAG f5e54359df4c26b6230fc61d38aa294581393084
        )
        FetchContent_MakeAvailable(fmt-header-only)
    endif()

    set(FMT_LIB_NAME "fmt::fmt-header-only")
endif()