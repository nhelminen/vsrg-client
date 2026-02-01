#pragma once

#include "core/screen.hpp"

#include <glad/glad.h>
#include <SDL3/SDL.h>
#include <SDL3/SDL_opengl.h>
#define GLM_FORCE_RADIANS
#include <glm/glm.hpp>

namespace vsrg {
    class Client;

    class InitScreen : public Screen {
    public:
        InitScreen(Client* client);
        ~InitScreen() override;

        void load() override;
        void unload() override;

        void update(float delta_time) override;
        void render() override;
    };
}