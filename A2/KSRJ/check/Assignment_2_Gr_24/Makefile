CC=g++
FLAGS= -std=c++14

SRKJ:  main.o signal_handlers.o pipeline.o commands.o utils.o 
	$(CC) $(FLAGS)  main.o signal_handlers.o pipeline.o commands.o utils.o  -o SKRJ

signal_handlers.o: signal_handlers.cpp signal_handlers.h
	$(CC) $(FLAGS) -c signal_handlers.cpp
pipeline.o: pipeline.cpp pipeline.h
	$(CC) $(FLAGS) -c pipeline.cpp
commands.o: commands.cpp commands.h
	$(CC) $(FLAGS) -c commands.cpp
utils.o: utils.cpp utils.h
	$(CC) $(FLAGS) -c utils.cpp
main.o: main.cpp pipeline.h commands.h signal_handlers.h
	$(CC) $(FLAGS) -c main.cpp
clean : 
	rm -rf *.o *.out SKRJ