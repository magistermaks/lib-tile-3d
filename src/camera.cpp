#include "camera.hpp"

void Camera::update() {
	
	this->updateTime();
	float x = 0, y = 0;

	// get cursors position
	getCursorPos(&x, &y);
	double delta_x = +1.0f * (cursor.x - x) * this->sensivity;
	double delta_y = -1.0f * (cursor.y - y) * this->sensivity;
	cursor.x = x;
	cursor.y = y;

	x = angle.x + delta_x;
	y = angle.y + delta_y;

	// limit viewing angles
	if( y > +89.0 ) y = +89.0;
	if( y < -89.0 ) y = -89.0;

	this->angle.x = x;
	this->angle.y = y;

	// calculate rotation
	x = glm::radians(x);
	y = glm::radians(y);
	this->rotation.x = x + glm::radians(-90.0f);
	this->rotation.y = y;

	// vector representing where the camera is currently pointing
	this->direction = {
		 cos(x) * cos(y),
		-sin(y),
		 sin(x) * cos(y)
	};

	this->direction = glm::normalize(direction);

	const float speed = this->speed * this->delta_time;
	GLFWwindow* window = GLHelper::window();

	//keyboard input
	if( glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS ) {
		this->pos += direction * speed;
	}

	if( glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS ) {
		this->pos -= direction * speed;
	}

	if( glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS ) {
		glm::vec3 vec = glm::cross( direction, glm::vec3(0, 1, 0) );
		this->pos -= glm::normalize(vec) * speed;
	}

	if( glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS ) {
		glm::vec3 vec = glm::cross( direction, glm::vec3(0, 1, 0) );
		this->pos += glm::normalize(vec) * speed;
	}

	this->pos.y -= (glfwGetKey(window, GLFW_KEY_Q) == GLFW_PRESS) ? speed : 0;
	this->pos.y += (glfwGetKey(window, GLFW_KEY_E) == GLFW_PRESS) ? speed : 0;
}

void Camera::getCursorPos( float* x, float* y ) {
	double cx, cy;
	glfwGetCursorPos(GLHelper::window(), &cx, &cy);

	*x = (float) cx;
	*y = (float) cy;
}

Camera::Camera( float sensivity, float speed ) {

	this->angle = glm::vec2(0);
	this->rotation = glm::vec3(0);
	this->pos = glm::vec3(0);

	this->sensivity = sensivity;
	this->speed = speed;

	getCursorPos(&cursor.x, &cursor.y);
	this->last_frame = glfwGetTime();
}

void Camera::move( const glm::vec3& pos ) {
	this->pos = pos;
}

void Camera::updateTime() {
	const double now = glfwGetTime();

	this->delta_time = now - this->last_frame;
	this->last_frame = now;
}

glm::vec3& Camera::getPosition() {
	return this->pos;
}

glm::vec3& Camera::getRotation() {
	return this->rotation;
}

glm::mat4 Camera::getView() {
	return glm::lookAt(this->pos, this->direction, glm::vec3(0.0f, 1.0f, 0.0f));
}

