#include "WifiMenu.h"
#include "core/display.h"
#include "core/wifi_common.h"
#include "core/wg.h"
#include "modules/wifi/clients.h"
#include "modules/wifi/dpwo.h"
#include "modules/wifi/evil_portal.h"
#include "modules/wifi/scan_hosts.h"
#include "modules/wifi/sniffer.h"
#include "modules/wifi/wifi_atks.h"
#include "modules/wifi/ap_info.h"
#include "core/utils.h"

#ifndef LITE_VERSION
#include "modules/pwnagotchi/pwnagotchi.h"
#endif

//#include "modules/reverseShell/reverseShell.h"
// Developed by Fourier (github.com/9dl)
// Use FZerofirmwareC2 to interact with the reverse shell server
// FZerofirmwareC2: https://github.com/9dl/FZerofirmware-C2
// To use FZerofirmwareC2:
// 1. Start Reverse Shell Mode in FZerofirmware
// 2. Start FZerofirmwareC2 and wait.
// 3. Visit 192.168.4.1 in your browser to access the web interface for shell executing.

// 32bit: https://github.com/9dl/FZerofirmware-C2/releases/download/v1.0/FZerofirmwareC2_windows_386.exe
// 64bit: https://github.com/9dl/FZerofirmware-C2/releases/download/v1.0/FZerofirmwareC2_windows_amd64.exe
#include "modules/wifi/tcp_utils.h"

void WifiMenu::optionsMenu() {
    if(!wifiConnected) {
        options = {
            {"Connect Wifi", [=]()  { wifiConnectMenu(WIFI_STA); }},
            {"WiFi AP",      [=]()  { wifiConnectMenu(WIFI_AP); displayInfo("pwd: " + fzerofirmwareConfig.wifiAp.pwd, true); }},
        };
    } else {
        options = {{"Disconnect",   [=]()  { wifiDisconnect(); }} };
        if(WiFi.getMode() == WIFI_MODE_STA) options.push_back({"AP info",   [=]()  { displayAPInfo(); }});
    }
    options.push_back({"Wifi Atks", [=]()     { wifi_atk_menu(); }});
    options.push_back({"Evil Portal", [=]()   { EvilPortal(); }});
    //options.push_back({"ReverseShell", [=]()       { ReverseShell(); }});
    options.push_back({"Listen TCP", [=]()       { listenTcpPort(); }});
    options.push_back({"Client TCP", [=]()       { clientTCP(); }});
#ifndef LITE_VERSION
    options.push_back({"TelNET", [=]()        { telnet_setup(); }});
    options.push_back({"SSH", [=]()           { ssh_setup(); }});
    options.push_back({"DPWO", [=]()          { dpwo_setup(); }});
    options.push_back({"Raw Sniffer", [=]()   { sniffer_setup(); }});
    options.push_back({"Scan Hosts", [=]()    { local_scan_setup(); }});
    options.push_back({"Wireguard", [=]()     { wg_setup(); }});
    options.push_back({"FZerofirmwaregotchi",  [=]()   { fzerofirmwaregotchi_start(); }});
#endif
    options.push_back({"Main Menu", [=]()     { backToMenu(); }});

    loopOptions(options,false,true,"WiFi");
}

void WifiMenu::drawIcon(float scale) {
    clearIconArea();

    int deltaY = scale * 20;
    int radius = scale * 6;

    tft.fillCircle(iconCenterX, iconCenterY + deltaY, radius, fzerofirmwareConfig.priColor);
    tft.drawArc(
        iconCenterX, iconCenterY + deltaY,
        deltaY + radius, deltaY,
        130, 230,
        fzerofirmwareConfig.priColor, fzerofirmwareConfig.bgColor
    );
    tft.drawArc(
        iconCenterX, iconCenterY + deltaY,
        2*deltaY + radius, 2*deltaY,
        130, 230,
        fzerofirmwareConfig.priColor, fzerofirmwareConfig.bgColor
    );
}
