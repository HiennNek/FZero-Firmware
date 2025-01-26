#include "ConfigMenu.h"
#include "core/utils.h"
#include "core/display.h"
#include "core/settings.h"
#include "core/i2c_finder.h"
#include "core/wifi_common.h"
#ifdef HAS_RGB_LED
#include "core/led_control.h"
#endif

void ConfigMenu::optionsMenu() {
    options = {
        {"Brightness",    [=]() { setBrightnessMenu(); }},
        {"Dim Time",      [=]() { setDimmerTimeMenu(); }},
        {"Orientation",   [=]() { gsetRotation(true); }},
        {"UI Color",      [=]() { setUIColor(); }},
    #ifdef HAS_RGB_LED
        {"LED Color",     [=]() { setLedColorConfig(); }},
        {"LED Brightness",[=]() { setLedBrightnessConfig(); }},
    #endif
        {"Sound On/Off",  [=]() { setSoundConfig(); }},
        {"Startup WiFi",  [=]() { setWifiStartupConfig(); }},
        {"Startup App",   [=]() { setStartupApp(); }},
        {"Clock",         [=]() { setClock(); }},
        {"Sleep",         [=]() { setSleepMode(); }},
        {"Restart",       [=]() { ESP.restart(); }},
    };

#if defined(T_EMBED_1101)
    options.emplace_back("Turn-off", [=]() { digitalWrite(PIN_POWER_ON,LOW); esp_sleep_enable_ext0_wakeup(GPIO_NUM_6,LOW); esp_deep_sleep_start(); });
#endif
    if (fzerofirmwareConfig.devMode) options.emplace_back("Dev Mode", [=]() { devMenu(); });

    options.emplace_back("Main Menu", [=]() { backToMenu(); });

    loopOptions(options,false,true,"Config");
}

void ConfigMenu::devMenu(){
    options = {
        {"Device Info",   [=]() { showDeviceInfo(); }},
        {"MAC Address",   [=]() { checkMAC(); }},
        {"I2C Finder",    [=]() { find_i2c_addresses(); }},
        {"Back",          [=]() { optionsMenu(); }},
    };

    loopOptions(options,false,true,"Dev Mode");
}

void ConfigMenu::drawIcon(float scale) {
    clearIconArea();

    int radius = scale * 9;

    int i=0;
    for(i=0; i<6; i++) {
        tft.drawArc(
            iconCenterX,
            iconCenterY,
            3.5*radius, 2*radius,
            15+60*i, 45+60*i,
            fzerofirmwareConfig.priColor,
            fzerofirmwareConfig.bgColor,
            true
        );
    }

    tft.drawArc(
        iconCenterX,
        iconCenterY,
        2.5*radius, radius,
        0, 360,
        fzerofirmwareConfig.priColor,
        fzerofirmwareConfig.bgColor,
        false
    );
}