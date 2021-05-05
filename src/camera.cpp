#include "camera.hpp"

glm::mat4 Camera::orbit() {
	float camX = sin(glfwGetTime()) * 10;
	float camZ = cos(glfwGetTime()) * 10;
	glm::mat4 view = glm::lookAt(glm::vec3(camX, camZ, camZ), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	view = glm::translate(view, glm::vec3(-1, -1, -1));
	return view;
}

glm::mat4 Camera::fpv(GLFWwindow* glwindow) {
	double xpos = 0, ypos = 0;

	glfwGetCursorPos(glwindow, &xpos, &ypos);
	xpos = cursorStartPos.x + invertX * (prevCursorPos.x - xpos);
	ypos = cursorStartPos.y + invertY * (prevCursorPos.y - ypos);

	cursorPos.x = xpos;
	cursorPos.y = ypos;
	xpos *= mouse_sensivity;
	ypos *= mouse_sensivity;

	//limit viewing angles
	if (ypos > 89.0f)
		ypos = 89.0f;
	if (ypos < -89.0f)
		ypos = -89.0f;

	//vector representing where the camera is currently pointing
	glm::vec3 camrot;
	camrot.x = cos(glm::radians(xpos)) * cos(glm::radians(ypos));
	camrot.y = sin(glm::radians(ypos));
	camrot.z = sin(glm::radians(xpos)) * cos(glm::radians(ypos));
	camrot = glm::normalize(camrot);

	//keyboard input
	if (glfwGetKey(glwindow, GLFW_KEY_W) == GLFW_PRESS) {
		pos.x += camrot.x * movement_sensitivity;
		pos.y += camrot.y * movement_sensitivity;
		pos.z += camrot.z * movement_sensitivity;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_S) == GLFW_PRESS) {
		pos.x -= camrot.x * movement_sensitivity;
		pos.y -= camrot.y * movement_sensitivity;
		pos.z -= camrot.z * movement_sensitivity;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 rcamrot(camrot);
		rcamrot = glm::normalize(glm::cross(rcamrot, glm::vec3(0, 1, 0)));
		pos.x -= rcamrot.x * movement_sensitivity;
		pos.y -= rcamrot.y * movement_sensitivity;
		pos.z -= rcamrot.z * movement_sensitivity;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 rcamrot(camrot);
		rcamrot = glm::normalize(glm::cross(rcamrot, glm::vec3(0, 1, 0)));
		pos.x += rcamrot.x * movement_sensitivity;
		pos.y += rcamrot.y * movement_sensitivity;
		pos.z += rcamrot.z * movement_sensitivity;
	}

	pos.y -= (glfwGetKey(glwindow, GLFW_KEY_Q) == GLFW_PRESS) ? movement_sensitivity : 0;
	pos.y += (glfwGetKey(glwindow, GLFW_KEY_E) == GLFW_PRESS) ? movement_sensitivity : 0;


	glm::mat4 view = glm::lookAt(pos, pos + camrot, glm::vec3(0.0, 1.0, 0.0));
	return view;
}


Camera::Camera(CameraMode cammode) {
	mode = cammode;
	prevCursorPos = glm::vec2(0);
	cursorPos = glm::vec2(0);
	cursorStartPos = glm::vec2(0);
	pos = glm::vec3(0);
}

glm::mat4 Camera::update(GLFWwindow* glwindow) {
	switch (mode) {
	case CameraMode::orbit:
		return orbit();

	case CameraMode::fpv:
		return fpv(glwindow);
	}
	return glm::mat4(0);
}
