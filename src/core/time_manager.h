#ifndef CARDOS_TIME_MANAGER_H
#define CARDOS_TIME_MANAGER_H

#include <Arduino.h>
#include <time.h>

namespace CardOS {

class TimeManager {
public:
    static TimeManager& getInstance();

    void init();
    void update();

    bool setTimezone(const char* tz);
    bool syncNTP(const char* server = "pool.ntp.org", uint32_t timeout_ms = 10000);
    bool isSynced() const { return synced_; }

    time_t getEpoch();
    uint32_t getEpochMillis();

    int getYear();
    int getMonth();
    int getDay();
    int getHour();
    int getMinute();
    int getSecond();
    int getDayOfWeek();

    String getDateStr(char sep = '-');
    String getTimeStr();
    String getDateTimeStr();
    String getDateFileStr();

    bool setDateTime(int year, int month, int day, int hour, int min, int sec);

    static bool isValidDate(int year, int month, int day);
    static bool isLeapYear(int year);
    static int daysInMonth(int year, int month);
    static int dayOfWeek(int year, int month, int day);

private:
    TimeManager() : synced_(false), last_update_(0) {}
    ~TimeManager() = default;
    TimeManager(const TimeManager&) = delete;
    TimeManager& operator=(const TimeManager&) = delete;

    bool synced_;
    uint64_t last_update_;
};

}

#endif
