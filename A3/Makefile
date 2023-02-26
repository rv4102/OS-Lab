all:	create.out check.out

create.out:	create.cpp graph.cpp graph.hpp
	g++ create.cpp graph.cpp -o create.out 

check.out:	check.cpp graph.cpp graph.hpp
	g++ check.cpp graph.cpp -o check.out 

producer.out:	producer.cpp graph.cpp graph.hpp
	g++ producer.cpp graph.cpp -o producer.out

clean:
	rm -f *.o *.out