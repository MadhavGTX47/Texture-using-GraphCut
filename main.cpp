#include <iostream>


#include "image.hpp"
#include "placer.hpp"
using namespace std;

int main(int argc, char* argv[]) 
{
    if (argc != 4) {
        cout << "Usage: graph_cut <input> <output> <canvas_size>" << endl;
        cout << "Example: graph_cut peas.png peas_output.png 512x512" << endl;
        exit(EXIT_SUCCESS);
    }

    cout << "Reading image from " << argv[1] << " ..." << endl;
    auto texture = make_shared<Image>(argv[1]);

    int w, h;
    sscanf(argv[3], "%dx%d", &w, &h);
    cout << "Making " << w << "x" << h << " canvas ..." << endl;
    auto canvas = make_shared<Canvas>(w, h);

    cout << "Begin to apply patches on canvas:" << endl;
    Placer::init(canvas, texture);

    auto canvas2 = make_shared<Canvas>(w, h);
    cout << "Segement output:" << endl;
    Placer::initseg(canvas2, texture);

    auto canvas3 = make_shared<Canvas>(w, h);
    cout << "Overlapped" << endl;
    Placer::initover(canvas3, texture);
    
    cout << "Writing result into " << argv[2] << " ..." << endl;
    canvas->write(argv[2]);

    string s="Segment.png";
    cout << "Writing result segment image"<<endl;
    canvas2->write(s);

    string s2="Overlapped.png";
    cout << "Writing result segment image"<<endl;
    canvas3->write(s2);


    return 0;
}
