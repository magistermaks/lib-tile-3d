#pragma once

// gotta go fast
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_SIMD_AVX2

// OpenCL
//#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200
#define CL_USE_DEPRECATED_OPENCL_1_1_APIS

// OpenCL is a little broken
#pragma GCC diagnostic ignored "-Wignored-attributes"

// This is concern
#ifdef __linux__
#	define GLFW_EXPOSE_NATIVE_GLX
#	define GLFW_EXPOSE_NATIVE_X11
#	define CL_DISPLAY_KHR CL_GLX_DISPLAY_KHR
#	define Region RegionX11
#	define Glyph GlyphX11
#endif

#ifdef _WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define CL_DISPLAY_KHR CL_WGL_HDC_KHR
#endif

// set to CL_TRUE or CL_FALSE
// defines if OpenCL implementation should copy the given buffer before 
// sending it to the GPU. it sounds like a really cpu and memory heavy
// operation but I haven't observed any significant performance
// impact so i disabled it for the time being, as it can introduce
// problems if the buffer to be sent is modified (or deleted) between
// issuing the copy command and the execution of the OpenCL kernel
#define OPENCL_COPY_ON_WRITE CL_TRUE

#include <opencl/opencl.hpp>

// external
#include <stb/image.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <murmur-3/murmur.hpp>
#include <thread_pool/thread_pool.hpp>
#include <stx/string.hpp>

#ifdef __linux__
#	undef Region
#	undef Glyph
#endif

// C++
#include <vector>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>
#include <tuple>
#include <unordered_map>
#include <chrono>
#include <queue>
#include <functional>
#include <mutex>
#include <thread>
#include <random>
#include <regex>

// C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;
typedef std::mt19937 rng_type;

// internal
#include "logger.hpp"
#include "math.hpp"
#include "glhelper.hpp"
#include "clhelper.hpp"
#include "voxel.hpp"
#include "camera.hpp"
#include "util/buffer.hpp"
#include "util/trait.hpp"
#include "util/charset.hpp"
#include "world/chunk.hpp"
#include "world/region.hpp"
#include "renderer/scene.hpp"
#include "renderer/renderer.hpp"
#include "renderer/material.hpp"
#include "renderer/pathtracer.hpp"

#ifdef WIN32 
#	define NOMINMAX
#	include <windows.h>
#	include <direct.h>
#	define CWD_MAX_PATH MAX_PATH
#	define POSIX_GETCWD _getcwd
#else
#	include <unistd.h>
#	include <sys/stat.h>
#	define CWD_MAX_PATH PATH_MAX
#	define POSIX_GETCWD getcwd
#endif

