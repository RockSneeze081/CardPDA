#include "time_manager.h"
#include <sys/time.h>

namespace CardOS {

TimeManager& TimeManager::getInstance() {
    static TimeManager instance;
    return instance;
}

void TimeManager::init() {
    configTime(0, 0, "pool.ntp.org", "time.nist.gov");
    update();
}

void TimeManager::update() {
    last_update_ = millis();
}

bool TimeManager::setTimezone(const char* tz) {
    if (tz == nullptr) return false;
    setenv("TZ", tz, 1);
    tzset();
    return true;
}

bool TimeManager::syncNTP(const char* server, uint32_t timeout_ms) {
    if (server == nullptr) return false;

    configTime(0, 0, server);
    
    uint64_t start = millis();
    time_t now;
    
    while (millis() - start < timeout_ms) {
        time(&now);
        if (now > 1000000000) {
            synced_ = true;
            Serial.println("NTP sync successful");
            return true;
        }
        delay(100);
    }
    
    Serial.println("NTP sync failed");
    synced_ = false;
    return false;
}

time_t TimeManager::getEpoch() {
    time_t now;
    time(&now);
    return now;
}

uint32_t TimeManager::getEpochMillis() {
    struct timeval tv;
    gettimeofday(&tv, nullptr);
    return (uint32_t)(tv.tv_sec * 1000 + tv.tv_usec / 1000);
}

int TimeManager::getYear() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_year + 1900;
}

int TimeManager::getMonth() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_mon + 1;
}

int TimeManager::getDay() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_mday;
}

int TimeManager::getHour() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_hour;
}

int TimeManager::getMinute() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_min;
}

int TimeManager::getSecond() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_sec;
}

int TimeManager::getDayOfWeek() {
    time_t now = getEpoch();
    struct tm timeinfo;
    localtime_r(&now, &timeinfo);
    return timeinfo.tm_wday;
}

String TimeManager::getDateStr(char sep) {
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d%c%02d%c%02d", 
             getYear(), sep, getMonth(), sep, getDay());
    return String(buf);
}

String TimeManager::getTimeStr() {
    char buf[9];
    snprintf(buf, sizeof(buf), "%02d:%02d:%02d",
             getHour(), getMinute(), getSecond());
    return String(buf);
}

String TimeManager::getDateTimeStr() {
    return getDateStr() + " " + getTimeStr();
}

String TimeManager::getDateFileStr() {
    char buf[11];
    snprintf(buf, sizeof(buf), "%04d-%02d-%02d",
             getYear(), getMonth(), getDay());
    return String(buf);
}

bool TimeManager::setDateTime(int year, int month, int day, int hour, int min, int sec) {
    if (!isValidDate(year, month, day)) return false;
    if (hour < 0 || hour > 23 || min < 0 || min > 59 || sec < 0 || sec > 59) return false;

    struct tm tm;
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    tm.tm_hour = hour;
    tm.tm_min = min;
    tm.tm_sec = sec;
    tm.tm_isdst = 0;

    time_t t = mktime(&tm);
    struct timeval tv = { t, 0 };
    settimeofday(&tv, nullptr);

    synced_ = true;
    return true;
}

bool TimeManager::isValidDate(int year, int month, int day) {
    if (year < 1970 || year > 2099) return false;
    if (month < 1 || month > 12) return false;
    if (day < 1 || day > daysInMonth(year, month)) return false;
    return true;
}

bool TimeManager::isLeapYear(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int TimeManager::daysInMonth(int year, int month) {
    static const int days[] = { 31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31 };
    if (month == 2 && isLeapYear(year)) return 29;
    return days[month - 1];
}

int TimeManager::dayOfWeek(int year, int month, int day) {
    struct tm tm = {};
    tm.tm_year = year - 1900;
    tm.tm_mon = month - 1;
    tm.tm_mday = day;
    mktime(&tm);
    return tm.tm_wday;
}

}
