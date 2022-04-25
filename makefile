cut: main.o 
	g++ main.o -o graph_cut
	graph_cut input.png output.png 512x512

main.o : main.cpp 
	g++ -O3 -c main.cpp

cleanw: 
	del *.o *.ppm *.exe output.png Overlapped.png Segment.png Graph.txt mincut.txt out 
	 
cleanl: 
	rm *.o *.ppm *.exe output.png Overlapped.png Segment.png Graph.txt mincut.txt out 