all: main.out producer.out consumer.out

main.out:	main.cpp graph.cpp graph.hpp
	g++ main.cpp graph.cpp -o main.out

producer.out:	producer.cpp graph.cpp graph.hpp
	g++ producer.cpp graph.cpp -o producer.out

consumer.out:	consumer.cpp graph.cpp graph.hpp
	g++ consumer.cpp graph.cpp -o consumer.out
clean:
	rm -f *.o *.out Consumer_*.txt
kill:
	killall -9 producer.out
	killall -9 consumer.out
	ipcrm -M 1235