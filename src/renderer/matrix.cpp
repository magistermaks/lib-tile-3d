
#include "matrix.hpp"

glm::mat4 MatrixStack::getProjection() {
	return glm::perspective(
		glm::radians(45.0f), // FoV
		800.0f / 600.0f, // apspect ratio
		0.1f, // near plane
		100.0f // far plane
	);
}

glm::mat4 MatrixStack::getModelIdentity() {
	return glm::mat4(1.0f);
}

glm::mat4 MatrixStack::getModelViewProjection(Camera& camera, glm::mat4& model) {
	return getProjection() * camera.getView() * model;
}

