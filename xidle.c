/*
 * Hipuranyhou - litd - v1.0.0 - 03.02.2020
 * Daemon for automatic management of keyboard and display brightness
 * using applesmc light sensor (for Mac on Linux.)
 */

// xidle.c author: https://github.com/gpolitis/xidle

#include <X11/extensions/scrnsaver.h>
#include "xidle.h"

int get_user_idle_time(void) {
    Display *display = XOpenDisplay(0);
    if (!display) return -1;
    XScreenSaverInfo *info = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(display, DefaultRootWindow(display), info);
    XCloseDisplay(display);
    return info->idle;
}

