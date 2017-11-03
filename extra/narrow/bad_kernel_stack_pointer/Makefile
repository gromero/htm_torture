%.o:    %.c
	gcc -c -g -O0 $< -o $@

%.S:    %.c
	gcc -S -g -O0 $< -o $@

all:	bad_kernel_stack_pointer.o single.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 bad_kernel_stack_pointer.o -o bad_kernel_stack_pointer
	gcc -pthread -mhtm -mcpu=power8 -g -O0 single.o -o single

clean:
	rm -fr *.o
	rm -fr bad_kernel_stack_pointer
	rm -fr single
