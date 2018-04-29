#pragma once

#include "./Shader.h"
#include <GLM/glm.hpp>
#include <GLM/gtc/matrix_transform.hpp>
#include <GLM/gtc/type_ptr.hpp>

class SceneManager {
public:
	SceneManager();
	~SceneManager();
	void Initialize(GLuint width, GLuint height);
	void InitializeGraphics();

	void AddShader(string vFilename, string fFilename);

	//GLFW callbacks
	static void key_callback(GLFWwindow* window, int key, int scanCode, int action, int mode);
	static void Resize(GLFWwindow* window, int width, int height);

	void DoMovement();
	void Render();
	void Run();
	void Finish();

	void SetupScene();
	void SetupCamera2D();
	void SetupTexture();
	void SetupBackground();

private:
	GLfloat x, y, horizontalPosition;
	unsigned int texture, background;

	GLFWwindow *window;
	
	Shader *shader;
	
	// Scene attributes
	GLuint VAO;
	
	// Transformations - Model Matrix
	glm::mat4 model;
	
	// 2D Camera - Projection matrix
	glm::mat4 projection;
	
	// Translation
	glm::mat4 translation;
};

