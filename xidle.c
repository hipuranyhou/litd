// https://github.com/gpolitis/xidle

#include <stdio.h>
#include <X11/extensions/scrnsaver.h>

int get_user_idle_time(void) {
    Display *display = XOpenDisplay(0);
    if (!display) return -1;
    XScreenSaverInfo *info = XScreenSaverAllocInfo();
    XScreenSaverQueryInfo(display, DefaultRootWindow(display), info);
    XCloseDisplay(display);
    return info->idle;
}

