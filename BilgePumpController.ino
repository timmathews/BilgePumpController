#include <SPI.h>
#include <Wire.h>
#include <RtcDS3231.h>
#include <avr/pgmspace.h>
#include <TimerOne.h>

#include "BilgePumpController.h"

RtcDS3231 Rtc;
Data data;

volatile bool doRefresh = false;

void refreshDisplay()
{
  doRefresh = true;
}

void setup() {
  Serial.begin(9600);

  pinMode(buzzerPin, OUTPUT);
  pinMode(ledPin, OUTPUT);

  Timer1.initialize(100000); // 0.10 seconds
  Timer1.attachInterrupt(refreshDisplay);

  Rtc.Begin();
  RtcDateTime compiled = RtcDateTime(__DATE__, __TIME__);
  if (!Rtc.IsDateTimeValid())  {
    Rtc.SetDateTime(compiled);
  }

  if(!Rtc.GetIsRunning()) {
    Rtc.SetIsRunning(true);
  }

  Rtc.Enable32kHzPin(false);
  Rtc.SetSquareWavePin(DS3231SquareWavePin_ModeNone);

  pump1 = new Pump(0);
  pump2 = new Pump(1);

  pumpPin1 = new Pin(pumpSensor1Pin, debounceDelay, handlePump1);
  pumpPin2 = new Pin(pumpSensor2Pin, debounceDelay, handlePump2);
  buttonPin1 = new Pin(button1Pin, debounceDelay, handleButton1);
  buttonPin2 = new Pin(button2Pin, debounceDelay, handleButton2);

  pc = new PageController(&Rtc, &getData);
  pc->drawCurrentPage();
}

void loop() {
  pumpPin1->Read();
  pumpPin2->Read();
  buttonPin1->Read();
  buttonPin2->Read();

  if(doRefresh && !isResetting)
  {
    noInterrupts();
    doRefresh = false;
    interrupts();

    pc->drawCurrentPage();
  }

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
}

void handleButton1(Event *e) {
  if(e->type == UP) {
    if(pc->currentPage == 1) {
      isBuzzing = false;
    } else {
      isResetting = false;
    }
  } else if(e->type == DOWN && pc->currentPage != 1) {
    startResetCounter = millis();
    isResetting = true;
  }
}

void handleButton2(Event *e) {
  if(e->type == DOWN) {
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
  }
}

void handlePump2(Event *e) {
  if(e->type == DOWN) {
    pump2->OnStart(e);
    isBuzzing = true;
  } else {
    pump2->OnStop(e);
    isBuzzing = false;
  }
}
