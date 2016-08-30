#include <Wire.h>
#include <avr/pgmspace.h>

#include "PageController.h"

void fmtDateTime(const RtcDateTime& dt, char &buf, int buflen)
{
  snprintf_P(&buf, buflen,
  PSTR("%04u-%02u-%02u %02u:%02u:%02u"),
  dt.Year(), dt.Month(), dt.Day(),
  dt.Hour(), dt.Minute(), dt.Second());
}

PageController::PageController(RtcDS3231 *rtc, getData_cb getData)
{
  // initialize with the I2C addr 0x3C and generate
  // HV for display internally.

  display = new Adafruit_SSD1306(-1);

  display->begin(SSD1306_SWITCHCAPVCC, 0x3C);
  display->clearDisplay();
  display->setTextSize(1);
  display->setTextColor(WHITE);
  display->setRotation(2);

  this->Rtc = rtc;

  this->getData = getData;
  this->currentPage = 1;
}

void PageController::drawTitle(const char *text)
{
  if(text != NULL) {
    display->setCursor((128 - strlen_P(text) * 6) / 2, 2);
    display->println(GetFString(text));
    display->drawRect(0, 0, 128, 11, WHITE);
  }
}

void PageController::drawMenu(const char *menuItem1, const char *menuItem2)
{
  if(menuItem1 != NULL && strlen_P(menuItem1) > 0) {
    //display->setCursor((64 - strlen_P(menuItem1) * 6) / 2, 54);
    display->setCursor(0, 54);
    display->println(GetFString(menuItem1));
  }

  if(menuItem2 != NULL && strlen_P((const char *)menuItem2) > 0) {
    //display->setCursor(64 + (64 - strlen_P(menuItem2) * 6) / 2, 54);
    display->setCursor(127 - strlen_P(menuItem2) * 6, 54);
    display->println(GetFString(menuItem2));
  }
}

void PageController::drawFStringAt(const char *string, int x, int y)
{
  display->setCursor(x, y);
  display->println(GetFString(string));
}

void PageController::drawStringAt(const char *string, int x, int y)
{
  display->setCursor(x, y);
  display->println(string);
}

void PageController::drawStringAt(long int s, int x, int y)
{
  display->setCursor(x, y);
  display->println(s);
}

void PageController::drawPage1()
{
  auto data = this->getData();
  currentPage = 1;

  display->clearDisplay();

  drawTitle((page1_title));

  if(data->isBuzzing) {
    drawMenu(quiet, next);
  } else {
    drawMenu(NULL, next);
  }

  drawFStringAt(lifetime, getCol(1), getRow(0));
  drawFStringAt(current,  getCol(2), getRow(1));
  drawFStringAt(lifetime, getCol(1), getRow(2));
  drawFStringAt(current,  getCol(2), getRow(3));

  int col = getCol(11);

  drawStringAt(data->pump1->GetLifetimeCount(), col, getRow(0));
  drawStringAt(data->pump1->GetCurrentCount(),  col, getRow(1));
  drawStringAt(data->pump2->GetLifetimeCount(), col, getRow(2));
  drawStringAt(data->pump2->GetCurrentCount(),  col, getRow(3));

  display->display();
}

void PageController::drawPage2()
{
  char datestring[20];

  currentPage = 2;

  Rtc->IsDateTimeValid();
  RtcDateTime dt = Rtc->GetDateTime();
  fmtDateTime(dt, *datestring, 20);

  display->clearDisplay();

  drawTitle(page2_title);
  drawMenu(reset, next);

  display->setCursor(getCol(1), getRow(1));
  display->println(datestring);

  display->setCursor(getCol(1), getRow(2));
  display->println(F("2013-12-11 10:09:08"));

  display->setCursor(getCol(1), getRow(3));
  display->println(F("2006-05-04 03:02:01"));

  display->display();
}

void PageController::drawPage3()
{
  auto data = this->getData();

  long s1 = data->pump1->GetShortestRun();
  long a1 = data->pump1->GetAverageRun();
  long l1 = data->pump1->GetLongestRun();
  long s2 = data->pump2->GetShortestRun();
  long a2 = data->pump2->GetAverageRun();
  long l2 = data->pump2->GetLongestRun();
  
  currentPage = 3;
  display->clearDisplay();

  drawTitle(page3_title);
  drawMenu(reset, next);

  drawFStringAt(min_avg_max, getCol(1), getRow(1));

  drawStringAt(s1, getCol(0x1), getRow(2));
  drawStringAt(a1, getCol(0x8), getRow(2));
  drawStringAt(l1, getCol(0xF), getRow(2));
  
  drawStringAt(s2, getCol(0x1), getRow(3));
  drawStringAt(a2, getCol(0x8), getRow(3));
  drawStringAt(l2, getCol(0xF), getRow(3));

  display->display();
}

void PageController::drawResetWindow(int num)
{
  display->clearDisplay();

  drawTitle(reset);

  display->setTextSize(4);

  display->setCursor(54, 20);
  display->println(num);

  display->setTextSize(1);

  display->display();
}

void PageController::drawCurrentPage()
{
  switch(currentPage) {
  case 1:
    drawPage1();
    break;
  case 2:
    drawPage2();
    break;
  case 3:
    drawPage3();
    break;
  }
}

void PageController::drawNextPage()
{
  switch(currentPage) {
  case 1:
    drawPage2();
    break;
  case 2:
    drawPage3();
    break;
  case 3:
    drawPage1();
    break;
  }
}

int PageController::getRow(int r)
{
  return 12 + r * 9;
}

int PageController::getCol(int c)
{
  return 1 + c * 6;
}
