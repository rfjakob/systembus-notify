#!/bin/bash

set -eux

./systembus-notify &
sleep 0.1
dbus-send --system --type=signal / net.nuetzlich.SystemNotifications.Notify \
	"string:systembus-notify test $$"
dbus-send --system --type=signal / net.nuetzlich.SystemNotifications.Notify \
	"string:systembus-notify test $$ with body" "string:$(date)"
dbus-send --system --type=signal / net.nuetzlich.SystemNotifications.Notify \
	"string:systembus-notify test $$ with body, app name, icon and timeout" "string:$(date)" "string:my app" "uint32:0" "string:error" "int32:2000"
sleep 0.1
kill %1
