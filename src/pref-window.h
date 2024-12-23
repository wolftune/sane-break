// Sane Break is a polite and sane break reminder preventing mindless skips.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#ifndef SANE_PREFERENCES_WINDOW_H
#define SANE_PREFERENCES_WINDOW_H
#include <QAudioOutput>
#include <QCheckBox>
#include <QComboBox>
#include <QLabel>
#include <QMainWindow>
#include <QMediaPlayer>
#include <QSlider>

namespace Ui {
class PrefWindow;
}

class PreferenceWindow : public QMainWindow {
  Q_OBJECT

 public:
  PreferenceWindow(QWidget *parent = nullptr);
  ~PreferenceWindow();
  void loadSettings();
  void saveSettings();

 private:
  Ui::PrefWindow *ui;
  void closeEvent(QCloseEvent *event);
  QMediaPlayer *soundPlayer;
  QAudioOutput *audioOutput;
  void setTab(int tabNum);
  void playStartSound();
  void playEndSound();
};
#endif  // SANE_PREFERENCES_WINDOW_H
