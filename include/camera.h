// camera header

#ifndef PRACTICE_CAMERA_H
#define PRACTICE_CAMERA_H

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

struct Camera {
	// values
	glm::vec3 position; // position
	glm::vec3 rotation; // rotation of camera
	glm::vec3 direction; // direction the camera is facing in, calculated via rotation
	
	// axes
	glm::vec3 up; // up axis of camera
	glm::vec3 right; // right axis of camera
	glm::vec3 forward; // forward axis (normalized direction)
	
	// matrices
	glm::mat4 view;
	glm::mat4 projection;
};

Camera createCamera(glm::vec3 position, glm::vec3 rotation, float fov, float aspect, float near, float far);
void updateCamera(Camera *camera);

#endif