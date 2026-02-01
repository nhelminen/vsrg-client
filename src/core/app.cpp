#include "core/app.hpp"

#include <iostream>

namespace vsrg
{
	Client::Client(int screen_width, int screen_height)
		: SCREEN_WIDTH(screen_width), SCREEN_HEIGHT(screen_height),
		  window(nullptr), gl_context(nullptr)
	{
		if (!SDL_Init(SDL_INIT_VIDEO))
		{
			gl_initialized = false;
			return;
		}

		SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
		SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
		SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

		window = SDL_CreateWindow("VSRG Client", SCREEN_WIDTH, SCREEN_HEIGHT, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

		if (window == nullptr)
		{
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		gl_context = SDL_GL_CreateContext(window);
		if (gl_context == nullptr)
		{
			SDL_DestroyWindow(window);
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			SDL_GL_DestroyContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		debugger = new Debugger();
		VSRG_LOG(*debugger, DebugLevel::INFO, "OpenGL Loaded");
		VSRG_LOG(*debugger, DebugLevel::INFO, std::string("Vendor: ") + reinterpret_cast<const char*>(glGetString(GL_VENDOR)));
		VSRG_LOG(*debugger, DebugLevel::INFO, std::string("Renderer: ") + reinterpret_cast<const char*>(glGetString(GL_RENDERER)));
		VSRG_LOG(*debugger, DebugLevel::INFO, std::string("Version: ") + reinterpret_cast<const char*>(glGetString(GL_VERSION)));

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// Initialize other stuff
		AudioManager* audio_manager = new AudioManager();
		if (!audio_manager->is_initialized()) {
			VSRG_LOG(*debugger, DebugLevel::ERROR, "Failed to initialize AudioManager");
		} else {
			VSRG_LOG(*debugger, DebugLevel::INFO, "AudioManager initialized successfully");
		}

		gl_initialized = true;
	}

	Client::~Client()
	{
		if (!gl_initialized)
			return;

		if (gl_context != nullptr)
			SDL_GL_DestroyContext(gl_context);
		if (window != nullptr)
			SDL_DestroyWindow(window);
		SDL_Quit();
	}

	void Client::start()
	{
		if (!gl_initialized)
			return;

		glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
		glEnable(GL_BLEND);
		glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		SDL_Event event;
		while (!should_close)
		{
			while (SDL_PollEvent(&event))
			{
				if (event.type == SDL_EVENT_QUIT)
					should_close = true;
				if (event.type == SDL_EVENT_WINDOW_PIXEL_SIZE_CHANGED)
				{
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
}