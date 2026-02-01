#pragma once

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "core/audio.hpp"

namespace vsrg {
	class Client {
	public:
		Client(int screen_width = 1280, int screen_height = 720);
		~Client();

		void start();
		bool is_initialized() const { return gl_initialized; }
	private:
		bool gl_initialized = false;
		bool should_close = false;

		SDL_Window* window;
		SDL_GLContext gl_context;
		AudioManager* audio_manager;

		int SCREEN_WIDTH;
		int SCREEN_HEIGHT;
	};
}