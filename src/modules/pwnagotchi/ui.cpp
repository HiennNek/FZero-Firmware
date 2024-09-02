/* Thanks to thoses developers for their projects:
* @7h30th3r0n3 : https://github.com/7h30th3r0n3/Evil-M5Core2 and https://github.com/7h30th3r0n3/PwnGridSpam
* @viniciusbo : https://github.com/viniciusbo/m5-palnagotchi
* @sduenasg : https://github.com/sduenasg/pio_palnagotchi
*/

#ifndef LITE_VERSION
#include "ui.h"

#define ROW_SIZE 40
#define PADDING 10

int32_t display_w;
int32_t display_h;
int32_t canvas_h;
int32_t canvas_center_x;
int32_t canvas_top_h;
int32_t canvas_bot_h;
int32_t canvas_peers_menu_h;
int32_t canvas_peers_menu_w;

uint8_t menu_current_cmd = 0;
uint8_t menu_current_opt = 0;

void initUi() {
  tft.setTextSize(1);
  tft.fillScreen(BGCOLOR);
  tft.setTextColor(FGCOLOR);

  display_w = WIDTH;
  display_h = HEIGHT;
  canvas_h = display_h * .8;
  canvas_center_x = display_w / 2;
  canvas_top_h = display_h * .1;
  canvas_bot_h = display_h * .9;
  canvas_peers_menu_h = display_h * .8;
  canvas_peers_menu_w = display_w * .8;
}

String getRssiBars(signed int rssi) {
  String rssi_bars = "";

  if (rssi != -1000) {
    if (rssi >= -67) {
      rssi_bars = "||||";
    } else if (rssi >= -70) {
      rssi_bars = "|||";
    } else if (rssi >= -80) {
      rssi_bars = "||";
    } else {
      rssi_bars = "|";
    }
  }

  return rssi_bars;
}

void drawTime() {
  tft.fillRect(50, 0, display_w, canvas_top_h - 3, BGCOLOR);
  tft.setTextDatum(TR_DATUM);
  unsigned long ellapsed = millis() / 1000;
  int8_t h = ellapsed / 3600;
  int sr = ellapsed % 3600;
  int8_t m = sr / 60;
  int8_t s = sr % 60;
  char right_str[50] = "UPS 0%  UP 00:00:00";
  sprintf(right_str, "UPS %i%% UP %02d:%02d:%02d", getBattery(), h, m, s);
  tft.drawString(right_str, display_w, 3);
}

void drawFooterData(uint8_t friends_run, uint8_t friends_tot, String last_friend_name, signed int rssi) {
  tft.fillRect(0, canvas_bot_h+1, display_w-50, canvas_bot_h+10, BGCOLOR);
  tft.setTextSize(1);
  tft.setTextColor(FGCOLOR);
  tft.setTextDatum(TL_DATUM);

  String rssi_bars = getRssiBars(rssi);
  char stats[25] = "FRND 0 (0)";
  if (friends_run > 0) {
    sprintf(stats, "FRND %d (%d) [%s] %s", friends_run, friends_tot,
            last_friend_name, rssi_bars);
  }

  tft.drawString(stats, 0, canvas_bot_h+5);
}

void updateUi(bool show_toolbars) {
  uint8_t mood_id = getCurrentMoodId();
  String mood_face = getCurrentMoodFace();
  String mood_phrase = getCurrentMoodPhrase();
  bool mood_broken = isCurrentMoodBroken();

  // Draw header and footer
  if (show_toolbars) {
    drawTime();
    drawFooterData(getPwngridRunTotalPeers(), getPwngridTotalPeers(), getPwngridLastFriendName(), getPwngridClosestRssi());
  }

  // Draw mood
  drawMood(mood_face, mood_phrase, mood_broken);
}

void drawTopCanvas() {
  tft.fillRect(0, 0, display_w, canvas_top_h, BGCOLOR);
  tft.setTextSize(1);
  tft.setTextColor(FGCOLOR);
  tft.setTextDatum(TL_DATUM);
  tft.drawString("CH *", 0, 3);

  tft.drawLine(0, canvas_top_h - 1, display_w, canvas_top_h - 1, FGCOLOR);
}

void drawBottomCanvas() {
  tft.fillRect(0, canvas_bot_h, display_w, canvas_bot_h+10, BGCOLOR);
  tft.setTextSize(1);
  tft.setTextColor(FGCOLOR);

  tft.setTextDatum(TR_DATUM);
  tft.drawString("NOT AI", display_w, canvas_bot_h+5);
  tft.drawLine(0, canvas_bot_h, display_w, canvas_bot_h, FGCOLOR);
}

void drawMood(String face, String phrase, bool broken) {
  tft.setTextColor(FGCOLOR);

  tft.setTextSize(4);
  tft.setTextDatum(MC_DATUM);
  tft.fillRect(0, canvas_top_h+10, display_w, canvas_bot_h-40, BGCOLOR);
  tft.drawCentreString(face, canvas_center_x, canvas_h / 3, SMOOTH_FONT);
  tft.setTextDatum(BC_DATUM);
  tft.setTextSize(1);
  tft.drawCentreString(phrase, canvas_center_x, canvas_h - 30, SMOOTH_FONT);
}
#endif
