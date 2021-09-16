
# Stolen from: https://github.com/opengl-tutorials/ogl/blob/master/CMakeLists.txt

project(LibTile3D)

cmake_minimum_required(VERSION 3.10)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if( CMAKE_BINARY_DIR STREQUAL CMAKE_SOURCE_DIR )
    message( FATAL_ERROR "Please select another Build Directory!" )
endif()

if( CMAKE_SOURCE_DIR MATCHES " " )
	message( "Your Source Directory contains spaces. If you experience problems when compiling, this can be the cause!" )
endif()

if( CMAKE_BINARY_DIR MATCHES " " )
	message( "Your Build Directory contains spaces. If you experience problems when compiling, this can be the cause!" )
endif()

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=native")

add_subdirectory (external)

if(INCLUDE_DISTRIB)
	add_subdirectory(distrib)
endif(INCLUDE_DISTRIB)

include_directories(
	${OpenCL_INCLUDE_DIR}
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/
	src/
    .
)

set(ALL_LIBS
	${OPENGL_LIBRARY}
	${OpenCL_LIBRARY}
	glfw
	GLEW_1130
	STB_IMAGE
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

file(GLOB_RECURSE LT3D_SOURCES RELATIVE 
	${CMAKE_SOURCE_DIR} 
	"src/*.cpp"
)

add_library(LT3D STATIC
	${LT3D_SOURCES}
)

target_link_libraries(LT3D
	${ALL_LIBS}
)

add_subdirectory(demo)

