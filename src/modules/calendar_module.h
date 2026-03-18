#ifndef CARDOS_CALENDAR_MODULE_H
#define CARDOS_CALENDAR_MODULE_H

#include "module_manager.h"
#include <vector>

namespace CardOS {

struct CalendarEvent {
    char date[11];
    char title[64];
};

class CalendarModule : public Module {
public:
    void init() override;
    void update() override;
    void render() override;
    void destroy() override;

    const char* getName() const override { return "Calendar"; }
    ModuleID getID() const override { return ModuleID::CALENDAR; }

    void onExit() override;

private:
    enum class Mode : uint8_t {
        MONTH_VIEW,
        DAY_VIEW,
        ADD_EVENT
    };

    void loadEvents();
    void saveEvents();
    void renderMonthView();
    void renderDayView();
    void renderAddEvent();
    void handleMonthInput();
    void handleDayInput();
    void handleAddInput();
    void addEvent(const char* date, const char* title);
    void deleteEvent(int index);
    std::vector<CalendarEvent> getEventsForDate(const char* date);

    Mode mode_ = Mode::MONTH_VIEW;
    int view_year_;
    int view_month_;
    int view_day_;
    int selected_day_ = 1;
    int scroll_offset_ = 0;

    static constexpr int VISIBLE_DAYS = 5;
    static constexpr int EVENT_INPUT_MAX = 48;
    char event_title_[EVENT_INPUT_MAX];
    int event_title_pos_ = 0;

    std::vector<CalendarEvent> events_;
    String events_file_ = "/cardos/calendar/events.json";
};

}

#endif
