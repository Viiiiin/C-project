CC = gcc
CTAG = -Wall -Wextra -O3

all: monoprocesso multiprocesso

monoprocesso: Monoprocesso.c
	$(CC) $(CTAG) Monoprocesso.c -o monoprocesso.out

multiprocesso: multiprocesso.c
	$(CC) $(CTAG) Multiprocesso.c -o multiprocesso.out

clean:
	rm -f *.out monoprocesso multiprocesso
