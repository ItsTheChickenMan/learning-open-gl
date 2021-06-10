// camera thing

#include <camera.h>

#include <cstdio>

// create a new camera with fov, aspect (width/height), near and far
Camera createCamera(glm::vec3 position, glm::vec3 rotation, float fov, float aspect, float near, float far){
	Camera camera;
	
	// projection
	camera.projection = glm::perspective( glm::radians(fov), aspect, near, far);
	
	// view matrices and position
	camera.position = position;
	camera.rotation = rotation;

	// assign axes
	camera.right = glm::vec3(1.0f, 0.0f, 0.0f);
	camera.up = glm::vec3(0.0f, 1.0f, 0.0f);
	
	// call camera update to initialize other values
	updateCamera(&camera);
	
	return camera;
}

void updateCamera(Camera *camera){
	// update direction
	camera->direction.x = cos(glm::radians(camera->rotation.y)) * cos(glm::radians(camera->rotation.x)); 
	camera->direction.y = sin(glm::radians(camera->rotation.x));
	camera->direction.z = sin(glm::radians(camera->rotation.y)) * cos(glm::radians(camera->rotation.x));
	
	// update forward
	camera->forward = glm::normalize(camera->direction);
	
	// update the view matrix
	camera->view = glm::lookAt(camera->position, camera->position + camera->forward, camera->up);
}