#include "sd_manager.h"
#include "config.h"

namespace CardOS {

SDManager& SDManager::getInstance() {
    static SDManager instance;
    return instance;
}

bool SDManager::init() {
    return init(SD_CS_PIN, 4000000);
}

bool SDManager::init(int8_t cs_pin, uint32_t frequency) {
    if (mounted_) {
        return true;
    }

    cs_pin_ = cs_pin;
    
    SPI.begin(SCK, MISO, MOSI, cs_pin);
    
    if (!SD.begin(cs_pin, SPI, frequency)) {
        Serial.println("SD Card mount failed!");
        return false;
    }

    uint8_t card_type = SD.cardType();
    if (card_type == CARD_NONE) {
        Serial.println("No SD card attached");
        return false;
    }

    Serial.printf("SD Card Type: %s\n", 
        card_type == CARD_MMC ? "MMC" :
        card_type == CARD_SD ? "SDSC" :
        card_type == CARD_SDHC ? "SDHC" : "UNKNOWN");

    Serial.printf("Total space: %llu MB\n", SD.totalBytes() / (1024 * 1024));
    Serial.printf("Free space: %llu MB\n", (SD.totalBytes() - SD.usedBytes()) / (1024 * 1024));

    mounted_ = true;
    return true;
}

void SDManager::flush() {
    if (mounted_) {
        last_flush_ = millis();
    }
}

void SDManager::deinit() {
    if (mounted_) {
        SD.end();
        SPI.end();
        mounted_ = false;
    }
}

bool SDManager::exists(const char* path) {
    if (!mounted_) return false;
    return SD.exists(path);
}

bool SDManager::createDirectory(const char* path) {
    if (!mounted_) return false;
    
    if (SD.exists(path)) {
        return true;
    }
    
    if (SD.mkdir(path)) {
        Serial.printf("Directory created: %s\n", path);
        flush();
        return true;
    }
    
    Serial.printf("Failed to create directory: %s\n", path);
    return false;
}

bool SDManager::removeDirectory(const char* path) {
    if (!mounted_) return false;
    
    File root = open(path, FILE_READ);
    if (!root) return false;

    File file = root.openNextFile();
    while (file) {
        String file_path = String(path) + "/" + file.name();
        if (file.isDirectory()) {
            removeDirectory(file_path.c_str());
        } else {
            SD.remove(file_path.c_str());
        }
        file.close();
        file = root.openNextFile();
    }
    root.close();

    bool result = SD.rmdir(path);
    if (result) flush();
    return result;
}

bool SDManager::remove(const char* path) {
    if (!mounted_) return false;
    bool result = SD.remove(path);
    if (result) flush();
    return result;
}

bool SDManager::rename(const char* old_path, const char* new_path) {
    if (!mounted_) return false;
    bool result = SD.rename(old_path, new_path);
    if (result) flush();
    return result;
}

File SDManager::open(const char* path, const char* mode) {
    if (!mounted_) return File();
    return SD.open(path, mode);
}

bool SDManager::writeFile(const char* path, const char* content) {
    if (!mounted_) return false;
    
    File file = open(path, FILE_WRITE);
    if (!file) {
        Serial.printf("Failed to open file for writing: %s\n", path);
        return false;
    }
    
    size_t written = file.print(content);
    file.close();
    flush();
    
    return written > 0;
}

bool SDManager::appendFile(const char* path, const char* content) {
    if (!mounted_) return false;
    
    File file = open(path, FILE_APPEND);
    if (!file) {
        Serial.printf("Failed to open file for append: %s\n", path);
        return false;
    }
    
    size_t written = file.print(content);
    file.close();
    flush();
    
    return written > 0;
}

String SDManager::readFile(const char* path) {
    String result;
    if (!mounted_) return result;
    
    File file = open(path, FILE_READ);
    if (!file) {
        return result;
    }
    
    while (file.available()) {
        result += (char)file.read();
    }
    file.close();
    
    return result;
}

bool SDManager::readFile(const char* path, char* buffer, size_t max_len) {
    if (!mounted_ || buffer == nullptr || max_len == 0) return false;
    
    File file = open(path, FILE_READ);
    if (!file) return false;
    
    size_t read_len = file.readBytes(buffer, max_len - 1);
    buffer[read_len] = '\0';
    file.close();
    
    return read_len > 0;
}

uint64_t SDManager::getTotalBytes() {
    if (!mounted_) return 0;
    return SD.totalBytes();
}

uint64_t SDManager::getFreeBytes() {
    if (!mounted_) return 0;
    return SD.totalBytes() - SD.usedBytes();
}

}
