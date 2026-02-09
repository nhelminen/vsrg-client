#include "core/screens/debugScreen.hpp"

#include "core/debug.hpp"
#include "core/engine/audio.hpp"
#include "core/engine/shader.hpp"
#include "core/utils.hpp"
#include "public/engineContext.hpp"


namespace vsrg {
DebugScreen::DebugScreen(EngineContext *engine_context)
    : Screen(engine_context, "DebugScreen", 1),
      text_component(engine_context,
                     engine_context->get_font_manager()->getFont("NotoSansJP-Regular.ttf")) {
    ComponentProperties properties = {
        true, 1.0f, 0.0f, 0, {16.0f, 16.0f}, {1.0f, 1.0f}, {0.0f, 0.0f},
    };
    TextRenderOptions text_options = {{1.0f, 1.0f, 1.0f}, 12.0f, 4.0f};

    text_component.setText("?");
    text_component.setProperties(properties);
    text_component.setTextOptions(text_options);

    Debugger *debugger = engine_context->get_debugger();
    debugger->log(DebugLevel::INFO, "DebugScreen loaded", __FILE__, __LINE__);

    gameplay_plugin = engine_context->get_plugin_manager()->find_plugin("mania");
    if (gameplay_plugin != nullptr) {
        engine_context->get_plugin_manager()->activate_plugin(gameplay_plugin->get_info().name);
        gameplay_plugin->load();
    } else {
        engine_context->get_debugger()->log(DebugLevel::INFO, "plugin doesnt exist", __FILE__,
                                            __LINE__);
    }
}

DebugScreen::~DebugScreen() {
    engine_context->get_debugger()->log(DebugLevel::INFO, "DebugScreen unloaded", __FILE__,
                                        __LINE__);
}

void DebugScreen::update(float delta_time) {
    if (gameplay_plugin) {
        gameplay_plugin->update(delta_time);
    }

    float fps = getFPS(delta_time);
    std::string memory = getFormattedMemoryUsage();

    std::stringstream textData;
    textData << "FPS: " << static_cast<int>(fps) << "\n";
    textData << "Memory: " << memory << "\n";

    text_component.setText(textData.str());
}

void DebugScreen::render() {
    if (gameplay_plugin) {
        gameplay_plugin->render();
    }

    text_component.render();
}
}  // namespace vsrg
