#include <globals.h>
#include "settings.h"
#include "display.h"  // calling loopOptions(options, true);
#include "wifi_common.h"
#include "utils.h"
#include "mykeyboard.h"
#include "sd_functions.h"
#include "powerSave.h"
#include "modules/rf/rf.h"  // for initRfModule

#ifdef USE_CC1101_VIA_SPI
#include <ELECHOUSE_CC1101_SRC_DRV.h>
#endif

// This function comes from interface.h
void _setBrightness(uint8_t brightval) { }

/*********************************************************************
**  Function: setBrightness
**  set brightness value
**********************************************************************/
void setBrightness(uint8_t brightval, bool save) {
  if(fzerofirmwareConfig.bright>100) fzerofirmwareConfig.setBright(100);
  _setBrightness(brightval);
  delay(10);

  if(save){
    fzerofirmwareConfig.setBright(brightval);
  }
}

/*********************************************************************
**  Function: getBrightness
**  get brightness value
**********************************************************************/
void getBrightness() {
  if(fzerofirmwareConfig.bright>100) {
    fzerofirmwareConfig.setBright(100);
    _setBrightness(fzerofirmwareConfig.bright);
    delay(10);
    setBrightness(100);
  }

  _setBrightness(fzerofirmwareConfig.bright);
  delay(10);
}

/*********************************************************************
**  Function: gsetRotation
**  get/set rotation value
**********************************************************************/
int gsetRotation(bool set){
  int getRot = fzerofirmwareConfig.rotation;
  int result = ROTATION;

  #if TFT_WIDTH>=240 && TFT_HEIGHT>=240
  getRot++;
  if(getRot>3 && set) result = 0;
  else if(set) result = getRot;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }
  #else
  if(getRot==1 && set) result = 3;
  else if(getRot==3 && set) result = 1;
  else if(getRot<=3) result = getRot;
  else {
    set=true;
    result = ROTATION;
  }
  #endif

  if(set) {
    fzerofirmwareConfig.setRotation(result);
    tft.setRotation(result);
    tft.setRotation(result); // must repeat, sometimes ESP32S3 miss one SPI command and it just jumps this step and don't rotate
  }
  returnToMenu=true;

  if(result & 0b01) { // if 1 or 3
      tftWidth=TFT_HEIGHT;
      #if defined(HAS_TOUCH)
        tftHeight=TFT_WIDTH - 20;
      #else
        tftHeight=TFT_WIDTH;
      #endif
  } else { // if 2 or 0
      tftWidth=TFT_WIDTH;
      #if defined(HAS_TOUCH)
      tftHeight=TFT_HEIGHT-20;
      #else
      tftHeight=TFT_HEIGHT;
      #endif
  }
  return result;
}

/*********************************************************************
**  Function: setBrightnessMenu
**  Handles Menu to set brightness
**********************************************************************/
void setBrightnessMenu() {
  int idx=0;
  if(fzerofirmwareConfig.bright==100) idx=0;
  else if(fzerofirmwareConfig.bright==75) idx=1;
  else if(fzerofirmwareConfig.bright==50) idx=2;
  else if(fzerofirmwareConfig.bright==25) idx=3;
  else if(fzerofirmwareConfig.bright== 1) idx=4;

  options = {
    {"100%", [=]() { setBrightness((uint8_t)100); }, fzerofirmwareConfig.bright == 100 },
    {"75 %", [=]() { setBrightness((uint8_t)75);  }, fzerofirmwareConfig.bright == 75 },
    {"50 %", [=]() { setBrightness((uint8_t)50);  }, fzerofirmwareConfig.bright == 50 },
    {"25 %", [=]() { setBrightness((uint8_t)25);  }, fzerofirmwareConfig.bright == 25 },
    {" 1 %", [=]() { setBrightness((uint8_t)1);   }, fzerofirmwareConfig.bright == 1 },
    {"Main Menu", [=]() { backToMenu(); }}, // this one bugs the brightness selection
  };
  loopOptions(options, true,false,"",idx);
}


/*********************************************************************
**  Function: setSleepMode
**  Turn screen off and reduces cpu clock
**********************************************************************/
void setSleepMode() {
  sleepModeOn();
  while (1) {
    if (check(AnyKeyPress)) {
      sleepModeOff();
      returnToMenu = true;
      break;
    }
  }
}

/*********************************************************************
**  Function: setDimmerTimeMenu
**  Handles Menu to set dimmer time
**********************************************************************/
void setDimmerTimeMenu() {
  int idx=0;
  if(fzerofirmwareConfig.dimmerSet==10) idx=0;
  else if(fzerofirmwareConfig.dimmerSet==20) idx=1;
  else if(fzerofirmwareConfig.dimmerSet==30) idx=2;
  else if(fzerofirmwareConfig.dimmerSet==60) idx=3;
  else if(fzerofirmwareConfig.dimmerSet== 0) idx=4;
  options = {
    {"10s", [=]() { fzerofirmwareConfig.setDimmer(10); }, fzerofirmwareConfig.dimmerSet == 10 },
    {"20s", [=]() { fzerofirmwareConfig.setDimmer(20); }, fzerofirmwareConfig.dimmerSet == 20 },
    {"30s", [=]() { fzerofirmwareConfig.setDimmer(30); }, fzerofirmwareConfig.dimmerSet == 30 },
    {"60s", [=]() { fzerofirmwareConfig.setDimmer(60); }, fzerofirmwareConfig.dimmerSet == 60 },
    {"Disabled", [=]() { fzerofirmwareConfig.setDimmer(0); }, fzerofirmwareConfig.dimmerSet == 0 },
  };
  loopOptions(options,idx);
}

/*********************************************************************
**  Function: setUIColor
**  Set and store main UI color
**********************************************************************/
#define LIGHT_BLUE 0x96FE
void setUIColor(){
  int idx=0;
  if(fzerofirmwareConfig.priColor==DEFAULT_PRICOLOR) idx=0;
  else if(fzerofirmwareConfig.priColor==TFT_WHITE) idx=1;
  else if(fzerofirmwareConfig.priColor==TFT_RED) idx=2;
  else if(fzerofirmwareConfig.priColor==TFT_DARKGREEN) idx=3;
  else if(fzerofirmwareConfig.priColor==TFT_BLUE) idx=4;
  else if(fzerofirmwareConfig.priColor==LIGHT_BLUE) idx=5;
  else if(fzerofirmwareConfig.priColor==TFT_YELLOW) idx=6;
  else if(fzerofirmwareConfig.priColor==TFT_MAGENTA) idx=7;
  else if(fzerofirmwareConfig.priColor==TFT_ORANGE) idx=8;
  else idx=9;  // custom theme

  options = {
    {"Default",   [=]() { fzerofirmwareConfig.setTheme(DEFAULT_PRICOLOR);}, fzerofirmwareConfig.priColor==DEFAULT_PRICOLOR},
    {"White",     [=]() { fzerofirmwareConfig.setTheme(TFT_WHITE);     }, fzerofirmwareConfig.priColor==TFT_WHITE     },
    {"Red",       [=]() { fzerofirmwareConfig.setTheme(TFT_RED);       }, fzerofirmwareConfig.priColor==TFT_RED       },
    {"Green",     [=]() { fzerofirmwareConfig.setTheme(TFT_DARKGREEN); }, fzerofirmwareConfig.priColor==TFT_DARKGREEN },
    {"Blue",      [=]() { fzerofirmwareConfig.setTheme(TFT_BLUE);      }, fzerofirmwareConfig.priColor==TFT_BLUE      },
    {"Light Blue",[=]() { fzerofirmwareConfig.setTheme(LIGHT_BLUE);    }, fzerofirmwareConfig.priColor==LIGHT_BLUE    },
    {"Yellow",    [=]() { fzerofirmwareConfig.setTheme(TFT_YELLOW);    }, fzerofirmwareConfig.priColor==TFT_YELLOW    },
    {"Magenta",   [=]() { fzerofirmwareConfig.setTheme(TFT_MAGENTA);   }, fzerofirmwareConfig.priColor==TFT_MAGENTA   },
    {"Orange",    [=]() { fzerofirmwareConfig.setTheme(TFT_ORANGE);    }, fzerofirmwareConfig.priColor==TFT_ORANGE    },
  };

  if (idx == 9) options.push_back({"Custom Theme", [=]() { backToMenu(); }, true});
  options.push_back({"Main Menu", [=]() { backToMenu(); }});

  loopOptions(options, idx);
  tft.setTextColor(fzerofirmwareConfig.bgColor, fzerofirmwareConfig.priColor);
}

/*********************************************************************
**  Function: setSoundConfig
**  Enable or disable sound
**********************************************************************/
void setSoundConfig() {
  options = {
    {"Sound off", [=]() { fzerofirmwareConfig.setSoundEnabled(0); }, fzerofirmwareConfig.soundEnabled == 0},
    {"Sound on",  [=]() { fzerofirmwareConfig.setSoundEnabled(1); }, fzerofirmwareConfig.soundEnabled == 1},
  };
  loopOptions(options, fzerofirmwareConfig.soundEnabled);
}

/*********************************************************************
**  Function: setWifiStartupConfig
**  Enable or disable wifi connection at startup
**********************************************************************/
void setWifiStartupConfig() {
  options = {
    {"Disable", [=]() { fzerofirmwareConfig.setWifiAtStartup(0); }, fzerofirmwareConfig.wifiAtStartup == 0},
    {"Enable",  [=]() { fzerofirmwareConfig.setWifiAtStartup(1); }, fzerofirmwareConfig.wifiAtStartup == 1},
  };
  loopOptions(options, fzerofirmwareConfig.wifiAtStartup);
}

/*********************************************************************
**  Function: setRFModuleMenu
**  Handles Menu to set the RF module in use
**********************************************************************/
void setRFModuleMenu() {
  int result = 0;
  int idx=0;
  if(fzerofirmwareConfig.rfModule==M5_RF_MODULE) idx=0;
  else if(fzerofirmwareConfig.rfModule==CC1101_SPI_MODULE) idx=1;

  options = {
    {"M5 RF433T/R",    [&]() { result = M5_RF_MODULE; }},
#ifdef USE_CC1101_VIA_SPI
    {"CC1101 on SPI",  [&]() { result = CC1101_SPI_MODULE; }},
#endif
/* WIP:
 * #ifdef USE_CC1101_VIA_PCA9554
 * {"CC1101+PCA9554",  [&]() { result = 2; }},
 * #endif
*/
  };
  loopOptions(options, idx);  // 2fix: idx highlight not working?
  if(result == CC1101_SPI_MODULE) {
    #ifdef USE_CC1101_VIA_SPI
    ELECHOUSE_cc1101.Init();
    if (ELECHOUSE_cc1101.getCC1101()){
      fzerofirmwareConfig.setRfModule(CC1101_SPI_MODULE);
      return;
    }
    #endif
    // else display an error
    displayError("CC1101 not found");
    while(!check(AnyKeyPress));
  }
  // fallback to "M5 RF433T/R" on errors
  fzerofirmwareConfig.setRfModule(M5_RF_MODULE);
}

/*********************************************************************
**  Function: setRFFreqMenu
**  Handles Menu to set the default frequency for the RF module
**********************************************************************/
void setRFFreqMenu() {
  float result = 433.92;
  String freq_str = keyboard(String(fzerofirmwareConfig.rfFreq), 10, "Default frequency:");
  if(freq_str.length() > 1) {
    result = freq_str.toFloat();  // returns 0 if not valid
    if(result>=300 && result<=928) { // TODO: check valid freq according to current module?
      fzerofirmwareConfig.setRfFreq(result);
      return;
    }
  }
  // else
  displayError("Invalid frequency");
  fzerofirmwareConfig.setRfFreq(433.92);  // reset to default
  delay(1000);
}

/*********************************************************************
**  Function: setRFIDModuleMenu
**  Handles Menu to set the RFID module in use
**********************************************************************/
void setRFIDModuleMenu() {
  options = {
    {"M5 RFID2",      [=]() { fzerofirmwareConfig.setRfidModule(M5_RFID2_MODULE); },  fzerofirmwareConfig.rfidModule == M5_RFID2_MODULE},
    {"PN532 on I2C",  [=]() { fzerofirmwareConfig.setRfidModule(PN532_I2C_MODULE); }, fzerofirmwareConfig.rfidModule == PN532_I2C_MODULE},
    {"PN532 on SPI",  [=]() { fzerofirmwareConfig.setRfidModule(PN532_SPI_MODULE); }, fzerofirmwareConfig.rfidModule == PN532_SPI_MODULE},
  };
  loopOptions(options, fzerofirmwareConfig.rfidModule);
}

/*********************************************************************
**  Function: addMifareKeyMenu
**  Handles Menu to add MIFARE keys into config list
**********************************************************************/
void addMifareKeyMenu() {
  String key = keyboard("", 12, "MIFARE key");
  fzerofirmwareConfig.addMifareKey(key);
}


/*********************************************************************
**  Function: setClock
**  Handles Menu to set timezone to NTP
**********************************************************************/
const char* ntpServer = "pool.ntp.org";
long  selectedTimezone;
const int   daylightOffset_sec = 0;
int timeHour;

TimeChangeRule BRST = {"BRST", Last, Sun, Oct, 0, timeHour};
Timezone myTZ(BRST, BRST);

WiFiUDP ntpUDP;
NTPClient timeClient(ntpUDP, ntpServer, selectedTimezone, daylightOffset_sec);

void setClock() {
  bool auto_mode=true;

  #if defined(HAS_RTC)
    RTC_TimeTypeDef TimeStruct;
    cplus_RTC _rtc;
    _rtc.GetBm8563Time();
  #endif

  options = {
    {"NTP Timezone", [&]() { auto_mode=true; }},
    {"Manually set", [&]() { auto_mode=false; }},
    {"Main Menu",    [=]() { backToMenu(); }},
  };
  loopOptions(options);

  if (returnToMenu) return;

  if (auto_mode) {
    if(!wifiConnected) wifiConnectMenu();

    options = {
      {"Los Angeles", [&]() { fzerofirmwareConfig.setTmz(-8); }, fzerofirmwareConfig.tmz==-8 },
      {"Chicago",     [&]() { fzerofirmwareConfig.setTmz(-6); }, fzerofirmwareConfig.tmz==-6 },
      {"New York",    [&]() { fzerofirmwareConfig.setTmz(-5); }, fzerofirmwareConfig.tmz==-5 },
      {"Brasilia",    [&]() { fzerofirmwareConfig.setTmz(-3); }, fzerofirmwareConfig.tmz==-3 },
      {"Pernambuco",  [&]() { fzerofirmwareConfig.setTmz(-2); }, fzerofirmwareConfig.tmz==-2 },
      {"Lisbon",      [&]() { fzerofirmwareConfig.setTmz(0);  }, fzerofirmwareConfig.tmz==0  },
      {"Paris",       [&]() { fzerofirmwareConfig.setTmz(1);  }, fzerofirmwareConfig.tmz==1  },
      {"Athens",      [&]() { fzerofirmwareConfig.setTmz(2);  }, fzerofirmwareConfig.tmz==2  },
      {"Moscow",      [&]() { fzerofirmwareConfig.setTmz(3);  }, fzerofirmwareConfig.tmz==3  },
      {"Dubai",       [&]() { fzerofirmwareConfig.setTmz(4);  }, fzerofirmwareConfig.tmz==4  },
      {"Ho Chi Minh", [&]() { fzerofirmwareConfig.setTmz(7);  }, fzerofirmwareConfig.tmz==7  },
      {"Hong Kong",   [&]() { fzerofirmwareConfig.setTmz(8);  }, fzerofirmwareConfig.tmz==8  },
      {"Tokyo",       [&]() { fzerofirmwareConfig.setTmz(9);  }, fzerofirmwareConfig.tmz==9  },
      {"Sydney",      [&]() { fzerofirmwareConfig.setTmz(10); }, fzerofirmwareConfig.tmz==10 },
      {"Main Menu",   [=]() { backToMenu(); }},
    };


    loopOptions(options);


    if (returnToMenu) return;

    timeClient.setTimeOffset(fzerofirmwareConfig.tmz * 3600);
    timeClient.begin();
    timeClient.update();
    localTime = myTZ.toLocal(timeClient.getEpochTime());

    #if defined(HAS_RTC)
      struct tm *timeinfo = localtime(&localTime);
      TimeStruct.Hours   = timeinfo->tm_hour;
      TimeStruct.Minutes = timeinfo->tm_min;
      TimeStruct.Seconds = timeinfo->tm_sec;
      _rtc.SetTime(&TimeStruct);
    #else
      rtc.setTime(timeClient.getEpochTime());
    #endif

    clock_set = true;
    runClockLoop();
  }
  else {
    int hr, mn, am;
    options = { };
    for(int i=0; i<12;i++) options.push_back({String(String(i<10?"0":"") + String(i)).c_str(), [&]() { delay(1); }});


    hr=loopOptions(options,false,true,"Set Hour");

    options = { };
    for(int i=0; i<60;i++) options.push_back({String(String(i<10?"0":"") + String(i)).c_str(), [&]() { delay(1); }});

    mn=loopOptions(options,false,true,"Set Minute");

    options = {
      {"AM", [&]() { am=0; }},
      {"PM", [&]() { am=12; }},
    };

    loopOptions(options);


    #if defined(HAS_RTC)
      TimeStruct.Hours   = hr+am;
      TimeStruct.Minutes = mn;
      TimeStruct.Seconds = 0;
      _rtc.SetTime(&TimeStruct);
    #else
      rtc.setTime(0,mn,hr+am,20,06,2024); // send me a gift, @Pirata!
    #endif
    clock_set = true;
    runClockLoop();
  }
}

void runClockLoop() {
  int tmp=0;

  #if defined(HAS_RTC)
    RTC_TimeTypeDef _time;
    cplus_RTC _rtc;
    _rtc.GetBm8563Time();
    _rtc.GetTime(&_time);
  #endif

  // Delay due to SelPress() detected on run
  tft.fillScreen(fzerofirmwareConfig.bgColor);
  tft.fillScreen(fzerofirmwareConfig.bgColor);
  delay(300);

  for (;;){
  if(millis()-tmp>1000) {
    #if !defined(HAS_RTC)
      updateTimeStr(rtc.getTimeStruct());
    #endif
    Serial.print("Current time: ");
    Serial.println(timeStr);
    tft.setTextColor(fzerofirmwareConfig.priColor, fzerofirmwareConfig.bgColor);
    tft.drawRect(10, 10, tftWidth-15,tftHeight-15, fzerofirmwareConfig.priColor);
    tft.setCursor(64, tftHeight/3+5);
    tft.setTextSize(4);
    #if defined(HAS_RTC)
      _rtc.GetBm8563Time();
      _rtc.GetTime(&_time);
      char timeString[9];  // Buffer para armazenar a string formatada "HH:MM:SS"
      snprintf(timeString, sizeof(timeString), "%02d:%02d:%02d", _time.Hours, _time.Minutes, _time.Seconds);
      tft.drawCentreString(timeString,tftWidth/2,tftHeight/2-13,1);
    #else
      tft.drawCentreString(timeStr,tftWidth/2,tftHeight/2-13,1);
    #endif
    tmp=millis();
  }

    // Checks para sair do loop
    if(check(SelPress) or check(EscPress)) { // Apertar o botão power dos sticks
      tft.fillScreen(fzerofirmwareConfig.bgColor);
      returnToMenu=true;
      break;
      //goto Exit;
    }
    delay(10);
  }
}

/*********************************************************************
**  Function: gsetIrTxPin
**  get or set IR Tx Pin
**********************************************************************/
int gsetIrTxPin(bool set){
  int result = fzerofirmwareConfig.irTx;

  if(result>50) fzerofirmwareConfig.setIrTxPin(LED);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = IR_TX_PINS;
    int idx=100;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==fzerofirmwareConfig.irTx && idx==100) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() { fzerofirmwareConfig.setIrTxPin(pin.second); }, pin.second==fzerofirmwareConfig.irTx });
    }

    loopOptions(options, idx);

    Serial.println("Saved pin: " + String(fzerofirmwareConfig.irTx));
  }

  returnToMenu=true;
  return fzerofirmwareConfig.irTx;
}

/*********************************************************************
**  Function: gsetIrRxPin
**  get or set IR Rx Pin
**********************************************************************/
int gsetIrRxPin(bool set){
  int result = fzerofirmwareConfig.irRx;

  if(result>45) fzerofirmwareConfig.setIrRxPin(GROVE_SCL);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = IR_RX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==fzerofirmwareConfig.irRx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() {fzerofirmwareConfig.setIrRxPin(pin.second);}, pin.second==fzerofirmwareConfig.irRx });
    }

    loopOptions(options);

  }

  returnToMenu=true;
  return fzerofirmwareConfig.irRx;
}

/*********************************************************************
**  Function: gsetRfTxPin
**  get or set RF Tx Pin
**********************************************************************/
int gsetRfTxPin(bool set){
  int result = fzerofirmwareConfig.rfTx;

  if(result>45) fzerofirmwareConfig.setRfTxPin(GROVE_SDA);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = RF_TX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==fzerofirmwareConfig.rfTx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() {fzerofirmwareConfig.setRfTxPin(pin.second);}, pin.second==fzerofirmwareConfig.rfTx });
    }

    loopOptions(options);

  }

  returnToMenu=true;
  return fzerofirmwareConfig.rfTx;
}

/*********************************************************************
**  Function: gsetRfRxPin
**  get or set FR Rx Pin
**********************************************************************/
int gsetRfRxPin(bool set){
  int result = fzerofirmwareConfig.rfRx;

  if(result>36) fzerofirmwareConfig.setRfRxPin(GROVE_SCL);
  if(set) {
    options.clear();
    std::vector<std::pair<std::string, int>> pins;
    pins = RF_RX_PINS;
    int idx=-1;
    int j=0;
    for (auto pin : pins) {
      if(pin.second==fzerofirmwareConfig.rfRx && idx<0) idx=j;
      j++;
      #ifdef ALLOW_ALL_GPIO_FOR_IR_RF
      int i=pin.second;
      if(i!=TFT_CS && i!=TFT_RST && i!=TFT_SCLK && i!=TFT_MOSI && i!=TFT_BL && i!=TOUCH_CS && i!=SDCARD_CS && i!=SDCARD_MOSI && i!=SDCARD_MISO)
      #endif
        options.push_back({pin.first, [=]() {fzerofirmwareConfig.setRfRxPin(pin.second);}, pin.second==fzerofirmwareConfig.rfRx });
    }

    loopOptions(options);

  }

  returnToMenu=true;
  return fzerofirmwareConfig.rfRx;
}

/*********************************************************************
**  Function: setStartupApp
**  Handles Menu to set startup app
**********************************************************************/
void setStartupApp() {
  int idx = 0;
  if (fzerofirmwareConfig.startupApp == "") idx=0;

  options = {
    {"None", [=]() { fzerofirmwareConfig.setStartupApp(""); }, fzerofirmwareConfig.startupApp == "" }
  };

  int index = 1;
  for (String appName : startupApp.getAppNames()) {
    if (fzerofirmwareConfig.startupApp == appName) idx=index++;

    options.emplace_back(
      appName.c_str(),
      [=]() { fzerofirmwareConfig.setStartupApp(appName); },
      fzerofirmwareConfig.startupApp == appName
    );
  }


  loopOptions(options, idx);

}

/*********************************************************************
**  Function: setGpsBaudrateMenu
**  Handles Menu to set the baudrate for the GPS module
**********************************************************************/
void setGpsBaudrateMenu() {
  options = {
    {"9600 bps",   [=]() { fzerofirmwareConfig.setGpsBaudrate(9600); }, fzerofirmwareConfig.gpsBaudrate == 9600},
    {"19200 bps",  [=]() { fzerofirmwareConfig.setGpsBaudrate(19200); }, fzerofirmwareConfig.gpsBaudrate == 19200},
    {"57600 bps",  [=]() { fzerofirmwareConfig.setGpsBaudrate(57600); }, fzerofirmwareConfig.gpsBaudrate == 57600},
    {"115200 bps", [=]() { fzerofirmwareConfig.setGpsBaudrate(115200); }, fzerofirmwareConfig.gpsBaudrate == 115200},
  };

  loopOptions(options, fzerofirmwareConfig.gpsBaudrate);

}
