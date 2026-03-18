#include <Arduino.h>
#include "core/system.h"
#include "core/sd_manager.h"
#include "core/time_manager.h"
#include "core/input_manager.h"
#include "core/ui_manager.h"
#include "modules/module_manager.h"
#include "modules/home_module.h"
#include "modules/notes_module.h"
#include "modules/tasks_module.h"
#include "modules/calendar_module.h"
#include "modules/files_module.h"
#include "modules/sync_module.h"

using namespace CardOS;

static HomeModule home_module;
static NotesModule notes_module;
static TasksModule tasks_module;
static CalendarModule calendar_module;
static FilesModule files_module;
static SyncModule sync_module;

void setup() {
    System::getInstance().init();
    
    UIManager::getInstance().init();
    InputManager::getInstance().init();
    TimeManager::getInstance().init();
    
    ModuleManager::getInstance().registerModule(&home_module);
    ModuleManager::getInstance().registerModule(&notes_module);
    ModuleManager::getInstance().registerModule(&tasks_module);
    ModuleManager::getInstance().registerModule(&calendar_module);
    ModuleManager::getInstance().registerModule(&files_module);
    ModuleManager::getInstance().registerModule(&sync_module);
    
    ModuleManager::getInstance().switchModule(ModuleID::HOME);
    
    Serial.println("\n=== CardOS Ready ===");
    Serial.println("Press function keys F1-F4 to switch modules");
    Serial.println("F1: Home | F2: Notes | F3: Tasks | F4: Calendar\n");
}

void loop() {
    InputManager::getInstance().update();
    TimeManager::getInstance().update();
    System::getInstance().update();
    ModuleManager::getInstance().loop();
    delay(10);
}
