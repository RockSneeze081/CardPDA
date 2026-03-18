#include "ui_manager.h"

namespace CardOS {

UIManager& UIManager::getInstance() {
    static UIManager instance;
    return instance;
}

void UIManager::init() {
    lcd.init();
    lcd.setRotation(1);
    lcd.fillScreen(TFT_BLACK);
    lcd.setTextSize(1);
    lcd.setFont(&fonts::Font2);
    width_ = lcd.width();
    height_ = lcd.height();
    brightness_ = 100;
    current_color_ = TFT_WHITE;
    current_bg_ = TFT_BLACK;
    Serial.println("UI Manager initialized (LovyanGFX)");
}

void UIManager::update() {
}

void UIManager::clear() {
    lcd.fillScreen(TFT_BLACK);
}

void UIManager::clear(uint16_t bg) {
    lcd.fillScreen(bg);
}

void UIManager::setTextColor(uint16_t color) {
    current_color_ = color;
    lcd.setTextColor(color);
}

void UIManager::setTextColor(uint16_t fg, uint16_t bg) {
    current_color_ = fg;
    current_bg_ = bg;
    lcd.setTextColor(fg, bg);
}

void UIManager::setTextSize(uint8_t size) {
    lcd.setTextSize(size);
}

void UIManager::setTextFont(uint8_t font) {
    lcd.setFont(nullptr);
    lcd.setTextFont(font);
}

void UIManager::setCursor(int16_t x, int16_t y) {
    lcd.setCursor(x, y);
}

void UIManager::getCursor(int16_t& x, int16_t& y) {
    x = lcd.getCursorX();
    y = lcd.getCursorY();
}

void UIManager::drawText(const char* text) {
    lcd.print(text);
}

void UIManager::drawTextAt(int16_t x, int16_t y, const char* text, TextAlign align) {
    int16_t w = textWidth(text);
    int16_t draw_x = x;

    switch(align) {
        case TextAlign::CENTER:
            draw_x = x - w / 2;
            break;
        case TextAlign::RIGHT:
            draw_x = x - w;
            break;
        default:
            break;
    }

    lcd.setCursor(draw_x, y);
    lcd.print(text);
}

void UIManager::drawChar(char c) {
    int16_t cx = lcd.getCursorX();
    int16_t cy = lcd.getCursorY();
    lcd.drawChar(c, cx, cy);
}

void UIManager::drawNumber(int num) {
    lcd.print(num);
}

void UIManager::drawNumber(float num, uint8_t decimals) {
    lcd.print(num, decimals);
}

void UIManager::drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color) {
    lcd.drawLine(x1, y1, x2, y2, color);
}

void UIManager::drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    lcd.drawRect(x, y, w, h, color);
}

void UIManager::fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color) {
    lcd.fillRect(x, y, w, h, color);
}

void UIManager::drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
    lcd.drawCircle(x, y, r, color);
}

void UIManager::fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color) {
    lcd.fillCircle(x, y, r, color);
}

void UIManager::drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t progress, uint16_t color, uint16_t fill_color) {
    lcd.drawRect(x, y, w, h, color);
    if (progress > 0) {
        uint16_t fill_w = (w - 2) * progress / 100;
        if (fill_w > 0) {
            lcd.fillRect(x + 1, y + 1, fill_w, h - 2, fill_color);
        }
    }
}

void UIManager::drawSelectionBox(int16_t x, int16_t y, int16_t w, int16_t h, bool selected) {
    if (selected) {
        lcd.fillRect(x, y, w, h, current_color_);
    } else {
        lcd.drawRect(x, y, w, h, current_color_);
    }
}

void UIManager::setBrightness(uint8_t brightness) {
    brightness_ = brightness;
    if (brightness > 100) brightness = 100;
    lcd.setBrightness(brightness);
}

int16_t UIManager::textWidth(const char* text) {
    return lcd.textWidth(text);
}

int16_t UIManager::textHeight() {
    return lcd.fontHeight();
}

void UIManager::drawHeader(const char* title) {
    fillRect(0, 0, width_, 24, TFT_BLACK);
    setTextColor(TFT_WHITE, TFT_BLACK);
    drawTextAt(width_ / 2, 6, title, TextAlign::CENTER);
}

void UIManager::drawFooter(const char* left, const char* center, const char* right) {
    fillRect(0, height_ - 20, width_, 20, TFT_BLACK);
    setTextColor(TFT_WHITE, TFT_BLACK);
    
    if (left) drawTextAt(4, height_ - 14, left, TextAlign::LEFT);
    if (center) drawTextAt(width_ / 2, height_ - 14, center, TextAlign::CENTER);
    if (right) drawTextAt(width_ - 4, height_ - 14, right, TextAlign::RIGHT);
}

void UIManager::drawStatusBar(const char* text) {
    fillRect(0, height_ - 36, width_, 16, TFT_BLACK);
    setTextColor(TFT_LIGHTGREY, TFT_BLACK);
    drawTextAt(width_ / 2, height_ - 32, text, TextAlign::CENTER);
}

void UIManager::drawLoading(const char* message, uint32_t dots) {
    static uint32_t counter = 0;
    clear();
    setTextColor(TFT_WHITE);
    drawTextAt(width_ / 2, height_ / 2 - 10, message, TextAlign::CENTER);
    
    char dot_str[8] = "";
    for (uint32_t i = 0; i < (counter / 20) % (dots + 1); i++) {
        strcat(dot_str, ".");
    }
    drawTextAt(width_ / 2 + textWidth(message), height_ / 2 - 10, dot_str);
    counter++;
}

void UIManager::drawListItem(int16_t y, const char* text, bool selected, bool checked) {
    const int16_t item_h = 28;
    const int16_t item_x = 4;
    const int16_t item_w = width_ - 8;

    if (selected) {
        fillRect(item_x, y, item_w, item_h, TFT_WHITE);
        setTextColor(TFT_BLACK, TFT_WHITE);
    } else {
        setTextColor(TFT_WHITE, TFT_BLACK);
    }

    if (checked) {
        drawText("[x] ");
    } else {
        drawText("[ ] ");
    }
    
    setCursor(item_x + textWidth("[ ] "), y + 8);
    drawText(text);
}

void UIManager::drawScrollbar(int16_t x, int16_t y, int16_t h, int16_t total_items, int16_t visible_items, int16_t scroll_pos) {
    if (total_items <= visible_items) return;

    int16_t thumb_h = h * visible_items / total_items;
    if (thumb_h < 10) thumb_h = 10;

    float ratio = (float)scroll_pos / (total_items - visible_items);
    if (total_items - visible_items <= 0) ratio = 0;
    
    int16_t thumb_y = y + ratio * (h - thumb_h);

    fillRect(x, y, 6, h, TFT_DARKGREY);
    setTextColor(TFT_BLACK);
    fillRect(x + 1, thumb_y, 4, thumb_h, TFT_WHITE);
}

}
