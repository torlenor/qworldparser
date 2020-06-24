
cmake_minimum_required(VERSION 2.8.8)

include_directories (${QWorldParser_SOURCE_DIR}/src)
include_directories (${QWorldParser_SOURCE_DIR}/ut)

# Make test executable
set(TEST_SOURCES 
	${CMAKE_CURRENT_SOURCE_DIR}/main.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/pointtest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/triangletest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/edgetest.cpp
        ${CMAKE_CURRENT_SOURCE_DIR}/delaunaytest.cpp
	)
	
add_executable(tests ${TEST_SOURCES})
