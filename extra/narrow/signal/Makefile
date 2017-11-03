all:
	gcc -c -O0 sleeper.c -o sleeper.o
	gcc -O0 -g sleeper.o signal2.c  -o signal -pthread
	gcc -O0 -g sleeper.o narrow.c  -o narrow -pthread
	gcc -DMSR_FP -O0 -g sleeper.o signal2.c  -o signalok -pthread
	gcc -DMSR_FP -O0 -g sleeper.o narrow.c  -o narrowok -pthread

clean:
	rm -fr signal sleeper.o signal2.o	
