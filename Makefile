all: a.out

bin/utils.o: src/utils.cpp src/utils.hpp
	clang++ -g -c src/utils.cpp -o bin/utils.o -std=c++11 -g

bin/Defaults.o: src/Defaults.cpp src/Defaults.hpp
	clang++ -g -c src/Defaults.cpp -o bin/Defaults.o -std=c++11 -g

bin/LoadSave.o: src/LoadSave.cpp src/LoadSave.hpp
	clang++ -g -c src/LoadSave.cpp -o bin/LoadSave.o -std=c++11 -g

bin/CommandLine.o: src/CommandLine.cpp src/CommandLine.hpp
	clang++ -g -c src/CommandLine.cpp -o bin/CommandLine.o -std=c++11 -g

bin/Selection.o: src/Selection.cpp src/Selection.hpp
	clang++ -g -c src/Selection.cpp -o bin/Selection.o -std=c++11 -g

bin/Drawer.o: src/Drawer.cpp src/Drawer.hpp
	clang++ -g -c src/Drawer.cpp -o bin/Drawer.o -std=c++11 -g

bin/SetParser.o: src/SetParser.cpp src/SetParser.hpp
	clang++ -g -c src/SetParser.cpp -o bin/SetParser.o -std=c++11 -g

bin/CreateMode.o: src/CreateMode.cpp src/CreateMode.hpp src/utils.hpp
	clang++ -g -c src/CreateMode.cpp -o bin/CreateMode.o -std=c++11 -g

bin/EditMode.o: src/EditMode.cpp src/EditMode.hpp src/utils.hpp
	clang++ -g -c src/EditMode.cpp -o bin/EditMode.o -std=c++11 -g

bin/main.o: src/main.cpp src/utils.hpp src/CreateMode.hpp src/EditMode.hpp src/Drawer.hpp src/Defaults.hpp
	clang++ -g -c src/main.cpp -o bin/main.o -std=c++11 -g

clean:
	rm bin/*

a.out: bin/main.o bin/utils.o bin/CreateMode.o bin/EditMode.o bin/Drawer.o bin/CommandLine.o bin/Selection.o bin/LoadSave.o bin/Defaults.o bin/SetParser.o
	clang++ -lsfml-graphics -lsfml-window -lsfml-system bin/*.o -o bin/a.out -g

run: a.out
	./bin/a.out

