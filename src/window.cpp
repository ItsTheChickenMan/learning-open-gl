// window manager

#include <window.h>
#include <callbacks.h>

#include <cstdio>

static Window nullWindow; // uninitialized window that serves as NULL

// returns address to nullWindow
Window *windowNull(){
	return &nullWindow;
}

// initializes glfw
s32 windowInit(){
	// init glfw
	if( glfwInit() != GL_TRUE){
		return windowError("was unable to properly initialize glfw", 1);
	}
	
	// assign min and max gl versions and core_profile type
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	return windowLog("initialized glfw");
}

// creates a new window struct with all the necessary cool stuff (and callbacks if useDefaultCallbacks is true)
Window windowCreate(const char* title, s32 width, s32 height, bool useDefaultCallbacks){
	// create new window struct
	Window window;
	
	// create glfw window
	window.glfwWindow = glfwCreateWindow(width, height, title, NULL, NULL);
	if(window.glfwWindow == NULL){
		windowError("Failed to create GLFW window", 1);
		windowTerminate();
		return nullWindow;
	}
	
	// make window current context of thread
	glfwMakeContextCurrent(window.glfwWindow);
	
	// init glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)){
		windowError("was unable to properly initialize glad", 2);
		windowTerminate();
		return nullWindow;
	}
	
	// for fun
	printf("Using OpenGL %d.%d\n", GLVersion.major, GLVersion.minor);
	
	// assign other values
	window.width = width;
	window.height = height;
	window.title = title;
	
	// set glViewport
	glViewport(0, 0, window.width, window.height);
	
	// assign default callbacks
	if(useDefaultCallbacks){
		glfwSetFramebufferSizeCallback(window.glfwWindow, default_framebuffer_size_callback);  
	}

	// return new window
	return window;
}

// update buffers and poll events
void windowUpdate(Window *window){
	glfwSwapBuffers(window->glfwWindow);
	glfwPollEvents();
}

// terminate window
void windowTerminate(){
	glfwTerminate();
}

// returns true if the window should close, otherwise false
GLboolean windowShouldClose(Window *window){
	return glfwWindowShouldClose(window->glfwWindow);
}

void windowEnableMSAA(u32 samples){
	glfwWindowHint(GLFW_SAMPLES, samples); // open the sample buffer
	// ...that's it
}

// log a thing
s32 windowLog(char* log){
	printf("Window log: %s\n", log);
	
	return 0;
}

// log an error
s32 windowError(char* error, s32 code){
	printf("Window error: %s\n", error);
	
	return code;
}