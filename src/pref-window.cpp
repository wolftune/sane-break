// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include "pref-window.h"

#include <QDesktopServices>
#include <QGridLayout>
#include <QIcon>
#include <QLabel>
#include <QMainWindow>
#include <QPushButton>
#include <QSettings>
#include <QSlider>
#include <QStyleOptionSlider>
#include <QVBoxLayout>

#include "config.h"
#include "notice-window.h"
#include "preferences.h"

SteppedSlider::SteppedSlider(Qt::Orientation orientation, QWidget *parent)
    : QSlider(orientation, parent) {
  setTickPosition(QSlider::TicksBelow);
}

void SteppedSlider::mousePressEvent(QMouseEvent *event) {
  if (event->button() == Qt::LeftButton) {
    int value = calculateValueFromPosition(event->pos());
    setValue(value);
    event->accept();
  } else {
    QSlider::mousePressEvent(event);
  }
}

void SteppedSlider::mouseMoveEvent(QMouseEvent *event) {
  if (event->buttons() & Qt::LeftButton) {
    int value = calculateValueFromPosition(event->pos());
    setValue(value);
    event->accept();
  } else {
    QSlider::mouseMoveEvent(event);
  }
}

int SteppedSlider::calculateValueFromPosition(const QPoint &pos) const {
  int min = minimum();
  int max = maximum();
  int step = singleStep();
  int range = max - min;

  QStyleOptionSlider opt;
  initStyleOption(&opt);
  QRect sliderRect = style()->subControlRect(QStyle::CC_Slider, &opt,
                                             QStyle::SC_SliderHandle, this);

  if (orientation() == Qt::Horizontal) {
    int sliderLength = width() - sliderRect.width();
    int posX = pos.x() - sliderRect.width() / 2;
    int value = min + (posX * range) / sliderLength;
    value = ((qBound(min, value, max) - min) / step) * step + min;
    return value;
  } else {
    int sliderLength = height() - sliderRect.height();
    int posY = pos.y() - sliderRect.height() / 2;
    int value = min + ((sliderLength - posY) * range) / sliderLength;
    value = ((qBound(min, value, max) - min) / step) * step + min;
    return value;
  }
}

PreferenceWindow::PreferenceWindow(QWidget *parent) : QMainWindow(parent) {
  setWindowFlag(Qt::Dialog);
  setWindowIcon(QIcon(":/images/icon.png"));
  resize(400, 500);

  QWidget *centralWidget = new QWidget(this);
  setCentralWidget(centralWidget);
  centralWidget->setContentsMargins(10, 10, 10, 10);

  QVBoxLayout *layout = new QVBoxLayout(centralWidget);
  layout->setAlignment(Qt::AlignCenter);
  layout->setSpacing(20);

  QHBoxLayout *titleLayout = new QHBoxLayout();
  titleLayout->setAlignment(Qt::AlignLeft);

  QLabel *imageLabel = new QLabel();
  QPixmap pixmap = QPixmap(":/images/icon-256.png");
  pixmap.setDevicePixelRatio(2.0);
  imageLabel->setPixmap(pixmap);
  imageLabel->setAlignment(Qt::AlignCenter);
  imageLabel->setMargin(20);
  titleLayout->addWidget(imageLabel);

  QVBoxLayout *titleTextLayout = new QVBoxLayout();
  titleTextLayout->setAlignment(Qt::AlignCenter);

  titleTextLayout->addWidget(new QLabel("<h1>Sane Break</h1>"));

  QLabel *copyrightLabel = new QLabel(
      QString("<p>A polite break reminder to keep you sane.</p>"
              "<p>Copyright (C) 2024 Allan Chain, "
              "<a href=https://www.gnu.org/licenses/gpl-3.0.en.html>GPL 3.0</a>"
              "<br>Version v%1. Source available at "
              "<a href=https://github.com/AllanChain/sane-break>GitHub</a></p>"
              "<p><a href=notice-window>Third party libraries notices</a></p>")
          .arg(PROJECT_VERSION));
  copyrightLabel->setTextFormat(Qt::RichText);
  connect(copyrightLabel, &QLabel::linkActivated, this, [=](QString url) {
    if (url == QString("notice-window"))
      (new NoticeWindow(this))->show();
    else
      QDesktopServices::openUrl(url);
  });
  titleTextLayout->addWidget(copyrightLabel);

  titleLayout->addLayout(titleTextLayout);
  layout->addLayout(titleLayout);

  layout->addWidget(new QLabel("<h3>Breaks</h3>"));

  QGridLayout *breakForm = new QGridLayout();
  layout->addLayout(breakForm);

  breakForm->addWidget(new QLabel("Small break every"), 0, 0);
  breakForm->addWidget(new QLabel("Small break for"), 1, 0);

  smallBreakEverySlider = new SteppedSlider(Qt::Horizontal);
  smallBreakEverySlider->setMaximum(60);
  smallBreakEverySlider->setSingleStep(5);
  breakForm->addWidget(smallBreakEverySlider, 0, 1);

  QLabel *smallBreakEveryLabel = new QLabel();
  breakForm->addWidget(smallBreakEveryLabel, 0, 2);
  smallBreakEveryLabel->setText(
      QString("%1 min").arg(smallBreakEverySlider->value()));
  connect(smallBreakEverySlider, &SteppedSlider::valueChanged, this,
          [smallBreakEveryLabel](int value) {
            smallBreakEveryLabel->setText(QString("%1 min").arg(value));
          });

  smallBreakForSlider = new SteppedSlider(Qt::Horizontal);
  smallBreakForSlider->setMaximum(60);
  smallBreakForSlider->setSingleStep(5);
  breakForm->addWidget(smallBreakForSlider, 1, 1);

  QLabel *smallBreakForLabel = new QLabel();
  breakForm->addWidget(smallBreakForLabel, 1, 2);
  smallBreakForLabel->setText(
      QString("%1 sec").arg(smallBreakForSlider->value()));
  connect(smallBreakForSlider, &SteppedSlider::valueChanged, this,
          [smallBreakForLabel](int value) {
            smallBreakForLabel->setText(QString("%1 sec").arg(value));
          });

  breakForm->addWidget(new QLabel("Big break after"), 2, 0);
  breakForm->addWidget(new QLabel("Big break for"), 3, 0);

  bigBreakAfterSlider = new SteppedSlider(Qt::Horizontal);
  bigBreakAfterSlider->setMinimum(1);
  bigBreakAfterSlider->setMaximum(20);
  breakForm->addWidget(bigBreakAfterSlider, 2, 1);

  QLabel *bigBreakAfterLabel = new QLabel();
  breakForm->addWidget(bigBreakAfterLabel, 2, 2);
  bigBreakAfterLabel->setText(
      QString("%1 small breaks").arg(bigBreakAfterSlider->value()));
  connect(bigBreakAfterSlider, &SteppedSlider::valueChanged, this,
          [bigBreakAfterLabel, this](int value) {
            bigBreakAfterLabel->setText(
                QString("%1 min").arg(value * smallBreakEverySlider->value()));
          });

  bigBreakForSlider = new SteppedSlider(Qt::Horizontal);
  bigBreakForSlider->setMaximum(300);
  bigBreakForSlider->setSingleStep(10);
  bigBreakForSlider->setTickInterval(60);
  bigBreakForSlider->setPageStep(60);
  breakForm->addWidget(bigBreakForSlider, 3, 1);

  QLabel *bigBreakForLabel = new QLabel();
  breakForm->addWidget(bigBreakForLabel, 3, 2);
  bigBreakForLabel->setText(QString("%1 sec").arg(bigBreakForSlider->value()));
  connect(bigBreakForSlider, &SteppedSlider::valueChanged, this,
          [bigBreakForLabel](int value) {
            bigBreakForLabel->setText(QString("%1 sec").arg(value));
          });

  pauseOnIdleSlider = new SteppedSlider(Qt::Horizontal);
  pauseOnIdleSlider->setMaximum(20);
  pauseOnIdleSlider->setMinimum(1);
  pauseOnIdleSlider->setTickPosition(SteppedSlider::TicksBelow);

  breakForm->addWidget(new QLabel("Pause on idle for"), 4, 0);
  breakForm->addWidget(pauseOnIdleSlider, 4, 1);
  QLabel *pauseOnIdleLabel = new QLabel();
  breakForm->addWidget(pauseOnIdleLabel, 4, 2);
  pauseOnIdleLabel->setText(QString("%1 min").arg(pauseOnIdleSlider->value()));
  connect(pauseOnIdleSlider, &SteppedSlider::valueChanged, this,
          [pauseOnIdleLabel](int value) {
            pauseOnIdleLabel->setText(QString("%1 min").arg(value));
          });

  resetOnIdleSlider = new SteppedSlider(Qt::Horizontal);
  resetOnIdleSlider->setMaximum(60);
  resetOnIdleSlider->setMinimum(5);
  resetOnIdleSlider->setSingleStep(5);
  resetOnIdleSlider->setTickPosition(SteppedSlider::TicksBelow);

  breakForm->addWidget(new QLabel("Reset on idle for"), 5, 0);
  breakForm->addWidget(resetOnIdleSlider, 5, 1);
  QLabel *resetOnIdleLabel = new QLabel();
  breakForm->addWidget(resetOnIdleLabel, 5, 2);
  resetOnIdleLabel->setText(QString("%1 min").arg(resetOnIdleSlider->value()));
  connect(resetOnIdleSlider, &SteppedSlider::valueChanged, this,
          [resetOnIdleLabel](int value) {
            resetOnIdleLabel->setText(QString("%1 min").arg(value));
          });

  QHBoxLayout *bottomButtonLayout = new QHBoxLayout();
  QPushButton *resetButton = new QPushButton("Reset");
  connect(resetButton, &QPushButton::pressed, this,
          &PreferenceWindow::loadSettings);
  bottomButtonLayout->addWidget(resetButton);
  bottomButtonLayout->addStretch();
  QPushButton *saveButton = new QPushButton("Save");
  connect(saveButton, &QPushButton::pressed, this, &PreferenceWindow::close);
  bottomButtonLayout->addWidget(saveButton);
  layout->addLayout(bottomButtonLayout);
}

void PreferenceWindow::loadSettings() {
  QSettings settings;
  smallBreakEverySlider->setValue(SanePreferences::smallEvery() / 60);
  smallBreakForSlider->setValue(SanePreferences::smallFor());
  bigBreakAfterSlider->setValue(SanePreferences::bigAfter());
  bigBreakForSlider->setValue(SanePreferences::bigFor());
  pauseOnIdleSlider->setValue(SanePreferences::pauseOnIdleFor() / 60);
  resetOnIdleSlider->setValue(SanePreferences::resetOnIdleFor() / 60);
}

void PreferenceWindow::saveSettings() {
  QSettings settings;
  settings.setValue("break/small-every", smallBreakEverySlider->value() * 60);
  settings.setValue("break/small-for", smallBreakForSlider->value());
  settings.setValue("break/big-after", bigBreakAfterSlider->value());
  settings.setValue("break/big-for", bigBreakForSlider->value());
  settings.setValue("break/pause-on-idle-for", pauseOnIdleSlider->value() * 60);
  settings.setValue("break/reset-on-idle-for", resetOnIdleSlider->value() * 60);
}

void PreferenceWindow::closeEvent(QCloseEvent *event) {
  saveSettings();
  QMainWindow::closeEvent(event);
}
