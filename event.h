#pragma once

enum EventType {
  DOWN,
  UP
};

struct Event {
  int id;
  long time;
  EventType type;
};

typedef void (*event_cb)(Event*);
