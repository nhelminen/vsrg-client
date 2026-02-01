#pragma once

#include <string>
#include <vector>

namespace vsrg
{
    class Client;
    enum class ScreenState
    {
        ACTIVE,
        INACTIVE
    };

    class Screen
    {
    public:
        Screen(Client *client, const std::string &name)
            : client(client), name(name)
        {
        }
        virtual ~Screen() = default;

        virtual void load() = 0;
        virtual void unload() = 0;

        virtual void update(float delta_time) = 0;
        virtual void render() = 0;

        ScreenState get_state() const { return state; }
        void set_state(ScreenState new_state) { state = new_state; }
        std::string get_name() const { return name; }

        int get_z_order() const { return z_order; }
        void set_z_order(int z) { z_order = z; }

        bool is_active() const { return state == ScreenState::ACTIVE; }
    protected:
        Client* client;
        std::string name;
        ScreenState state = ScreenState::INACTIVE;

        int z_order = 0;
    };

    class ScreenManager
    {
    public:
        ScreenManager(Client *client);
        ~ScreenManager();

        void add_screen(Screen *screen);
        void remove_screen(const std::string &name);

        void update(float delta_time);
        void render();

        void clear();

    private:
        Client *client;
        std::vector<Screen *> screens;
    };
}