#include <Arduino.h>
#include "Pin.h"

Pin::Pin(short pin, short debounceDelay, event_cb onChange)
{
  this->pin = pin;
  this->debounceDelay = debounceDelay;
  this->onChange = onChange;

  currentState = LOW;
  lastState = LOW;
  lastReadTime = 0;

  pinMode(pin, INPUT);
}

void Pin::Read()
{
  int readState = digitalRead(pin);

  if(readState != lastState) {
    lastReadTime = millis();
  }

  if((millis() - lastReadTime) > debounceDelay && readState != currentState) {
    currentState = readState;
    HandleChange();
  }

  lastState = readState;
}

void Pin::HandleChange()
{
  Event *event;

  event = new Event();

  event->id = pin;
  event->time = lastReadTime;

  if(currentState == HIGH) {
    event->type = DOWN;
  } else {
    event->type = UP;
  }

  if(onChange) {
    onChange(event);
  }

  delete event;
}
