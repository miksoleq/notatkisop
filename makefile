CC=gcc
CFLAGS=-Wall -fsanitize=address,undefined
LDFLAGS=-fsanitize=address,undefined

all: prog1 prog2 prog3 prog4 prog5 prog6 prog7

prog1: prog1.c
		gcc -Wall -fsanitize=address,undefined -o prog1 prog1.c

prog2: prog2.c
		gcc -Wall -fsanitize=address,undefined -o prog2 prog2.c

prog3: prog3.c
		gcc -Wall -fsanitize=address,undefined -o prog3 prog3.c

prog4: prog4.c
		${CC} ${CFLAGS} -o prog4 prog4.c

prog5: prog5.c
		${CC} ${CFLAGS} -o prog5 prog5.c

prog6: prog6.c
		${CC} ${CFLAGS} -o prog6 prog6.c		

prog7: prog7.c
		${CC} ${CFLAGS} -o prog7 prog7.c	

prog8: prog8.c
		${CC} ${CFLAGS} -o prog8 prog8.c

.PHONY: clean

clean:
		-rm -f prog1 prog2 prog3 prog4 prog5 prog6 prog7 prog8