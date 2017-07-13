%.o:    %.c
	gcc -c -g -O0 $< -o $@

%.S:    %.c
	gcc -S -g -O0 $< -o $@

all:	bad_kernel_stack_pointer.o
	gcc -pthread -mhtm -mcpu=power8 -g -O0 bad_kernel_stack_pointer.o -o bad_kernel_stack_pointer

clean:
	rm -fr *.o
	rm -fr bad_kernel_stack_pointer
