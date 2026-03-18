#ifndef CARDOS_SYSTEM_H
#define CARDOS_SYSTEM_H

#include <Arduino.h>
#include <esp_system.h>

#define CARDOS_VERSION "0.1.0"
#define CARDOS_DEVICE "M5Stack Cardputer"
#define CARDOS_MCU "ESP32-S3"

namespace CardOS {

constexpr uint32_t MAX_RAM_USAGE = 300 * 1024;

enum class ModuleID : uint8_t {
    NONE = 0,
    HOME,
    NOTES,
    TASKS,
    CALENDAR,
    FILES,
    SYNC,
    SETTINGS
};

struct SystemConfig {
    bool sd_mounted = false;
    bool wifi_enabled = false;
    bool rtc_synced = false;
    uint32_t boot_count = 0;
    uint32_t free_heap_start = 0;
};

class System {
public:
    static System& getInstance();

    void init();
    void update();
    void restart();

    uint32_t getFreeHeap();
    uint32_t getMinFreeHeap();
    uint32_t getHeapSize();
    float getHeapUsagePercent();

    SystemConfig& getConfig() { return config_; }
    ModuleID getCurrentModule() const { return current_module_; }
    void setCurrentModule(ModuleID module) { current_module_ = module; }

    bool isPSRAMEnabled();
    uint32_t getFreePSRAM();

    uint64_t getUptimeMs() const { return uptime_ms_; }

private:
    System() : uptime_ms_(0), last_heap_check_(0) {}
    ~System() = default;
    System(const System&) = delete;
    System& operator=(const System&) = delete;

    void initHardware();
    void initFileSystem();
    void logBoot();

    SystemConfig config_;
    ModuleID current_module_ = ModuleID::HOME;
    uint64_t uptime_ms_ = 0;
    uint32_t last_heap_check_;
};

}

#endif
