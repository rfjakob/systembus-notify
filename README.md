systembus-notify - system bus notification daemon
=================================================

Tiny daemon that listens for `net.nuetzlich.SystemNotifications`
signals on the D-Bus **system** bus and shows them as
[desktop notifications](https://developer.gnome.org/notification-spec/)
using the **user** bus.

See https://github.com/rfjakob/earlyoom/issues/183 for the raison d'être.


Compile
-------

```
make
```

Test
----

Run `systembus-notify` (should stop with no output), then in another
terminal, run

```
dbus-send --system --type=signal / net.nuetzlich.SystemNotifications.Notify "string:hello world"
```

You should see "hello world" printed out by `systembus-notify` and also see "hello world"
as a desktop notification.


Dependencies
------------

systembus-notify uses the `sd-bus` D-Bus library that is part of `libsystemd`.
Install like this:

Debian / Ubuntu

```
apt install libsystemd-dev
```

Fedora

```
dnf install systemd-devel
```
