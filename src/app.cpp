#include "app.hpp"

#include <iostream>

namespace vsrg {

	bool gl_initialized = false;
	bool should_close = false;
	SDL_Window* window = nullptr;
	SDL_GLContext gl_context = nullptr;
	int SCREEN_WIDTH = 800;
	int SCREEN_HEIGHT = 600;

	void initialize() {
		if (!SDL_Init(SDL_INIT_VIDEO)) {
			gl_initialized = false;
			return;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		window = SDL_CreateWindow("VSRG Client", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

		if (window == nullptr) {
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		gl_context = SDL_GL_CreateContext(window);
		if (gl_context == nullptr) {
			SDL_DestroyWindow(window);
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
			SDL_GL_DestroyContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		std::cout << "OpenGL Loaded" << std::endl;
		std::cout << "Vendor: " << glGetString(GL_VENDOR) << std::endl;
		std::cout << "Renderer: " << glGetString(GL_RENDERER) << std::endl;
		std::cout << "Version: " << glGetString(GL_VERSION) << std::endl;

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// Initializate other stuff here

		gl_initialized = true;
	}

	void run() {
		if (!gl_initialized) return;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		SDL_Event event;
		while (!should_close) {
			while (SDL_PollEvent(&event)) {
				if (event.type == SDL_EVENT_QUIT) should_close = true;
				if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED) {
					SCREEN_WIDTH = (int)event.window.data1;
					SCREEN_HEIGHT = (int)event.window.data2;
					glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);
				}
			}

			glClear(GL_COLOR_BUFFER_BIT);

			// Rendering here

			SDL_GL_SwapWindow(window);

		}

	}

	void destroy() {
		if (!gl_initialized) return;

		SDL_GL_DestroyContext(gl_context);
		SDL_DestroyWindow(window);
		SDL_Quit();
	}

}