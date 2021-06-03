CC := clang++
CFLAGS := -std=c++11 -g

all: a.out

bin/utils.o: src/utils.cpp src/utils.hpp
	$(CC) -c src/utils.cpp -o bin/utils.o $(CFLAGS)

bin/TextEdit.o: src/TextEdit.cpp src/TextEdit.hpp
	$(CC) -c src/TextEdit.cpp -o bin/TextEdit.o $(CFLAGS)

bin/Defaults.o: src/Defaults.cpp src/Defaults.hpp
	$(CC) -c src/Defaults.cpp -o bin/Defaults.o $(CFLAGS)

bin/LoadSave.o: src/LoadSave.cpp src/LoadSave.hpp
	$(CC) -c src/LoadSave.cpp -o bin/LoadSave.o $(CFLAGS)

bin/CommandLine.o: src/CommandLine.cpp src/CommandLine.hpp
	$(CC) -c src/CommandLine.cpp -o bin/CommandLine.o $(CFLAGS)

bin/Selection.o: src/Selection.cpp src/Selection.hpp
	$(CC) -c src/Selection.cpp -o bin/Selection.o $(CFLAGS)

bin/Drawer.o: src/Drawer.cpp src/Drawer.hpp
	$(CC) -c src/Drawer.cpp -o bin/Drawer.o $(CFLAGS)

bin/SetParser.o: src/SetParser.cpp src/SetParser.hpp
	$(CC) -c src/SetParser.cpp -o bin/SetParser.o $(CFLAGS)

bin/CreateMode.o: src/CreateMode.cpp src/CreateMode.hpp src/utils.hpp
	$(CC) -c src/CreateMode.cpp -o bin/CreateMode.o $(CFLAGS)

bin/EditMode.o: src/EditMode.cpp src/EditMode.hpp src/utils.hpp
	$(CC) -c src/EditMode.cpp -o bin/EditMode.o $(CFLAGS)

bin/main.o: src/main.cpp src/utils.hpp src/CreateMode.hpp src/EditMode.hpp src/Drawer.hpp src/Defaults.hpp
	$(CC) -c src/main.cpp -o bin/main.o $(CFLAGS)

clean:
	rm bin/*

a.out: bin/main.o bin/utils.o bin/CreateMode.o bin/EditMode.o bin/Drawer.o bin/CommandLine.o bin/Selection.o bin/LoadSave.o bin/Defaults.o bin/SetParser.o bin/TextEdit.o
	$(CC) -lsfml-graphics -lsfml-window -lsfml-system bin/*.o -o bin/a.out -g

run: a.out
	./bin/a.out

