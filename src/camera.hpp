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
		float mouse_sensivity = 0.1f;
		float movement_sensitivity = 15.0f;
		float invertX = -1.0f, invertY = 1.0f;

		double lastFrame;
		double deltaTime;

		glm::mat4 orbit();
		glm::mat4 fpv(GLFWwindow* glwindow);

	public:
		Camera(CameraMode cammode, GLFWwindow* glwindow);
		glm::mat4 update(GLFWwindow* glwindow);

};
