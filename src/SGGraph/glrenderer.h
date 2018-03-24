/*
 * Copyright (c) 2018 Mohamed Emam
 * The code is licensed under the MIT License.
 * You can check the file LICENSE for the full license.
 *
 * A class for opening and managing windows.
 */

#ifndef SGLGLRENDERER_H
#define SGLGLRENDERER_H

#include <glad\glad.h>
#include <GLFW\glfw3.h>
#include <vector>

namespace SGGraph
{
	typedef GLFWwindow* GLWindow;

	class GLRenderer
	{
	public:		
		// Lazy Renderer creation, destroyed at program termination. (thread safe in C++11)
		static GLRenderer& createRenderer();

		// Makes sure to not get copies.
		GLRenderer(const GLRenderer&) = delete;
		void operator=(const GLRenderer&) = delete;

		~GLRenderer();

		// Functions
		GLWindow openWindow(const int width, const int height, const char* name = "window");
		GLWindow getCurrentWindow();
		int getWindowIndex(GLWindow w);
		void setCurrentWindow(int index);
		void setCurrentWindow(GLWindow w);
		void closeWindow(int index);
		void closeWindow(GLWindow w);

	private:
		GLRenderer();

		std::vector<GLWindow> windows;
		GLWindow currentWindow;
		bool failure = false;
	};
}

inline SGGraph::GLWindow SGGraph::GLRenderer::getCurrentWindow() { return currentWindow; }

#endif