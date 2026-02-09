#include "core/engine/screen.hpp"

#include <algorithm>

#include "public/engineContext.hpp"


namespace vsrg {
void Screen::set_z_order(int z) {
    z_order = z;
    if (engine_context != nullptr) engine_context->get_screen_manager()->mark_dirty();
}

ScreenManager::ScreenManager(EngineContext *engine_context) : engine_context(engine_context) {}

ScreenManager::~ScreenManager() {
    clear();
}

void ScreenManager::add_screen(std::unique_ptr<Screen> screen) {
    screen->set_state(ScreenState::ACTIVE);
    screens.push_back(std::move(screen));

    needs_sort = true;
}

void ScreenManager::remove_screen(const std::string &name) {
    for (auto it = screens.begin(); it != screens.end(); ++it) {
        if ((*it)->get_name() == name) {
            (*it)->set_state(ScreenState::INACTIVE);
            screens.erase(it);
            break;
        }
    }
}

void ScreenManager::update(float delta_time) {
    for (auto &screen : screens) {
        if (screen->is_active()) {
            screen->update(delta_time);
        }
    }
}

void ScreenManager::render() {
    if (needs_sort) {
        std::sort(screens.begin(), screens.end(),
                  [](const auto &a, const auto &b) { return a->get_z_order() < b->get_z_order(); });
        needs_sort = false;
    }

    for (auto &screen : screens) {
        if (screen->is_active()) {
            screen->render();
        }
    }
}

void ScreenManager::clear() {
    for (auto &screen : screens) {
        screen->set_state(ScreenState::INACTIVE);
    }
    screens.clear();
}
}  // namespace vsrg