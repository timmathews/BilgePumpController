#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

#include "BilgePumpController.h"

Adafruit_SSD1306 display(-1);
Data data;

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  pump1 = new Pump(0);
  pump2 = new Pump(1);

  pumpPin1 = new Pin(pumpSensor1Pin, debounceDelay, handlePump1);
  pumpPin2 = new Pin(pumpSensor2Pin, debounceDelay, handlePump2);
  buttonPin1 = new Pin(button1Pin, debounceDelay, handleButton1);
  buttonPin2 = new Pin(button2Pin, debounceDelay, handleButton2);

  pc = new PageController(&display, &getData);

  pc->drawCurrentPage();
}

void loop() {
  pumpPin1->Read();
  pumpPin2->Read();
  buttonPin1->Read();
  buttonPin2->Read();

  if(isBuzzing) {
    digitalWrite(buzzerPin, HIGH);
    digitalWrite(ledPin, HIGH);
  } else {
    digitalWrite(buzzerPin, LOW);
    digitalWrite(ledPin, LOW);
  }

  if(isResetting) {
    int elapsed = (millis() - startResetCounter) / 1000;
    int remaining = resetDelay - elapsed;
    
    pc->drawResetWindow(remaining);

    if(remaining == 0) {
      doReset();
    }
  }
}

Data *getData() {
  data.pump1 = pump1;
  data.pump2 = pump2;
  data.isBuzzing = isBuzzing;

  return &data;
}

void doReset() {
  isResetting = false;

  pump1->ResetStatistics();
  pump2->ResetStatistics();

  delay(500);

  pc->currentPage = 1;
  pc->drawCurrentPage();
}

void handleButton1(Event *e) {
  if(e->type == UP) {
    if(pc->currentPage == 1) {
      isBuzzing = false;
    } else {
      isResetting = false;
      pc->drawCurrentPage();
    }
  } else if(e->type == DOWN && pc->currentPage != 1) {
    startResetCounter = millis();
    isResetting = true;
  }
}

void handleButton2(Event *e) {
  if(e->type == UP) {
    pc->drawNextPage();
  }
}

void handlePump1(Event *e) {
  if(e->type == DOWN) {
    pump1->OnStart(e);
    isBuzzing = true;
  } else {
    pump1->OnStop(e);
    isBuzzing = false;
    pc->drawCurrentPage();
  }
}

void handlePump2(Event *e) {
  if(e->type == DOWN) {
    pump2->OnStart(e);
    isBuzzing = true;
  } else {
    pump2->OnStop(e);
    isBuzzing = false;
    pc->drawCurrentPage();
  }
}
