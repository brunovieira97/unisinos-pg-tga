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
	static void KeyCallback(GLFWwindow* window, int key, int scanCode, int action, int mode);
	static void Resize(GLFWwindow* window, int width, int height);

	void DoMovement();
	
	void Render();
	void RenderBackground();
	void RenderForeground();
	void RenderCharacter();
	void RenderBox();

	void Run();
	void Finish();

	void SetupScene();
	void SetupBackground();
	void SetupForeground();
	void SetupCharacter();
	void SetupBox();
	
	void SetupBackgroundTexture();
	void SetupForegroundTexture();
	void SetupCharacterTexture();
	void SetupBoxTexture();

	void SetupCamera2D();

private:
	GLfloat x, y, backgroundPosition, foregroundPosition, characterPosition, boxPosition;

	unsigned int backgroundTexture, foregroundTexture, characterTexture, boxTexture;

	GLFWwindow *window;
	
	Shader *shader;
	
	// Scene attributes
	GLuint bgVAO, fgVAO, charVAO, boxVAO;
	
	// Transformations - Model Matrix
	glm::mat4 model;
	
	// 2D Camera - Projection matrix
	glm::mat4 projection;
	
	// Translation
	glm::mat4 translation;
};

