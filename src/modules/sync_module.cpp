#include "sync_module.h"
#include "../core/ui_manager.h"
#include "../core/input_manager.h"
#include "../core/sd_manager.h"
#include "../core/time_manager.h"
#include "../core/system.h"

namespace CardOS {

void SyncModule::init() {
    state_ = State::IDLE;
    server_running_ = false;
    server_ = nullptr;
    input_mode_ = 0;
    input_pos_ = 0;
    ssid_[0] = '\0';
    password_[0] = '\0';
    last_update_time_ = 0;
}

void SyncModule::update() {
    if (state_ == State::IDLE || state_ == State::ERROR) {
        handleIdleInput();
    } else if (state_ == State::CONNECTED || state_ == State::SERVER_RUNNING) {
        handleConnectedInput();
    }
}

void SyncModule::render() {
    UIManager& ui = UIManager::getInstance();
    ui.clear(TFT_BLACK);
    
    if (state_ == State::IDLE) {
        renderIdle();
    } else if (state_ == State::CONNECTING) {
        renderConnecting();
    } else if (state_ == State::CONNECTED || state_ == State::SERVER_RUNNING) {
        renderConnected();
    } else {
        renderError();
    }
}

void SyncModule::renderIdle() {
    UIManager& ui = UIManager::getInstance();
    
    ui.setTextColor(TFT_CYAN, TFT_BLACK);
    ui.fillRect(0, 0, 320, 20, TFT_CYAN);
    ui.setTextColor(TFT_BLACK, TFT_CYAN);
    ui.drawTextAt(160, 4, "Sync - WiFi Server", TextAlign::CENTER);
    
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.drawTextAt(160, 50, "WiFi Sync Module", TextAlign::CENTER);
    ui.drawTextAt(160, 70, "Connect to share data", TextAlign::CENTER);
    ui.drawTextAt(160, 90, "with other devices", TextAlign::CENTER);
    
    if (input_mode_ == 0) {
        ui.setTextColor(TFT_DARKGREY, TFT_BLACK);
        ui.drawTextAt(160, 130, "Press [ENT] to configure", TextAlign::CENTER);
    } else if (input_mode_ == 1) {
        ui.setTextColor(TFT_WHITE, TFT_BLACK);
        ui.drawTextAt(10, 130, "SSID:");
        ui.fillRect(10, 145, 300, 24, TFT_WHITE);
        ui.setTextColor(TFT_BLACK, TFT_WHITE);
        ui.setCursor(15, 150);
        ui.drawText(ssid_);
    } else if (input_mode_ == 2) {
        ui.setTextColor(TFT_WHITE, TFT_BLACK);
        ui.drawTextAt(10, 130, "Password:");
        ui.fillRect(10, 145, 300, 24, TFT_WHITE);
        ui.setTextColor(TFT_BLACK, TFT_WHITE);
        ui.setCursor(15, 150);
        ui.drawText(password_);
    }
    
    ui.setTextColor(TFT_DARKGREY, TFT_BLACK);
    ui.drawFooter("[ESC] Back", "[ENT] Next", "");
}

void SyncModule::renderConnecting() {
    UIManager& ui = UIManager::getInstance();
    
    ui.setTextColor(TFT_YELLOW, TFT_BLACK);
    ui.fillRect(0, 0, 320, 20, TFT_YELLOW);
    ui.setTextColor(TFT_BLACK, TFT_YELLOW);
    ui.drawTextAt(160, 4, "Connecting...", TextAlign::CENTER);
    
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.drawTextAt(160, 60, "Connecting to WiFi", TextAlign::CENTER);
    ui.drawTextAt(160, 85, ssid_, TextAlign::CENTER);
    
    uint32_t dots = (millis() / 500) % 4;
    char loading[5] = "";
    for (uint32_t i = 0; i < dots; i++) strcat(loading, ".");
    ui.drawTextAt(160 + ui.textWidth(ssid_) / 2 + 5, 85, loading);
    
    ui.setTextColor(TFT_DARKGREY, TFT_BLACK);
    ui.drawFooter("[ESC] Cancel", "", "");
}

void SyncModule::renderConnected() {
    UIManager& ui = UIManager::getInstance();
    
    uint16_t header_color = server_running_ ? TFT_GREEN : TFT_YELLOW;
    ui.fillRect(0, 0, 320, 20, header_color);
    ui.setTextColor(TFT_BLACK, header_color);
    ui.drawTextAt(160, 4, server_running_ ? "Server Running" : "Connected", TextAlign::CENTER);
    
    ui.setTextColor(TFT_WHITE, TFT_BLACK);
    ui.drawTextAt(160, 40, "Connected to WiFi", TextAlign::CENTER);
    ui.drawTextAt(160, 60, ssid_, TextAlign::CENTER);
    
    if (WiFi.status() == WL_CONNECTED) {
        ui.setTextColor(TFT_CYAN, TFT_BLACK);
        ui.drawTextAt(160, 85, "IP Address:", TextAlign::CENTER);
        ui.setTextColor(TFT_GREEN, TFT_BLACK);
        ui.drawTextAt(160, 105, WiFi.localIP().toString().c_str(), TextAlign::CENTER);
        
        if (server_running_) {
            ui.setTextColor(TFT_GREEN, TFT_BLACK);
            ui.drawTextAt(160, 135, "HTTP Server Active", TextAlign::CENTER);
            ui.drawTextAt(160, 155, "Access /api/...", TextAlign::CENTER);
        }
    }
    
    ui.setTextColor(TFT_DARKGREY, TFT_BLACK);
    if (server_running_) {
        ui.drawFooter("[ESC] Stop", "[S] Toggle", "");
    } else {
        ui.drawFooter("[ESC] Disconnect", "[S] Start", "");
    }
}

void SyncModule::renderError() {
    UIManager& ui = UIManager::getInstance();
    
    ui.fillRect(0, 0, 320, 20, TFT_RED);
    ui.setTextColor(TFT_WHITE, TFT_RED);
    ui.drawTextAt(160, 4, "Connection Error", TextAlign::CENTER);
    
    ui.setTextColor(TFT_DARKGREY, TFT_BLACK);
    ui.drawTextAt(160, 60, "Failed to connect", TextAlign::CENTER);
    ui.drawTextAt(160, 85, "Check credentials", TextAlign::CENTER);
    
    ui.drawFooter("[ESC] Back", "", "");
}

void SyncModule::handleIdleInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC) || input.wasKeyJustPressed(KeyEvent::F1)) {
        ModuleManager::getInstance().switchModule(ModuleID::HOME);
        return;
    }
    
    if (input.wasKeyJustPressed(KeyEvent::ENTER)) {
        if (input_mode_ == 0) {
            input_mode_ = 1;
            input_pos_ = 0;
            ssid_[0] = '\0';
        } else if (input_mode_ == 1) {
            input_mode_ = 2;
            input_pos_ = 0;
            password_[0] = '\0';
        } else if (input_mode_ == 2) {
            if (ssid_[0] != '\0') {
                state_ = State::CONNECTING;
                input_mode_ = 0;
                if (connectWiFi(ssid_, password_)) {
                    state_ = State::CONNECTED;
                } else {
                    state_ = State::ERROR;
                }
            }
        }
        return;
    }
    
    if (input_mode_ > 0) {
        char ch = input.getCharInput();
        if (ch >= 32 && ch < 127 && input_pos_ < 31) {
            if (input_mode_ == 1) {
                ssid_[input_pos_++] = ch;
                ssid_[input_pos_] = '\0';
            } else {
                password_[input_pos_++] = ch;
                password_[input_pos_] = '\0';
            }
        }
        
        if (input.wasKeyJustPressed(KeyEvent::BACKSPACE)) {
            if (input_pos_ > 0) {
                input_pos_--;
                if (input_mode_ == 1) {
                    ssid_[input_pos_] = '\0';
                } else {
                    password_[input_pos_] = '\0';
                }
            }
        }
    }
}

void SyncModule::handleConnectedInput() {
    InputManager& input = InputManager::getInstance();
    
    if (input.wasKeyJustPressed(KeyEvent::ESC)) {
        stopServer();
        disconnectWiFi();
        state_ = State::IDLE;
        return;
    }
}

bool SyncModule::connectWiFi(const char* ssid, const char* password) {
    if (ssid == nullptr || strlen(ssid) == 0) {
        return false;
    }
    
    WiFi.mode(WIFI_STA);
    WiFi.begin(ssid, password ? password : "");
    
    uint32_t start = millis();
    while (WiFi.status() != WL_CONNECTED && millis() - start < 15000) {
        delay(100);
    }
    
    return WiFi.status() == WL_CONNECTED;
}

void SyncModule::disconnectWiFi() {
    WiFi.disconnect();
    WiFi.mode(WIFI_OFF);
}

void SyncModule::startServer() {
    if (server_ != nullptr) {
        stopServer();
    }
    
    server_ = new WebServer(80);
    
    server_->on("/api/notes/today", HTTP_GET, [this]() {
        String date = TimeManager::getInstance().getDateFileStr();
        String path = "/cardos/notes/" + date + ".txt";
        if (SDManager::getInstance().exists(path.c_str())) {
            String content = SDManager::getInstance().readFile(path.c_str());
            server_->send(200, "text/plain", content);
        } else {
            server_->send(404, "text/plain", "No notes for today");
        }
    });
    
    server_->on("/api/tasks/today", HTTP_GET, [this]() {
        String date = TimeManager::getInstance().getDateFileStr();
        String path = "/cardos/tasks/" + date + ".json";
        if (SDManager::getInstance().exists(path.c_str())) {
            String content = SDManager::getInstance().readFile(path.c_str());
            server_->send(200, "application/json", content);
        } else {
            server_->send(200, "application/json", "{\"tasks\":[]}");
        }
    });
    
    server_->on("/api/calendar", HTTP_GET, [this]() {
        String path = "/cardos/calendar/events.json";
        if (SDManager::getInstance().exists(path.c_str())) {
            String content = SDManager::getInstance().readFile(path.c_str());
            server_->send(200, "application/json", content);
        } else {
            server_->send(200, "application/json", "{\"events\":[]}");
        }
    });
    
    server_->on("/api/health", HTTP_GET, [this]() {
        String response = "{\"status\":\"ok\"}";
        server_->send(200, "application/json", response);
    });
    
    server_->onNotFound([this]() {
        server_->send(404, "text/plain", "CardOS API - Use /api/* endpoints");
    });
    
    server_->begin();
    server_running_ = true;
    state_ = State::SERVER_RUNNING;
}

void SyncModule::stopServer() {
    if (server_ != nullptr) {
        server_->stop();
        delete server_;
        server_ = nullptr;
    }
    server_running_ = false;
    if (WiFi.status() == WL_CONNECTED) {
        state_ = State::CONNECTED;
    }
}

void SyncModule::onExit() {
    stopServer();
    disconnectWiFi();
}

void SyncModule::destroy() {
    stopServer();
    disconnectWiFi();
}

}
