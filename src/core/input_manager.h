#ifndef CARDOS_INPUT_MANAGER_H
#define CARDOS_INPUT_MANAGER_H

#include <Arduino.h>

namespace CardOS {

enum class KeyEvent : uint8_t {
    NONE = 0,
    UP,
    DOWN,
    LEFT,
    RIGHT,
    ENTER,
    BACK,
    ESC,
    TAB,
    SPACE,
    DEL,
    BACKSPACE,
    PAGE_UP,
    PAGE_DOWN,
    HOME,
    END,
    CHAR,
    F1,
    F2,
    F3,
    F4
};

enum class KeyModifiers : uint8_t {
    NONE = 0,
    SHIFT = 1,
    CTRL = 2,
    ALT = 4
};

struct KeyInfo {
    KeyEvent event;
    KeyModifiers modifiers;
    char character;
    uint8_t key_code;
};

class InputManager {
public:
    static InputManager& getInstance();

    void init();
    void update();

    bool isKeyPressed(KeyEvent key);
    bool wasKeyPressed(KeyEvent key);
    bool wasKeyJustPressed(KeyEvent key);

    KeyInfo getKeyInfo();
    char getCharInput();

    void setDebounceTime(uint16_t ms) { debounce_time_ = ms; }
    uint16_t getDebounceTime() const { return debounce_time_; }

    void enableKeyRepeat(bool enable) { key_repeat_enabled_ = enable; }
    bool isKeyRepeatEnabled() const { return key_repeat_enabled_; }

    void setRepeatDelay(uint16_t delay_ms) { repeat_delay_ = delay_ms; }
    void setRepeatRate(uint16_t rate_ms) { repeat_rate_ = rate_ms; }

private:
    InputManager() 
        : debounce_time_(50)
        , key_repeat_enabled_(false)
        , repeat_delay_(500)
        , repeat_rate_(100)
        , last_key_time_(0)
        , repeat_started_(false) {}
    ~InputManager() = default;
    InputManager(const InputManager&) = delete;
    InputManager& operator=(const InputManager&) = delete;

    KeyEvent mapKey(uint8_t row, uint8_t col);
    char mapChar(uint8_t row, uint8_t col, bool shift);

    uint16_t debounce_time_;
    bool key_repeat_enabled_;
    uint16_t repeat_delay_;
    uint16_t repeat_rate_;
    uint64_t last_key_time_;
    bool repeat_started_;
    
    KeyEvent last_key_ = KeyEvent::NONE;
    KeyEvent current_key_ = KeyEvent::NONE;
    char current_char_ = 0;
    bool shift_active_ = false;
};

}

#endif
