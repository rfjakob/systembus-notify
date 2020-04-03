CFLAGS += -Wall -Wextra -Wformat-security -Wconversion -fstack-protector-all -std=gnu99 -lsystemd -g

systembus-notify: *.c *.h Makefile
	$(CC) $(LDFLAGS) $(CFLAGS) -o systembus-notify *.c

# Depends on compilation to make sure the syntax is ok.
format: systembus-notify
	clang-format --style=WebKit -i *.h *.c
