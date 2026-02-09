#pragma once

#include <atomic>
#include <future>
#include <vector>

#include "core/ui/solidComponent.hpp"
#include "core/ui/sprite.hpp"
#include "public/engineContext.hpp"
#include "rhythm/charts/chartData.hpp"
#include "rhythm/conductor.hpp"
#include "rhythm/holdNote.hpp"
#include "rhythm/math/scroll.hpp"
#include "rhythm/note.hpp"
#include "rhythm/strum.hpp"

namespace mania {
class Playfield : public vsrg::SolidComponent {
public:
    Playfield(vsrg::EngineContext *ctx, const ChartData *chart_data, vsrg::Conductor *conductor,
              int key_count, glm::vec4 background_color = glm::vec4(0.0f, 0.0f, 0.0f, 0.8f));
    ~Playfield();

    void update(float delta_time);
    void render() override;

    const ChartData *getChartData() const { return chart_data; }
    vsrg::Conductor *getConductor() const { return conductor; }

    glm::vec2 getSize() const override;

    void setScrollSpeed(float speed, ScrollSpeedMode mode) {
        scroll_speed = speed;
        switch (mode) {
            case ScrollSpeedMode::XMOD:
                scroll_calculator.setXMod(scroll_speed);
            default:
                scroll_calculator.setCMod(scroll_speed);
        }
    }

    float getScrollSpeed() const { return scroll_speed; }
    ScrollSpeedCalculator *getScrollSpeedCalculator() { return &scroll_calculator; }

    float getStrumLineY() const { return strum_line_y; }
    float getPlayfieldWidth() const { return properties.render_size.x; }

    void setPosition(float x, float y) {
        properties.position = glm::vec2(x, y);
        updateStrumPositions();
    }

    bool isLoading() const { return is_loading.load(); }

private:
    vsrg::EngineContext *engine_context;

    const ChartData *chart_data;
    vsrg::Conductor *conductor;

    int key_count;
    std::vector<Strum *> strums;
    std::vector<Note *> notes;

    float scroll_speed;
    float strum_line_y;

    ScrollSpeedCalculator scroll_calculator;

    std::atomic<bool> is_loading;
    std::future<void> loading_task;

    void createNotes();
    void createNotesAsync();
    void updateStrumPositions();
};
}  // namespace mania