systembus-notify - system bus notification daemon
=================================================

[![C/C++ CI](https://github.com/rfjakob/systembus-notify/workflows/C/C++%20CI/badge.svg)](https://github.com/rfjakob/systembus-notify/actions)

Tiny daemon that listens for `net.nuetzlich.SystemNotifications.Notify`
signals on the D-Bus **system** bus and shows them as
[desktop notifications](https://developer.gnome.org/notification-spec/)
using the **user** bus. Works on Linux with Gnome, Cinnamon, KDE, LXQT, ...

![Bus Diagram](bus-diagram.svg)

See https://github.com/rfjakob/earlyoom/issues/183 for the raison d'être.


Compile
-------

Just run

```
make
```

If you want `systembus-notify` to automatically start when you log in,
also run (no sudo required):

```
make install
```

Test
----

Just run

```
make test
```

You should see "systembus-notify test" as a desktop notification.

Or manually:

```
./systembus-notify &
dbus-send --system / net.nuetzlich.SystemNotifications.Notify 'string:summary text only'
dbus-send --system / net.nuetzlich.SystemNotifications.Notify 'string:summary text' 'string:and body text'
```

Dependencies
------------

systembus-notify uses the `sd-bus` D-Bus library that is part of `libsystemd`.
Install it like this:

Debian / Ubuntu

```
sudo apt install libsystemd-dev
```

Fedora

```
sudo dnf install systemd-devel
```

Send notifications from your service
------------------------------------

The simplest way is to call the `dbus-send` tool.

Shell:
```sh
dbus-send --system / net.nuetzlich.SystemNotifications.Notify "string:hello world"

```

C (don't use `system()`, it's insecure!):
```C
int pid = fork();
if (pid == 0)
	execl("/usr/bin/dbus-send", "dbus-send", "--system", "/", "net.nuetzlich.SystemNotifications.Notify", "string:hello world", NULL);
```

You can also use libdbus or another d-bus library to send the signal.
In any case, systembus-notify must be running for the notification to pop up.

See Also
--------

Original idea is from https://wiki.debianforum.de/Desktop-Notification_von_Systemservice_mittels_dbus (German).

Similar thing implemented in Python: https://github.com/xundeenergie/system-notification
