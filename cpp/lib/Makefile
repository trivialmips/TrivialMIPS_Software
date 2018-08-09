srcs=$(wildcard *.c)
objs=$(srcs:.c=.o) 

libtinyc.a: $(objs)
	$(AR) -cr $@ $^

clean:
	rm -f *.o *.a *.s

%.o: %.c
	${CROSS_COMPILE}gcc $(CFLAGS) -fno-pic -g -I ../include -c $< -nostdinc -nostdlib