#include "rhythm/playfield.hpp"

#include "core/debug.hpp"


namespace mania {
Playfield::Playfield(vsrg::EngineContext *ctx, const ChartData *chart_data,
                     vsrg::Conductor *conductor, int key_count, glm::vec4 background_color)
    : vsrg::SolidComponent(ctx, background_color),
      engine_context(ctx),
      chart_data(chart_data),
      conductor(conductor),
      key_count(key_count),
      scroll_speed(1600.0f),
      is_loading(false),
      scroll_calculator(conductor) {
    VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO,
             "creating playfield with " + std::to_string(key_count) +
                 " keys, scroll speed: " + std::to_string(scroll_speed));

    float screen_width = static_cast<float>(ctx->get_screen_width());
    float screen_height = static_cast<float>(ctx->get_screen_height());

    float strum_width = 96.0f;
    float strum_spacing = 0.0f;
    float total_width = (strum_width * key_count) + (strum_spacing * (key_count - 1));

    float start_x = (screen_width - total_width) / 2.0f;
    strum_line_y = screen_height - 128.0f;

    properties.position = glm::vec2(start_x - strum_spacing, 0.0f);
    properties.render_size = glm::vec2(total_width + (strum_spacing * 2.0f), screen_height);
    properties.layer = 1;

    VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO,
             "set playfield size to " + std::to_string(properties.render_size.x) + "x" +
                 std::to_string(properties.render_size.y));

    VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "pre-loading textures...");

    ctx->get_texture_cache()->getTexture("strum.png");
    ctx->get_texture_cache()->getTexture("note.png");
    ctx->get_texture_cache()->getTexture("mine.png");
    ctx->get_texture_cache()->getTexture("holdBody.png");
    ctx->get_texture_cache()->getTexture("holdEnd.png");

    for (int i = 0; i < key_count; ++i) {
        std::string suffix = std::to_string(i) + ".png";
        ctx->get_texture_cache()->getTexture("strum" + suffix);
        ctx->get_texture_cache()->getTexture("note" + suffix);
        ctx->get_texture_cache()->getTexture("holdBody" + suffix);
        ctx->get_texture_cache()->getTexture("holdEnd" + suffix);
    }

    VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "textures pre-loaded!");

    for (int i = 0; i < key_count; i++) {
        float x = start_x + (i * (strum_width + strum_spacing));

        Strum *strum = new Strum(ctx, "strum.png", i);
        strum->setPosition(x, strum_line_y);
        strum->setSize(strum_width, strum_width);

        strums.push_back(strum);
    }

    if (chart_data) {
        VSRG_LOG(*ctx->get_debugger(), vsrg::DebugLevel::INFO, "starting async note loading...");
        is_loading.store(true);
        loading_task = std::async(std::launch::async, [this]() { this->createNotesAsync(); });
    }

    setScrollSpeed(scroll_speed / conductor->get_playback_rate(), ScrollSpeedMode::CMOD);
    properties.visible = true;
}

Playfield::~Playfield() {
    if (loading_task.valid()) {
        loading_task.wait();
    }

    for (auto *strum : strums) {
        delete strum;
    }
    strums.clear();

    for (auto *note : notes) {
        delete note;
    }
    notes.clear();
}

void Playfield::createNotesAsync() {
    if (!chart_data) {
        is_loading.store(false);
        return;
    }

    VSRG_LOG(
        *engine_context->get_debugger(), vsrg::DebugLevel::INFO,
        "creating notes from chart data, note count: " + std::to_string(chart_data->notes.size()));

    float screen_width = static_cast<float>(engine_context->get_screen_width());
    float strum_width = 96.0f;
    float strum_spacing = 0.0f;
    float total_width = (strum_width * key_count) + (strum_spacing * (key_count - 1));
    float start_x = (screen_width - total_width) / 2.0f;

    int created_count = 0;
    int failed_count = 0;

    std::vector<Note *> temp_notes;

    for (const auto &vsrg_note : chart_data->notes) {
        if (vsrg_note.column >= key_count) continue;

        float x = start_x + (vsrg_note.column * (strum_width + strum_spacing));

        Note *note = nullptr;
        try {
            if (vsrg_note.type == VSRGNoteType::HOLD) {
                note =
                    new HoldNote(engine_context, "note" + std::to_string(vsrg_note.column) + ".png",
                                 vsrg_note.column, vsrg_note.time, vsrg_note.end_time);
            } else if (vsrg_note.type == VSRGNoteType::TAP) {
                note = new Note(engine_context, "note" + std::to_string(vsrg_note.column) + ".png",
                                vsrg_note.column, vsrg_note.time, NoteType::NORMAL);
            } else if (vsrg_note.type == VSRGNoteType::MINE) {
                note = new Note(engine_context, "mine.png", vsrg_note.column, vsrg_note.time,
                                NoteType::MINE);
            } else {
                note = new Note(engine_context, "note" + std::to_string(vsrg_note.column) + ".png",
                                vsrg_note.column, vsrg_note.time, NoteType::NORMAL);
            }

            if (note) {
                note->setPosition(x, -100.0f);
                note->setSize(strum_width, strum_width);
                note->setCanRender(false);

                temp_notes.push_back(note);
                created_count++;
            }
        } catch (const std::exception &e) {
            VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::ERROR,
                     std::string("failed to create note: ") + e.what());
            failed_count++;
            if (note) delete note;
        }
    }

    notes = std::move(temp_notes);

    VSRG_LOG(*engine_context->get_debugger(), vsrg::DebugLevel::INFO,
             "created " + std::to_string(created_count) + " notes successfully, " +
                 std::to_string(failed_count) + " failed");

    is_loading.store(false);
}

void Playfield::createNotes() {
    is_loading.store(true);
    createNotesAsync();
    is_loading.store(false);
}

void Playfield::updateStrumPositions() {
    float screen_width = static_cast<float>(engine_context->get_screen_width());
    float strum_width = 96.0f;
    float strum_spacing = 0.0f;
    float total_width = (strum_width * key_count) + (strum_spacing * (key_count - 1));
    float start_x = (screen_width - total_width) / 2.0f;

    for (int i = 0; i < static_cast<int>(strums.size()); i++) {
        float x = start_x + (i * (strum_width + strum_spacing));
        strums[i]->setPosition(x + properties.position.x, strum_line_y + properties.position.y);
    }
}

void Playfield::update(float delta_time) {
    if (!conductor) return;

    for (auto *strum : strums) {
        if (strum) strum->update(delta_time);
    }

    if (is_loading.load()) {
        return;
    }

    float song_position = conductor->get_song_position();
    float screen_height = static_cast<float>(engine_context->get_screen_height());

    for (auto *note : notes) {
        if (!note) continue;

        if (note->shouldDespawn()) {
            note->setCanRender(false);
            continue;
        }

        float note_time = note->getTime();
        float y_pos =
            scroll_calculator.calculateNoteYPosition(note_time, song_position, strum_line_y);

        note->setPosition(note->getPosition().x, y_pos);

        // this is for debug, presses notes when theyre on the strum
        if (song_position >= note_time && !note->isPressed()) {
            note->setPressed(true);

            if (note->getType() == NoteType::HOLD) {
                HoldNote *hold_note = dynamic_cast<HoldNote *>(note);
                if (hold_note) {
                    hold_note->setHolding(true);
                }
            }
        }

        if (note->getType() == NoteType::HOLD) {
            HoldNote *hold_note = dynamic_cast<HoldNote *>(note);
            if (hold_note) {
                float end_note_time = hold_note->getEndTime();

                if (hold_note->isHolding() && song_position >= end_note_time) {
                    hold_note->setHolding(false);
                    hold_note->despawnNote();
                } else if (!hold_note->isFadingOut() && !hold_note->isHolding() &&
                           !note->isPressed()) {
                    if (y_pos > strum_line_y) {
                        hold_note->startFadeOut();
                    }
                }
            }
        } else {
            if (y_pos > strum_line_y + note->getSize().y && !note->shouldDespawn()) {
                note->despawnNote();
            }
        }

        if (note->getType() == NoteType::HOLD) {
            HoldNote *hold_note = dynamic_cast<HoldNote *>(note);
            if (hold_note) {
                float end_note_time = hold_note->getEndTime();
                float end_y_pos = scroll_calculator.calculateNoteYPosition(
                    end_note_time, song_position, strum_line_y);

                hold_note->setEndY(end_y_pos);
                if (hold_note->isHolding()) {
                    note->setPosition(note->getPosition().x, strum_line_y);
                    y_pos = strum_line_y;
                }

                float note_head_y = y_pos;
                float hold_end_y = hold_note->getEndY();
                float note_height = note->getSize().y;

                float top_y = std::min(note_head_y, hold_end_y);
                float bottom_y = std::max(note_head_y + note_height, hold_end_y + note_height);

                if (bottom_y < 0 || top_y > screen_height) {
                    note->setCanRender(false);
                } else {
                    note->setCanRender(true);
                }
            }
        } else {
            float note_height = note->getSize().y;
            if (y_pos < 0 - note_height || y_pos > screen_height) {
                note->setCanRender(false);
            } else {
                note->setCanRender(true);
            }
        }

        note->update(delta_time);
    }
}

void Playfield::render() {
    if (!properties.visible) {
        return;
    }

    SolidComponent::render();
    engine_context->get_sprite_renderer()->begin();

    for (auto *strum : strums) {
        if (strum) strum->render();
    }

    if (!is_loading.load()) {
        for (auto *note : notes) {
            if (note) note->render();
        }
    }

    engine_context->get_sprite_renderer()->end();
}

glm::vec2 Playfield::getSize() const {
    if (strums.empty()) {
        return glm::vec2(0.0f);
    }

    float total_width = 0.0f;
    float max_height = 0.0f;

    for (const auto *strum : strums) {
        total_width += strum->getSize().x;
        max_height = std::max(max_height, strum->getSize().y);
    }

    return glm::vec2(total_width, max_height);
}
}  // namespace mania