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
	horizontalPosition = 0;

	glfwInit();

	window = glfwCreateWindow(width, height, "Game", nullptr, nullptr);
	glfwMakeContextCurrent(window);

	// Set the required callback functions
	glfwSetKeyCallback(window, key_callback);

	glfwSetWindowSizeCallback(window, Resize);

	// glad: load all OpenGL function pointers
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
		std::cout << "Failed to initialize GLAD" << std::endl;

	AddShader("Shaders/Character.vs", "Shaders/Character.frag");

	SetupScene();

	// Forcing camera setup on first run
	resized = true;
}

void SceneManager::AddShader(string vFilename, string fFilename) {
	shader = new Shader(vFilename.c_str(), fFilename.c_str());
}

void SceneManager::key_callback(GLFWwindow * window, int key, int scanCode, int action, int mode) {
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
	glViewport(0, 0, width, height);
}


void SceneManager::DoMovement() {
	if (keys[GLFW_KEY_LEFT])
		if ((horizontalPosition - 0.0025) > -0.925) 
			horizontalPosition -= 0.0025f;

	if (keys[GLFW_KEY_RIGHT])
		if ((horizontalPosition + 0.0025) < 0.925) 
			horizontalPosition += 0.0025f;

	if (keys[GLFW_KEY_ESCAPE])
		glfwSetWindowShouldClose(window, GL_TRUE);
}

void SceneManager::Render() {
	// Clear the colorbuffer
	glClearColor(0.5f, 0.5f, 0.5f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Render scene
	shader -> Use();

	// Create transformations 
	model = glm::mat4();
	model = glm::translate(model, glm::vec3(horizontalPosition, 0, 0));

	GLint modelLoc = glGetUniformLocation(shader->Program, "model");

	// Passes transformations to Shaders
	glUniformMatrix4fv(modelLoc, 1, GL_FALSE, glm::value_ptr(model));

	if (resized) {
		SetupCamera2D();
		resized = false;
	}
	
	glBindTexture(GL_TEXTURE_2D, texture);
	glUniform1i(glGetUniformLocation(shader -> Program, "texture"), 0);

    // Render container
    glBindVertexArray(VAO);
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


void SceneManager::SetupScene() {

	/** 
	 * 3-float values collections, in order:
	 * 	Positions
	 * 	Colors
	 * 	Texture coordinates
	 * Lines order:
	 * 	Top right
	 * 	Bottom right
	 * 	Bottom horizontalPosition
	 * 	Top horizontalPosition
	**/
	float character[] = {
		 0.125f,	 0.125f, 0.0f,	1.0f, 0.0f, 0.0f,	1.0/4.0, 1.0/2.0,
		 0.125f,	-0.125f, 0.0f,	0.0f, 1.0f, 0.0f,	1.0/4.0, 0.0f,
		-0.125f,	-0.125f, 0.0f,	0.0f, 0.0f, 1.0f,	0.0f, 0.0f,
		-0.125f,	 0.125f, 0.0f,	1.0f, 1.0f, 0.0f,	0.0f, 1.0/2.0  
	};

	// First line = first triangle and so on
	unsigned int indices[] = {
		0, 1, 3,
		1, 2, 3 
	};

	unsigned int VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(character), character, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coords
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

	SetupTexture();

	glBindVertexArray(0);
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

void SceneManager::SetupTexture() {
	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture); 
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	// Loads image, creates texture and generates mipmaps
	int width, height, nrChannels;
	unsigned char *data = stbi_load("Resources/Character.png", &width, &height, &nrChannels, 0);
	
	if (data) {
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	} else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

	glBindTexture(GL_TEXTURE_2D, 0);

	glActiveTexture(GL_TEXTURE0);

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}

void SceneManager::SetupBackground(){

}