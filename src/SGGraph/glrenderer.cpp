/*
 * Copyright (c) 2018 Mohamed Emam
 * The code is licensed under the MIT License.
 * You can check the file LICENSE for the full license.
 */

#include "glrenderer.h"
#include <iostream>

using namespace SGGraph;

#define GLVERSIONMAJOR 3
#define GLVERSIONMINOR 3


GLRenderer::GLRenderer()
{
	glfwInit();
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, GLVERSIONMAJOR);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, GLVERSIONMINOR);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
}

GLRenderer::~GLRenderer() 
{
	glfwTerminate();
}

GLRenderer& GLRenderer::createRenderer()
{
	static GLRenderer instance;
	return instance;
}

GLWindow GLRenderer::openWindow(const int width, const int height, const char* name)
{
	GLFWwindow* window = glfwCreateWindow(width, height, name, NULL, NULL);
	if (window == NULL)
	{
		std::cout << "Failed to create the GLFW window" << std::endl;
		glfwTerminate();
		failure = true;
	}
	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cout << "Failed to initialize GLAD" << std::endl;
		failure = true;
	}

	currentWindow = window;
	windows.push_back(window);

	return window;
}

int GLRenderer::getWindowIndex(GLWindow w)
{
	for (int i = 0; i < windows.size(); i++)
		if (w == windows[i])
			return i;

	return -1;
}

void GLRenderer::setCurrentWindow(int index)
{
	if (index < windows.size())
	{
		currentWindow = windows[index];
		glfwMakeContextCurrent(currentWindow);
		glfwFocusWindow(currentWindow);
	}
}

void GLRenderer::setCurrentWindow(GLWindow w)
{
	if (w)
	{
		currentWindow = w;
		glfwMakeContextCurrent(currentWindow);
		glfwFocusWindow(currentWindow);
	}
}

void GLRenderer::closeWindow(int index)
{
	if (index < windows.size())
	{
		int i = getWindowIndex(currentWindow);
		glfwDestroyWindow(windows[index]);
		windows.erase(windows.begin() + index);
		if (index == i)
			setCurrentWindow(index - 1);
	}
}

void GLRenderer::closeWindow(GLWindow w)
{
	int index = getWindowIndex(w);
	if (index != -1)
	{
		int i = getWindowIndex(currentWindow);
		glfwDestroyWindow(w);
		windows.erase(windows.begin() + index);
		if (index == i)
			setCurrentWindow(index - 1);
	}
}