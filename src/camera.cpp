#include "camera.hpp"

glm::mat4 Camera::orbit() {
	float camX = sin(glfwGetTime()) * 10;
	float camZ = cos(glfwGetTime()) * 10;
	glm::mat4 view = glm::lookAt(glm::vec3(camX, camZ, camZ), glm::vec3(0.0, 1.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	view = glm::translate(view, glm::vec3(-1, -1, -1));
	return view;
}

glm::vec3 Camera::fpv(GLFWwindow* glwindow, glm::vec3* camrot) {
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

	xpos = glm::radians(xpos);
	ypos = glm::radians(ypos);
	camrot->x = xpos + glm::radians(-90.0f);
	camrot->y = ypos;


	//vector representing where the camera is currently pointing
	glm::vec3 vcamrot;
	vcamrot.x = cos(xpos) * cos(ypos);
	vcamrot.y = -sin(ypos);
	vcamrot.z = sin(xpos) * cos(ypos);
	vcamrot = glm::normalize(vcamrot);

	const float speed = movement_sensitivity * deltaTime;

	//keyboard input
	if (glfwGetKey(glwindow, GLFW_KEY_W) == GLFW_PRESS) {
		pos.x += vcamrot.x * speed;
		pos.y += vcamrot.y * speed;
		pos.z += vcamrot.z * speed;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_S) == GLFW_PRESS) {
		pos.x -= vcamrot.x * speed;
		pos.y -= vcamrot.y * speed;
		pos.z -= vcamrot.z * speed;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_D) == GLFW_PRESS) {
		glm::vec3 rcamrot(vcamrot);
		rcamrot = glm::normalize(glm::cross(rcamrot, glm::vec3(0, 1, 0)));
		pos.x -= rcamrot.x * speed;
		pos.y -= rcamrot.y * speed;
		pos.z -= rcamrot.z * speed;
	}
	if (glfwGetKey(glwindow, GLFW_KEY_A) == GLFW_PRESS) {
		glm::vec3 rcamrot(vcamrot);
		rcamrot = glm::normalize(glm::cross(rcamrot, glm::vec3(0, 1, 0)));
		pos.x += rcamrot.x * speed;
		pos.y += rcamrot.y * speed;
		pos.z += rcamrot.z * speed;
	}

	pos.y -= (glfwGetKey(glwindow, GLFW_KEY_Q) == GLFW_PRESS) ? speed : 0;
	pos.y += (glfwGetKey(glwindow, GLFW_KEY_E) == GLFW_PRESS) ? speed : 0;

	return pos;
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

glm::vec3 Camera::update(GLFWwindow* glwindow, glm::vec3* camrot) {
	double cutime = glfwGetTime();
	deltaTime = cutime - lastFrame;
	lastFrame = cutime;

	switch (mode) {
		case CameraMode::orbit:
			return glm::vec3(0); //orbit();

		case CameraMode::fpv:
			return fpv(glwindow, camrot);
	}

	return glm::vec3(0);
}
