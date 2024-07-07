#ifdef __APPLE__
#ifndef SANE_IDLE_DARWIN_H
#define SANE_IDLE_DARWIN_H

#include <QTimer>

#include "system.h"

int systemIdleTime();

class IdleTimeDarwin : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeDarwin();
  void startWatching();
  void stopWatching();

 private:
  QTimer* timer;
  void tick();
  bool isIdle;
};

#endif  // SANE_IDLE_DARWIN_H
#endif  // __APPLE__