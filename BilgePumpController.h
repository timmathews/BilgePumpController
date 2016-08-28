#pragma once

#include "event.h"
#include "PageController.h"
#include "Pin.h"
#include "Pump.h"

typedef void (*page_cb)();

// Configure pins
const int ledPin = 13;
const int pumpSensor1Pin = 12;
const int pumpSensor2Pin = 11;
const int button1Pin = 10;
const int button2Pin = 9;
const int buzzerPin = 6;

const int resetDelay = 3; // seconds
const long debounceDelay = 50; // milliseconds

bool isBuzzing = false;
bool isResetting = false;
long startResetCounter;

page_cb drawPage;

PageController *pc;
Pin *pumpPin1;
Pin *pumpPin2;
Pin *buttonPin1;
Pin *buttonPin2;
Pump *pump1;
Pump *pump2;

Data *getData();
void doReset();
void handleButton1(Event e);
void handleButton2(Event e);
void handlePump1(Event *e);
void handlePump2(Event *e);
