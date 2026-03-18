#include "calendar_module.h"
#include "../core/ui_manager.h"
#include "../core/input_manager.h"
#include "../core/time_manager.h"
#include "../core/sd_manager.h"

namespace CardOS {

void CalendarModule::init() {
    TimeManager& time = TimeManager::getInstance();
    view_year_ = time.getYear();
    view_month_ = time.getMonth();
    view_day_ = time.getDay();
    selected_day_ = view_day_;
    mode_ = Mode::MONTH_VIEW;
    event_title_[0] = '\0';
    event_title_pos_ = 0;
    scroll_offset_ = 0;
    
    loadEvents();
}

void CalendarModule::update() {
    if (mode_ == Mode::MONTH_VIEW) {
        handleMonthInput();
    } else if (mode_ == Mode::DAY_VIEW) {
        handleDayInput();
    } else {
        handleAddInput();
    }
}

void CalendarModule::render() {
    UIManager& ui = UIManager::getInstance();
    ui.clear(TFT_BLACK);
    
    if (mode_ == Mode::MONTH_VIEW) {
        renderMonthView();
    } else if (mode_ == Mode::DAY_VIEW) {
        renderDayView();
    } else {
        renderAddEvent();
    }
}

void CalendarModule::renderMonthView() {
    UIManager& ui = UIManager::getInstance();
    TimeManager& time = TimeManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_ORANGE);
    ui.setTextColor(TFT_BLACK, TFT_ORANGE);
    ui.drawTextAt(160, 4, "Calendar", TextAlign::CENTER);
    
    char nav_buf[32];
    snprintf(nav_buf, sizeof(nav_buf), "< %04d-%02d >", view_year_, view_month_);
    ui.setTextColor(TFT_CYAN);
    ui.drawTextAt(160, 24, nav_buf, TextAlign::CENTER);
    
    static const char* DAY_NAMES[] = {"Su", "Mo", "Tu", "We", "Th", "Fr", "Sa"};
    ui.setTextColor(TFT_DARKGREY);
    for (int d = 0; d < 7; d++) {
        int x = 30 + d * 40;
        ui.drawTextAt(x, 46, DAY_NAMES[d]);
    }
    
    int days = TimeManager::daysInMonth(view_year_, view_month_);
    int start_day = TimeManager::dayOfWeek(view_year_, view_month_, 1);
    
    for (int day = 1; day <= days; day++) {
        int cell = start_day + day - 1;
        int col = cell % 7;
        int row = cell / 7;
        
        int x = 30 + col * 40;
        int y = 60 + row * 36;
        
        bool has_events = false;
        char date_str[11];
        snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", view_year_, view_month_, day);
        auto day_events = getEventsForDate(date_str);
        if (!day_events.empty()) has_events = true;
        
        bool is_selected = (day == selected_day_);
        bool is_today = (view_year_ == time.getYear() && 
                        view_month_ == time.getMonth() && 
                        day == time.getDay());
        
        if (is_selected) {
            ui.fillRect(x - 2, y - 2, 34, 28, TFT_CYAN);
            ui.setTextColor(TFT_BLACK, TFT_CYAN);
        } else if (is_today) {
            ui.fillRect(x - 2, y - 2, 34, 28, TFT_YELLOW);
            ui.setTextColor(TFT_BLACK, TFT_YELLOW);
        } else {
            ui.setTextColor(TFT_WHITE, TFT_BLACK);
        }
        
        char day_str[4];
        snprintf(day_str, sizeof(day_str), "%2d", day);
        ui.drawTextAt(x, y, day_str);
        
        if (has_events) {
            ui.setTextColor(TFT_GREEN);
            ui.drawTextAt(x + 20, y, "*");
        }
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Back", "[L/R] Month", "[ENT] Day");
}

void CalendarModule::renderDayView() {
    UIManager& ui = UIManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_PURPLE);
    ui.setTextColor(TFT_WHITE, TFT_PURPLE);
    
    char date_str[20];
    snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", view_year_, view_month_, selected_day_);
    ui.drawTextAt(160, 4, date_str, TextAlign::CENTER);
    
    auto day_events = getEventsForDate(date_str);
    
    if (day_events.empty()) {
        ui.setTextColor(TFT_DARKGREY);
        ui.drawTextAt(160, 60, "No events", TextAlign::CENTER);
        ui.drawTextAt(160, 80, "Press [+] to add", TextAlign::CENTER);
    } else {
        ui.setTextColor(TFT_YELLOW);
        ui.drawTextAt(160, 26, "Events", TextAlign::CENTER);
        
        for (size_t i = 0; i < day_events.size() && i < 6; i++) {
            int y = 45 + i * 28;
            
            ui.fillRect(10, y, 300, 24, TFT_BLACK);
            
            char num_str[8];
            snprintf(num_str, sizeof(num_str), "%d.", i + 1);
            ui.setTextColor(TFT_CYAN, TFT_BLACK);
            ui.drawTextAt(15, y + 4, num_str);
            
            ui.setTextColor(TFT_WHITE, TFT_BLACK);
            ui.drawTextAt(40, y + 4, day_events[i].title);
        }
        
        if (day_events.size() > 6) {
            ui.drawScrollbar(308, 45, 168, day_events.size(), 6, scroll_offset_);
        }
    }
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Back", "[+] Add", "[x] Delete");
}

void CalendarModule::renderAddEvent() {
    UIManager& ui = UIManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_GREEN);
    ui.setTextColor(TFT_BLACK, TFT_GREEN);
    ui.drawTextAt(160, 4, "Add Event", TextAlign::CENTER);
    
    char date_str[20];
    snprintf(date_str, sizeof(date_str), "Date: %04d-%02d-%02d", view_year_, view_month_, selected_day_);
    ui.setTextColor(TFT_WHITE);
    ui.drawTextAt(10, 40, date_str);
    
    ui.drawTextAt(10, 70, "Title:");
    ui.fillRect(10, 85, 300, 24, TFT_WHITE);
    ui.setCursor(15, 90);
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.drawText(event_title_);
    
    ui.setTextColor(TFT_DARKGREY);
    ui.drawFooter("[ESC] Cancel", "", "[ENT] Save");
}

void CalendarModule::handleMonthInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC) || input.wasKeyJustPressed(KeyEvent::F1)) {
        ModuleManager::getInstance().switchModule(ModuleID::HOME);
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::LEFT)) {
        view_month_--;
        if (view_month_ < 1) {
            view_month_ = 12;
            view_year_--;
        }
        selected_day_ = 1;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::RIGHT)) {
        view_month_++;
        if (view_month_ > 12) {
            view_month_ = 1;
            view_year_++;
        }
        selected_day_ = 1;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::UP)) {
        selected_day_--;
        if (selected_day_ < 1) {
            selected_day_ = TimeManager::daysInMonth(view_year_, view_month_);
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::DOWN)) {
        selected_day_++;
        if (selected_day_ > TimeManager::daysInMonth(view_year_, view_month_)) {
            selected_day_ = 1;
        }
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        mode_ = Mode::DAY_VIEW;
        scroll_offset_ = 0;
    }
}

void CalendarModule::handleDayInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC)) {
        mode_ = Mode::MONTH_VIEW;
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::TAB) || input.getCharInput() == '+') {
        mode_ = Mode::ADD_EVENT;
        event_title_[0] = '\0';
        event_title_pos_ = 0;
    }
}

void CalendarModule::handleAddInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC)) {
        mode_ = Mode::DAY_VIEW;
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        if (event_title_pos_ > 0) {
            char date_str[11];
            snprintf(date_str, sizeof(date_str), "%04d-%02d-%02d", view_year_, view_month_, selected_day_);
            addEvent(date_str, event_title_);
        }
        mode_ = Mode::DAY_VIEW;
        return;
    }
    
    char ch = input.getCharInput();
    if (ch >= 32 && ch < 127 && event_title_pos_ < EVENT_INPUT_MAX - 1) {
        event_title_[event_title_pos_++] = ch;
        event_title_[event_title_pos_] = '\0';
    }
    
    if (input.wasKeyJustPressed(KeyEvent::BACKSPACE) && event_title_pos_ > 0) {
        event_title_pos_--;
        event_title_[event_title_pos_] = '\0';
    }
}

void CalendarModule::addEvent(const char* date, const char* title) {
    CalendarEvent event;
    strncpy(event.date, date, sizeof(event.date) - 1);
    event.date[sizeof(event.date) - 1] = '\0';
    strncpy(event.title, title, sizeof(event.title) - 1);
    event.title[sizeof(event.title) - 1] = '\0';
    events_.push_back(event);
    saveEvents();
}

void CalendarModule::deleteEvent(int index) {
    if (index >= 0 && index < (int)events_.size()) {
        events_.erase(events_.begin() + index);
        saveEvents();
    }
}

std::vector<CalendarEvent> CalendarModule::getEventsForDate(const char* date) {
    std::vector<CalendarEvent> result;
    for (const auto& event : events_) {
        if (strcmp(event.date, date) == 0) {
            result.push_back(event);
        }
    }
    return result;
}

void CalendarModule::loadEvents() {
    events_.clear();
    
    if (!SDManager::getInstance().exists(events_file_.c_str())) {
        return;
    }
    
    String content = SDManager::getInstance().readFile(events_file_.c_str());
    
    int events_array_start = content.indexOf("\"events\":");
    if (events_array_start == -1) return;
    
    int array_start = content.indexOf("[", events_array_start);
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
        
        CalendarEvent event;
        event.date[0] = '\0';
        event.title[0] = '\0';
        
        int date_start = obj.indexOf("\"date\":");
        if (date_start != -1) {
            int q1 = obj.indexOf("\"", date_start + 7);
            int q2 = obj.indexOf("\"", q1 + 1);
            if (q1 != -1 && q2 != -1) {
                String d = obj.substring(q1 + 1, q2);
                strncpy(event.date, d.c_str(), sizeof(event.date) - 1);
                event.date[sizeof(event.date) - 1] = '\0';
            }
        }
        
        int title_start = obj.indexOf("\"title\":");
        if (title_start != -1) {
            int q1 = obj.indexOf("\"", title_start + 8);
            int q2 = obj.indexOf("\"", q1 + 1);
            if (q1 != -1 && q2 != -1) {
                String t = obj.substring(q1 + 1, q2);
                strncpy(event.title, t.c_str(), sizeof(event.title) - 1);
                event.title[sizeof(event.title) - 1] = '\0';
            }
        }
        
        if (event.date[0] != '\0' && event.title[0] != '\0') {
            events_.push_back(event);
        }
        
        pos = obj_end + 1;
    }
}

void CalendarModule::saveEvents() {
    String content = "{\n\"events\": [\n";
    
    for (size_t i = 0; i < events_.size(); i++) {
        content += "{\"date\": \"";
        content += events_[i].date;
        content += "\", \"title\": \"";
        content += events_[i].title;
        content += "\"}";
        if (i < events_.size() - 1) content += ",";
        content += "\n";
    }
    
    content += "]\n}\n";
    
    SDManager::getInstance().writeFile(events_file_.c_str(), content.c_str());
}

void CalendarModule::onExit() {
    saveEvents();
}

void CalendarModule::destroy() {
    events_.clear();
}

}
