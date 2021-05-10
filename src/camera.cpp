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
	double deltax = invertX * (prevCursorPos.x - xpos) * mouse_sensivity;
	double deltay = invertY * (prevCursorPos.y - ypos) * mouse_sensivity;
	prevCursorPos.x = xpos;
	prevCursorPos.y = ypos;

	xpos = cameraRot.x + deltax;
	ypos = cameraRot.y + deltay;

	//limit viewing angles
	if (ypos > 89.0)
		ypos = 89.0;
	if (ypos < -89.0)
		ypos = -89.0;

	cameraRot.x = xpos;
	cameraRot.y = ypos;

	//vector representing where the camera is currently pointing
	glm::vec3 camrot;
	camrot.x = cos(glm::radians(xpos)) * cos(glm::radians(ypos));
	camrot.y = sin(glm::radians(ypos));
	camrot.z = sin(glm::radians(xpos)) * cos(glm::radians(ypos));
	camrot = glm::normalize(camrot);

	const float speed = movement_sensitivity * deltaTime;

	//keyboard input
	if (glfwGetKey(glwindow, GLFW_KEY_W) == GLFW_PRESS) {
		pos.x += camrot.x * speed;
		pos.y += camrot.y * speed;
		pos.z += camrot.z * speed;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_S) == GLFW_PRESS) {
		pos.x -= camrot.x * speed;
		pos.y -= camrot.y * speed;
		pos.z -= camrot.z * speed;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 rcamrot(camrot);
		rcamrot = glm::normalize(glm::cross(rcamrot, glm::vec3(0, 1, 0)));
		pos.x -= rcamrot.x * speed;
		pos.y -= rcamrot.y * speed;
		pos.z -= rcamrot.z * speed;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 rcamrot(camrot);
		rcamrot = glm::normalize(glm::cross(rcamrot, glm::vec3(0, 1, 0)));
		pos.x += rcamrot.x * speed;
		pos.y += rcamrot.y * speed;
		pos.z += rcamrot.z * speed;
	}

	pos.y -= (glfwGetKey(glwindow, GLFW_KEY_Q) == GLFW_PRESS) ? speed : 0;
	pos.y += (glfwGetKey(glwindow, GLFW_KEY_E) == GLFW_PRESS) ? speed : 0;


	glm::mat4 view = glm::lookAt(pos, pos + camrot, glm::vec3(0.0, 1.0, 0.0));
	return view;
}


Camera::Camera(CameraMode cammode, GLFWwindow* glwindow) {
	mode = cammode;
	cameraRot = glm::vec2(0);
	cursorStartPos = glm::vec2(0);
	pos = glm::vec3(0, 10, 0);
	double x, y;
	glfwGetCursorPos(glwindow, &x, &y);
	lastFrame = glfwGetTime();
	prevCursorPos = glm::vec2(x, y);
}

glm::mat4 Camera::update(GLFWwindow* glwindow) {
	double cutime = glfwGetTime();
	deltaTime = cutime - lastFrame;
	lastFrame = cutime;

	switch (mode) {
		case CameraMode::orbit:
			return orbit();

		case CameraMode::fpv:
			return fpv(glwindow);
	}

	return glm::mat4(0);
}
