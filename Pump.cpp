#include <EEPROM.h>
#include "Pump.h"

Pump::Pump(int eepromOffset)
{
  this->eepromOffset = eepromOffset;

  currentCount = 0;
  lastRunTime = 0;
  lastRunDuration = 0;
  currentRunStart = 0;

  EEPROM.get(sizeof(NonVolatileStats) * eepromOffset, nv);
}

void Pump::OnStart(Event *e)
{
  currentCount++;
  currentRunStart = e->time;
  nv.lifetimeCount++;

  EEPROM.put(sizeof(NonVolatileStats) * eepromOffset, nv);
}

void Pump::OnStop(Event *e)
{
  lastRunTime = currentRunStart;
  lastRunDuration = e->time - lastRunTime;
  currentRunStart = 0;

  if(lastRunDuration < nv.shortestRun || nv.shortestRun == 0) {
    nv.shortestRun = lastRunDuration;
  }

  if(lastRunDuration > nv.longestRun) {
    nv.longestRun = lastRunDuration;
  }

  EEPROM.put(sizeof(NonVolatileStats) * eepromOffset, nv);
}

void Pump::ResetStatistics()
{
  nv.shortestRun = 0;
  nv.longestRun = 0;
  currentCount = 0;
  lastRunTime = 0;
  lastRunDuration = 0;

  EEPROM.put(sizeof(NonVolatileStats) * eepromOffset, nv);
}

long Pump::GetCurrentCount()
{
  return currentCount;
}

long Pump::GetLifetimeCount()
{
  return nv.lifetimeCount;
}

long Pump::GetShortestRun()
{
  return nv.shortestRun;
}

long Pump::GetLongestRun()
{
  return nv.longestRun;
}

long Pump::GetAverageRun()
{
  return (nv.longestRun + nv.shortestRun) / 2;
}

