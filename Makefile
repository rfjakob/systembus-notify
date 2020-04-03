CFLAGS += -Wall -Wextra -Wformat-security -Wconversion -fstack-protector-all -std=gnu99 -lsystemd -g

system-notify: *.c *.h Makefile
	$(CC) $(LDFLAGS) $(CFLAGS) -o system-notify *.c

# Depends on compilation to make sure the syntax is ok.
format: system-notify
	clang-format --style=WebKit -i *.h *.c
