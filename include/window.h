// window header

#ifndef PRACTICE_WINDOW_H
#define PRACTICE_WINDOW_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <types.h>

#define WINDOW_SUCCESS 0

// window
struct Window {
	GLFWwindow *glfwWindow;
	
	u32 width;
	u32 height;
	
	const char* title;
};

Window *windowNull();

s32 windowInit();
Window windowCreate(const char* title, s32 width, s32 height, bool useDefaultCallbacks);
void windowUpdate(Window *window);
void windowTerminate();
GLboolean windowShouldClose(Window *window);

void windowEnableMSAA(u32 samples);

s32 windowLog(char* log);
s32 windowError(char* error, s32 code);

#endif