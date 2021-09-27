#
# CMake listfile to specify the build process, see:
# http://www.cmake.org/cmake/help/documentation.html
#
project(zxing)
cmake_minimum_required(VERSION 2.8.0)

set(CMAKE_LIBRARY_PATH /opt/local/lib ${CMAKE_LIBRARY_PATH})
set(CMAKE_CXX_STANDARD 17)

# Check for polluted source tree.
if(EXISTS ${CMAKE_SOURCE_DIR}/CMakeCache.txt OR
    EXISTS ${CMAKE_SOURCE_DIR}/CMakeFiles)
    message(FATAL_ERROR
        "Source directory is polluted:"
        "\n  * remove CMakeCache.txt"
        "\n  * remove CMakeFiles directory")
endif()

# Suppress in-source builds.
if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
    message(FATAL_ERROR
        "CMake generation is not allowed within the source directory:"
        "\n  * mkdir build"
        "\n  * cd build"
        "\n  * Unix-like: cmake -G \"Unix Makefiles\" .."
        "\n  * Windows: cmake -G \"Visual Studio 10\" ..")
endif()

# Adjust CMake's module path.
set(CMAKE_MODULE_PATH ${CMAKE_SOURCE_DIR}/cmake/)

# Suppress MSVC CRT warnings.
if(MSVC)
    add_definitions(-D_CRT_SECURE_NO_WARNINGS)
    add_definitions(/Za)
endif()

# Add libzxing library.
file(GLOB_RECURSE LIBZXING_FILES
    "./core/src/*.cpp"
    "./core/src/*.h"
    "./core/src/*.cc"
    "./core/src/*.hh"
)
if(WIN32)
    file(GLOB LIBZXING_WIN32_FILES
        "./core/lib/win32/*.c"
        "./core/lib/win32/*.h"
    )
    set(LIBZXING_FILES ${LIBZXING_FILES} ${LIBZXING_WIN32_FILES})
    include_directories(SYSTEM "./core/lib/win32/")
endif()
include_directories("./core/src/")
add_library(libzxing STATIC ${LIBZXING_FILES})
set_target_properties(libzxing PROPERTIES PREFIX "")


target_compile_options(libzxing PRIVATE -fno-rtti)
target_compile_options(libzxing PRIVATE -fno-exceptions)
target_compile_options(libzxing PRIVATE -fstack-usage)
#target_compile_options(libzxing PRIVATE -Wredundant-move)
target_compile_options(libzxing PRIVATE -Wall)
target_compile_options(libzxing PRIVATE -Wpedantic)

#target_compile_options(libzxing PRIVATE -finstrument-functions)
target_compile_options(libzxing PRIVATE -fsanitize=address)

#find_package(Iconv)
#if(ICONV_FOUND)
#    include_directories(${ICONV_INCLUDE_DIR})
#    target_link_libraries(libzxing ${ICONV_LIBRARIES})
#else()
#    add_definitions(-DNO_ICONV=1)
#endif()

add_definitions(-DNO_ICONV=1)


# Add cli executable.
file(GLOB_RECURSE ZXING_FILES
    "./cli/src/main.cpp"
    "./cli/src/jpgd.cpp"
    "./cli/src/ImageReaderSource.cpp"
    "./cli/src/*.h"
)

add_executable(zxing ${ZXING_FILES})
target_compile_options(zxing PRIVATE -g)
target_compile_options(zxing PRIVATE -fstack-usage)
#target_compile_options(zxing PRIVATE -finstrument-functions)

target_link_libraries(zxing libzxing)
target_link_libraries(zxing asan)

# Add opencv executable.
file(GLOB_RECURSE OPENCV_ZXING_FILES
    "./cli/src/demo.cpp"
)

find_package(OpenCV 4.0.0 REQUIRED COMPONENTS core imgproc highgui videoio)

include_directories(${OpenCV_INCLUDE_DIRS})
add_executable(opencv_zxing ${OPENCV_ZXING_FILES})
target_compile_options(opencv_zxing PRIVATE -g)
target_compile_options(opencv_zxing PRIVATE -fstack-usage)
target_compile_options(opencv_zxing PRIVATE -fsanitize=address)

target_link_libraries(opencv_zxing libzxing)
target_link_libraries(opencv_zxing asan)
target_link_libraries(opencv_zxing ${OpenCV_LIBS})

# Add testrunner executable.
find_package(CPPUNIT)
if(CPPUNIT_FOUND)
    file(GLOB_RECURSE TESTRUNNER_FILES
        "./core/tests/src/*.cpp"
        "./core/tests/src/*.h"
    )
    add_executable(testrunner ${TESTRUNNER_FILES})
    include_directories(${CPPUNIT_INCLUDE_DIR})
    target_link_libraries(testrunner libzxing  ${CPPUNIT_LIBRARIES})
else()
    message(WARNING "Not building testrunner, because CppUnit is missing")
endif()
