#pragma once

#include "event.h"

struct NonVolatileStats {
  long lifetimeCount;
  long shortestRun;
  long longestRun;
};

class Pump {
public:
  Pump(int eepromOffset);
  void OnStart(Event *e);
  void OnStop(Event *e);
  void ResetStatistics();

  long GetCurrentCount();
  long GetLifetimeCount();
  long GetShortestRun();
  long GetLongestRun();
  long GetAverageRun();

private:
  NonVolatileStats nv;
  long currentCount;
  long lastRunTime;
  long lastRunDuration;
  long currentRunStart;
  short eepromOffset;
};
