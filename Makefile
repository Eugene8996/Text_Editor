EXEC := exec
SRC := main.c
SRC_OBJ := main.o

L_FLAGS := -lncurses

all: 
	gcc -std=c99 -c $(SRC) -o $(SRC_OBJ)
	gcc -std=c99 $(SRC_OBJ) -o $(EXEC) $(L_FLAGS)

clean:
	rm -f $(EXEC) $(SRC_OBJ)

# -g -Wall -Werror