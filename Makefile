CFLAGS += -Wall -Wextra -Wformat-security -Wconversion -fstack-protector-all -std=gnu99 -g

LIBSYSTEMD_PROVIDER=$(shell pkg-config --exists libelogind && printf "libelogind" || printf "libsystemd")
LIBSYSTEMD_PROVIDER_INCLUDE_DIR=$(shell { [ -n "$$(pkg-config --cflags-only-I $(LIBSYSTEMD_PROVIDER))" ] && pkg-config --cflags-only-I $(LIBSYSTEMD_PROVIDER); } || printf " ")
LIBSYSTEMD_PROVIDER_LDFLAGS=$(shell pkg-config --libs $(LIBSYSTEMD_PROVIDER))

systembus-notify: *.c *.h Makefile
	$(CC) $(LDFLAGS) $(CFLAGS) $(LIBSYSTEMD_PROVIDER_INCLUDE_DIR) -o systembus-notify *.c $(LIBSYSTEMD_PROVIDER_LDFLAGS)

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

.PHONY: clean
clean:
	rm -f systembus-notify
