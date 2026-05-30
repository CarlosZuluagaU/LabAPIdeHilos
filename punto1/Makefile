CC      = gcc
CFLAGS  = -Wall -O2
LDFLAGS = -lm

all: pi_s pi_p

pi_s: pi.c
	$(CC) $(CFLAGS) -o pi_s pi.c $(LDFLAGS)

pi_p: pi_p.c
	$(CC) $(CFLAGS) -o pi_p pi_p.c -lpthread $(LDFLAGS)

clean:
	rm -f pi_s pi_p
