#include "core/screen.hpp"

#include <algorithm>

namespace vsrg
{
    ScreenManager::ScreenManager(Client *client)
        : client(client)
    {
    }

    ScreenManager::~ScreenManager()
    {
        clear();
    }

    void ScreenManager::add_screen(Screen *screen)
    {
        screens.push_back(screen);
        screen->set_state(ScreenState::ACTIVE);
        screen->load();
    }

    void ScreenManager::remove_screen(const std::string &name)
    {
        for (auto it = screens.begin(); it != screens.end(); ++it)
        {
            if ((*it)->get_name() == name)
            {
                (*it)->set_state(ScreenState::INACTIVE);
                (*it)->unload();
                screens.erase(it);
                break;
            }
        }
    }

    void ScreenManager::update(float delta_time)
    {
        for (auto &screen : screens)
        {
            if (screen->is_active())
            {
                screen->update(delta_time);
            }
        }
    }

    void ScreenManager::render()
    {
        // sort screens by z-order before rendering
        std::sort(screens.begin(), screens.end(),
                  [](Screen *a, Screen *b)
                  { return a->get_z_order() < b->get_z_order(); });

        for (auto &screen : screens)
        {
            if (screen->is_active())
            {
                screen->render();
            }
        }
    }

    void ScreenManager::clear()
    {
        for (auto &screen : screens)
        {
            delete screen;
        }
        screens.clear();
    }
}