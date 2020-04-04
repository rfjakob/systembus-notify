CFLAGS += -Wall -Wextra -Wformat-security -Wconversion -fstack-protector-all -std=gnu99 -lsystemd -g

systembus-notify: *.c *.h Makefile
	$(CC) $(LDFLAGS) $(CFLAGS) -o systembus-notify *.c

# Depends on compilation to make sure the syntax is ok.
format: systembus-notify
	clang-format --style=WebKit -i *.h *.c

install: systembus-notify
	mkdir -p ${HOME}/bin ${HOME}/.config/autostart
	cp systembus-notify ${HOME}/bin
	cp systembus-notify.desktop ${HOME}/.config/autostart

test: systembus-notify
	./testmessage.sh
