CC = gcc
CFLAGS = -std=c99 -fopenmp
LDFLAGS = -lm
SRC = linear_equations.c
EXEC = linear_equations

all: $(EXEC)

$(EXEC): $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LDFLAGS)

clean:
	rm -f $(EXEC)

.PHONY: all clean