# CMakeList.txt : CMake project for TrackerValidation, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

add_compile_definitions(FREEGLUT_BUILD_STATIC_LIBS)

# Add source to this project's executable.
add_executable (TrackerValidation "TrackerValidation.cpp"  "Validator.h" "MeasuredData.h"  "Validator.cpp" "MeasuredDataStream.h" "MeasuredDataStream.cpp" "MeasuredData.cpp" "ScreenPositionStore.h" "ManualScreenPosition.h" "ManualScreenPosition.cpp" "ValidatorUI.h" "ValidatorUI.cpp" "ValidatorUIOpenGL.h" "ValidatorUIOpenGL.cpp" "ScreenPositionStore.cpp" "common.h" "TrackerDataCollector.h" "TrackerDataCollector.cpp" "DummyTrackerCollector.h" "DummyTrackerCollector.cpp" "ThreadTrackerCollector.h" "ThreadTrackerCollector.cpp" "GazepointGP3Collector.h" "GazepointGP3Collector.cpp")

add_subdirectory ("gazepoint")
add_subdirectory("freeglut-3.2.1")
target_include_directories(TrackerValidation PUBLIC "freeglut-3.2.1/include")
target_link_libraries(TrackerValidation "GPClient")
target_link_libraries(TrackerValidation "freeglut_static")

find_package(Threads)
target_link_libraries(TrackerValidation ${CMAKE_THREAD_LIBS_INIT})

# TODO: Add tests and install targets if needed.
