#include "system.h"
#include "sd_manager.h"
#include "time_manager.h"
#include <esp_timer.h>

namespace CardOS {

System& System::getInstance() {
    static System instance;
    return instance;
}

void System::init() {
    Serial.begin(115200);
    delay(500);

    Serial.printf("\n=== CardOS v%s ===\n", CARDOS_VERSION);
    Serial.printf("Device: %s\n", CARDOS_DEVICE);
    Serial.printf("MCU: %s\n", CARDOS_MCU);
    Serial.printf("Free Heap at boot: %u bytes\n", ESP.getFreeHeap());

    config_.free_heap_start = ESP.getFreeHeap();
    config_.boot_count = 0;

    initHardware();
    initFileSystem();
    logBoot();

    Serial.println("System initialized successfully\n");
}

void System::initHardware() {
    if (isPSRAMEnabled()) {
        Serial.printf("PSRAM: %u bytes available\n", getFreePSRAM());
    }
    Serial.printf("Chip Model: %s\n", ESP.getChipModel());
    Serial.printf("Chip Revision: %d\n", ESP.getChipRevision());
    Serial.printf("CPU Frequency: %d MHz\n", ESP.getCpuFreqMHz());
}

void System::initFileSystem() {
    if (SDManager::getInstance().init()) {
        config_.sd_mounted = true;

        SDManager::getInstance().createDirectory("/cardos");
        SDManager::getInstance().createDirectory("/cardos/notes");
        SDManager::getInstance().createDirectory("/cardos/tasks");
        SDManager::getInstance().createDirectory("/cardos/calendar");
        SDManager::getInstance().createDirectory("/cardos/vault");
        SDManager::getInstance().createDirectory("/cardos/config");
        SDManager::getInstance().createDirectory("/cardos/logs");

        Serial.println("SD card initialized and directories created");
    } else {
        Serial.println("WARNING: SD card not available");
    }
}

void System::logBoot() {
    config_.boot_count++;
    uptime_ms_ = 0;
}

void System::update() {
    uint64_t now = esp_timer_get_time() / 1000;
    uptime_ms_ = now;
}

void System::restart() {
    SDManager::getInstance().flush();
    delay(100);
    ESP.restart();
}

uint32_t System::getFreeHeap() {
    return ESP.getFreeHeap();
}

uint32_t System::getMinFreeHeap() {
    return ESP.getMinFreeHeap();
}

uint32_t System::getHeapSize() {
    return ESP.getHeapSize();
}

float System::getHeapUsagePercent() {
    uint32_t total = getHeapSize();
    uint32_t free = getFreeHeap();
    if (total == 0) return 0.0f;
    return 100.0f * (total - free) / total;
}

bool System::isPSRAMEnabled() {
    return psramFound();
}

uint32_t System::getFreePSRAM() {
    if (isPSRAMEnabled()) {
        return ESP.getFreePsram();
    }
    return 0;
}

}
