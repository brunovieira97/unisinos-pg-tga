#include <Classes/SceneManager.h>

static bool keys[1024];
static bool resized;
static GLuint width, height;

SceneManager::SceneManager() {}

SceneManager::~SceneManager() {}

void SceneManager::Initialize(GLuint width, GLuint height) {
	::width = width;
	::height = height;

	// GLFW - GLEW - OPENGL general setup -- TODO: config file
	InitializeGraphics();
}

void SceneManager::InitializeGraphics() {
	backgroundPosition = 0.0;
	foregroundPosition = 0.0;
	characterPosition = 0.85;
	boxPosition = -0.85;
	verticalPosition = -0.275;
	offsetX = 0.0;
	offsetY = 0.0;

	glfwInit();

	window = glfwCreateWindow(width, height, "Game", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, KeyCallback);

	glfwSetWindowSizeCallback(window, Resize);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	AddShader("Shaders/Shader.vs", "Shaders/Shader.frag");

	SetupScene();

	// Forcing camera setup on first run
	resized = true;
}

void SceneManager::AddShader(string vFilename, string fFilename) {
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}

void SceneManager::KeyCallback(GLFWwindow * window, int key, int scanCode, int action, int mode) {
	if (key == GLFW_KEY_ESCAPE && action == GLFW_PRESS)
		glfwSetWindowShouldClose(window, GL_TRUE);
	if (key >= 0 && key < 1024) {
		if (action == GLFW_PRESS)
			keys[key] = true;
		else if (action == GLFW_RELEASE)
			keys[key] = false;
	}
}

void SceneManager::Resize(GLFWwindow * window, int width, int height) {
	::width = width;
	::height = height;
	::resized = true;

	// Define the viewport dimensions
	glViewport(0, 0, ::width, ::height);
}

void SceneManager::DoMovement() {
	if (keys[GLFW_KEY_LEFT])
		if ((characterPosition - 0.001) > -0.95) {
			characterPosition -= 0.001f;
			backgroundPosition += 0.0002f;
			foregroundPosition += 0.0005f;
			boxPosition += 0.0005f;
			offsetY = 1.0;
			offsetX -= 1.0/4.0;

			if(TestCollision()) {
				Finish();
				InitializeGraphics();
				keys[GLFW_KEY_LEFT] = false;
				std::cout << "You died!" << std::endl;
			}
		}

	if (keys[GLFW_KEY_RIGHT])
		if ((characterPosition + 0.001) < 0.95) {
			characterPosition += 0.001f;
			backgroundPosition -= 0.0002f;
			foregroundPosition -= 0.0005f;
			boxPosition -= 0.0005f;
			offsetY = 1.0/2.0;
			offsetX += 1.0/4.0;
		}

	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void SceneManager::Render() {
	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	RenderBackground();
	RenderForeground();
	RenderCharacter();
	RenderBox();
}

void SceneManager::RenderBackground(){
	shader -> Use();

	// Create transformations 
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(backgroundPosition, 0, 0));
	glUniform1f(glGetUniformLocation(shader -> Program, "offsetx"), 0);
	glUniform1f(glGetUniformLocation(shader -> Program, "offsety"), 0);

	GLint modelLoc = glGetUniformLocation(shader->Program, "model");

	// Passes transformations to Shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	if (resized) {
		SetupCamera2D();
		resized = false;
	}

	glBindTexture(GL_TEXTURE_2D, backgroundTexture);
	glUniform1i(glGetUniformLocation(shader -> Program, "texture"), 0);

	// Render container
	glBindVertexArray(bgVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SceneManager::RenderForeground(){
	shader -> Use();

	// Create transformations 
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(foregroundPosition, 0, 0));
	glUniform1f(glGetUniformLocation(shader -> Program, "offsetx"), 0);
	glUniform1f(glGetUniformLocation(shader -> Program, "offsety"), 0);

	GLint modelLoc = glGetUniformLocation(shader->Program, "model");

	// Passes transformations to Shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	if (resized) {
		SetupCamera2D();
		resized = false;
	}

	glBindTexture(GL_TEXTURE_2D, foregroundTexture);
	glUniform1i(glGetUniformLocation(shader -> Program, "texture"), 0);

	// Render container
	glBindVertexArray(fgVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SceneManager::RenderCharacter(){
	shader -> Use();

	// Create transformations 
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(characterPosition, verticalPosition, 1));
	glUniform1f(glGetUniformLocation(shader -> Program, "offsetx"), offsetX);
	glUniform1f(glGetUniformLocation(shader -> Program, "offsety"), offsetY);

	GLint modelLoc = glGetUniformLocation(shader->Program, "model");

	// Passes transformations to Shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	if (resized) {
		SetupCamera2D();
		resized = false;
	}

	glBindTexture(GL_TEXTURE_2D, characterTexture);
	glUniform1i(glGetUniformLocation(shader -> Program, "texture"), 0);

	// Render container
	glBindVertexArray(charVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

void SceneManager::RenderBox(){
	shader -> Use();

	// Create transformations 
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(boxPosition, verticalPosition, 2));
	glUniform1f(glGetUniformLocation(shader -> Program, "offsetx"), 0);
	glUniform1f(glGetUniformLocation(shader -> Program, "offsety"), 0);

	GLint modelLoc = glGetUniformLocation(shader -> Program, "model");

	// Passes transformations to Shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	if (resized) {
		SetupCamera2D();
		resized = false;
	}

	glBindTexture(GL_TEXTURE_2D, boxTexture);
	glUniform1i(glGetUniformLocation(shader -> Program, "texture"), 0);

	// Render container
	glBindVertexArray(boxVAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

	glBindVertexArray(0);
}

void SceneManager::Run() {
	// Game Loop
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();
		DoMovement();
		Render();
		glfwSwapBuffers(window);
	}
}

void SceneManager::Finish() {
	glfwTerminate();
}

void SceneManager::SetupCamera2D() {
	// Corrects aspect ratio
	float ratio;
	float xMin = -1.0, xMax = 1.0, yMin = -1.0, yMax = 1.0, zNear = -1.0, zFar = 1.0;

	if (width >= height) {
		ratio = width / (float)height;
		projection = glm::ortho(xMin*ratio, xMax*ratio, yMin, yMax, zNear, zFar);
	} else {
		ratio = height / (float)width;
		projection = glm::ortho(xMin, xMax, yMin*ratio, yMax*ratio, zNear, zFar);
	}

	GLint projLoc = glGetUniformLocation(shader->Program, "projection");
	glUniformMatrix4fv(projLoc, 1, GL_FALSE, glm::value_ptr(projection));
}

void SceneManager::SetupScene() {
	SetupBackground();
	RenderBackground();

	SetupForeground();
	RenderForeground();

	SetupBox();	
	RenderBox();

	SetupCharacter();
	RenderCharacter();
}

void SceneManager::SetupBackground(){
	/** 
	 * 3-float values collections, in order:
	 * 	Positions
	 * 	Colors
	 * 	Texture coordinates
	 * Lines order:
	 * 	Top right
	 * 	Bottom right
	 * 	Bottom left
	 * 	Top left
	**/
	float background[] = {
		 2.000f,	 1.000f, 0.0f,	1.0f, 0.0f, 0.0f,	 1.0,		 1.0,
		 2.000f,	-1.500f, 0.0f,	0.0f, 1.0f, 0.0f,	 1.0,		 0.0,
		-4.000f,	-1.500f, 0.0f,	0.0f, 0.0f, 1.0f,	 0.0,		 0.0,
		-4.000f,	 1.000f, 0.0f,	1.0f, 1.0f, 0.0f,	 0.0,		 1.0  
	};
	
	// First line = first triangle and so on
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};

	unsigned int bgVBO, bgEBO;

	glGenVertexArrays(1, &bgVAO);
	glGenBuffers(1, &bgVBO);
	glGenBuffers(1, &bgEBO);

	glBindVertexArray(bgVAO);

	glBindBuffer(GL_ARRAY_BUFFER, bgVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(background), background, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, bgEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	SetupBackgroundTexture();

	glBindVertexArray(0);
}

void SceneManager::SetupForeground(){
	/** 
	 * 3-float values collections, in order:
	 * 	Positions
	 * 	Colors
	 * 	Texture coordinates
	 * Lines order:
	 * 	Top right
	 * 	Bottom right
	 * 	Bottom left
	 * 	Top left
	**/
	float foreground[] = {
		 2.000f,	 1.000f, -1.0f,	0.5f, 0.5f, 0.5f,	 1.0,		 1.0,
		 2.000f,	-1.000f, -1.0f,	0.5f, 0.5f, 0.5f,	 1.0,		 0.0,
		-4.000f,	-1.000f, -1.0f,	0.5f, 0.5f, 0.5f,	 0.0,		 0.0,
		-4.000f,	 1.000f, -1.0f,	0.5f, 0.5f, 0.5f,	 0.0,		 1.0 
	};
	
	// First line = first triangle and so on
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};

	unsigned int fgVBO, fgEBO;

	glGenVertexArrays(1, &fgVAO);
	glGenBuffers(1, &fgVBO);
	glGenBuffers(1, &fgEBO);

	glBindVertexArray(fgVAO);

	glBindBuffer(GL_ARRAY_BUFFER, fgVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(foreground), foreground, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, fgEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	SetupForegroundTexture();

	glBindVertexArray(0);
}

void SceneManager::SetupCharacter(){
	/** 
	 * 3-float values collections, in order:
	 * 	Positions
	 * 	Colors
	 * 	Texture coordinates
	 * Lines order:
	 * 	Top right
	 * 	Bottom right
	 * 	Bottom left
	 * 	Top left
	**/
	float character[] = {
		 0.125f,	 0.239f, 1.0f,	1.0f, 0.0f, 0.0f,	1.0/4.0,	1.0/2.0,
		 0.125f,	-0.011f, 1.0f,	0.0f, 1.0f, 0.0f,	1.0/4.0,	0.0f,
		-0.125f,	-0.011f, 1.0f,	0.0f, 0.0f, 1.0f,	0.0f,		0.0f,
		-0.125f,	 0.239f, 1.0f,	1.0f, 1.0f, 0.0f,	0.0f,		1.0/2.0  
	};
	
	// First line = first triangle and so on
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};

	unsigned int charVBO, charEBO;

	glGenVertexArrays(1, &charVAO);
	glGenBuffers(1, &charVBO);
	glGenBuffers(1, &charEBO);

	glBindVertexArray(charVAO);

	glBindBuffer(GL_ARRAY_BUFFER, charVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(character), character, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, charEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	SetupCharacterTexture();

	glBindVertexArray(0);
}

void SceneManager::SetupBox(){
	/** 
	 * 3-float values collections, in order:
	 * 	Positions
	 * 	Colors
	 * 	Texture coordinates
	 * Lines order:
	 * 	Top right
	 * 	Bottom right
	 * 	Bottom left
	 * 	Top left
	**/
	float box[] = {
		 0.075f,	 0.125f, 1.0f,	1.0f, 0.0f, 0.0f,	 1.0f,		 1.0f,
		 0.075f,	-0.000f, 1.0f,	0.0f, 1.0f, 0.0f,	 1.0f,		 0.0f, 
		-0.075f,	-0.000f, 1.0f,	0.0f, 0.0f, 1.0f,	 0.0f,		 0.0f,
		-0.075f,	 0.125f, 1.0f,	1.0f, 1.0f, 0.0f,	 0.0f,		 1.0f  
	};

	// First line = first triangle and so on
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};

	unsigned int boxVBO, boxEBO;

	glGenVertexArrays(1, &boxVAO);
	glGenBuffers(1, &boxVBO);
	glGenBuffers(1, &boxEBO);

	glBindVertexArray(boxVAO);

	glBindBuffer(GL_ARRAY_BUFFER, boxVBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(box), box, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, boxEBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	SetupBoxTexture();

	glBindVertexArray(0);
}

void SceneManager::SetupBackgroundTexture(){
	glGenTextures(1, &backgroundTexture);
	glBindTexture(GL_TEXTURE_2D, backgroundTexture); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image, creates texture and generates mipmaps
	int bgWidth, bgHeight, bgNrChannels;
	unsigned char *bgData = stbi_load("Resources/Background.jpg", &bgWidth, &bgHeight, &bgNrChannels, 0);

	if (bgData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, bgWidth, bgHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, bgData);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load background texture" << std::endl;
	}
	stbi_image_free(bgData);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::SetupForegroundTexture(){
	glGenTextures(1, &foregroundTexture);
	glBindTexture(GL_TEXTURE_2D, foregroundTexture); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image, creates texture and generates mipmaps
	int fgWidth, fgHeight, fgNrChannels;
	unsigned char *fgData = stbi_load("Resources/Foreground.png", &fgWidth, &fgHeight, &fgNrChannels, 0);

	if (fgData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, fgWidth, fgHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, fgData);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load foreground texture" << std::endl;
	}
	stbi_image_free(fgData);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::SetupCharacterTexture() {
	glGenTextures(1, &characterTexture);
	glBindTexture(GL_TEXTURE_2D, characterTexture); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image, creates texture and generates mipmaps
	int charWidth, charHeight, charNrChannels;
	unsigned char *charData = stbi_load("Resources/Character.png", &charWidth, &charHeight, &charNrChannels, 0);

	if (charData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, charWidth, charHeight, 0, GL_RGBA, GL_UNSIGNED_BYTE, charData);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load main character texture" << std::endl;
	}
	stbi_image_free(charData);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::SetupBoxTexture(){
	glGenTextures(1, &boxTexture);
	glBindTexture(GL_TEXTURE_2D, boxTexture); 

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER);

	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image, creates texture and generates mipmaps
	int boxWidth, boxHeight, boxNrChannels;
	unsigned char *boxData = stbi_load("Resources/TNT.jpg", &boxWidth, &boxHeight, &boxNrChannels, 0);

	if (boxData) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, boxWidth, boxHeight, 0, GL_RGB, GL_UNSIGNED_BYTE, boxData);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load TNT box texture" << std::endl;
	}
	stbi_image_free(boxData);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

bool SceneManager::TestCollision(){
	if (characterPosition <= boxPosition + 0.075)
		return true;
	return false;
}