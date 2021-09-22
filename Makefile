main: main.o cmd.o utils.o config.o pwd.o name.o
	gcc -g main.o cmd.o utils.o config.o pwd.o name.o -o main

main.o: main.c config.h cmd.h
	gcc -g -c main.c

cmd.o: cmd.c config.h utils.h cmd.h
	gcc -g -c cmd.c

utils.o: utils.c config.h
	gcc -g -c utils.c

config.o: config.c config.h
	gcc -g -c config.c

config.h: pwd.h name.h

pwd.o: pwd.c
	gcc -g -c pwd.c

name.o: name.c
	gcc -g -c name.c

clean:
	rm -f *.o main