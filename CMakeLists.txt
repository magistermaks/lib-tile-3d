
# Stolen from: https://github.com/opengl-tutorials/ogl/blob/master/CMakeLists.txt

project(LibTile3D)

find_package(OpenGL REQUIRED)
find_package(OpenCL REQUIRED)
find_package(OpenMP REQUIRED)

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
	external/glfw-3.1.2/include/
	external/glm-0.9.7.1/
	external/glew-1.13.0/include/
	external/
    .
)

set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${OpenMP_C_FLAGS}")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${OpenMP_CXX_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${OpenMP_EXE_LINKER_FLAGS}")


set(ALL_LIBS
	${OPENGL_LIBRARY}
	${OpenCL_LIBRARY}
	glfw
	GLEW_1130
)

add_definitions(
	-DTW_STATIC
	-DTW_NO_LIB_PRAGMA
	-DTW_NO_DIRECT3D
	-DGLEW_STATIC
	-D_CRT_SECURE_NO_WARNINGS
)

add_executable(main 
	external/murmur-3/murmur.cpp
	external/thread_pool/thread_pool.cpp

	src/world/mesh.cpp
	src/world/chunk.cpp
	src/world/region.cpp

	src/renderer/renderer.cpp
	src/renderer/smallpt.cpp
	
	src/math.cpp
	src/main.cpp
	src/logger.cpp
	src/glhelper.cpp
	src/clhelper.cpp
	src/camera.cpp
	src/voxel.cpp
)

target_link_libraries(main
	${ALL_LIBS}
)

