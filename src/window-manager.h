// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_BREAK_WINDOW_MANAGER_H
#define SANE_BREAK_WINDOW_MANAGER_H

#include <QAudioOutput>
#include <QList>
#include <QMediaPlayer>
#include <QObject>

#include "break-window.h"
#include "idle-time.h"

class BreakWindowManager : public QObject {
  Q_OBJECT

 public:
  BreakWindowManager();
  ~BreakWindowManager();
  int remainingTime;
  void show(BreakType type);
  bool isShowing();
  void close();

 signals:
  void timeout();

 private:
  bool isIdle = false;
  bool isForceBreak = false;
  int totalTime;
  QList<BreakWindow *> windows;
  QTimer *countdownTimer;
  QTimer *forceBreakTimer;
  QMediaPlayer *soundPlayer;
  QAudioOutput *audioOutput;
  SystemIdleTime *idleTimer;
  void createWindows(BreakType type);
  void tick();
  void forceBreak();
  void onIdleStart();
  void onIdleEnd();
};

#endif  // SANE_BREAK_WINDOW_MANAGER_H
