#include "home_module.h"
#include "../core/ui_manager.h"
#include "../core/input_manager.h"
#include "../core/time_manager.h"
#include "../core/sd_manager.h"
#include "../core/system.h"

namespace CardOS {

static const char* MODULE_NAMES[] = {
    "Home", "Notes", "Tasks", "Calendar", "Files", "Sync"
};

static const ModuleID MODULE_IDS[] = {
    ModuleID::HOME, ModuleID::NOTES, ModuleID::TASKS,
    ModuleID::CALENDAR, ModuleID::FILES, ModuleID::SYNC
};

void HomeModule::init() {
    last_render_time_ = 0;
}

void HomeModule::update() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::UP)) {
        selected_module_ = (selected_module_ - 1 + 6) % 6;
        last_render_time_ = 0;
    }
    if (input.wasKeyJustPressed(KeyEvent::DOWN)) {
        selected_module_ = (selected_module_ + 1) % 6;
        last_render_time_ = 0;
    }
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        ModuleManager::getInstance().switchModule(MODULE_IDS[selected_module_]);
    }
    if (input.wasKeyJustPressed(KeyEvent::F1)) {
        ModuleManager::getInstance().switchModule(MODULE_IDS[0]);
    }
    if (input.wasKeyJustPressed(KeyEvent::F2)) {
        ModuleManager::getInstance().switchModule(MODULE_IDS[1]);
    }
    if (input.wasKeyJustPressed(KeyEvent::F3)) {
        ModuleManager::getInstance().switchModule(MODULE_IDS[2]);
    }
    if (input.wasKeyJustPressed(KeyEvent::F4)) {
        ModuleManager::getInstance().switchModule(MODULE_IDS[3]);
    }
}

void HomeModule::render() {
    uint32_t now = millis();
    bool needs_full_render = (now - last_render_time_) > 1000 || last_render_time_ == 0;
    
    UIManager& ui = UIManager::getInstance();
    TimeManager& time = TimeManager::getInstance();
    
    ui.clear(TFT_BLACK);
    
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.setTextSize(2);
    ui.drawTextAt(160, 8, time.getTimeStr().c_str(), TextAlign::CENTER);
    
    ui.setTextSize(1);
    ui.drawTextAt(160, 36, time.getDateStr().c_str(), TextAlign::CENTER);
    
    drawMiniCalendar(time.getYear(), time.getMonth());
    
    int pending = countPendingTasks();
    char task_buf[32];
    snprintf(task_buf, sizeof(task_buf), "Tasks: %d pending", pending);
    ui.setTextColor(TFT_CYAN);
    ui.drawTextAt(160, 120, task_buf, TextAlign::CENTER);
    
    ui.setTextColor(TFT_WHITE);
    for (int i = 0; i < 6; i++) {
        int y = 150 + i * 14;
        if (i == selected_module_) {
            ui.setTextColor(TFT_BLACK, TFT_WHITE);
            ui.fillRect(50, y - 2, 220, 14, TFT_WHITE);
        } else {
            ui.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        ui.drawTextAt(160, y, MODULE_NAMES[i], TextAlign::CENTER);
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.setTextSize(1);
    ui.drawFooter("[Fn] Launch", "UP/DOWN", "[ENT] Open");
    
    last_render_time_ = now;
}

void HomeModule::destroy() {
}

void HomeModule::drawMiniCalendar(int year, int month) {
    UIManager& ui = UIManager::getInstance();
    TimeManager& time = TimeManager::getInstance();
    
    static const char* MONTH_NAMES[] = {
        "Jan", "Feb", "Mar", "Apr", "May", "Jun",
        "Jul", "Aug", "Sep", "Oct", "Nov", "Dec"
    };
    static const char* DAY_NAMES[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};

    ui.setTextColor(TFT_YELLOW);
    char header[16];
    snprintf(header, sizeof(header), "%s %04d", MONTH_NAMES[month - 1], year);
    ui.drawTextAt(160, 48, header, TextAlign::CENTER);
    
    ui.setTextColor(TFT_DARKGREY);
    for (int d = 0; d < 7; d++) {
        int x = 68 + d * 24;
        ui.drawTextAt(x, 64, DAY_NAMES[d]);
    }
    
    int days = TimeManager::daysInMonth(year, month);
    int start_day = TimeManager::dayOfWeek(year, month, 1);
    
    int today = time.getDay();
    int cur_month = time.getMonth();
    int cur_year = time.getYear();
    bool is_current_month = (year == cur_year && month == cur_month);
    
    for (int day = 1; day <= days; day++) {
        int cell = start_day + day - 1;
        int col = cell % 7;
        int row = cell / 7;
        
        int x = 68 + col * 24;
        int y = 76 + row * 14;
        
        char day_str[4];
        snprintf(day_str, sizeof(day_str), "%2d", day);
        
        if (is_current_month && day == today) {
            ui.setTextColor(TFT_BLACK, TFT_CYAN);
            ui.fillRect(x - 2, y - 1, 22, 12, TFT_CYAN);
        } else {
            ui.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        
        ui.drawTextAt(x, y, day_str);
    }
}

int HomeModule::countPendingTasks() {
    char path[64];
    snprintf(path, sizeof(path), "/cardos/tasks/%s.json", 
             TimeManager::getInstance().getDateFileStr().c_str());
    
    if (!SDManager::getInstance().exists(path)) {
        return 0;
    }
    
    String content = SDManager::getInstance().readFile(path);
    int count = 0;
    
    for (unsigned int i = 0; i < content.length(); i++) {
        if (content.substring(i).startsWith("\"done\": false")) {
            count++;
        }
    }
    
    return count;
}

}
