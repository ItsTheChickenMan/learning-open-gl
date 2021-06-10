// callbacks (stored in here in case globals are necessary)

#include <callbacks.h>

void default_framebuffer_size_callback(GLFWwindow *window, int width, int height){
	glViewport(0, 0, width, height);
}