SRCS = graph.cc main.cc
OBJS = graph.o main.o

CFLAGS = -Iinclude -Wall -O2 -g -lfmt

%.o: %.c
	g++ $(CFLAGS) -c $< -o $@

all: $(OBJS)
	g++ -lfmt $(OBJS) -o exp.x

clean:
	rm -f *.out *.o *.x *.dot *.ps