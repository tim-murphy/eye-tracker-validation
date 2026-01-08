# CMakeList.txt : CMake project for TrackerValidation, include source and define
# project specific logic here.
#
cmake_minimum_required(VERSION 3.15)
project(TrackerValidation LANGUAGES CXX VERSION 1.02)
enable_testing()

add_compile_definitions(FREEGLUT_BUILD_STATIC_LIBS=ON)
add_compile_definitions(FREEGLUT_STATIC)
add_compile_definitions(FREEGLUT_LIB_PRAGMAS=0)

# Version information (use git hash)
find_package(Git QUIET)
execute_process(
    COMMAND "${GIT_EXECUTABLE}" describe --always HEAD
    WORKING_DIRECTORY "${CMAKE_SOURCE_DIR}"
    RESULT_VARIABLE unused
    OUTPUT_VARIABLE GIT_HASH
    ERROR_QUIET
    OUTPUT_STRIP_TRAILING_WHITESPACE)
message(STATUS "Current git hash: ${GIT_HASH}")
add_definitions(-DLATEST_GIT_HASH=${GIT_HASH})

# OpenGL policy - use GLVND instead of LEGACY
if (POLICY CMP0072)
    cmake_policy (SET CMP0072 OLD)
endif(POLICY CMP0072)
find_package(OpenGL)

# Note: CMake documentation says using GLOB for sources is not best practice
#       as CMakeLists.txt is not updated when files are added/removed. That
#       said, maintaining files in two places (in the directory and here) is
#       unnecessary duplication. Any issues will be caught by the CI system
#       anyway, so let's keep it simple.
file(GLOB SOURCES "*.cpp")
file(GLOB MAINSRC "TrackerValidation.cpp")
list(REMOVE_ITEM SOURCES ${MAINSRC})

# Eyelink stuff
file(GLOB EYELINKLIB "eyelink/eyelink_core64.lib")
file(GLOB EYELINK_DUMMY "eyelink/dummy.cpp")
list(APPEND SOURCES ${EYELINK_DUMMY})

if (WIN32)
    if (EXISTS ${EYELINKLIB})
        list(APPEND EXELIBS ${CMAKE_SOURCE_DIR}/TrackerValidation/eyelink/eyelink_core64.lib)
        list(REMOVE_ITEM SOURCES ${EYELINK_DUMMY})
    endif()
    set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
endif()

set(MAINEXE "TrackerValidation")
set(SOURCESLIB "validator_static")

# Create a static library of all sources except main - makes testing easier.
add_library(${SOURCESLIB} STATIC ${SOURCES})
target_include_directories(${SOURCESLIB} PUBLIC "eyelink")

# Add source to this project's executable.
add_executable(${MAINEXE} ${MAINSRC})
list(APPEND EXELIBS ${SOURCESLIB})

add_subdirectory("gazepoint")
list(APPEND EXELIBS "GPClient")

add_subdirectory("freeglut-3.2.1")
target_include_directories(${SOURCESLIB} PUBLIC "freeglut-3.2.1/include")
list(APPEND EXELIBS "freeglut_static")

find_package(Threads REQUIRED)
list(APPEND EXELIBS ${CMAKE_THREAD_LIBS_INIT})

if (NOT WIN32)
    find_package(X11)
    list(APPEND EXELIBS ${X11_LIBRARIES})
endif ()

target_link_libraries(${MAINEXE} ${EXELIBS})

# unittests
set(TESTEXE "unittests")
file(GLOB TESTSRC "test/*.cpp")
add_executable(${TESTEXE} ${TESTSRC})
target_link_libraries(${TESTEXE} ${EXELIBS})
add_test(UnitTests ${TESTEXE})
add_test(CompileTests "${CMAKE_COMMAND}" --build "${CMAKE_SOURCES_DIR}" --target "${TESTEXE}")
set_tests_properties(UnitTests PROPERTIES DEPENDS CompileTests)

