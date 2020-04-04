#!/bin/bash

set -eux

pkill -u "$USER" systembus-notif || true
# 15 character limit          ^^^

./systembus-notify &
sleep 0.1
dbus-send --system --type=signal / net.nuetzlich.SystemNotifications.Notify \
	"string:systembus-notify test $$"
dbus-send --system --type=signal / net.nuetzlich.SystemNotifications.Notify \
	"string:systembus-notify test $$ with body" "string:$(date)"
sleep 0.1
kill %1
