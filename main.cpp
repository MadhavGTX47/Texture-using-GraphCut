#define cimg_display 0
#include <iostream>
#include <string>
#include <vector>
#include <sstream>
#include "CImg.h"
#include <Eigen/Dense>
using namespace cimg_library;
using namespace std;


int main(int argc, char * argv[]) {

    char * input_image = argv[1]; //storing input image name    
    char * input_image2 = argv[2]; //storing input image2 name  
    char * output_image = argv[3]; //storing output image name
    int output_width = atoi(argv[4]); //storing output image width    
    int output_height = atoi(argv[5]); //Storing output image height

    CImg < double > input(argv[1]); //Stub code for converting input image to LAB color
    CImg < double > lab = input.RGBtoLab();
    Vector3d *image = new Vector3d[input.width() * input.height()];
    for (unsigned int i = 0; i < input.width(); i++) {
        for (unsigned int j = 0; j < input.height(); j++) {
            image[i * input.height() + j][0] = lab(i, j, 0);
            image[i * input.height() + j][1] = lab(i, j, 1);
            image[i * input.height() + j][2] = lab(i, j, 2);
        }
    }

//Code is from here


// here we provdie mask for both images, or one mask an invert of it and the image part outside the maks belongs to sink and for the other image part int the sink belongs to A and the rest of things could be considered ovelappin and the maxflow runs on to find min seam

//first matrix A is an m*n x m*n adjacency matrix
//cost metric M assigned to the directed edge between pixels s and t where s is the row index in A, t is the column index for A and M is the value at A(s,t)
//Entries in A are only added for pixels that are not under either source or target mask.
auto graph1 = graphBuilder.build(
            texels, patch, patchBeg, overlapSize, patchHistory);


//second matrix T is an m*n x 2 matrix.
// Entries in this matrix represent the high cost constraints for each pixel under a mask and the neighboring pixels to the mask. Column one represents pixels constrained to the source image and column two contains pixels constrained to the target image.
auto graph2 = graphBuilder.build(
            texels, patch, patchBeg, overlapSize, patchHistory);

// three vectors are maintained per matrix which are used to populate each matrix. Row and column vectors contain the index values for the matrix and a value vector that contains the entry of the matrix at row and column. 

M(s,t,A,B) = [A(s)-B(s)]* [A(s)-B(s)] + [A(t)-B(t)] *[A(t)-B(t)];

caledge(Vector3d v1, vector3d v2 ,Vector3d v3, vector3d v4)
 {
   return v1-v3 * v1-v3 + v2-v4 * v2-v4;
 }



//bit wise and to get the final output after getting the mask

for ()
{
 find seam( graph1 >>graph2 );
  uto minCut = graph1+graph2.findMinCut();
  reaplce seam1 with image
}























CImg < double > input(argv[2]); //Stub code for converting input image2 to LAB color
    CImg < double > lab = input.RGBtoLab();
    Vector3d *image2 = new Vector3d[input.width() * input.height()];
    for (unsigned int i = 0; i < input.width(); i++) {
        for (unsigned int j = 0; j < input.height(); j++) {
            image2[i * input.height() + j][0] = lab(i, j, 0);
            image2[i * input.height() + j][1] = lab(i, j, 1);
            image2[i * input.height() + j][2] = lab(i, j, 2);
        }
    }

    
    int width = input.width();
    int height = input.height();


















    for (; width > output_width; width--) {
        image = delseam(image, width, input.height());
    }






  CImg<double> output(atoi(argv[4]), atoi(argv[5]), input.depth(), input.spectrum(), 0); 
  for (unsigned int i=0; i<output.width(); i++) {
	for (unsigned int j=0; j<output.height(); j++) {
	  output(i, j, 0) = image[i*output.height()+j][0];
	  output(i, j, 1) = image[i*output.height()+j][1];
	  output(i, j, 2) = image[i*output.height()+j][2];
	}
  }

  CImg<double> rgb = output.LabtoRGB();
  if (strstr(argv[3], "png"))
	rgb.save_png(argv[3]);
  else if (strstr(argv[3], "jpg"))
	rgb.save_jpeg(argv[3]);

  delete [] image;
  return 0;
}
