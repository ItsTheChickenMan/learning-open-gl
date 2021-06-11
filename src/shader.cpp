// shader manager

#include <shader.h>
#include <fileio.h>

#include <cstdio>
#include <cstdlib>

// create a shader at a path and compiles it
u32 createShader(char* shaderPath, GLenum shaderType){
	// create shader
	u32 shader = glCreateShader(shaderType);
	
	// load + assign shader source
	char* shaderSource = read_entire_file(shaderPath);
	
	glShaderSource(shader, 1, &shaderSource, NULL);
	
	// compile shader and check for errors
	glCompileShader(shader);
	
	// check for errors (idk why error checking is this complicated, thanks opengl)
	int success;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &success); // get compilation status
	// if unsuccessful, log why
	if(!success){
		// buffer for error
		char info[512];
		
		// get error
		glGetShaderInfoLog(shader, 512, NULL, info);
		
		printf("shader compilation error: %s\n", info);
	}
	
	printf("successfully compiled %s shader\n", shaderPath);

	free(shaderSource);

	return shader;
}

// delete a shader (only when done with it)
void deleteShader(u32 shader){
	glDeleteShader(shader);
}

// create a shader program
ShaderProgram createShaderProgram(u32 vertexShader, u32 fragmentShader){
	// create program
	ShaderProgram program = glCreateProgram();
	
	// attach shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	
	// link shaders
	glLinkProgram(program);
	
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success){
		char info[512];
		glGetProgramInfoLog(program, 512, NULL, info);
		printf("failed to link shader program: %s\n", info);
	}  

	return program;
}

// create a shader program with names for more error info
ShaderProgram createShaderProgram(u32 vertexShader, u32 fragmentShader, const char* shaderName, const char* vertexName, const char* fragmentName){
	// create program
	ShaderProgram program = glCreateProgram();
	
	// attach shaders
	glAttachShader(program, vertexShader);
	glAttachShader(program, fragmentShader);
	
	// link shaders
	glLinkProgram(program);
	
	int success;
	glGetProgramiv(program, GL_LINK_STATUS, &success);
	if(!success){
		char info[512];
		glGetProgramInfoLog(program, 512, NULL, info);
		printf("failed to link shader program %s (vertex: %s, fragment: %s): %s\n", shaderName, vertexName, fragmentName, info);
	}

	return program;
}

// use a shader program
void useShader(ShaderProgram *program){
	glUseProgram(*program);
}

// set a uniform float (singular)
void setUniformFloat(ShaderProgram program, const char* location, float data){
	int uniformLocation = glGetUniformLocation(program, location);
	glUseProgram(program);
	
	glUniform1f(uniformLocation, data);
}

// set a uniform float(s)
// num must be >0 and <=4
void setUniformFloat(ShaderProgram program, const char* location, float *data, s32 num){
	int uniformLocation = glGetUniformLocation(program, location);
	glUseProgram(program);
	
	// todo: way to make this easier?
	switch(num){
		case 1:
			glUniform1f(uniformLocation, data[0]);
			break;
		case 2:
			glUniform2f(uniformLocation, data[0], data[1]);
			break;
		case 3:
			glUniform3f(uniformLocation, data[0], data[1], data[2]);
			break;
		case 4:
			glUniform4f(uniformLocation, data[0], data[1], data[2], data[3]);
			break;
		default:
			printf("invalid number of floats\n");
			break;
	}
}

// set a uniform int (singular)
void setUniformInt(ShaderProgram program, const char* location, int data){
	int uniformLocation = glGetUniformLocation(program, location);
	glUseProgram(program);
	
	glUniform1i(uniformLocation, data);
}

// set a uniform int(s)
// num must be >0 and <=4
void setUniformInt(ShaderProgram program, const char* location, int *data, s32 num){
	int uniformLocation = glGetUniformLocation(program, location);
	glUseProgram(program);
	
	// todo: way to make this easier?
	switch(num){
		case 1:
			glUniform1i(uniformLocation, data[0]);
			break;
		case 2:
			glUniform2i(uniformLocation, data[0], data[1]);
			break;
		case 3:
			glUniform3i(uniformLocation, data[0], data[1], data[2]);
			break;
		case 4:
			glUniform4i(uniformLocation, data[0], data[1], data[2], data[3]);
			break;
		default:
			printf("invalid number of ints\n");
			break;
	}
}

// sadly we can't make different types for the matrix without using templates, which I don't feel like doing right now
void setUniformMat4(ShaderProgram program, const char* location, glm::mat4 matrix){
	int uniformLocation = glGetUniformLocation(program, location);
	glUseProgram(program);
	
	glUniformMatrix4fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}

void setUniformMat3(ShaderProgram program, const char* location, glm::mat3 matrix){
	int uniformLocation = glGetUniformLocation(program, location);
	glUseProgram(program);
	
	glUniformMatrix3fv(uniformLocation, 1, GL_FALSE, glm::value_ptr(matrix));
}