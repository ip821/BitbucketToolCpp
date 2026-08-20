set(CMAKE_SYSTEM_NAME Windows)
set(CMAKE_SYSTEM_PROCESSOR x86_64)

find_program(mingw_c_compiler
        NAMES x86_64-w64-mingw32-gcc
        REQUIRED
        NO_CMAKE_FIND_ROOT_PATH
)
find_program(mingw_cxx_compiler
        NAMES x86_64-w64-mingw32-g++
        REQUIRED
        NO_CMAKE_FIND_ROOT_PATH
)
find_program(mingw_rc_compiler
        NAMES x86_64-w64-mingw32-windres
        REQUIRED
        NO_CMAKE_FIND_ROOT_PATH
)

set(CMAKE_C_COMPILER "${mingw_c_compiler}")
set(CMAKE_CXX_COMPILER "${mingw_cxx_compiler}")
set(CMAKE_RC_COMPILER "${mingw_rc_compiler}")

execute_process(
        COMMAND "${mingw_cxx_compiler}" -print-sysroot
        RESULT_VARIABLE mingw_sysroot_result
        OUTPUT_VARIABLE mingw_sysroot
        OUTPUT_STRIP_TRAILING_WHITESPACE
)
if (NOT mingw_sysroot_result EQUAL 0 OR NOT mingw_sysroot)
    message(FATAL_ERROR "Could not determine the MinGW-w64 sysroot")
endif ()

set(CMAKE_FIND_ROOT_PATH "${mingw_sysroot}")
set(CMAKE_FIND_ROOT_PATH_MODE_PROGRAM NEVER)
set(CMAKE_FIND_ROOT_PATH_MODE_LIBRARY ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_INCLUDE ONLY)
set(CMAKE_FIND_ROOT_PATH_MODE_PACKAGE ONLY)
