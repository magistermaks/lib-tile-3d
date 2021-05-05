#pragma once

// external
#include <GL/glew.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

// C++
#include <vector>
#include <sstream>
#include <vector>
#include <fstream>
#include <string>
#include <iostream>
#include <ctime>

// C
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

typedef unsigned char byte;

// internal
#include "logger.hpp"
#include "glhelper.hpp"
#include "mesh.hpp"
#include "chunk.hpp"
#include "camera.hpp"

#ifdef WIN32 
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

