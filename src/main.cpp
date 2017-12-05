#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <iostream>
#include <string>
#include <filesystem>
#include <vector>
#include <thread>

#include "shader.h"

void framebuffer_size_callback(GLFWwindow* window, int width, int height);
void processInput(GLFWwindow* window);
std::string mainMenu();
void render(Shader& shader, GLFWwindow* window, const GLuint& VAO);
void handleConsoleOperation(bool& quit, bool& run, Shader& s);

int main()
{
	bool run = true;
	std::string fragPath = mainMenu();
	if (fragPath == "")
		return 0;

	// initialize glfw
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	 GLFWwindow* window = glfwCreateWindow(800, 800, "Shadart", NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create GLFW window" << std::endl;
		glfwTerminate();
	}
	glfwMakeContextCurrent(window);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// initialize glad
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
	}

	Shader shader("Shaders/vertex.vs", fragPath.c_str());

	// Full ViewPort Rectangle
	float vertices[] =
	{
		1.0f,  1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		-1.0f,  1.0f, 0.0f,
	};

	unsigned int indices[] =
	{
		0, 1, 3,
		1, 2, 3
	};

	GLuint VAO, VBO, EBO;
	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);

	glBindVertexArray(0);

	bool quit = false;
	std::thread console(handleConsoleOperation, std::ref(quit), std::ref(run), std::ref(shader));

	// render loop
	while (!glfwWindowShouldClose(window) && !quit)
	{
		processInput(window);

		if (!run)
			continue;

		render(shader, window, VAO);
		
		glfwPollEvents();
	}

	console.join();

	glfwTerminate();
	return 0;
}

void processInput(GLFWwindow* window)
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height)
{
	glViewport(0, 0, width, height);
}

std::string mainMenu()
{
	std::cout << "Welcome to Shadart" << std::endl;
	std::cout << "Choose an option below:" << std::endl;
	std::cout << "A. Create New Shader" << std::endl;
	std::cout << "B. Load Shader" << std::endl;
	std::cout << "C. Quit" << std::endl;

	char option[2];
	std::cin.getline(option, 2);

	while(option[0] != 'A' && option[0] != 'a' && option[0] != 'B' && option[0] != 'b' && option[0] != 'C' && option[0] != 'c')
		std::cin.getline(option, 2);

	switch (option[0])
	{
		case 'A':
		case 'a':
			break;
		case 'B':
		case 'b':
		{
			std::vector<std::string> files;
			std::string path = "Shaders/";
			int index = 0;
			for (auto &p : std::experimental::filesystem::directory_iterator(path)) //needs C++17
			{
				std::ostringstream oss;
				oss << p;
				std::string dir = oss.str();
				if (dir.substr(8) == "vertex.vs")
					continue;
				files.push_back(dir);
				std::cout << index++ << ". " << dir.substr(8) << std::endl;
			}
			std::cout << "Choose a shader:" << std::endl;
			std::cin >> index;
			return files[index];
			break;
		}
		case 'C':
		case 'c':
			return "";
	}
	return "Shaders/fragment.frag";
}

void render(Shader& shader, GLFWwindow* window, const GLuint& VAO)
{
	glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT);

	shader.use();

	float time = glfwGetTime();
	int wwidth, wheight;
	glfwGetWindowSize(window, &wwidth, &wheight);

	shader.setUniform1f("uTime", time);
	shader.setUniform2f("uResolution", wwidth, wheight);

	glBindVertexArray(VAO);
	glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
	glBindVertexArray(0);

	glfwSwapBuffers(window);
}

void handleConsoleOperation(bool& q, bool& run, Shader& s)
{
	while (!q)
	{
		std::cout << "Controls:" << std::endl;
		std::cout << "press p to play/pause" << std::endl;
		std::cout << "press m to go back to main menu" << std::endl;

		char option[2];
		std::cin.getline(option, 2);

		switch (option[0])
		{
		case 'p':
		case 'P':
			run = !run; // invert
			break;
		case 'm':
		case 'M':
		{
			run = false;
			const std::string fragPath = mainMenu();
			if (fragPath == "")
			{
				q = true;
				break;
			}
			s.reCompile("Shaders/vertex.vs", fragPath.c_str());
			run = true;
			break;
		}
		default:
			// no op
			break;
		}
	}
}