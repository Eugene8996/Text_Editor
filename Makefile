EXEC := exec

L_FLAGS := -lncurses -L.

all: $(patsubst %.c,%.o,$(wildcard *.c))
	gcc -g $^ -o $(EXEC) $(L_FLAGS)

%.o: %.c
	gcc -g -c $(wildcard *.c) $(L_FLAGS)

clean:
	rm -f *.o $(EXEC)

# -g -Wall -Werror


