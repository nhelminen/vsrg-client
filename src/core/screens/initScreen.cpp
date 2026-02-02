#include "core/screens/initScreen.hpp"
#include "core/debug.hpp"
#include "public/engineContext.hpp"

#include "core/screens/testScreen.hpp"
#include <glad/glad.h>

namespace vsrg {
    InitScreen::InitScreen(EngineContext* engine_context)
        : Screen(engine_context, "InitScreen", 0)
    {
        engine_context->get_debugger()->log(DebugLevel::INFO, "InitScreen loaded", __FILE__, __LINE__);
        // for testing, add test screen after init
        engine_context->get_screen_manager()->add_screen(std::make_unique<TestScreen>(engine_context));
    }

    InitScreen::~InitScreen()
    {
        engine_context->get_debugger()->log(DebugLevel::INFO, "InitScreen unloaded", __FILE__, __LINE__);
    }

    void InitScreen::update(float delta_time)
    {
        // update, use this for any time-based changes
    }

    void InitScreen::render()
    {
        glClearColor(0.2f, 0.3f, 0.4f, 1.0f);
    }
}