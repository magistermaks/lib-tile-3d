#pragma once

#include "../config.hpp"

class Scene;


class Worldgen {

	public:
		static void gen_chunk_world(Region& region);
		static void gen_chunk_scene1(Region& region);

};