.POSIX:
.SUFFIXES:

CFLAGS = -O2 -g
all: main
SDL_FLAGS = `pkg-config --libs --cflags sdl`
0 = main.c
main: $0
	$(CC) $0 $(CFLAGS) $(SDL_FLAGS) -lm $(LDFLAGS) -o $@
clean:
	-rm main
