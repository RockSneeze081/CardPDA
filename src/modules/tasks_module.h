#ifndef CARDOS_TASKS_MODULE_H
#define CARDOS_TASKS_MODULE_H

#include "module_manager.h"
#include <vector>

namespace CardOS {

struct Task {
    char text[128];
    bool done;
};

class TasksModule : public Module {
public:
    void init() override;
    void update() override;
    void render() override;
    void destroy() override;

    const char* getName() const override { return "Tasks"; }
    ModuleID getID() const override { return ModuleID::TASKS; }

    void onExit() override;

private:
    enum class Mode : uint8_t {
        LIST,
        ADD,
        EDIT
    };

    void load();
    void save();
    void renderList();
    void renderAddEdit();
    void handleListInput();
    void handleAddEditInput();
    void addTask(const char* text);
    void toggleTask(int index);
    void deleteTask(int index);
    void editTask(int index, const char* new_text);
    int countDone();

    std::vector<Task> tasks_;
    int selected_index_ = 0;
    int scroll_offset_ = 0;
    Mode mode_ = Mode::LIST;

    static constexpr int VISIBLE_ITEMS = 7;
    static constexpr int INPUT_MAX_LEN = 64;
    char input_buffer_[INPUT_MAX_LEN];
    int input_pos_ = 0;
    
    String current_file_;
};

}

#endif
