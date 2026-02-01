#pragma once

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

#include "core/audio.hpp"
#include "core/debug.hpp"
#include "core/screen.hpp"

namespace vsrg {
	class Client {
	public:
		Client(int screen_width = 1280, int screen_height = 720);
		~Client();

		Client(const Client&) = delete;
		Client& operator=(const Client&) = delete;
		Client(Client&&) = delete;
		Client& operator=(Client&&) = delete;

		void start();
		bool is_initialized() const { return gl_initialized; }

		Debugger* get_debugger() const { return debugger; }
		AudioManager* get_audio_manager() const { return audio_manager; }
		ScreenManager* get_screen_manager() const { return screen_manager; }
	private:
		Uint64 last_time;
		float delta_time;

		bool gl_initialized = false;
		bool should_close = false;

		SDL_Window* window;
		SDL_GLContext gl_context;

		Debugger* debugger = nullptr;
		AudioManager* audio_manager = nullptr;
		ScreenManager* screen_manager = nullptr;
		
		int SCREEN_WIDTH;
		int SCREEN_HEIGHT;
	};
}