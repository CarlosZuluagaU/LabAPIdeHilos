CC     = gcc
CFLAGS = -Wall -O2

all: fibonacci fibonacci_s

fibonacci: fibonacci.c
	$(CC) $(CFLAGS) -o fibonacci fibonacci.c -lpthread -lm

fibonacci_s: fibonacci_s.c
	$(CC) $(CFLAGS) -o fibonacci_s fibonacci_s.c -lm

clean:
	rm -f fibonacci fibonacci_s
