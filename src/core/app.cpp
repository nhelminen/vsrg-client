#include "core/app.hpp"

#include <iostream>

#include "core/screens/initScreen.hpp"
#include "core/utils.hpp"


namespace vsrg {
Client::Client(int screen_width, int screen_height)
    : SCREEN_WIDTH(screen_width),
      SCREEN_HEIGHT(screen_height),
      window(nullptr),
      gl_context(nullptr) {
    if (!SDL_Init(SDL_INIT_VIDEO)) {
        gl_initialized = false;
        return;
    }

    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);

    window = SDL_CreateWindow("VSRG Client", SCREEN_WIDTH, SCREEN_HEIGHT,
                              SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);

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

    // nvm im limiting the fps again
    // SDL_GL_SetSwapInterval(0);

    if (!gladLoadGLLoader((GLADloadproc)SDL_GL_GetProcAddress)) {
        SDL_GL_DestroyContext(gl_context);
        SDL_DestroyWindow(window);
        SDL_Quit();
        gl_initialized = false;
        return;
    }

    engine_context = new EngineContext(this);
    glViewport(0, 0, SCREEN_WIDTH, SCREEN_HEIGHT);

    Debugger *debugger = engine_context->get_debugger();

    VSRG_LOG(*debugger, DebugLevel::INFO, "Game initialized");
    VSRG_LOG(*debugger, DebugLevel::INFO, "OpenGL Loaded");

    const char *vendor = reinterpret_cast<const char *>(glGetString(GL_VENDOR));
    const char *renderer = reinterpret_cast<const char *>(glGetString(GL_RENDERER));
    const char *version = reinterpret_cast<const char *>(glGetString(GL_VERSION));

    if (vendor) VSRG_LOG(*debugger, DebugLevel::INFO, std::string(" Vendor: ") + vendor);
    if (renderer) VSRG_LOG(*debugger, DebugLevel::INFO, std::string(" Renderer: ") + renderer);
    if (version) VSRG_LOG(*debugger, DebugLevel::INFO, std::string(" Version: ") + version);

    std::string execDir = getExecutableDir();
    std::string pluginDir = joinPaths(execDir, "plugins");

    engine_context->get_plugin_manager()->discover_plugins(pluginDir);
    engine_context->get_screen_manager()->add_screen(std::make_unique<InitScreen>(engine_context));

    gl_initialized = true;
}

Client::~Client() {
    if (engine_context != nullptr) {
        delete engine_context;
        engine_context = nullptr;
    }

    if (gl_context != nullptr) SDL_GL_DestroyContext(gl_context);
    if (window != nullptr) SDL_DestroyWindow(window);

    SDL_Quit();
}

void Client::start() {
    if (!gl_initialized) return;

    glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    glEnable(GL_DEPTH_TEST);
    glClearDepth(1.0);
    glDepthFunc(GL_LEQUAL);

    last_time = Clock::now();

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

        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        // temporarily here till i multithread properly
        auto current_time = Clock::now();
        delta_time = std::chrono::duration<float>(current_time - last_time).count();
        last_time = current_time;

        // think we might need 2 threads eventually, one for logic and one for
        // rendering but for now this is fine?
        engine_context->get_screen_manager()->update(delta_time);
        engine_context->get_screen_manager()->render();

        SDL_GL_SwapWindow(window);
    }
}
}  // namespace vsrg
