all: clean compile link 

compile:
	echo 'Compiling'
	g++ -std=c++11 -g -Wall -Weffc++ -c -o bin/Order.o src/Order.cpp
	g++ -std=c++11 -g -Wall -Weffc++ -c -o bin/Customer.o src/Customer.cpp
	g++ -std=c++11 -g -Wall -Weffc++ -c -o bin/Volunteer.o src/Volunteer.cpp
	g++ -std=c++11 -g -Wall -Weffc++ -c -o bin/Action.o src/Action.cpp
	g++ -std=c++11 -g -Wall -Weffc++ -c -o bin/WareHouse.o src/WareHouse.cpp
	g++ -std=c++11 -g -Wall -Weffc++ -c -o bin/main.o src/main.cpp

link:
	g++ -std=c++11 -g -Wall -Weffc++ -o	bin/wareHouse bin/*.o

clean:
	echo 'cleaning'
	find bin -type f -not -path 'bin/rest/*' -delete