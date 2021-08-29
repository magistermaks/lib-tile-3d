
#include "matrix.hpp"

glm::mat4 MatrixHelper::getModelIdentity() {
	return glm::mat4(1.0f);
}

glm::mat4 MatrixHelper::getVoxelIdentity() {
	const float scale = 2.0f;
	return glm::scale(glm::mat4(1.0f), glm::vec3(scale, scale, scale));
}

void MatrixHelper::uniform( GLint location, glm::mat4& matrix ) {
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(matrix));
}
