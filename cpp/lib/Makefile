SRCS=$(wildcard *.c)
OBJS=$(SRCS:.c=.o) 

libtinyc.a: $(OBJS)
	$(AR) -cr $@ $^

clean:
	rm -f *.o *.a *.s

%.o: %.c
	$(CXX) $(CFLAGS) -I ../include -c -o $@ $<