
#extras = -std=c++11 -O3
extras = -std=c++0x -g
#extras = -std=gnu++11 -O3
#compiler = clang++ -stdlib=libstdc++
compiler = g++  
CC = $(compiler) $(extras)

debug: CC += -DDEBUG -g

test: VoseUtil.o VoseTree.o Vose.o test.o
	$(CC) VoseUtil.o VoseTree.o Vose.o test.o -o test

main: VoseUtil.o VoseTree.o Vose.o main.o
	$(CC) VoseUtil.o VoseTree.o Vose.o main.o -o main

test.o: test.cc
	$(CC) -c test.cc

main.o: main.cc
	$(CC) -c main.cc

Vose.o: Vose.cc
	$(CC) -c Vose.cc

VoseTree.o: VoseTree.cc
	$(CC) -c VoseTree.cc

VoseUtil.o: VoseUtil.cc
	$(CC) -c VoseUtil.cc

clean:
	rm -f *.o
	rm -f main
	rm -f test
