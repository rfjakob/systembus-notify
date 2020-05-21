CFLAGS += -Wall -Wextra -Wformat-security -Wconversion -fstack-protector-all -std=gnu99 -g

systembus-notify: *.c *.h Makefile
	$(CC) $(LDFLAGS) $(CFLAGS) -o systembus-notify *.c -lsystemd

# Depends on compilation to make sure the syntax is ok.
format: systembus-notify
	clang-format --style=WebKit -i *.h *.c

.PHONY: install
install: systembus-notify
	mkdir -p ${HOME}/bin ${HOME}/.config/autostart ${HOME}/.config/systemd/user
	cp systembus-notify ${HOME}/bin
	cp systembus-notify.desktop ${HOME}/.config/autostart
	cp systembus-notify.service ${HOME}/.config/systemd/user

.PHONY: test
test: systembus-notify
	cppcheck -q . || true
	./testmessage.sh
