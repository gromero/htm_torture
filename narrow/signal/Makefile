all:
	gcc -c -O0 sleeper.c -o sleeper.o
	gcc -O0 -g sleeper.o signal2.c  -o signal -pthread

clean:
	rm -fr signal sleeper.o signal2.o	
