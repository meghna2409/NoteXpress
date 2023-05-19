CC = gcc
CFLAGS = -Wall -pthread

all: note_app

note_app: note_app.c
	$(CC) $(CFLAGS) -o note_app note_app.c

clean:
	rm -f note_app
