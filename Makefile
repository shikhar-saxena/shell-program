main: main.o exec.o cmd.o utils.o config.o pwd.o name.o
	gcc -g main.o exec.o cmd.o utils.o config.o pwd.o name.o -o main

main.o: main.c config.h exec.h headers.h
	gcc -g -c main.c

exec.o: exec.c cmd.h exec.h headers.h
	gcc -g -c exec.c

cmd.o: cmd.c config.h utils.h headers.h
	gcc -g -c cmd.c

utils.o: utils.c config.h headers.h
	gcc -g -c utils.c

config.o: config.c config.h headers.h
	gcc -g -c config.c

config.h: pwd.h name.h

pwd.o: pwd.c headers.h
	gcc -g -c pwd.c

name.o: name.c headers.h
	gcc -g -c name.c

clean:
	rm -f *.o main