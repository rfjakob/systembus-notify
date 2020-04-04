systembus-notify - system bus notification daemon
=================================================

Tiny daemon that listens for `net.nuetzlich.SystemNotifications`
signals on the D-Bus **system** bus and shows them as
[desktop notifications](https://developer.gnome.org/notification-spec/)
using the **user** bus.

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

You should see "systembus-notify test message" as a desktop notification.


Dependencies
------------

systembus-notify uses the `sd-bus` D-Bus library that is part of `libsystemd`.
Install it like this:

Debian / Ubuntu

```
apt install libsystemd-dev
```

Fedora

```
dnf install systemd-devel
```
