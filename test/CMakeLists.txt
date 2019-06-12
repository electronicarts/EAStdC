#-------------------------------------------------------------------------------------------
# Copyright (C) Electronic Arts Inc.  All rights reserved.
#-------------------------------------------------------------------------------------------

#-------------------------------------------------------------------------------------------
# CMake info
#-------------------------------------------------------------------------------------------
cmake_minimum_required(VERSION 3.1)
project(EAStdCTest CXX)
include(CTest)

#-------------------------------------------------------------------------------------------
# Defines
#-------------------------------------------------------------------------------------------
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-D_SCL_SECURE_NO_WARNINGS)
add_definitions(-D_CHAR16T)

#-------------------------------------------------------------------------------------------
# Compiler Flags
#-------------------------------------------------------------------------------------------
set (CMAKE_MODULE_PATH "${CMAKE_MODULE_PATH};${CMAKE_CURRENT_SOURCE_DIR}/packages/EASTL/scripts/CMake")
include(CommonCppFlags)

#-------------------------------------------------------------------------------------------
# Source files
#-------------------------------------------------------------------------------------------
file(GLOB EASTDCTEST_SOURCES "source/*.cpp")
set(SOURCES ${EASTDCTEST_SOURCES})

#-------------------------------------------------------------------------------------------
# Executable definition
#-------------------------------------------------------------------------------------------
add_executable(EAStdCTest ${SOURCES})


#-------------------------------------------------------------------------------------------
# Include directories
#-------------------------------------------------------------------------------------------
target_include_directories(EAStdCTest PUBLIC include)

#-------------------------------------------------------------------------------------------
# Dependencies 
#-------------------------------------------------------------------------------------------
add_subdirectory(packages/EABase)
add_subdirectory(packages/EAAssert)
add_subdirectory(packages/EAMain)
add_subdirectory(packages/EASTL)
add_subdirectory(packages/EATest)
add_subdirectory(packages/EAThread)

target_link_libraries(EAStdCTest EABase)
target_link_libraries(EAStdCTest EAAssert)
target_link_libraries(EAStdCTest EAMain)
target_link_libraries(EAStdCTest EASTL)
target_link_libraries(EAStdCTest EATest)
target_link_libraries(EAStdCTest EAThread)

set(THREADS_PREFER_PTHREAD_FLAG ON)
find_package(Threads REQUIRED)

if((NOT APPLE) AND (NOT WIN32))
    target_link_libraries(EAStdCTest ${EASTLTest_Libraries} Threads::Threads rt)
else()
    target_link_libraries(EAStdCTest ${EASTLTest_Libraries} Threads::Threads)
endif()

#-------------------------------------------------------------------------------------------
# Run Unit tests and verify the results.
#-------------------------------------------------------------------------------------------
add_test(EAStdCTestRuns EAStdCTest)
set_tests_properties (EAStdCTestRuns PROPERTIES PASS_REGULAR_EXPRESSION "RETURNCODE=0")

