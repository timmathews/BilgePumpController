#pragma once

#include "event.h"

class Pin {
public:
  Pin(short pin, short debounceDelay, event_cb onChange);
  void Read();

private:
  void HandleChange();

  short pin;
  short debounceDelay;
  short currentState;
  short lastState;
  long lastReadTime;

  event_cb onChange;
};
