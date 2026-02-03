#include "core/app.hpp"
#include "core/screens/initScreen.hpp"

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

		SDL_GL_SetSwapInterval(0);

		if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress))
		{
			SDL_GL_DestroyContext(gl_context);
			SDL_DestroyWindow(window);
			SDL_Quit();
			gl_initialized = false;
			return;
		}

		engine_context = new EngineContext(this);

		debugger = new Debugger();

		VSRG_LOG(*debugger, DebugLevel::INFO, "Debugger initialized");
		VSRG_LOG(*debugger, DebugLevel::INFO, "OpenGL Loaded");

		const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
		const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
		const char *version = reinterpret_cast<const char *>(glGetString(GL_VERSION));

		if (vendor)
			VSRG_LOG(*debugger, DebugLevel::INFO, std::string("Vendor: ") + vendor);
		if (renderer)
			VSRG_LOG(*debugger, DebugLevel::INFO, std::string("Renderer: ") + renderer);
		if (version)
			VSRG_LOG(*debugger, DebugLevel::INFO, std::string("Version: ") + version);

		glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

		// Initialize other stuff
		audio_manager = new AudioManager();
		if (!audio_manager->is_initialized())
		{
			// why is this a syntax error??? i think my editor is tweaking
			VSRG_LOG(*debugger, DebugLevel::ERROR, "Failed to initialize AudioManager");
			gl_initialized = false;
			return;
		}
		else
		{
			VSRG_LOG(*debugger, DebugLevel::INFO, "AudioManager initialized successfully");
		}

		screen_manager = new ScreenManager(engine_context);
		VSRG_LOG(*debugger, DebugLevel::INFO, "ScreenManager created");

		engine_context->update();
		screen_manager->add_screen(std::make_unique<InitScreen>(engine_context));

		gl_initialized = true;
	}

	Client::~Client()
	{
		if (screen_manager != nullptr)
		{
			delete screen_manager;
			screen_manager = nullptr;
		}
		if (audio_manager != nullptr)
		{
			delete audio_manager;
			audio_manager = nullptr;
		}
		if (engine_context != nullptr)
		{
			delete engine_context;
			engine_context = nullptr;
		}
		if (debugger != nullptr)
		{
			delete debugger;
			debugger = nullptr;
		}

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

		// initialize timing (not the final solution, need a separate thread for timing management for accurate results)
		last_time = Clock::now();

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

			// temporarily here till i multithread properly
			auto current_time = Clock::now();
			delta_time = std::chrono::duration<float>(current_time - last_time).count();
			last_time = current_time;

			// think we might need 2 threads eventually, one for logic and one for rendering
			// but for now this is fine?
			screen_manager->update(delta_time);
			screen_manager->render();

			SDL_GL_SwapWindow(window);
		}
	}
}