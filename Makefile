EXEC := exec
SRC := main.c
SRC_OBJ := main.o

L_FLAGS := -lncurses

all: 
	gcc -std=c99 -g -Wall -Werror -c $(SRC) -o $(SRC_OBJ)
	gcc -std=c99 -g $(SRC_OBJ) -o $(EXEC) $(L_FLAGS)

clean:
	rm -f $(EXEC) $(SRC_OBJ)