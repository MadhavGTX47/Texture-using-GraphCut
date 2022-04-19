cut: main.o 
	g++ main.o -o graph_cut
	graph_cut input.png output.png 1024x1024

main.o : main.cpp 
	g++ -O3 -c main.cpp

clean:
	rm *.o *.ppm *.exe output out
	 
