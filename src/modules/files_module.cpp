#include "files_module.h"
#include "../core/ui_manager.h"
#include "../core/input_manager.h"
#include "../core/sd_manager.h"

namespace CardOS {

void FilesModule::init() {
    current_path_ = "/cardos";
    selected_index_ = 0;
    scroll_offset_ = 0;
    viewer_mode_ = false;
    input_mode_ = 0;
    input_buffer_[0] = '\0';
    input_pos_ = 0;
    
    listDirectory(current_path_.c_str());
}

void FilesModule::update() {
    if (viewer_mode_) {
        handleViewerInput();
    } else {
        handleListInput();
    }
}

void FilesModule::render() {
    UIManager& ui = UIManager::getInstance();
    ui.clear(TFT_BLACK);
    
    if (viewer_mode_) {
        renderViewer();
    } else {
        renderList();
    }
}

void FilesModule::renderList() {
    UIManager& ui = UIManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_CYAN);
    ui.setTextColor(TFT_BLACK, TFT_CYAN);
    ui.drawTextAt(160, 4, "File Manager", TextAlign::CENTER);
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawTextAt(10, 24, current_path_.c_str());
    
    if (current_path_ != "/cardos" && current_path_ != "/") {
        ui.setTextColor(TFT_YELLOW);
        ui.drawTextAt(310, 24, "[..]", TextAlign::RIGHT);
    }
    
    if (files_.empty()) {
        ui.setTextColor(TFT_DARKGREY);
        ui.drawTextAt(160, 100, "Empty folder", TextAlign::CENTER);
    } else {
        for (int i = 0; i < VISIBLE_ITEMS && (i + scroll_offset_) < (int)files_.size(); i++) {
            int idx = i + scroll_offset_;
            int y = 40 + i * 24;
            bool selected = (idx == selected_index_);
            
            if (selected) {
                ui.fillRect(8, y, 304, 22, TFT_WHITE);
                ui.setTextColor(TFT_BLACK, TFT_WHITE);
            } else {
                ui.setTextColor(TFT_WHITE, TFT_BLACK);
            }
            
            if (files_[idx].is_dir) {
                ui.setTextColor(selected ? TFT_BLACK : TFT_CYAN, selected ? TFT_WHITE : TFT_BLACK);
                ui.drawTextAt(12, y + 4, "[DIR] ");
            } else {
                ui.drawTextAt(12, y + 4, "[FILE]");
            }
            
            ui.drawText(files_[idx].name);
            
            if (!files_[idx].is_dir) {
                char size_buf[16];
                if (files_[idx].size > 1024 * 1024) {
                    snprintf(size_buf, sizeof(size_buf), "%.1fMB", files_[idx].size / (1024.0 * 1024.0));
                } else if (files_[idx].size > 1024) {
                    snprintf(size_buf, sizeof(size_buf), "%.1fKB", files_[idx].size / 1024.0);
                } else {
                    snprintf(size_buf, sizeof(size_buf), "%d B", files_[idx].size);
                }
                ui.setTextColor(selected ? TFT_BLACK : TFT_DARKGREY, selected ? TFT_WHITE : TFT_BLACK);
                ui.drawTextAt(308, y + 4, size_buf, TextAlign::RIGHT);
            }
        }
        
        if (files_.size() > VISIBLE_ITEMS) {
            ui.drawScrollbar(310, 40, VISIBLE_ITEMS * 24, files_.size(), VISIBLE_ITEMS, scroll_offset_);
        }
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Back", "[ENT] Open", "[DEL] Delete");
}

void FilesModule::renderViewer() {
    UIManager& ui = UIManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_MAGENTA);
    ui.setTextColor(TFT_WHITE, TFT_MAGENTA);
    ui.drawTextAt(160, 4, viewer_filename_.c_str(), TextAlign::CENTER);
    
    ui.setTextColor(TFT_WHITE);
    ui.setCursor(4, 24);
    
    int lines = 0;
    int pos = 0;
    int line_start = 0;
    
    while (pos < (int)viewer_content_.length() && lines < 14) {
        if (viewer_content_[pos] == '\n' || pos - line_start >= 40) {
            String line = viewer_content_.substring(line_start, pos);
            ui.drawText(line.c_str());
            ui.setCursor(4, 24 + lines * 14);
            lines++;
            if (viewer_content_[pos] == '\n') {
                pos++;
            }
            line_start = pos;
        } else {
            pos++;
        }
    }
    
    if (pos < (int)viewer_content_.length()) {
        String line = viewer_content_.substring(line_start, pos);
        ui.drawText(line.c_str());
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Close", "", "");
}

void FilesModule::handleListInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC) || input.wasKeyJustPressed(KeyEvent::F1)) {
        ModuleManager::getInstance().switchModule(ModuleID::HOME);
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::UP)) {
        if (selected_index_ > 0) {
            selected_index_--;
            if (selected_index_ < scroll_offset_) {
                scroll_offset_ = selected_index_;
            }
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::DOWN)) {
        if (selected_index_ < (int)files_.size() - 1) {
            selected_index_++;
            if (selected_index_ >= scroll_offset_ + VISIBLE_ITEMS) {
                scroll_offset_ = selected_index_ - VISIBLE_ITEMS + 1;
            }
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::LEFT)) {
        navigateUp();
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        if (selected_index_ >= 0 && selected_index_ < (int)files_.size()) {
            if (files_[selected_index_].is_dir) {
                current_path_ = current_path_ + "/" + files_[selected_index_].name;
                selected_index_ = 0;
                scroll_offset_ = 0;
                listDirectory(current_path_.c_str());
            } else {
                openFile(files_[selected_index_].name);
            }
        } else if (selected_index_ == (int)files_.size() && current_path_ != "/cardos" && current_path_ != "/") {
            navigateUp();
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::DEL) || input.wasKeyJustPressed(KeyEvent::BACKSPACE)) {
        if (selected_index_ >= 0 && selected_index_ < (int)files_.size()) {
            deleteFile(files_[selected_index_].name);
            listDirectory(current_path_.c_str());
        }
    }
}

void FilesModule::handleViewerInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC)) {
        viewer_mode_ = false;
        viewer_content_ = "";
    }
}

bool FilesModule::navigateUp() {
    int last_slash = current_path_.lastIndexOf('/');
    if (last_slash > 0) {
        current_path_ = current_path_.substring(0, last_slash);
    } else if (last_slash == 0 && current_path_.length() > 1) {
        current_path_ = "/cardos";
    }
    
    if (current_path_.length() == 0) {
        current_path_ = "/cardos";
    }
    
    selected_index_ = 0;
    scroll_offset_ = 0;
    listDirectory(current_path_.c_str());
    return true;
}

void FilesModule::listDirectory(const char* path) {
    files_.clear();
    
    File dir = SDManager::getInstance().open(path);
    if (!dir || !dir.isDirectory()) {
        return;
    }
    
    File file = dir.openNextFile();
    while (file) {
        FileEntry entry;
        strncpy(entry.name, file.name(), sizeof(entry.name) - 1);
        entry.name[sizeof(entry.name) - 1] = '\0';
        
        int name_start = 0;
        for (int i = 0; entry.name[i]; i++) {
            if (entry.name[i] == '/') {
                name_start = i + 1;
            }
        }
        if (name_start > 0) {
            memmove(entry.name, &entry.name[name_start], strlen(&entry.name[name_start]) + 1);
        }
        
        entry.is_dir = file.isDirectory();
        entry.size = file.size();
        
        if (strcmp(entry.name, ".") != 0) {
            files_.push_back(entry);
        }
        
        file.close();
        file = dir.openNextFile();
    }
    dir.close();
    
    std::sort(files_.begin(), files_.end(), [](const FileEntry& a, const FileEntry& b) {
        if (a.is_dir != b.is_dir) return a.is_dir;
        return strcmp(a.name, b.name) < 0;
    });
}

void FilesModule::openFile(const char* filename) {
    String full_path = current_path_ + "/" + filename;
    String ext = filename;
    int dot = ext.lastIndexOf('.');
    if (dot >= 0) {
        ext = ext.substring(dot);
        ext.toLowerCase();
    }
    
    if (ext == ".txt" || ext == ".json" || ext == ".md" || ext == ".log") {
        viewer_content_ = SDManager::getInstance().readFile(full_path.c_str());
        viewer_filename_ = filename;
        viewer_mode_ = true;
    } else {
        viewer_content_ = "Cannot preview this file type";
        viewer_filename_ = filename;
        viewer_mode_ = true;
    }
}

void FilesModule::deleteFile(const char* filename) {
    String full_path = current_path_ + "/" + filename;
    SDManager::getInstance().remove(full_path.c_str());
}

void FilesModule::renameFile(const char* old_name, const char* new_name) {
    String old_path = current_path_ + "/" + old_name;
    String new_path = current_path_ + "/" + new_name;
    SDManager::getInstance().rename(old_path.c_str(), new_path.c_str());
}

void FilesModule::destroy() {
    files_.clear();
}

}
