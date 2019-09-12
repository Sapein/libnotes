.POSIX:
.PHONY: install remove

PROJECT_NAME=libnotes

CC = cc
CFLAGS = -Wall -Wpedantic -Wextra 
LDLIBS = -lm
LDFLAGS = -g
PREFIX = /usr/local


all: $(PROJECT_NAME)

$(PROJECT_NAME):
	$(CC) $(LDFLAGS) -o $(PROJECT_NAME) $< $(LDLIBS)

install: install_man
uninstall: remove
remove: remove_man

clean:
	rm *.o */*.o || true

fullclean: clean doxygen_clean

doxygen:
	doxygen libnotes/Doxyfile

doxygen_clean: 
	rm -rf doxy || true

install_man:
	cp libnotes/docs/*.3 $(PREFIX)/share/man/man3

remove_man:
	rm $(PREFIX)/share/man/man3/libnotes.3 || true
	rm $(PREFIX)/share/man/man3/Note_*.3 || true
