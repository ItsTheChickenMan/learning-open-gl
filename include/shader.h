// shader manager (header)

#ifndef PRACTICE_SHADER_H
#define PRACTICE_SHADER_H

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <types.h>

#define SHADER_VERTEX 	GL_VERTEX_SHADER
#define SHADER_FRAGMENT	GL_FRAGMENT_SHADER

typedef u32 ShaderProgram;

u32 createShader(char* shaderPath, GLenum shaderType);
void deleteShader(u32 shader);
ShaderProgram createShaderProgram(u32 vertexShader, u32 fragmentShader);
void useShader(ShaderProgram *program);

void setUniformFloat(ShaderProgram program, const char* location, float data);
void setUniformFloat(ShaderProgram program, const char* location, float *data, s32 num);
void setUniformInt(ShaderProgram program, const char* location, int data);
void setUniformInt(ShaderProgram program, const char* location, int *data, s32 num);
void setUniformMat4(ShaderProgram program, const char* location, glm::mat4 matrix);
void setUniformMat3(ShaderProgram program, const char* location, glm::mat3 matrix);

#endif