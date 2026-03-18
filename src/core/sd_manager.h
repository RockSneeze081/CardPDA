#ifndef CARDOS_SD_MANAGER_H
#define CARDOS_SD_MANAGER_H

#include <Arduino.h>
#include <FS.h>
#include <SD.h>
#include <SPI.h>

namespace CardOS {

class SDManager {
public:
    static SDManager& getInstance();

    bool init();
    bool init(int8_t cs_pin, uint32_t frequency = 4000000);
    void flush();
    void deinit();

    bool exists(const char* path);
    bool createDirectory(const char* path);
    bool removeDirectory(const char* path);
    bool remove(const char* path);
    bool rename(const char* old_path, const char* new_path);

    File open(const char* path, const char* mode = FILE_READ);
    File open(const String& path, const char* mode = FILE_READ) {
        return open(path.c_str(), mode);
    }

    bool writeFile(const char* path, const char* content);
    bool appendFile(const char* path, const char* content);
    String readFile(const char* path);
    bool readFile(const char* path, char* buffer, size_t max_len);

    bool isMounted() const { return mounted_; }
    uint64_t getTotalBytes();
    uint64_t getFreeBytes();

private:
    SDManager() : mounted_(false), cs_pin_(-1), last_flush_(0) {}
    ~SDManager() = default;
    SDManager(const SDManager&) = delete;
    SDManager& operator=(const SDManager&) = delete;

    bool mounted_;
    int8_t cs_pin_;
    uint64_t last_flush_;
};

}

#endif
