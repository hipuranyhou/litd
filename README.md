# litd
Daemon for automatic management of keyboard and display brightness using applesmc light sensor (for Mac on Linux.)

### Setup
Clone this repo where you want to have litd installed.

**litd cannot run as root (to get user idle time properly)**
 
Make sure you have `rw` permissions on keyboard brightness file `/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness` and display brightness file `/sys/class/backlight/intel_backlight/brightness`.

Permissions for keyboard and display brightness files need to be set on every boot. For this use `brightness-perm.service`. Put it in `/etc/systemd/system/`, reload systemd daemon
```Shell
sudo systemctl daemon-reload
```

and enable
```Shell
sudo systemctl enable --now brightness-perm.service
```

Then make sure you have `r` permission on light sensor file `/sys/devices/platform/applesmc.768/light` (you should have it by deafult.)

After this edit where will litd create its `litd.pid` file (`daemonize.c` - at the end.)

Next edit `bin/litd-autostart` to point to your install directory, add `x` permission and autostart it at boot. **Make sure you keep the `-d` option in your path.** 

### Compilation 
```Shell
g++ litd.c xidle.c daemonize.c functions.c -lXss -lX11 -o bin/litd
```

Reboot and check that everything works.

### Debug

For debugging you can enable verbose mode (not allowed in daemon mode.)
```Shell
./litd -v
```