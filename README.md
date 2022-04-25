nd92132, SAI MADHAV KOLLURI

# Texture using GraphCut

#### Libraries Used:

- Used stb single-file public domain libraries for C/C++ for image loading and writing.
> File Names: `stb_image` and `stb_image_write` belong to this.

https://github.com/nothings/stb

#### Running the Code:
- To run the code use the below  command, which will compile the code and generate cut.exe and it runs cut.exe with default parameters of `graph_cut input.png output.png 512x512`, to use the default command copy and input image into this folder and and name the file as input.
```sh
make cut
```
- Alternatively, you after running the above command you can run the code with you own parameters as 
```sh
graph_cut input_image_path output_image_path output_resolution
```
> here input_image_path output_image_path are paths to images, or just name if they are already present in the directory
and  output_resolution is given as width x height.

- To clean the ouptut and run fresh use the command below for windows based machines. 
```sh
make cleanw
```
And for linux based machines. 
```sh
make cleanl
```
#### Understanding the output:
- The Output will be generated in this folder after running the commands above.
- There will be 6 Output files which are :-
-- ` output.png` :- The main ouput image generated.
-- `Overlapped.png` :- This image shows the overlapped portion for two consecutive patches which are pasted in the output canvas with random mentod, which is the default method used in this (for more info on random method please check the paper this project was based on).
-- `Segment.png` :- It shows us the mincut of the overlapped region (including the patch which needs to be taken from source).
-- `output.png` :- The main output image generated.
-- `Graph.txt` :- Here we print 3 Values of the graph generated for the overlapped region seen in `Overlapped.png` which are the the first vertex and the 2nd vertexe the value, here this second value can be -1 to signifty that we cannot traverse further (things further don't include in our min cut) and  and the edge weight which is the capacity between the two vertex.
-- `mincut.txt` :- Prints the coordiantes of the mincut.

#### References:
- https://www.cc.gatech.edu/cpl/projects/graphcuttextures/
- https://cs.brown.edu/courses/csci1950-g/asgn/proj3/
- https://cs.brown.edu/courses/csci1950-g/results/proj3/dkendall/
- https://stackoverflow.com/questions/13445688/how-to-generate-a-random-number-in-c
- https://github.com/mineshmathew/Image-Stitching-using-Graphcut
- https://github.com/MicrosoftDocs/cpp-docs/blob/main/docs/cpp/how-to-create-and-use-shared-ptr-instances.md
- https://github.com/LyricZhao/SeamCarving
- https://github.com/nothings/stb/blob/master/stb_image_write.h
