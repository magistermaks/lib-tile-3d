
// currently unused //

#pragma once

#include "config.hpp"

class Pallet {

	private:
		GLuint id;
		GLuint size;

	public:
		Pallet();
		void bind();
		size_t size();
		byte bytes();

};

