#pragma once

#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>
#include <RtcDS3231.h>

#include "event.h"
#include "Pump.h"

#define GetFString(s) (reinterpret_cast<const __FlashStringHelper *>(s))

struct Data {
  Pump* pump1;
  Pump* pump2;
  bool isBuzzing;
};

typedef Data *(*getData_cb)();

const char page1_title[] PROGMEM = "Bilge Pump Monitor";
const char page2_title[] PROGMEM = "Last Run Times";
const char page3_title[] PROGMEM = "Statistics";
const char min_avg_max[] PROGMEM = "Min    Avg    Max";
const char    last_run[] PROGMEM = "Last Pump Run";
const char        next[] PROGMEM = "Next";
const char       quiet[] PROGMEM = "Quiet";
const char       reset[] PROGMEM = "Reset";
const char    lifetime[] PROGMEM = "Lifetime:";
const char     current[] PROGMEM = "Current:";

class PageController {
public:
  PageController(RtcDS3231 *rtc, getData_cb getData);
  void drawCurrentPage();
  void drawNextPage();
  void drawResetWindow(int num);

  int currentPage;
  
private:
  Adafruit_SSD1306 *display;
  RtcDS3231 *Rtc;

  getData_cb getData;

  void drawPage1();
  void drawPage2();
  void drawPage3();
  void drawTitle(const char *text);
  void drawMenu(const char *menuItem1, const char *menuItem2);
  void drawFStringAt(const char *string, int x, int y);
  void drawStringAt(const char *string, int x, int y);
  void drawStringAt(long int s, int x, int y);

  int getRow(int r);
  int getCol(int c);
};

