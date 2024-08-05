// Sane Break is a polite break reminder to keep you sane.
// Copyright (C) 2024 Allan Chain
// SPDX-License-Identifier: GPL-3.0-or-later

#include <qglobal.h>

#ifdef Q_OS_LINUX
#ifndef SANE_IDLE_WAYLAND_H
#define SANE_IDLE_WAYLAND_H

#include <wayland-client.h>

#include <QElapsedTimer>
#include <QTimer>

#include "idle-time.h"
#include "wayland-ext-idle-notify-v1-client-protocol.h"

class IdleTimeWayland : public SystemIdleTime {
  Q_OBJECT
 public:
  IdleTimeWayland();
  void startWatching(WatchOption option);
  void stopWatching();

 private:
  static void globalAdded(void *data, wl_registry *registry, uint32_t name,
                          const char *interface, uint32_t version);
  static void globalRemoved(void *data, wl_registry *registry, uint32_t name);
  const struct wl_registry_listener globalListener {
    globalAdded, globalRemoved
  };
  static void idled(void *data, ext_idle_notification_v1 *object);
  static void resumed(void *data, ext_idle_notification_v1 *object);
  const struct ext_idle_notification_v1_listener idleListener {
    idled, resumed
  };
  wl_seat *seat;
  ext_idle_notifier_v1 *idleNotifier;
  ext_idle_notification_v1 *idleNotification;
  QElapsedTimer *elapseTimer;
  bool isIdle;
  bool isWatching = false;
};

#endif  // SANE_IDLE_WAYLAND_H
#endif  // Q_OS_LINUX
