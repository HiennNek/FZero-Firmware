#include "config.h"
#include "sd_functions.h"


JsonDocument FZerofirmwareConfig::toJson() const {
    JsonDocument jsonDoc;
    JsonObject setting = jsonDoc.to<JsonObject>();

    setting["priColor"] = String(priColor, HEX);
    setting["secColor"] = String(secColor, HEX);
    setting["bgColor"] = String(bgColor, HEX);

    setting["rot"] = rotation;
    setting["dimmerSet"] = dimmerSet;
    setting["bright"] = bright;
    setting["tmz"] = tmz;
    setting["soundEnabled"] = soundEnabled;
    setting["wifiAtStartup"] = wifiAtStartup;

    setting["ledBright"] = ledBright;
    setting["ledColor"] = String(ledColor, HEX);

    JsonObject _webUI = setting.createNestedObject("webUI");
    _webUI["user"] = webUI.user;
    _webUI["pwd"] = webUI.pwd;

    JsonObject _wifiAp = setting.createNestedObject("wifiAp");
    _wifiAp["ssid"] = wifiAp.ssid;
    _wifiAp["pwd"] = wifiAp.pwd;

    JsonObject _wifi = setting.createNestedObject("wifi");
    for (const auto& pair : wifi) {
        _wifi[pair.first] = pair.second;
    }

    setting["irTx"] = irTx;
    setting["irRx"] = irRx;

    setting["rfTx"] = rfTx;
    setting["rfRx"] = rfRx;
    setting["rfModule"] = rfModule;
    setting["rfFreq"] = rfFreq;
    setting["rfFxdFreq"] = rfFxdFreq;
    setting["rfScanRange"] = rfScanRange;

    setting["rfidModule"] = rfidModule;

    JsonArray _mifareKeys = setting.createNestedArray("mifareKeys");
    for (auto key : mifareKeys) _mifareKeys.add(key);

    setting["gpsBaudrate"] = gpsBaudrate;

    setting["startupApp"] = startupApp;
    setting["wigleBasicToken"] = wigleBasicToken;
    setting["devMode"] = devMode;

    JsonArray dm = setting.createNestedArray("disabledMenus");
    for(int i=0; i < disabledMenus.size(); i++){
        dm.add(disabledMenus[i]);
    }

    JsonArray qrArray = setting.createNestedArray("qrCodes");
    for (const auto& entry : qrCodes) {
        JsonObject qrEntry = qrArray.createNestedObject();
        qrEntry["menuName"] = entry.menuName;
        qrEntry["content"] = entry.content;
    }

    return jsonDoc;
}


void FZerofirmwareConfig::fromFile() {
    FS *fs;
    if(!getFsStorage(fs)) return;

    if(!fs->exists(filepath)) return saveFile();

    File file;
    file = fs->open(filepath, FILE_READ);
    if (!file) {
        log_e("Config file not found. Using default values");
        return;
    }

    // Deserialize the JSON document
    JsonDocument jsonDoc;
    if (deserializeJson(jsonDoc, file)) {
        log_e("Failed to read config file, using default configuration");
        return;
    }
    file.close();

    JsonObject setting = jsonDoc.as<JsonObject>();
    int count = 0;

    if(!setting["priColor"].isNull())  { priColor  = strtoul(setting["priColor"], nullptr, 16); } else { count++; log_e("Fail"); }
    if(!setting["secColor"].isNull())  { secColor  = strtoul(setting["secColor"], nullptr, 16); } else { count++; log_e("Fail"); }
    if(!setting["bgColor"].isNull())   { bgColor   = strtoul(setting["bgColor"], nullptr, 16); } else { count++; log_e("Fail"); }

    if(!setting["rot"].isNull())       { rotation  = setting["rot"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["dimmerSet"].isNull()) { dimmerSet = setting["dimmerSet"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["bright"].isNull())    { bright    = setting["bright"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["tmz"].isNull())       { tmz       = setting["tmz"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["soundEnabled"].isNull())    { soundEnabled  = setting["soundEnabled"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["wifiAtStartup"].isNull())   { wifiAtStartup = setting["wifiAtStartup"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["ledBright"].isNull()) { ledBright = setting["ledBright"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["ledColor"].isNull())  { ledColor  = strtoul(setting["ledColor"], nullptr, 16); } else { count++; log_e("Fail"); }

    if(!setting["webUI"].isNull()) {
        JsonObject webUIObj = setting["webUI"].as<JsonObject>();
        webUI.user = webUIObj["user"].as<String>();
        webUI.pwd  = webUIObj["pwd"].as<String>();
    } else { count++; log_e("Fail"); }

    if(!setting["wifiAp"].isNull()) {
        JsonObject wifiApObj = setting["wifiAp"].as<JsonObject>();
        wifiAp.ssid = wifiApObj["ssid"].as<String>();
        wifiAp.pwd  = wifiApObj["pwd"].as<String>();
    } else { count++; log_e("Fail"); }

    if(!setting["wifi"].isNull()) {
        wifi.clear();
        for (JsonPair kv : setting["wifi"].as<JsonObject>())
            wifi[kv.key().c_str()] = kv.value().as<String>();
    } else { count++; log_e("Fail"); }

    if(!setting["irTx"].isNull())        { irTx        = setting["irTx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["irRx"].isNull())        { irRx        = setting["irRx"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["rfTx"].isNull())        { rfTx        = setting["rfTx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfRx"].isNull())        { rfRx        = setting["rfRx"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfModule"].isNull())    { rfModule    = setting["rfModule"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfFreq"].isNull())      { rfFreq      = setting["rfFreq"].as<float>(); } else { count++; log_e("Fail"); }
    if(!setting["rfFxdFreq"].isNull())   { rfFxdFreq   = setting["rfFxdFreq"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["rfScanRange"].isNull()) { rfScanRange = setting["rfScanRange"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["rfidModule"].isNull())  { rfidModule  = setting["rfidModule"].as<int>(); } else { count++; log_e("Fail"); }
    if(!setting["mifareKeys"].isNull()) {
        mifareKeys.clear();
        JsonArray _mifareKeys = setting["mifareKeys"].as<JsonArray>();
        for (JsonVariant key : _mifareKeys) mifareKeys.insert(key.as<String>());
    } else { count++; log_e("Fail"); }

    if(!setting["gpsBaudrate"].isNull()) { gpsBaudrate  = setting["gpsBaudrate"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["startupApp"].isNull())      { startupApp  = setting["startupApp"].as<String>(); } else { count++; log_e("Fail"); }
    if(!setting["wigleBasicToken"].isNull()) { wigleBasicToken  = setting["wigleBasicToken"].as<String>(); } else { count++; log_e("Fail"); }
    if(!setting["devMode"].isNull())         { devMode  = setting["devMode"].as<int>(); } else { count++; log_e("Fail"); }

    if(!setting["disabledMenus"].isNull()) {
        disabledMenus.clear();
        JsonArray dm = setting["disabledMenus"].as<JsonArray>();
        for (JsonVariant e : dm) {
            disabledMenus.push_back(e.as<String>());
        }
    } else { count++; log_e("Fail"); }

    if (!setting["qrCodes"].isNull()) {
        qrCodes.clear();
        JsonArray qrArray = setting["qrCodes"].as<JsonArray>();
        for (JsonObject qrEntry : qrArray) {
            String menuName = qrEntry["menuName"].as<String>();
            String content = qrEntry["content"].as<String>();
            qrCodes.push_back({menuName, content});
            }
    } else {
        count++;
        log_e("Fail to load qrCodes");
    }

    validateConfig();
    if (count>0) saveFile();

    log_i("Using config from file");
}


void FZerofirmwareConfig::saveFile() {
    FS *fs = &LittleFS;
    JsonDocument jsonDoc = toJson();

    // Open file for writing
    File file = fs->open(filepath, FILE_WRITE);
    if (!file) {
        log_e("Failed to open config file");
        file.close();
        return;
    };

    // Serialize JSON to file
    serializeJsonPretty(jsonDoc, Serial);
    if (serializeJsonPretty(jsonDoc, file) < 5) log_e("Failed to write config file");
    else log_i("config file written successfully");

    file.close();

    if (setupSdCard()) copyToFs(LittleFS, SD, filepath, false);
}


void FZerofirmwareConfig::validateConfig() {
    validateTheme();
    validateRotationValue();
    validateDimmerValue();
    validateBrightValue();
    validateTmzValue();
    validateSoundEnabledValue();
    validateWifiAtStartupValue();
    validateLedBrightValue();
    validateLedColorValue();
    validateRfScanRangeValue();
    validateRfModuleValue();
    validateRfidModuleValue();
    validateMifareKeysItems();
    validateGpsBaudrateValue();
    validateDevModeValue();
}


void FZerofirmwareConfig::setTheme(uint16_t primary, uint16_t secondary, uint16_t background) {
    priColor = primary;
    secColor = secondary == NULL ? primary - 0x2000 : secondary;
    bgColor = background == NULL ? 0x0 : background;
    validateTheme();
    saveFile();
}


void FZerofirmwareConfig::validateTheme() {
    if (priColor < 0 || priColor > 0xFFFF) priColor = DEFAULT_PRICOLOR;
    if (secColor < 0 || secColor > 0xFFFF) secColor = priColor - 0x2000;
    if (bgColor  < 0 || bgColor  > 0xFFFF) bgColor  = 0;
}


void FZerofirmwareConfig::setRotation(int value) {
    rotation = value;
    validateRotationValue();
    saveFile();
}


void FZerofirmwareConfig::validateRotationValue() {
    if (rotation < 0 || rotation > 3) rotation = 1;
}


void FZerofirmwareConfig::setDimmer(int value) {
    dimmerSet = value;
    validateDimmerValue();
    saveFile();
}


void FZerofirmwareConfig::validateDimmerValue() {
    if (dimmerSet < 0) dimmerSet = 10;
    if (dimmerSet > 60) dimmerSet = 0;
}


void FZerofirmwareConfig::setBright(uint8_t value) {
    bright = value;
    validateBrightValue();
    saveFile();
}


void FZerofirmwareConfig::validateBrightValue() {
    if (bright > 100) bright = 100;
}


void FZerofirmwareConfig::setTmz(int value) {
    tmz = value;
    validateTmzValue();
    saveFile();
}


void FZerofirmwareConfig::validateTmzValue() {
    if (tmz < -12 || tmz > 12) tmz = 0;
}


void FZerofirmwareConfig::setSoundEnabled(int value) {
    soundEnabled = value;
    validateSoundEnabledValue();
    saveFile();
}


void FZerofirmwareConfig::validateSoundEnabledValue() {
    if (soundEnabled > 1) soundEnabled = 1;
}


void FZerofirmwareConfig::setWifiAtStartup(int value) {
    wifiAtStartup = value;
    validateWifiAtStartupValue();
    saveFile();
}


void FZerofirmwareConfig::validateWifiAtStartupValue() {
    if (wifiAtStartup > 1) wifiAtStartup = 1;
}


void FZerofirmwareConfig::setLedBright(int value) {
    ledBright = value;
    validateLedBrightValue();
    saveFile();
}


void FZerofirmwareConfig::validateLedBrightValue() {
    ledBright = max(0, min(100, ledBright));
}


void FZerofirmwareConfig::setLedColor(uint32_t value) {
    ledColor = value;
    validateLedColorValue();
    saveFile();
}


void FZerofirmwareConfig::validateLedColorValue() {
    ledColor = max((uint32_t)0, min(0xFFFFFFFF, ledColor));
}


void FZerofirmwareConfig::setWebUICreds(const String& usr, const String& pwd) {
    webUI.user = usr;
    webUI.pwd = pwd;
    saveFile();
}


void FZerofirmwareConfig::setWifiApCreds(const String& ssid, const String& pwd) {
    wifiAp.ssid = ssid;
    wifiAp.pwd = pwd;
    saveFile();
}


void FZerofirmwareConfig::addWifiCredential(const String& ssid, const String& pwd) {
    wifi[ssid] = pwd;
    saveFile();
}


String FZerofirmwareConfig::getWifiPassword(const String& ssid) const {
    auto it = wifi.find(ssid);
    if (it != wifi.end()) return it->second;
    return "";
}


void FZerofirmwareConfig::setIrTxPin(int value) {
    irTx = value;
    saveFile();
}


void FZerofirmwareConfig::setIrRxPin(int value) {
    irRx = value;
    saveFile();
}


void FZerofirmwareConfig::setRfTxPin(int value) {
    rfTx = value;
    saveFile();
}


void FZerofirmwareConfig::setRfRxPin(int value) {
    rfRx = value;
    saveFile();
}


void FZerofirmwareConfig::setRfModule(RFModules value) {
    rfModule = value;
    validateRfModuleValue();
    saveFile();
}


void FZerofirmwareConfig::validateRfModuleValue() {
    if (rfModule != M5_RF_MODULE && rfModule != CC1101_SPI_MODULE) {
        rfModule = M5_RF_MODULE;
    }
}


void FZerofirmwareConfig::setRfFreq(float value, int fxdFreq) {
    rfFreq = value;
    if (fxdFreq != NULL) rfFxdFreq = fxdFreq;
    saveFile();
}


void FZerofirmwareConfig::setRfFxdFreq(float value) {
    rfFxdFreq = value;
    saveFile();
}


void FZerofirmwareConfig::setRfScanRange(int value, int fxdFreq) {
    rfScanRange = value;
    rfFxdFreq = fxdFreq;
    validateRfScanRangeValue();
    saveFile();
}


void FZerofirmwareConfig::validateRfScanRangeValue() {
    if (rfScanRange < 0 || rfScanRange > 3) rfScanRange = 3;
}


void FZerofirmwareConfig::setRfidModule(RFIDModules value) {
    rfidModule = value;
    validateRfidModuleValue();
    saveFile();
}


void FZerofirmwareConfig::validateRfidModuleValue() {
    if (
        rfidModule != M5_RFID2_MODULE
        && rfidModule != PN532_I2C_MODULE
        && rfidModule != PN532_SPI_MODULE
    ) {
        rfidModule = M5_RFID2_MODULE;
    }
}


void FZerofirmwareConfig::addMifareKey(String value) {
    if (value.length() != 12) return;
    mifareKeys.insert(value);
    validateMifareKeysItems();
    saveFile();
}


void FZerofirmwareConfig::validateMifareKeysItems() {
    for (auto key = mifareKeys.begin(); key != mifareKeys.end();) {
        if (key->length() != 12) key = mifareKeys.erase(key);
        else ++key;
    }
}


void FZerofirmwareConfig::setGpsBaudrate(int value) {
    gpsBaudrate = value;
    validateGpsBaudrateValue();
    saveFile();
}


void FZerofirmwareConfig::validateGpsBaudrateValue() {
    if (gpsBaudrate != 9600 && gpsBaudrate != 115200) gpsBaudrate = 9600;
}


void FZerofirmwareConfig::setStartupApp(String value) {
    startupApp = value;
    saveFile();
}


void FZerofirmwareConfig::setWigleBasicToken(String value) {
    wigleBasicToken = value;
    saveFile();
}


void FZerofirmwareConfig::setDevMode(int value) {
    devMode = value;
    validateDevModeValue();
    saveFile();
}


void FZerofirmwareConfig::validateDevModeValue() {
    if (devMode > 1) devMode = 1;
}


void FZerofirmwareConfig::addDisabledMenu(String value) {
    // TODO: check if duplicate
    disabledMenus.push_back(value);
    saveFile();
}

void FZerofirmwareConfig::addQrCodeEntry(const String& menuName, const String& content) {
    qrCodes.push_back({menuName, content});
    saveFile();
}

void FZerofirmwareConfig::removeQrCodeEntry(const String& menuName) {
    qrCodes.erase(std::remove_if(qrCodes.begin(), qrCodes.end(),
        [&](const QrCodeEntry& entry) { return entry.menuName == menuName; }), qrCodes.end());
    saveFile();
}
