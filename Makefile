CC := g++
CXXFLAGS := --std=c++11 -Wall -Iinclude
SRC := src/task_cli.cc
OBJ := task_cli.o
EXE := task_cli.out

$(EXE): $(OBJ)
	$(CC) $(CXXFLAGS) -o $(EXE) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CXXFLAGS) -c $(SRC)

clean:
	rm -f $(OBJ) $(EXE)

.PHONY: clean
