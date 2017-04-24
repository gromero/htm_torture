all:
	gcc -g worker.c x.c workload.c threads.c -lpthread
