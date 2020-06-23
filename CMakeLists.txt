cmake_minimum_required (VERSION 3.0.2)
project (QWorldParser)

set(COMPANY "Abyle")
set(DESCRIPTION "QWorldParser - A desktop DynDNS updater written in QT")

# ------------------------------------------------------------------------- #
# VERSION SETTINGS
# ------------------------------------------------------------------------- #
set(QWORLDPARSER_MAJOR_VERSION 0)
set(QWORLDPARSER_MINOR_VERSION 0)
set(QWORLDPARSER_PATCH_VERSION 0)
set(QWORLDPARSER_BUILD_VERSION 0)
set(QWORLDPARSER_REVISION_VERSION 1)

# file (STRINGS "src/MAJOR.ver" QWORLDPARSER_MAJOR_VERSION)
# string(SUBSTRING ${QWORLDPARSER_MAJOR_VERSION} 0 2 QWORLDPARSER_MAJOR_VERSION)

# file (STRINGS "src/MINOR.ver" QWORLDPARSER_MINOR_VERSION)
# string(SUBSTRING ${QWORLDPARSER_MINOR_VERSION} 0 2 QWORLDPARSER_MINOR_VERSION)

# file (STRINGS "src/PATCH.ver" QWORLDPARSER_PATCH_VERSION)
# string(SUBSTRING ${QWORLDPARSER_PATCH_VERSION} 0 2 QWORLDPARSER_PATCH_VERSION)

# file (STRINGS "src/BUILD.ver" QWORLDPARSER_BUILD_VERSION)
# string(SUBSTRING ${QWORLDPARSER_BUILD_VERSION} 0 4 QWORLDPARSER_BUILD_VERSION)

set(QWORLDPARSER_VERSION
        ${QWORLDPARSER_MAJOR_VERSION}.${QWORLDPARSER_MINOR_VERSION}.${QWORLDPARSER_PATCH_VERSION}.${QWORLDPARSER_BUILD_VERSION})

# ------------------------------------------------------------------------- #
# MAGIC STARTS HERE
# ------------------------------------------------------------------------- #             
message(STATUS "Configuring ${PROJECT_NAME}...")

# file(WRITE ${CMAKE_BINARY_DIR}/MAJOR.ver ${QWORLDPARSER_MAJOR_VERSION})
# file(WRITE ${CMAKE_BINARY_DIR}/MINOR.ver ${QWORLDPARSER_MINOR_VERSION})
# file(WRITE ${CMAKE_BINARY_DIR}/PATCH.ver ${QWORLDPARSER_PATCH_VERSION})
# file(WRITE ${CMAKE_BINARY_DIR}/BUILD.ver ${QWORLDPARSER_BUILD_VERSION})
        
if(NOT QT_ROOT_PATH)
    message(STATUS "QT_ROOT_PATH not set. Use cmake -DQT_ROOT_PATH=PATH_TO_QT")
    message(STATUS "Trying to use system QT")
else(NOT QT_ROOT_PATH)
    message(STATUS "Using QT_ROOT_PATH = ${QT_ROOT_PATH}")
endif(NOT QT_ROOT_PATH)

if(NOT SSL_DIR)
    message(STATUS "SSL_DIR not set. Use cmake -DSSL_DIR=PATH_TO_OPENSSL")
    message(STATUS "Trying to use system SSL")
else(NOT SSL_DIR)
    message(STATUS "Using SSL_DIR = ${SSL_DIR}")
endif(NOT SSL_DIR)

if(MINGW)
    message(STATUS "Using MINGW")
else(MINGW)
    message(STATUS "Using SOMETHING ELSE")
endif(MINGW)

set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -Wall")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -Wall -O3")

include(CheckCXXCompilerFlag)
CHECK_CXX_COMPILER_FLAG("-std=c++11" COMPILER_SUPPORTS_CXX11)
CHECK_CXX_COMPILER_FLAG("-std=c++0x" COMPILER_SUPPORTS_CXX0X)
if(COMPILER_SUPPORTS_CXX11)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
elseif(COMPILER_SUPPORTS_CXX0X)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++0x")
else()
    message(STATUS "The compiler ${CMAKE_CXX_COMPILER} has no C++11 support. Please use a different C++ compiler.")
endif()

add_subdirectory (src)
add_subdirectory (ut)
