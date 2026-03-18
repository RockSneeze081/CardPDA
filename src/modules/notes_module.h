#ifndef CARDOS_NOTES_MODULE_H
#define CARDOS_NOTES_MODULE_H

#include "module_manager.h"

namespace CardOS {

class NotesModule : public Module {
public:
    void init() override;
    void update() override;
    void render() override;
    void destroy() override;

    const char* getName() const override { return "Notes"; }
    ModuleID getID() const override { return ModuleID::NOTES; }

    void onExit() override;

private:
    void save();
    void load();
    void renderText();
    void handleInput();

    static constexpr size_t MAX_TEXT_LEN = 8192;
    static constexpr uint32_t AUTO_SAVE_INTERVAL = 5000;

    char* text_buffer_ = nullptr;
    size_t text_len_ = 0;
    size_t cursor_pos_ = 0;
    uint32_t last_save_time_ = 0;
    bool is_dirty_ = false;
    String current_file_;
};

}

#endif
