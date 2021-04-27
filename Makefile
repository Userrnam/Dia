all: a.out

bin/main.o: src/main.cpp
	clang++ -g -c src/main.cpp -o bin/main.o -std=c++11 -g

clean:
	rm bin/*

a.out: bin/main.o
	clang++ -lsfml-graphics -lsfml-window -lsfml-system bin/*.o -o bin/a.out -g

run: a.out
	./bin/a.out

