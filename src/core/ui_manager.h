#ifndef CARDOS_UI_MANAGER_H
#define CARDOS_UI_MANAGER_H

#include <Arduino.h>
#include <LovyanGFX.hpp>

namespace CardOS {

enum class TextAlign : uint8_t {
    LEFT,
    CENTER,
    RIGHT
};

class LGFX_ESP32_STM32_ILI9341 : public lgfx::LGFX_Device {
public:
    lgfx::Panel_ILI9341 _panel;
    lgfx::Bus_SPI _bus;

    LGFX_ESP32_STM32_ILI9341(void) {
        auto cfg = _bus.config();
        cfg.spi_host = SPI2_HOST;
        cfg.spi_mode = 0;
        cfg.freq_write = 40000000;
        cfg.freq_read = 16000000;
        cfg.spi_3wire = false;
        cfg.use_lock = true;
        cfg.dma_channel = SPI_DMA_CH_AUTO;
        cfg.pin_mosi = 37;
        cfg.pin_miso = 35;
        cfg.pin_sclk = 36;
        cfg.pin_dc = 8;
        _bus.config(cfg);
        _panel.setBus(&_bus);

        auto pcfg = _panel.config();
        pcfg.pin_cs = 7;
        pcfg.pin_rst = 9;
        pcfg.pin_busy = -1;
        pcfg.memory_width = 240;
        pcfg.memory_height = 320;
        pcfg.panel_width = 240;
        pcfg.panel_height = 320;
        pcfg.offset_x = 0;
        pcfg.offset_y = 0;
        pcfg.rgb_order = false;
        pcfg.dummy_read_pixel = 8;
        pcfg.dummy_read_bits = 1;
        pcfg.readable = true;
        pcfg.invert = false;
        pcfg.bus_shared = false;
        _panel.config(pcfg);

        setPanel(&_panel);
    }
};

class UIManager {
public:
    static UIManager& getInstance();

    void init();
    void update();
    void clear();
    void clear(uint16_t bg);

    void setTextColor(uint16_t color);
    void setTextColor(uint16_t fg, uint16_t bg);
    void setTextSize(uint8_t size);
    void setTextFont(uint8_t font);
    void setCursor(int16_t x, int16_t y);
    void getCursor(int16_t& x, int16_t& y);

    void drawText(const char* text);
    void drawText(const String& text) { drawText(text.c_str()); }
    void drawTextAt(int16_t x, int16_t y, const char* text, TextAlign align = TextAlign::LEFT);
    void drawTextAt(int16_t x, int16_t y, const String& text, TextAlign align = TextAlign::LEFT) {
        drawTextAt(x, y, text.c_str(), align);
    }

    void drawChar(char c);
    void drawNumber(int num);
    void drawNumber(float num, uint8_t decimals);

    void drawLine(int16_t x1, int16_t y1, int16_t x2, int16_t y2, uint16_t color);
    void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void fillRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t color);
    void drawCircle(int16_t x, int16_t y, int16_t r, uint16_t color);
    void fillCircle(int16_t x, int16_t y, int16_t r, uint16_t color);

    void drawProgressBar(int16_t x, int16_t y, int16_t w, int16_t h, uint8_t progress, uint16_t color = 0xFFFF, uint16_t fill_color = 0x07E0);
    void drawSelectionBox(int16_t x, int16_t y, int16_t w, int16_t h, bool selected);

    int16_t getWidth() const { return width_; }
    int16_t getHeight() const { return height_; }

    void setBrightness(uint8_t brightness);
    uint8_t getBrightness() const { return brightness_; }

    void drawHeader(const char* title);
    void drawFooter(const char* left, const char* center, const char* right);
    void drawStatusBar(const char* text);

    void drawLoading(const char* message, uint32_t dots = 3);

    int16_t textWidth(const char* text);
    int16_t textWidth(const String& text) { return textWidth(text.c_str()); }
    int16_t textHeight();

    void drawListItem(int16_t y, const char* text, bool selected, bool checked = false);
    void drawScrollbar(int16_t x, int16_t y, int16_t h, int16_t total_items, int16_t visible_items, int16_t scroll_pos);

    lgfx::LGFX_Device* getDisplay() { return &lcd; }

private:
    UIManager() : width_(320), height_(240), brightness_(100), current_color_(0xFFFF), current_bg_(0x0000) {}
    ~UIManager() = default;
    UIManager(const UIManager&) = delete;
    UIManager& operator=(const UIManager&) = delete;

    int16_t width_;
    int16_t height_;
    uint8_t brightness_;
    uint16_t current_color_;
    uint16_t current_bg_;
    
    LGFX_ESP32_STM32_ILI9341 lcd;
};

#define TFT_BLACK       0x0000
#define TFT_NAVY        0x000F
#define TFT_DARKGREEN   0x03E0
#define TFT_DARKCYAN    0x03EF
#define TFT_MAROON      0x7800
#define TFT_PURPLE      0x780F
#define TFT_OLIVE       0x7BE0
#define TFT_LIGHTGREY   0xC618
#define TFT_DARKGREY    0x7BEF
#define TFT_BLUE        0x001F
#define TFT_GREEN       0x07E0
#define TFT_CYAN        0x07FF
#define TFT_RED         0xF800
#define TFT_MAGENTA     0xF81F
#define TFT_YELLOW      0xFFE0
#define TFT_WHITE       0xFFFF
#define TFT_ORANGE      0xFD20
#define TFT_GREENYELLOW 0xB7E0
#define TFT_PINK        0xF81F

}
#endif
