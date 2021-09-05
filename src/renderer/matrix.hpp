#pragma once

#include <config.hpp>

class MatrixHelper {

	public:

		static glm::mat4 getModelIdentity();
		static glm::mat4 getVoxelIdentity();
		static void uniform( GLint location, glm::mat4& matrix );

};

