CC=g++
FLAGS= -std=c++14

SRKJ:  main.o signal_handlers.o pipeline.o commands.o utils.o 
	$(CC) $(FLAGS)  main.o signal_handlers.o pipeline.o commands.o utils.o  -o SKRJ

signal_handlers.o: signal_handlers.cpp signal_handlers.hpp
	$(CC) $(FLAGS) -c signal_handlers.cpp
pipeline.o: pipeline.cpp pipeline.hpp
	$(CC) $(FLAGS) -c pipeline.cpp
commands.o: commands.cpp commands.hpp
	$(CC) $(FLAGS) -c commands.cpp
utils.o: utils.cpp utils.hpp
	$(CC) $(FLAGS) -c utils.cpp
main.o: main.cpp pipeline.hpp commands.hpp signal_handlers.hpp
	$(CC) $(FLAGS) -c main.cpp
clean : 
	rm -rf *.o *.out SRKJ