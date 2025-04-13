CC := g++
CXXFLAGS := --std=c++11 -Wall -Iinclude
SRC := src/task_cli.cc src/task_handler.cc
OBJ := task_cli.o task_handler.o
EXE := task_cli.out

# Test json
TST_JSON_SRC := test/test_json.cc
TST_JSON_OBJ := test_json.o
TST_JSON_EXE := test_json.out

$(EXE): $(OBJ)
	$(CC) $(CXXFLAGS) -o $(EXE) $(OBJ)

$(OBJ): $(SRC)
	$(CC) $(CXXFLAGS) -c $(SRC)

test_json: $(TST_JSON_OBJ)
	$(CC) $(CXXFLAGS) -o $(TST_JSON_EXE) $(TST_JSON_OBJ)

$(TST_JSON_OBJ): $(TST_JSON_SRC)
	$(CC) $(CXXFLAGS) -c $(TST_JSON_SRC)

clean:
	rm -f $(OBJ) $(EXE) $(TST_JSON_OBJ) $(TST_JSON_EXE)

.PHONY: clean test_json
