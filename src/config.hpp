#pragma once

// external
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <murmur-3/murmur.hpp>
#include <thread_pool/thread_pool.hpp>

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
#include "glhelper.hpp"
#include "voxel.hpp"
#include "world/mesh.hpp"
#include "world/chunk.hpp"
#include "camera.hpp"
#include "world/region.hpp"
#include "renderer/renderer.hpp"

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

// Settings:
// primitive used for rendering, set to GL_QUADS or GL_TRIANGLES
#define LT3D_PRIMITIVE GL_QUADS

// maximum mesh buffer size, should be set to (64*64*64*6*<bytes per face>)/2
#define LT3D_MAX_MESH_SIZE (64*64*64*3*(6*4))


