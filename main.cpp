#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include <queue>


using namespace std;



/*
struct edge_t {         //edge data structure
    int s , t , cost;
    edge_t(int s,int t,int cost):s(s),t(t),cost(cost){}
};

template<typename T>    //fucntinon to cacluate any square
T square (T t){
    return t*t;
}

int cost(const Vec3b &src1t,const cv::Vec3b &src2t,const cv::Vec3b &src1s,const cv::Vec3b &src2s) {
    int weight = 0;
    for(int c=0;c<3;++c){
        weight += square(int(src1t[c])-int(src2t[c])) + square(int(src1s[c])-int(src2s[c]));
    }
    return weight;
}



*/


int main(int argc, char *argv[]) {
  
  int w, h;

    cout << "Reading image" << argv[1] << endl;
    auto texture = make_shared<Image>(argv[1]);

    
    sscanf(argv[3], "%dx%d", &w, &h);
    auto canvas = make_shared<Canvas>(w, h);

    cout << "Begin to apply patches on canvas:" << endl;
    cut(canvas, texture);


    cout << "Output" << argv[2] << endl;
    canvas->write(argv[2]);

    return 0;


/*
//Rough idea space is from here


// here we provdie mask for both images, or one mask an invert of it and the image part outside the maks belongs to sink and for the other image part int the sink belongs to A and the rest of things could be considered ovelappin and the maxflow runs on to find min seam

//first matrix A is an m*n x m*n adjacency matrix
//cost metric M assigned to the directed edge between pixels s and t where s is the row index in A, t is the column index for A and M is the value at A(s,t)
//Entries in A are only added for pixels that are not under either source or target mask.



//second matrix T is an m*n x 2 matrix.
// Entries in this matrix represent the high cost constraints for each pixel under a mask and the neighboring pixels to the mask. Column one represents pixels constrained to the source image and column two contains pixels constrained to the target image.
;

// three vectors are maintained per matrix which are used to populate each matrix. Row and column vectors contain the index values for the matrix and a value vector that contains the entry of the matrix at row and column. 

M(s,t,A,B) = [A(s)-B(s)]* [A(s)-B(s)] + [A(t)-B(t)] *[A(t)-B(t)];

//bit wise and to get the final output after getting the mask ?

for ()
{
 find seam( graph1 >>graph2 );
  uto minCut = graph1+graph2.findMinCut();
  reaplce seam1 with image
}

*/


  return 0;
}