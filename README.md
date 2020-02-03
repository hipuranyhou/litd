# litd
Daemon for automatic management of keyboard and display brightness using applesmc light sensor (for Mac on Linux.)


## Installation guide
**litd cannot run as root (to get user idle time properly)**

### Prerequisities
To use litd your system has to fulfill all of these criteria:
1. Linux on a Mac.
1. Libraries: `libXss` and `libX11` (for getting user idle time.)
1. Files (permissions): 
   1. `/sys/devices/platform/applesmc.768/light` (o=r)
   1. `/sys/class/backlight/intel_backlight/brightness` (o=rw)
   1. `/sys/class/backlight/intel_backlight/max_brightness` (o=r)
   1. `/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness` (o=rw)
   1. `/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/max_brightness` (o=r)
1. GCC compiler.

ii) and iv) need `rw` permissions which have to be set on every boot. If you use systemd, you can use the included `brightness-perm.service` (how to use it is explained later in this guide.)

If your system fulfills all of these criteria, you can continue in this guide and use litd.


### Setup
Clone this repo where you want to have litd installed.
 
Make sure you have `rw` permissions on keyboard brightness file `/sys/devices/platform/applesmc.768/leds/smc::kbd_backlight/brightness` and display brightness file `/sys/class/backlight/intel_backlight/brightness`.

Permissions for keyboard and display brightness files need to be set on every boot. For this use `brightness-perm.service`. Put it in `/etc/systemd/system/`, reload systemd daemon
```Shell
$ systemctl daemon-reload
```

and enable
```Shell
$ systemctl enable --now brightness-perm.service
```


### Compilation 
Compile `litd` (probably into `bin/` directory) like this:
```Shell
$ g++ litd.c xidle.c daemonize.c control.c config.c -O3 -lXss -lX11 -o bin/litd
```


### Generate config file
To generate default config file, simply run `./litd` ([where is config file located?](#config-file))

Next you need to edit `display_max`, `display_min`, `keyboard_max` and `keyboard_min` values in config file.
See [litd.conf](https://github.com/Hipuranyhou/litd/blob/master/litd.conf) for determining values of these settings.


### Auto start
Next edit `bin/litd-autostart` to point at your compiled executable, add `x` permission and **auto start it at boot**. 

**Make sure you keep the `-d` option in your path (to start litd in daemon mode.)** 

**Reboot and check that everything works**


## Usage
**litd cannot run as root (to get user idle time properly)**


### Flags

`-d` to run in daemon mode (`litd.pid` file located at the same location as [config file](#config-file))
```Shell
$ ./litd -d
```

`-v` to run in verbose mode (not allowed in daemon mode)
```Shell
$ ./litd -v
```


### Config file
litd creates its config file (and .pid file in daemon mode) in directory `litd/` which location is determined based on `$XDG_HOME_DIRS`. If this variable does not exist or is empty, then default path `$HOME/.config/` is used.

See [litd.conf](https://github.com/Hipuranyhou/litd/blob/master/litd.conf) file for more info.


### Signals
litd ignores sensor value for [reset](https://github.com/Hipuranyhou/litd/blob/master/litd.conf) seconds if you adjust brightness manually. It does this seperately for keyboard and display. You can reset this by sending `SIGUSR1` to litd.
<pre>
$ kill -SIGUSR1 <i>litd_pid</i>
</pre>

You can reload config by sending `SIGHUP` to litd.
<pre>
$ kill -SIGHUP <i>litd_pid</i>
</pre>
