#pragma once

#include "config.hpp"

enum CameraMode {
	orbit,
	fpv
};

class Camera {
private:
	CameraMode mode;

	glm::vec2 prevCursorPos;
	glm::vec2 cursorPos;
	glm::vec2 cursorStartPos;
	glm::vec3 pos;
	float mouse_sensivity = 0.4f;
	float movement_sensitivity = 0.001;
	float invertX = 1.0f, invertY = -1.0f;

	glm::mat4 orbit();
	glm::mat4 fpv(GLFWwindow* glwindow);

public:
	Camera(CameraMode cammode);
	glm::mat4 update(GLFWwindow* glwindow);
};
