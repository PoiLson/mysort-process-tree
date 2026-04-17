CC = gcc

program:
	$(CC) -o ./build/main ./src/coordinator.c ./src/voter.c  ./src/quicksort.c ./src/utilities.c
	$(CC) -o ./build/splitter ./src/splitter.c ./src/voter.c ./src/quicksort.c ./src/utilities.c
	$(CC) -o ./build/sorting1 ./src/sorting1.c ./src/voter.c ./src/bubblesort.c ./src/utilities.c
	$(CC) -o ./build/sorting2 ./src/sorting2.c ./src/voter.c ./src/quicksort.c ./src/utilities.c

execute:
	./build/main -i ./voterfiles/voters50.bin -k 4 -e1 ./build/sorting1 -e2 ./build/sorting2
