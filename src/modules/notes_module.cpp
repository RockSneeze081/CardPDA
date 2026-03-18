#include "notes_module.h"
#include "../core/ui_manager.h"
#include "../core/input_manager.h"
#include "../core/time_manager.h"
#include "../core/sd_manager.h"

namespace CardOS {

void NotesModule::init() {
    text_buffer_ = new char[MAX_TEXT_LEN];
    if (text_buffer_) {
        text_buffer_[0] = '\0';
    }
    text_len_ = 0;
    cursor_pos_ = 0;
    last_save_time_ = millis();
    is_dirty_ = false;
    
    current_file_ = "/cardos/notes/" + TimeManager::getInstance().getDateFileStr() + ".txt";
    load();
}

void NotesModule::update() {
    handleInput();
    
    uint32_t now = millis();
    if (is_dirty_ && (now - last_save_time_) >= AUTO_SAVE_INTERVAL) {
        save();
    }
}

void NotesModule::render() {
    UIManager& ui = UIManager::getInstance();
    
    ui.clear(TFT_BLACK);
    
    ui.fillRect(0, 0, 320, 20, TFT_CYAN);
    ui.setTextColor(TFT_BLACK, TFT_CYAN);
    ui.drawTextAt(160, 4, current_file_.c_str(), TextAlign::CENTER);
    
    renderText();
    
    ui.setTextColor(TFT_DARKGREY, TFT_BLACK);
    ui.drawFooter("[ESC] Save+Exit", "", "[DEL] Clear");
    
    if (is_dirty_) {
        ui.setTextColor(TFT_YELLOW);
        ui.drawTextAt(310, 4, "*", TextAlign::LEFT);
    }
}

void NotesModule::renderText() {
    UIManager& ui = UIManager::getInstance();
    
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.setTextSize(1);
    
    const int line_height = 14;
    const int visible_lines = 14;
    const int start_y = 26;
    
    int scroll_offset = 0;
    int lines_rendered = 0;
    int current_line_char_start = 0;
    int cursor_line = 0;
    int cursor_col = 0;
    
    for (size_t i = 0; i <= text_len_; i++) {
        if (i == cursor_pos_ || i == text_len_) {
            if (i == cursor_pos_) {
                cursor_line = lines_rendered;
                cursor_col = i - current_line_char_start;
            }
        }
        if (text_buffer_[i] == '\n' || i == text_len_) {
            if (lines_rendered >= scroll_offset && lines_rendered < scroll_offset + visible_lines) {
                int y = start_y + (lines_rendered - scroll_offset) * line_height;
                if (y >= start_y && y < start_y + visible_lines * line_height) {
                    ui.setCursor(4, y);
                    
                    if (lines_rendered == cursor_line && cursor_col < 40) {
                        char display[42];
                        int len = i - current_line_char_start;
                        if (len > 40) len = 40;
                        strncpy(display, &text_buffer_[current_line_char_start], len);
                        display[len] = '\0';
                        
                        for (int c = 0; display[c]; c++) {
                            if (c == cursor_col) {
                                ui.setTextColor(TFT_BLACK, TFT_WHITE);
                            }
                            ui.drawChar(display[c]);
                            if (c == cursor_col) {
                                ui.setTextColor(TFT_WHITE, TFT_BLACK);
                            }
                        }
                    } else {
                        char display[42];
                        int len = i - current_line_char_start;
                        if (len > 40) len = 40;
                        strncpy(display, &text_buffer_[current_line_char_start], len);
                        display[len] = '\0';
                        ui.drawText(display);
                    }
                }
            }
            lines_rendered++;
            current_line_char_start = i + 1;
        }
    }
    
    if (lines_rendered > visible_lines) {
        ui.drawScrollbar(312, start_y, visible_lines * line_height, lines_rendered, visible_lines, scroll_offset);
    }
}

void NotesModule::handleInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC)) {
        save();
        ModuleManager::getInstance().switchModule(ModuleID::HOME);
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::DEL) || input.wasKeyJustPressed(KeyEvent::BACKSPACE)) {
        if (text_len_ > 0 && cursor_pos_ > 0) {
            memmove(&text_buffer_[cursor_pos_ - 1], &text_buffer_[cursor_pos_], text_len_ - cursor_pos_ + 1);
            text_len_--;
            cursor_pos_--;
            is_dirty_ = true;
        }
        return;
    }
    
    char ch = input.getCharInput();
    if (ch >= 32 && ch < 127) {
        if (text_len_ < MAX_TEXT_LEN - 1) {
            memmove(&text_buffer_[cursor_pos_ + 1], &text_buffer_[cursor_pos_], text_len_ - cursor_pos_ + 1);
            text_buffer_[cursor_pos_] = ch;
            text_len_++;
            cursor_pos_++;
            is_dirty_ = true;
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        if (text_len_ < MAX_TEXT_LEN - 1) {
            memmove(&text_buffer_[cursor_pos_ + 1], &text_buffer_[cursor_pos_], text_len_ - cursor_pos_ + 1);
            text_buffer_[cursor_pos_] = '\n';
            text_len_++;
            cursor_pos_++;
            is_dirty_ = true;
        }
    }
}

void NotesModule::save() {
    if (text_buffer_ && is_dirty_) {
        SDManager::getInstance().writeFile(current_file_.c_str(), text_buffer_);
        is_dirty_ = false;
        last_save_time_ = millis();
    }
}

void NotesModule::load() {
    if (SDManager::getInstance().exists(current_file_.c_str())) {
        String content = SDManager::getInstance().readFile(current_file_.c_str());
        if (text_buffer_ && content.length() < MAX_TEXT_LEN) {
            strncpy(text_buffer_, content.c_str(), MAX_TEXT_LEN - 1);
            text_buffer_[MAX_TEXT_LEN - 1] = '\0';
            text_len_ = strlen(text_buffer_);
            cursor_pos_ = text_len_;
        }
    }
}

void NotesModule::onExit() {
    save();
}

void NotesModule::destroy() {
    if (text_buffer_) {
        delete[] text_buffer_;
        text_buffer_ = nullptr;
    }
}

}
