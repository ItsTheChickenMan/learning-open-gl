// learning how to use open gl, so that I can implement a rendering engine into the chat 3d without it having to be the learning space

#include <window.h>
#include <shader.h>
#include <shapes.h>
#include <graphics.h>
#include <camera.h>

#include <ctgmath>

#include <cstdio>

// helpful macros
#define EXIT_SUCCESS 0
#define EXIT_FAILURE 1

#define WIDTH 800
#define HEIGHT 600

#define WIDTHF (float)WIDTH
#define HEIGHTF (float)HEIGHT

int main(){
	// init glfw and stuff
	if( windowInit() != WINDOW_SUCCESS){
		printf("couldn't initialize window manager\n");
		return EXIT_FAILURE;
	}
	
	// create main window
	Window mainWindow = windowCreate("OpenGL Practice", WIDTH, HEIGHT, true);
	
	// create shaders
	u32 vertexShader = createShader("./shaders/vertex.glsl", SHADER_VERTEX);
	u32 fragmentShader = createShader("./shaders/fragment.glsl", SHADER_FRAGMENT);
	u32 untexturedVs = createShader("./shaders/untexturedvs.glsl", SHADER_VERTEX);
	u32 untexturedFs = createShader("./shaders/untexturedfs.glsl", SHADER_FRAGMENT);
	u32 unlitFs = createShader("./shaders/unlitfs.glsl", SHADER_FRAGMENT);
	
	// create and link shader program
	ShaderProgram mainShader = createShaderProgram(vertexShader, fragmentShader, "mainShader", "vertexShader", "fragmentShader"); // textures
	ShaderProgram lightSourceShader = createShaderProgram(untexturedVs, unlitFs, "lightSourceShader", "untexturedVs", "unlitFs"); // used for light sources
	ShaderProgram texturelessShader = createShaderProgram(untexturedVs, untexturedFs, "texturelessShader", "untexturedVs", "untexturedFs"); // no textures, only colors

	// delete shaders
	deleteShader(vertexShader);
	deleteShader(fragmentShader);
	deleteShader(untexturedVs);
	deleteShader(untexturedFs);
	
	// textures
	Texture_Data texture1 = createTexture("./textures/container.png");
	Texture_Data texture1_specular = createTexture("./textures/container_specular.png"); // ./textures/container_specular.png
	Texture_Data texture1_emission = createTexture("./textures/matrix.jpg");
	Texture_Data texture2 = createTexture("./textures/money.jpg");
	
	// materials
	Material pinkMaterial = createMaterial(glm::vec3(1.0f, 0.0f, 0.5f), 64, 0.5);
	//Material redMaterial = createMaterial(glm::vec3(1.0f, 0.0f, 0.0f), 64, 0.5);
	//Material emerald = createMaterial(glm::vec3(0.07568, 0.61424, 0.07568), 76.8, 1.0);
	Material whiteMaterial = createMaterial(glm::vec3(1.0, 1.0, 1.0), 64, 1.0);
	
	// create triangle vertex data
	//Vertex_Data triangle = createVertexData(triangle_vertices, 3, sizeof(triangle_vertices));
	//Vertex_Data rectangle = createVertexData(rectangle_vertices, 4, sizeof(rectangle_vertices), rectangle_indices, 6, sizeof(rectangle_indices));
	Vertex_Data cubeVertices = createVertexData(cube_vertices, 36, sizeof(cube_vertices));
	
	bindTextureToVertexData(&cubeVertices, &texture1, DIFFUSE_MAP);
	bindTextureToVertexData(&cubeVertices, &texture1_specular, SPECULAR_MAP);
	//bindTextureToVertexData(&cubeVertices, &texture1_emission, EMISSION_MAP);
	
	setUniformInt(texturelessShader, "material.diffuseMap", 0);
	setUniformInt(texturelessShader, "material.specularMap", 1);
	//setUniformInt(texturelessShader, "material.emissionMap", 2);
	
	// create object data from vertex data
	Object_Data cube3D = createObjectData(&cubeVertices, glm::vec3(0.0f, 0.95f, 0.0f), glm::vec3(0.0f, 0.0f, 0.0f), &pinkMaterial);
	Object_Data litCube = createObjectData(&cubeVertices, glm::vec3(2.0f, 0.0f, 2.0f), glm::vec3(0.0f, 0.0f, 0.0f), &whiteMaterial);
	
	// lights and stuff
	//Light light = createLight(cube3D.position, glm::vec3(1.0f, 1.0f, 1.0f), 0.1, 1.0, 0.5);
	//PointLight light = createPointLight(glm::vec3(-0.2f, -1.0f, -0.3f), 1.0f, 0.09f, 0.032f, glm::vec3(1.0f, 1.0f, 1.0f), 0.1, 1.0, 0.5);
	SpotLight light = createSpotLight(cube3D.position, glm::vec3(-0.2f, -1.0f, -0.3f), glm::radians(12.0f), glm::radians(16.0f), 1.0f, 0.09f, 0.032f, glm::vec3(1.0f, 1.0f, 1.0f), 0.1, 1.0, 1.0);
	
	// create camera
	Camera mainCamera = createCamera(glm::vec3(0.0f, 0.0f, 3.0f), glm::vec3(0.0f, -90.0f, 0.0f), 45.0f, WIDTHF / HEIGHTF, 0.1f, 100.0f);

	// mouse settings
	glfwSetInputMode(mainWindow.glfwWindow, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	double mouseX, mouseY; // x and y positions of mouse
	double mouseDeltaX, mouseDeltaY;
	bool firstMouse = true;
	
	// opengl stuff
	glClearColor(0.3, 0.0, 0.0, 1.0); // assing clear color
	glEnable(GL_DEPTH_TEST); // enable depth testing
	
	glm::vec3 cubePositions[] = {
		glm::vec3(2.0f, 0.0f, 2.0f),
		glm::vec3(-2.0f, 0.0f, -2.0f),
		glm::vec3(0.0f, -0.5f, 0.0f),
		glm::vec3(-2.0f, -3.0f, -4.0f),
		glm::vec3(4.0f, -4.0f, 1.0f),
		glm::vec3(-6.0f, 4.0f, -1.0f)
	};
	
	glm::vec3 cubeRotations[] = {
		glm::vec3(15.0f, 45.0f, 0.0f),
		glm::vec3(75.0f, 15.0f, 100.0f),
		glm::vec3(80.0f, 55.0f, 63.0f),
		glm::vec3(21.0f, 22.0f, 23.0f),
		glm::vec3(10.0f, 20.0f, 30.0f),
		glm::vec3(40.0f, 50.0f, 60.0f)
	};
	
	glm::vec3 pointLights[] = {
		
	};
	
	float delta = 1.0f;
	float lastFrame = 0.0f;
	while(!windowShouldClose(&mainWindow)){
		// delta
		delta = glfwGetTime() - lastFrame;
		lastFrame = glfwGetTime();
		
		// mouse input TODO: wrap this in function
		double tempMouseX, tempMouseY;
		glfwGetCursorPos(mainWindow.glfwWindow, &tempMouseX, &tempMouseY);
		if(firstMouse){
			firstMouse = false;
			mouseX = tempMouseX;
			mouseY = tempMouseY;
		}
		mouseDeltaX = tempMouseX - mouseX;
		mouseDeltaY = mouseY - tempMouseY;
		mouseX = tempMouseX;
		mouseY = tempMouseY;
		
		// handle input
		float cameraSpeed = 2.0f * delta;
		
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_W) == GLFW_PRESS)
			mainCamera.position += cameraSpeed * mainCamera.forward;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_S) == GLFW_PRESS)
			mainCamera.position -= cameraSpeed * mainCamera.forward;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_A) == GLFW_PRESS)
			mainCamera.position -= glm::normalize(glm::cross(mainCamera.forward, mainCamera.up)) * cameraSpeed;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_D) == GLFW_PRESS)
			mainCamera.position += glm::normalize(glm::cross(mainCamera.forward, mainCamera.up)) * cameraSpeed;
		
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS)
			mainCamera.position += glm::normalize(-mainCamera.up) * cameraSpeed;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_SPACE) == GLFW_PRESS)
			mainCamera.position += glm::normalize(mainCamera.up) * cameraSpeed;

		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_UP) == GLFW_PRESS)
			cube3D.position += cameraSpeed * mainCamera.forward;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_DOWN) == GLFW_PRESS)
			cube3D.position -= cameraSpeed * mainCamera.forward;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_LEFT) == GLFW_PRESS)
			cube3D.position -= glm::normalize(glm::cross(mainCamera.forward, mainCamera.up)) * cameraSpeed;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_RIGHT) == GLFW_PRESS)
			cube3D.position += glm::normalize(glm::cross(mainCamera.forward, mainCamera.up)) * cameraSpeed;
		
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_Q) == GLFW_PRESS)
			cube3D.position += glm::normalize(-mainCamera.up) * cameraSpeed;
		if (glfwGetKey(mainWindow.glfwWindow, GLFW_KEY_E) == GLFW_PRESS)
			cube3D.position += glm::normalize(mainCamera.up) * cameraSpeed;
		
		// set camera pos in uniform
		float pos[] = {mainCamera.position.x, mainCamera.position.y, mainCamera.position.z};
		
		setUniformFloat(texturelessShader, "cameraPos", pos, 3);
		
		// rotate camera
		float sensitivity = 0.1f;
		mainCamera.rotation.x += mouseDeltaY*sensitivity;
		mainCamera.rotation.y += mouseDeltaX*sensitivity;
		
		if(mainCamera.rotation.x > 89.0f)
			mainCamera.rotation.x = 89.0f;
		if(mainCamera.rotation.x < -89.0f)
			mainCamera.rotation.x = -89.0f;
			
		// rendering
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
		
		light.position = mainCamera.position;
		light.direction = mainCamera.direction;
		
		// set uniforms
		setUniformSpotLight(&light, texturelessShader, "light");
		setUniformMaterial(&pinkMaterial, texturelessShader);
		
		// update objects
		updateObjectData(&cube3D);
		//updateObjectData(&litCube);
		
		for(int i = 0; i < sizeof(pointLights)/sizeof(glm::vec3); i++){
			cube3D.position = cubePositions[i];
		}
		
		for(int i = 0; i < sizeof(cubePositions)/sizeof(glm::vec3); i++){
			litCube.position = cubePositions[i];
			litCube.rotation = cubeRotations[i];
			
			updateObjectData(&litCube);
			drawObjectData(&litCube, &mainCamera, &texturelessShader);
		}
		
		
		//drawObjectData(&cube3D, &mainCamera, &lightSourceShader);
		//drawObjectData(&litCube, &mainCamera, &texturelessShader);
		
		// update camera
		updateCamera(&mainCamera);
		
		// swap buffers, poll events
		windowUpdate(&mainWindow);
	}
	
	windowTerminate();
	
	return EXIT_SUCCESS;
}