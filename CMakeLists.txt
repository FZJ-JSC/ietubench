cmake_minimum_required(VERSION 3.20 FATAL_ERROR)

# Define project
project(ietubench
	DESCRIPTION "ietubench"
	VERSION 1.0
	LANGUAGES CXX ASM
  )

# Main project language
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
enable_language(CXX ASM)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
set(THREADS_PREFER_PTHREAD_FLAG TRUE)

find_package(Threads REQUIRED)

set(PROJECT_USER_LIB_PATH "/usr" CACHE STRING "User libraries path")

option(PROJECT_USE_M5 "Enable m5 instrumentation" OFF)
option(PROJECT_USE_LIKWID "Enable likwid instrumentation" OFF)

if(PROJECT_USE_M5)
    add_library(m5 STATIC IMPORTED)
    set_target_properties(m5 PROPERTIES
        IMPORTED_LOCATION "${PROJECT_USER_LIB_PATH}/lib/libm5.a"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_USER_LIB_PATH}/include"
        )
endif()

if(PROJECT_USE_LIKWID)
    add_library(likwid SHARED IMPORTED)
    set_target_properties(likwid PROPERTIES
        IMPORTED_LOCATION "${PROJECT_USER_LIB_PATH}/lib/liblikwid.so"
        INTERFACE_INCLUDE_DIRECTORIES "${PROJECT_USER_LIB_PATH}/include"
        )
endif()

add_subdirectory(src)
