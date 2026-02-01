#pragma once

#include <memory>
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
        Screen(Client *client, const std::string &name, int z_order = 0)
            : client(client), name(name), z_order(z_order)
        {
        }
        virtual ~Screen() = default;

        virtual void update(float delta_time) = 0;
        virtual void render() = 0;

        ScreenState get_state() const { return state; }
        void set_state(ScreenState new_state) { state = new_state; }
        std::string get_name() const { return name; }

        int get_z_order() const { return z_order; }
        void set_z_order(int z);

        bool is_active() const { return state == ScreenState::ACTIVE; }

    protected:
        Client *client;
        std::string name;
        ScreenState state = ScreenState::INACTIVE;

        int z_order = 0;
    };

    class ScreenManager
    {
    public:
        ScreenManager(Client *client);
        ~ScreenManager();

        void add_screen(std::unique_ptr<Screen> screen);
        void remove_screen(const std::string &name);

        void update(float delta_time);
        void render();

        void clear();
        void mark_dirty() { needs_sort = true; }

    private:
        Client *client;
        std::vector<std::unique_ptr<Screen>> screens;

        bool needs_sort = false;
    };
}