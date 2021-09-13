#pragma once

// simple forward definitions
class MatrixHelper;
class Screen;
class Camera;
class Scene;
class Texture;
class ShaderProgram;
class Charset;
class VertexConsumer;
class World;
class PathTracer;
class OctreeVoxel;
class Worldgen;
class Chunk;
class ChunkManager;
class ThreadPool;
class Task;

// template forward definitions
template<typename T> class Octree;
template<typename T> class ReusableBuffer;

// typedefs
typedef unsigned char byte;
typedef std::chrono::high_resolution_clock Clock;
typedef std::chrono::milliseconds milliseconds;
typedef std::mt19937 rng_type;

