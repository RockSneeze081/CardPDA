#ifndef CARDOS_SYNC_MODULE_H
#define CARDOS_SYNC_MODULE_H

#include "module_manager.h"
#include <WiFi.h>
#include <WebServer.h>

namespace CardOS {

class SyncModule : public Module {
public:
    void init() override;
    void update() override;
    void render() override;
    void destroy() override;

    const char* getName() const override { return "Sync"; }
    ModuleID getID() const override { return ModuleID::SYNC; }

    void onExit() override;

private:
    enum class State : uint8_t {
        IDLE,
        CONNECTING,
        CONNECTED,
        SERVER_RUNNING,
        ERROR
    };

    void renderIdle();
    void renderConnecting();
    void renderConnected();
    void renderError();
    void handleIdleInput();
    void handleConnectedInput();
    void handleWebRequest();
    void startServer();
    void stopServer();
    bool connectWiFi(const char* ssid, const char* password);
    void disconnectWiFi();

    State state_ = State::IDLE;
    bool server_running_ = false;
    WebServer* server_ = nullptr;
    char ssid_[32];
    char password_[32];
    int input_mode_ = 0;
    int input_pos_ = 0;
    uint32_t last_update_time_ = 0;
};

}

#endif
