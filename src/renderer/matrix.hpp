#pragma once

#include "../config.hpp"

class Camera;

class MatrixStack {

	public:

		static glm::mat4 getProjection();
		static glm::mat4 getModelIdentity();
		static glm::mat4 getModelViewProjection(Camera& camera, glm::mat4& model);

};

