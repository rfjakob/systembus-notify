CFLAGS += -Wall -Wextra -Wformat-security -Wconversion -fstack-protector-all -std=gnu99 -g

# If the user has libelogind installed, we can assume
# that they want to use libelogind instead of libsystemd.
$(shell pkg-config --exists libelogind 2> /dev/null)
ifeq ($(.SHELLSTATUS),0)
$(info libelogind is installed: using libelogind instead of libsystemd)
CFLAGS += $(shell pkg-config --cflags libelogind)
LDLIBS += $(shell pkg-config --libs libelogind)
else
LDLIBS += -lsystemd
endif

systembus-notify: *.c *.h Makefile
	$(CC) $(LDFLAGS) $(CFLAGS) -o systembus-notify *.c $(LDLIBS)

# Depends on compilation to make sure the syntax is ok.
format: systembus-notify
	clang-format --style=WebKit -i *.h *.c

.PHONY: install
install: systembus-notify
	mkdir -p ${HOME}/bin ${HOME}/.config/autostart
	cp systembus-notify ${HOME}/bin
	cp systembus-notify.desktop ${HOME}/.config/autostart

.PHONY: uninstall
uninstall:
	# delete in reverse order compared to install
	rm -f ${HOME}/.config/systemd/user/systembus-notify.service
	rm -f ${HOME}/.config/autostart/systembus-notify.desktop
	rm -f ${HOME}/bin/systembus-notify

.PHONY: test
test: systembus-notify
	cppcheck -q . || true
	./testmessage.sh
