#pragma once

#include <include/config.hpp>
#include <include/external.hpp>
#include <include/forward.hpp>

// internal
#include "logger.hpp"
#include "glhelper.hpp"
#include "clhelper.hpp"
#include "camera.hpp"
#include "util/buffer.hpp"
#include "util/trait.hpp"
#include "util/charset.hpp"
#include "world/chunk.hpp"
#include "world/region.hpp"
#include "world/worldgen.hpp"
#include "world/octree.hpp"
#include "renderer/shader.hpp"
#include "renderer/scene.hpp"
#include "renderer/renderer.hpp"
#include "renderer/texture.hpp"
#include "renderer/vertex.hpp"
#include "renderer/matrix.hpp"
#include "renderer/pathtracer.hpp"

// when adding a file here, remember to add its
// forward definition to the include/forward.hpp file!

