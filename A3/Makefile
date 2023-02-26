all: main.out producer.out consumer.out

main.out:	main.cpp graph.cpp graph.hpp
	g++ main.cpp graph.cpp -o main.out

create.out:	create.cpp graph.cpp graph.hpp
	g++ create.cpp graph.cpp -o create.out 

check.out:	check.cpp graph.cpp graph.hpp
	g++ check.cpp graph.cpp -o check.out 

producer.out:	producer.cpp graph.cpp graph.hpp
	g++ producer.cpp graph.cpp -o producer.out

consumer.out:	consumer.cpp graph.cpp graph.hpp
	g++ consumer.cpp graph.cpp -o consumer.out
clean:
	rm -f *.o *.out Consumer_*.txt
kill:
	killall -9 producer.out
	killall -9 consumer.out