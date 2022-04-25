#include "Src/maxCut.h"

using namespace std;


    static void random(const std::shared_ptr<Canvas> &canvas, const std::shared_ptr<Image> &texture) {
        auto patch = std::make_shared<Patch>(texture, Random(0, texture->w - 1), Random(0, texture->h - 1));
        canvas->apply(patch);
    }

int main(int argc, char* argv[]) 
{
    if (argc != 4) {
        cout << "Give correct Arguments" << endl;
        exit(EXIT_SUCCESS);
    }

    cout << "Reading image from " << argv[1] << " ..." << endl; // Make a texture patch with input
    auto texture = make_shared<Image>(argv[1]);

    int w, h;
    sscanf(argv[3], "%dx%d", &w, &h);
    cout << "Making " << w << "x" << h << " canvas ..." << endl; // Making a blank output canvas
    auto canvas = make_shared<Canvas>(w, h);

    cout << "Begin to apply patches on canvas:" << endl;
     
     for (int y = 0; y < canvas->h; y += Random(texture->h / 3, texture->h * 2 / 3)) {
            for (int x = 0; x < canvas->w; x += Random(texture->w / 3, texture->w * 2 / 3)) {
                auto patch = std::make_shared<Patch>(texture, x, y);
                canvas->apply(patch);
            }
        }

    auto canvas2 = make_shared<Canvas>(w, h);
    cout << "Segement output:" << endl;
    int i=0;

        for (int y = 0; y < canvas2->h; y += Random(texture->h / 3, texture->h * 2 / 3)) {
            for (int x = 0; x < canvas2->w; x += Random(texture->w / 3, texture->w * 2 / 3)) {
                auto patch = std::make_shared<Patch>(texture, x, y);
                canvas2->applyseg(patch);

                if(i==1) break;
                
                i++;
            }
            if(i==1) break;

        }

    auto canvas3 = make_shared<Canvas>(w, h);
    cout << "Overlapped" << endl;
    i=0;
        for (int y = 0; y < canvas3->h; y += Random(texture->h / 3, texture->h * 2 / 3)) {
            for (int x = 0; x < canvas3->w; x += Random(texture->w / 3, texture->w * 2 / 3)) {
                auto patch = std::make_shared<Patch>(texture, x, y);
                canvas3->applyover(patch);
                if(i==1) break;
                
                i++;
            }
            if(i==1) break;
        }
    
    cout << "Writing result into " << argv[2] << " ..." << endl;
    canvas->write(argv[2]);

    string s="Segment.png";
    cout << "Writing result segment image"<<endl;
    canvas2->write(s);

    string s2="Overlapped.png";
    cout << "Writing result Overlapped image"<<endl;
    canvas3->write(s2);


    return 0;
}
