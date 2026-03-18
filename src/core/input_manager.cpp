#include "input_manager.h"
#include <driver/gpio.h>
#include <driver/uart.h>

namespace CardOS {

InputManager& InputManager::getInstance() {
    static InputManager instance;
    return instance;
}

void InputManager::init() {
    Serial.begin(115200);
    last_key_time_ = 0;
    current_key_ = KeyEvent::NONE;
    last_key_ = KeyEvent::NONE;
    current_char_ = 0;
    shift_active_ = false;
    repeat_started_ = false;
    Serial.println("Input Manager initialized (Serial + GPIO)");
}

void InputManager::update() {
    last_key_ = current_key_;
    current_key_ = KeyEvent::NONE;
    current_char_ = 0;

    if (Serial.available() > 0) {
        char c = Serial.read();
        
        switch(c) {
            case '\n':
            case '\r':
                current_key_ = KeyEvent::ENTER;
                break;
            case '\t':
                current_key_ = KeyEvent::TAB;
                break;
            case 127:
            case '\b':
                current_key_ = KeyEvent::BACKSPACE;
                break;
            case 27:
                current_key_ = KeyEvent::ESC;
                break;
            case ' ':
                current_key_ = KeyEvent::SPACE;
                current_char_ = ' ';
                break;
            case 'w':
            case 'W':
            case 'i':
            case 'I':
            case 72:
                current_key_ = KeyEvent::UP;
                break;
            case 's':
            case 'S':
            case 'k':
            case 'K':
            case 66:
                current_key_ = KeyEvent::DOWN;
                break;
            case 'a':
            case 'A':
            case 'j':
            case 'J':
            case 37:
                current_key_ = KeyEvent::LEFT;
                break;
            case 'd':
            case 'D':
            case 'l':
            case 'L':
            case 39:
                current_key_ = KeyEvent::RIGHT;
                break;
            case 33:
                current_key_ = KeyEvent::PAGE_UP;
                break;
            case 34:
                current_key_ = KeyEvent::PAGE_DOWN;
                break;
            case 'q':
            case 'Q':
                current_key_ = KeyEvent::ESC;
                break;
            case '1':
                if (shift_active_) current_key_ = KeyEvent::F1;
                break;
            case '2':
                if (shift_active_) current_key_ = KeyEvent::F2;
                break;
            case '3':
                if (shift_active_) current_key_ = KeyEvent::F3;
                break;
            case '4':
                if (shift_active_) current_key_ = KeyEvent::F4;
                break;
            default:
                if (c >= 32 && c < 127) {
                    current_char_ = c;
                    current_key_ = KeyEvent::CHAR;
                }
                break;
        }
        
        if (c == '`') {
            shift_active_ = !shift_active_;
        }
        
        last_key_time_ = millis();
        repeat_started_ = false;
    }
}

bool InputManager::isKeyPressed(KeyEvent key) {
    return current_key_ == key;
}

bool InputManager::wasKeyPressed(KeyEvent key) {
    return last_key_ == key || current_key_ == key;
}

bool InputManager::wasKeyJustPressed(KeyEvent key) {
    if (current_key_ == key && last_key_ != key) {
        return true;
    }
    return false;
}

KeyInfo InputManager::getKeyInfo() {
    KeyInfo info;
    info.event = current_key_;
    info.modifiers = shift_active_ ? KeyModifiers::SHIFT : KeyModifiers::NONE;
    info.character = current_char_;
    info.key_code = static_cast<uint8_t>(current_key_);
    return info;
}

char InputManager::getCharInput() {
    return current_char_;
}

KeyEvent InputManager::mapKey(uint8_t row, uint8_t col) {
    (void)row;
    (void)col;
    return KeyEvent::NONE;
}

char InputManager::mapChar(uint8_t row, uint8_t col, bool shift) {
    (void)row;
    (void)col;
    (void)shift;
    return 0;
}

}
