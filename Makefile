#compilador
CC = gcc

#flags do compilador
CFLAGS = -Wall -std=c99 -O3

# Sources
SOURCE = aedmaps.c auxiliar.c

# Objectos
OBJ = aedmaps.o auxiliar.o

aedmaps: $(OBJ)
	$(CC) -o $@ $(OBJ)

aedmaps.o: auxiliar.h

auxiliar.o: auxiliar.h

clean:
	rm -f *.o a.out aedmaps *~

depend:
	makedepend $(SOURCE)
