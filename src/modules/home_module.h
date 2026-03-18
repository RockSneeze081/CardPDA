#ifndef CARDOS_HOME_MODULE_H
#define CARDOS_HOME_MODULE_H

#include "module_manager.h"

namespace CardOS {

class HomeModule : public Module {
public:
    void init() override;
    void update() override;
    void render() override;
    void destroy() override;

    const char* getName() const override { return "Home"; }
    ModuleID getID() const override { return ModuleID::HOME; }

private:
    void drawMiniCalendar(int year, int month);
    int countPendingTasks();

    int selected_module_ = 0;
    uint32_t last_render_time_ = 0;
};

}

#endif
