#include "led_control.h"
#ifdef HAS_RGB_LED
#include <globals.h>
#include "core/display.h"
#include "core/utils.h"
#include <FastLED.h>

CRGB leds[LED_COUNT];

CRGB hsvToRgb(uint16_t h, uint8_t s, uint8_t v) {
    uint8_t f = (h % 60) * 255 / 60;
    uint8_t p = (255 - s) * (uint16_t)v / 255;
    uint8_t q = (255 - f * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t t = (255 - (255 - f) * (uint16_t)s / 255) * (uint16_t)v / 255;
    uint8_t r = 0, g = 0, b = 0;
    switch ((h / 60) % 6) {
        case 0: r = v; g = t; b = p; break;
        case 1: r = q; g = v; b = p; break;
        case 2: r = p; g = v; b = t; break;
        case 3: r = p; g = q; b = v; break;
        case 4: r = t; g = p; b = v; break;
        case 5: r = v; g = p; b = q; break;
    }

    CRGB c;
    c.red = r;
    c.green = g;
    c.blue = b;
    return c;
}


void beginLed() {
#ifdef RGB_LED_CLK
    FastLED.addLeds<LED_TYPE, RGB_LED, RGB_LED_CLK, LED_ORDER>(leds, LED_COUNT);
#else
    FastLED.addLeds<LED_TYPE, RGB_LED, LED_ORDER>(leds, LED_COUNT); // Initialize the LED Object. Only 1 LED.
#endif
    setLedColor(fzerofirmwareConfig.ledColor);
    setLedBrightness(fzerofirmwareConfig.ledBright);
}


void setLedColor(CRGB color) {
    for (int i = 0; i < LED_COUNT; i++) leds[i] = color;
    FastLED.show();
}


void setLedBrightness(int value) {
    value = max(0, min(255, value));
    int bright = 255 * value/100;
    FastLED.setBrightness(bright);
    FastLED.show();
}


void setLedColorConfig() {
    int idx;
    if (fzerofirmwareConfig.ledColor==CRGB::Black) idx=0;
    else if (fzerofirmwareConfig.ledColor==CRGB::Purple) idx=1;
    else if (fzerofirmwareConfig.ledColor==CRGB::White) idx=2;
    else if (fzerofirmwareConfig.ledColor==CRGB::Red) idx=3;
    else if (fzerofirmwareConfig.ledColor==CRGB::Green) idx=4;
    else if (fzerofirmwareConfig.ledColor==CRGB::Blue) idx=5;
    else idx=6;  // custom color

    options = {
        {"OFF",    [=]() { fzerofirmwareConfig.setLedColor(CRGB::Black); }, fzerofirmwareConfig.ledColor == CRGB::Black },
        {"Purple", [=]() { fzerofirmwareConfig.setLedColor(CRGB::Purple); }, fzerofirmwareConfig.ledColor == CRGB::Purple},
        {"White",  [=]() { fzerofirmwareConfig.setLedColor(CRGB::White); }, fzerofirmwareConfig.ledColor == CRGB::White},
        {"Red",    [=]() { fzerofirmwareConfig.setLedColor(CRGB::Red); }, fzerofirmwareConfig.ledColor == CRGB::Red},
        {"Green",  [=]() { fzerofirmwareConfig.setLedColor(CRGB::Green); }, fzerofirmwareConfig.ledColor == CRGB::Green},
        {"Blue",   [=]() { fzerofirmwareConfig.setLedColor(CRGB::Blue); }, fzerofirmwareConfig.ledColor == CRGB::Blue},
    };

    if (idx == 6) options.emplace_back("Custom Color", [=]() { backToMenu(); }, true);
    options.emplace_back("Main Menu", [=]() { backToMenu(); });

    loopOptions(options, idx);
    setLedColor(fzerofirmwareConfig.ledColor);
}


void setLedBrightnessConfig() {
    int idx;
    if (fzerofirmwareConfig.ledBright==10) idx=0;
    else if (fzerofirmwareConfig.ledBright==25) idx=1;
    else if (fzerofirmwareConfig.ledBright==50) idx=2;
    else if (fzerofirmwareConfig.ledBright==75) idx=3;
    else if (fzerofirmwareConfig.ledBright==100) idx=4;

    options = {
        {"10 %", [=]() { fzerofirmwareConfig.setLedBright(10);  }, fzerofirmwareConfig.ledBright == 10 },
        {"25 %", [=]() { fzerofirmwareConfig.setLedBright(25);  }, fzerofirmwareConfig.ledBright == 25 },
        {"50 %", [=]() { fzerofirmwareConfig.setLedBright(50);  }, fzerofirmwareConfig.ledBright == 50 },
        {"75 %", [=]() { fzerofirmwareConfig.setLedBright(75);  }, fzerofirmwareConfig.ledBright == 75 },
        {"100%", [=]() { fzerofirmwareConfig.setLedBright(100); }, fzerofirmwareConfig.ledBright == 100 },
        {"Main Menu", [=]() { backToMenu(); }},
    };

    loopOptions(options, idx);
    setLedBrightness(fzerofirmwareConfig.ledBright);
}
#endif