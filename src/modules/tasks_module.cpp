#include "tasks_module.h"
#include "../core/ui_manager.h"
#include "../core/input_manager.h"
#include "../core/time_manager.h"
#include "../core/sd_manager.h"

namespace CardOS {

void TasksModule::init() {
    tasks_.clear();
    selected_index_ = 0;
    scroll_offset_ = 0;
    mode_ = Mode::LIST;
    input_pos_ = 0;
    input_buffer_[0] = '\0';
    
    current_file_ = "/cardos/tasks/" + TimeManager::getInstance().getDateFileStr() + ".json";
    load();
}

void TasksModule::update() {
    if (mode_ == Mode::LIST) {
        handleListInput();
    } else {
        handleAddEditInput();
    }
}

void TasksModule::render() {
    UIManager& ui = UIManager::getInstance();
    
    ui.clear(TFT_BLACK);
    
    if (mode_ == Mode::LIST) {
        renderList();
    } else {
        renderAddEdit();
    }
}

void TasksModule::renderList() {
    UIManager& ui = UIManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_GREEN);
    ui.setTextColor(TFT_BLACK, TFT_GREEN);
    ui.drawTextAt(160, 4, "Tasks - Daily Checklist", TextAlign::CENTER);
    
    char header[32];
    snprintf(header, sizeof(header), "Tasks: %d/%d", 
             tasks_.size() > 0 ? tasks_.size() - countDone() : 0, tasks_.size());
    ui.setTextColor(TFT_DARKGREY);
    ui.drawTextAt(310, 4, header, TextAlign::RIGHT);
    
    if (tasks_.empty()) {
        ui.setTextColor(TFT_DARKGREY);
        ui.drawTextAt(160, 110, "No tasks yet", TextAlign::CENTER);
        ui.drawTextAt(160, 130, "Press [+] to add", TextAlign::CENTER);
    } else {
        for (int i = 0; i < VISIBLE_ITEMS && (i + scroll_offset_) < (int)tasks_.size(); i++) {
            int idx = i + scroll_offset_;
            int y = 28 + i * 28;
            bool selected = (idx == selected_index_);
            
            ui.drawListItem(y, tasks_[idx].text, selected, tasks_[idx].done);
        }
        
        if (tasks_.size() > VISIBLE_ITEMS) {
            ui.drawScrollbar(312, 28, VISIBLE_ITEMS * 28, tasks_.size(), VISIBLE_ITEMS, scroll_offset_);
        }
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Back", "[+]Add [x]Del", "[ENT]Toggle");
}

void TasksModule::renderAddEdit() {
    UIManager& ui = UIManager::getInstance();
    
    const char* title = (mode_ == Mode::ADD) ? "Add Task" : "Edit Task";
    
    ui.fillRect(0, 0, 320, 20, TFT_BLUE);
    ui.setTextColor(TFT_WHITE, TFT_BLUE);
    ui.drawTextAt(160, 4, title, TextAlign::CENTER);
    
    ui.setTextColor(TFT_WHITE);
    ui.drawTextAt(10, 50, "Task:");
    
    ui.fillRect(10, 65, 300, 24, TFT_WHITE);
    ui.setCursor(15, 70);
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.drawText(input_buffer_);
    
    int cursor_x = 15 + ui.textWidth(input_buffer_);
    if (millis() % 1000 < 500) {
        ui.fillRect(cursor_x, 68, 8, 16, TFT_WHITE);
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Cancel", "", "[ENT] Save");
}

void TasksModule::handleListInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC) || input.wasKeyJustPressed(KeyEvent::F1)) {
        save();
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
        if (selected_index_ < (int)tasks_.size() - 1) {
            selected_index_++;
            if (selected_index_ >= scroll_offset_ + VISIBLE_ITEMS) {
                scroll_offset_ = selected_index_ - VISIBLE_ITEMS + 1;
            }
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER) || input.wasKeyJustPressed(KeyEvent::SPACE)) {
        if (!tasks_.empty()) {
            toggleTask(selected_index_);
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::TAB) || input.getCharInput() == '+') {
        mode_ = Mode::ADD;
        input_pos_ = 0;
        input_buffer_[0] = '\0';
    }
    
    if (input.wasKeyJustPressed(KeyEvent::DEL) || input.wasKeyJustPressed(KeyEvent::BACKSPACE)) {
        if (!tasks_.empty()) {
            deleteTask(selected_index_);
            if (selected_index_ >= (int)tasks_.size() && selected_index_ > 0) {
                selected_index_ = tasks_.size() - 1;
            }
        }
    }
}

void TasksModule::handleAddEditInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC)) {
        mode_ = Mode::LIST;
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        if (input_pos_ > 0) {
            if (mode_ == Mode::ADD) {
                addTask(input_buffer_);
            }
            mode_ = Mode::LIST;
        }
        return;
    }
    
    char ch = input.getCharInput();
    if (ch >= 32 && ch < 127 && input_pos_ < INPUT_MAX_LEN - 1) {
        input_buffer_[input_pos_++] = ch;
        input_buffer_[input_pos_] = '\0';
    }
    
    if (input.wasKeyJustPressed(KeyEvent::BACKSPACE) && input_pos_ > 0) {
        input_pos_--;
        input_buffer_[input_pos_] = '\0';
    }
}

void TasksModule::addTask(const char* text) {
    Task task;
    strncpy(task.text, text, sizeof(task.text) - 1);
    task.text[sizeof(task.text) - 1] = '\0';
    task.done = false;
    tasks_.push_back(task);
    save();
}

void TasksModule::toggleTask(int index) {
    if (index >= 0 && index < (int)tasks_.size()) {
        tasks_[index].done = !tasks_[index].done;
        save();
    }
}

void TasksModule::deleteTask(int index) {
    if (index >= 0 && index < (int)tasks_.size()) {
        tasks_.erase(tasks_.begin() + index);
        save();
    }
}

void TasksModule::editTask(int index, const char* new_text) {
    if (index >= 0 && index < (int)tasks_.size()) {
        strncpy(tasks_[index].text, new_text, sizeof(tasks_[index].text) - 1);
        tasks_[index].text[sizeof(tasks_[index].text) - 1] = '\0';
        save();
    }
}

int TasksModule::countDone() {
    int count = 0;
    for (const auto& task : tasks_) {
        if (task.done) count++;
    }
    return count;
}

void TasksModule::load() {
    if (!SDManager::getInstance().exists(current_file_.c_str())) {
        return;
    }
    
    String content = SDManager::getInstance().readFile(current_file_.c_str());
    
    tasks_.clear();
    
    int task_start = content.indexOf("\"tasks\":");
    if (task_start == -1) return;
    
    int array_start = content.indexOf("[", task_start);
    int array_end = content.indexOf("]", array_start);
    if (array_start == -1 || array_end == -1) return;
    
    String array_content = content.substring(array_start + 1, array_end);
    
    int pos = 0;
    while (pos < (int)array_content.length()) {
        int obj_start = array_content.indexOf("{", pos);
        if (obj_start == -1) break;
        int obj_end = array_content.indexOf("}", obj_start);
        if (obj_end == -1) break;
        
        String obj = array_content.substring(obj_start, obj_end + 1);
        
        Task task;
        task.done = false;
        task.text[0] = '\0';
        
        int text_start = obj.indexOf("\"text\":");
        if (text_start != -1) {
            int quote1 = obj.indexOf("\"", text_start + 7);
            int quote2 = obj.indexOf("\"", quote1 + 1);
            if (quote1 != -1 && quote2 != -1) {
                String text_val = obj.substring(quote1 + 1, quote2);
                strncpy(task.text, text_val.c_str(), sizeof(task.text) - 1);
                task.text[sizeof(task.text) - 1] = '\0';
            }
        }
        
        if (obj.indexOf("\"done\": true") != -1) {
            task.done = true;
        }
        
        if (task.text[0] != '\0') {
            tasks_.push_back(task);
        }
        
        pos = obj_end + 1;
    }
}

void TasksModule::save() {
    String content = "{\n\"tasks\": [\n";
    
    for (size_t i = 0; i < tasks_.size(); i++) {
        content += "{\"text\": \"";
        content += tasks_[i].text;
        content += "\", \"done\": ";
        content += tasks_[i].done ? "true" : "false";
        content += "}";
        if (i < tasks_.size() - 1) content += ",";
        content += "\n";
    }
    
    content += "]\n}\n";
    
    SDManager::getInstance().writeFile(current_file_.c_str(), content.c_str());
}

void TasksModule::onExit() {
    save();
}

void TasksModule::destroy() {
    tasks_.clear();
}

}
