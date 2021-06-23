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
	glm::vec2 cameraRot;
	glm::vec2 cursorStartPos;
	glm::vec3 pos;
	glm::vec3 camrot;
	float mouse_sensivity = 0.2f;
	float movement_sensitivity = 15.0f;
	float invertX = 1.0f, invertY = -1.0f;

	double lastFrame;
	double deltaTime;

	glm::mat4 orbit();
	glm::vec3 fpv( GLFWwindow* glwindow );

public:
	Camera( CameraMode cammode, GLFWwindow* glwindow );
	glm::vec3 update( GLFWwindow* glwindow );
	glm::vec3& getPosition();
	glm::vec3& getRotation();

};
