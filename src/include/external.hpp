#pragma once

// gotta go fast
#define GLM_FORCE_DEFAULT_ALIGNED_GENTYPES
#define GLM_FORCE_SIMD_AVX2

// OpenCL
#define CL_HPP_ENABLE_EXCEPTIONS
#define CL_HPP_TARGET_OPENCL_VERSION 200

// required for cl::Image2DGL
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
#	define Screen ScreenX11
#endif

#ifdef _WIN32
#	define GLFW_EXPOSE_NATIVE_WGL
#	define GLFW_EXPOSE_NATIVE_WIN32
#	define CL_DISPLAY_KHR CL_WGL_HDC_KHR
#endif

#include <opencl/opencl.hpp>

// external
#include <stb/image/read.h>
#include <stb/image/write.h>
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <GLFW/glfw3native.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <murmur-3/murmur.hpp>
#include <perlin_noise/PerlinNoise.hpp>

#define glfwTerminate dont_use_glfw_terminate_because_it_causes_segfault_on_my_driver_thank_you

#ifdef __linux__
#	undef Screen
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

