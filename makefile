make: main.o 
	g++ main.o -o main

main.o : main.cpp 
	g++ -I/usr/include/eigen3/ -O3 -c main.cpp 


clean:
	rm *.o *.ppm *.exe output out
	 
