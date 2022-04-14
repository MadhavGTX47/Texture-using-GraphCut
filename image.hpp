#pragma once
#include <cmath>
#include <complex>
#include <vector>
#include <iostream>
#include <fstream>
#include "stb_image.h"
#include "stb_image_write.h"

#include "graph.hpp"

using namespace std;

#pragma pack()
struct Pixel {
    uint8_t r, g, b;

    Pixel(uint8_t r, uint8_t g, uint8_t b): r(r), g(g), b(b) {}

   inline uint64_t sqr_sum() const {
        uint64_t u_r = r, u_g = g, u_b = b;
        return u_r * u_r + u_g * u_g + u_b * u_b;
    }

   int distance(const Pixel &pixel) const {
        int r_d = static_cast<int> (r) - pixel.r;
        int g_d = static_cast<int> (g) - pixel.g;
        int b_d = static_cast<int> (b) - pixel.b;
        return sqrt(r_d * r_d + g_d * g_d + b_d * b_d);
    }

    int sqr_distance(const Pixel &pixel) const {
        int r_d = static_cast<int> (r) - pixel.r;
        int g_d = static_cast<int> (g) - pixel.g;
        int b_d = static_cast<int> (b) - pixel.b;
        return r_d * r_d + g_d * g_d + b_d * b_d;
    }
};



class Image {
private:
    bool from_stbi;

public:
    int w = 0, h = 0;
    Pixel *data = nullptr;

    explicit Image(const string &path) {
        from_stbi = true;
        int c;
        data = reinterpret_cast<Pixel*> (stbi_load(path.c_str(), &w, &h, &c, 3));
        if (not data) {
            cerr << "Unable to load image from " << path << endl;
            exit(EXIT_FAILURE);
        }
    }

    Image(int w, int h): w(w), h(h) {
        from_stbi = false;
        data = static_cast<Pixel*> (malloc(w * h * sizeof(Pixel)));
    }

    ~Image() {
        if (data) {
            from_stbi ? stbi_image_free(data) : free(data);
            data = nullptr;
        }
    }

     uint64_t variance() const {
        uint64_t r = 0, g = 0, b = 0;
        for (int i = 0; i < w * h; ++ i) {
            r += data[i].r, g += data[i].g, b += data[i].b;
        }
        r /= w * h, g /= w * h, b /= w * h;
        uint64_t var = 0;
        auto sqr = [](uint64_t x) {
            return x * x;
        };
        for (int i = 0; i < w * h; ++ i) {
            var += sqr(max<uint64_t>(r, data[i].r) - min<uint64_t>(r, data[i].r));
            var += sqr(max<uint64_t>(g, data[i].g) - min<uint64_t>(g, data[i].g));
            var += sqr(max<uint64_t>(b, data[i].b) - min<uint64_t>(b, data[i].b));
        }
        return var / (w * h);
    }

    void write(const string &path) const {
       
        if (not stbi_write_png(path.c_str(), w, h, 3, reinterpret_cast<uint8_t*>(data), 0)) {
            cerr << "Unable to write image to " << path << endl;
            exit(EXIT_FAILURE);
        }
    }

    inline void set(int x, int y, const Pixel &pixel) const {
        data[y * w + x] = pixel;
    }

     inline Pixel pixel(int x, int y) const {
        return data[y * w + x];
    }

    shared_ptr<Image> flip() const {
        auto flipped = make_shared<Image>(w, h);
        for (int y = 0, index = 0; y < h; ++ y) {
            for (int x = 0; x < w; ++ x, ++ index) {
                flipped->set(w - x - 1, h - y - 1, data[index]);
            }
        }
        return flipped;
    }
};


class Patch {
public:
    int x, y;
    shared_ptr<Image> image;

    Patch(const shared_ptr<Image> &image, int x, int y): x(x), y(y), image(image) {}

     inline int x_end() const {
        return x + image->w;
    }

    inline int y_end() const {
        return y + image->h;
    }
    
    inline Pixel pixel(int a, int b) const {
        return image->pixel(a - x, b - y);
    }

     inline bool in_range(int a, int b) const {
        return 0 <= a - x and a - x < image->w and 0 <= b - y and b - y < image->h;
    }
};


class Canvas: public Image {
private:
    vector<shared_ptr<Patch>> origin;

public:
    Canvas(int w, int h): Image(w, h), origin(w * h) {}

     bool none_empty() const {
        for (int i = 0; i < w * h; ++ i) {
            if (not origin[i]) {
                return false;
            }
        }
        return true;
    }

     inline bool in_range(int x, int y) const {
        return 0 <= x and x < w and 0 <= y and y < h;
    }

     uint64_t ssd(const shared_ptr<Patch> &patch, int canvas_x=-1, int canvas_y=-1, int sub_patch_w=-1, int sub_patch_h=-1) const {
        int x_begin = max(patch->x, 0);
        int y_begin = max(patch->y, 0);
        int x_end = min(patch->x_end(), w);
        int y_end = min(patch->y_end(), h);
        if (canvas_x != -1 and canvas_y != -1 and sub_patch_w != -1 and sub_patch_h != -1) {
            x_begin = canvas_x, y_begin = canvas_y;
            x_end = min(x_end, x_begin + sub_patch_w), y_end = min(y_end, y_begin + sub_patch_h);
        }

        int overlapped = 0;
        uint64_t ssd = 0;
        for (int y = y_begin; y < y_end; ++ y) {
            for (int x = x_begin; x < x_end; ++ x) {
                int index = y * w + x;
                if (origin[index]) {
                    ssd += data[index].sqr_distance(patch->pixel(x, y));
                    ++ overlapped;
                }
            }
        }
        return ssd / overlapped;
    }

    void apply(const shared_ptr<Patch> &patch) {
        cout << " > Applying a new patch at (" << patch->x << ", " << patch->y << ")" << endl;
        int x_begin = max(patch->x, 0);
        int y_begin = max(patch->y, 0);
        int x_end = min(patch->x_end(), w);
        int y_end = min(patch->y_end(), h);

        // Fill non-overlapped area first
        static int dx[4] = { 0, +1,  0, -1};
        static int dy[4] = {+1,  0, -1,  0};
        int n_old_seam_nodes = 0;
        vector<pair<int, int>> overlapped;
        vector<int> overlapped_index(w * h, -1);

        for (int y = y_begin; y < y_end; ++ y) {
            for (int x = x_begin; x < x_end; ++ x) {
                int index = y * w + x;
                
                if (not origin[index]) {
                    origin[index] = patch;
                    data[index] = patch->pixel(x, y);
                } 
                
                else {
                    overlapped_index[index] = overlapped.size();
                    overlapped.emplace_back(x, y);
                    
                    for (int d = 0; d < 2; ++ d) {
                        int a = x + dx[d], b = y + dy[d];
                        int neighbor_index = b * w + a;
                        
                        if (in_range(a, b) and origin[neighbor_index] and origin[neighbor_index] != origin[index]) {
                            ++ n_old_seam_nodes;
                        }
                    }
                }
            }
        }

        // Build graph
        Graph graph(overlapped.size() + n_old_seam_nodes + 2);
        int s = overlapped.size() + n_old_seam_nodes, t = overlapped.size() + n_old_seam_nodes + 1;
        int old_sean_node_index = overlapped.size();
        for (int i = 0; i < overlapped.size(); ++ i) {

            auto xy = overlapped[i];
            int index = xy.second * w + xy.first;
            int m_s = pixel(xy.first, xy.second).distance(patch->pixel(xy.first, xy.second));
            for (int d = 0; d < 4; ++ d) {
                int a = xy.first + dx[d], b = xy.second + dy[d];
                int neighbor_index = b * w + a;
                
                if (in_range(a, b) and origin[neighbor_index]) {
                    
                    if (origin[neighbor_index] == patch) {
                        graph.add_edge(i, t, Graph::inf_flow);
                    } 
                    
                    else {
                        
                        if (overlapped_index[neighbor_index] == -1) {
                            graph.add_edge(s, i, Graph::inf_flow);
                        } 
                        
                        else if (d < 2) { // `add_edge` is bi-directional
                            
                            if (origin[index] != origin[neighbor_index] and origin[index]->in_range(a, b) and
                                origin[neighbor_index]->in_range(a, b)) { // Old seam node
                                int m_t = data[neighbor_index].distance(patch->pixel(a, b));
                                graph.add_edge(old_sean_node_index, i, m_s + m_t);
                                graph.add_edge(old_sean_node_index, overlapped_index[neighbor_index], m_s + m_t);
                                int old_m_s = origin[index]->pixel(xy.first, xy.second).distance(origin[neighbor_index]->pixel(xy.first, xy.second));
                                int old_m_t = origin[index]->pixel(a, b).distance(origin[neighbor_index]->pixel(a, b));
                                graph.add_edge(old_sean_node_index, t, old_m_s + old_m_t);
                                ++ old_sean_node_index;
                            } 
                            
                            else {
                                int m_t = data[neighbor_index].distance(patch->pixel(a, b));
                                graph.add_edge(i, overlapped_index[neighbor_index], m_s + m_t);
                            }
                        }
                    }
                }
            }
        }
        
      
        
        auto decisions = graph.min_cut(s, t);
        

        for (int i = 0; i < overlapped.size(); ++ i) {
            
            if (decisions[i]) { // Belongs to the new patch
                auto xy = overlapped[i];
                int index = xy.second * w + xy.first;
                  origin[index] = patch;
                
              
           data[index] = patch->pixel(xy.first, xy.second); 
               // Pixel meow(255,255,0); 
               // data[index]=meow; // overlapping code
            }

          
        }

    }

void applyseg(const shared_ptr<Patch> &patch) {
        cout << " > Applying a new patch at (" << patch->x << ", " << patch->y << ")" << endl;
        int x_begin = max(patch->x, 0);
        int y_begin = max(patch->y, 0);
        int x_end = min(patch->x_end(), w);
        int y_end = min(patch->y_end(), h);

        // Fill non-overlapped area first
        static int dx[4] = { 0, +1,  0, -1};
        static int dy[4] = {+1,  0, -1,  0};
        int n_old_seam_nodes = 0;
        vector<pair<int, int>>   overlapped;
        vector<int> overlapped_index(w * h, -1);

        for (int y = y_begin; y < y_end; ++ y) {
            for (int x = x_begin; x < x_end; ++ x) {
                int index = y * w + x;
                
                if (not origin[index]) {
                    origin[index] = patch;
                    data[index] = patch->pixel(x, y);
                } 
                
                else {
                    overlapped_index[index] = overlapped.size();
                    overlapped.emplace_back(x, y);
                    
                    for (int d = 0; d < 2; ++ d) {
                        int a = x + dx[d], b = y + dy[d];
                        int neighbor_index = b * w + a;
                        
                        if (in_range(a, b) and origin[neighbor_index] and origin[neighbor_index] != origin[index]) {
                            ++ n_old_seam_nodes;
                        }
                    }
                }
            }
        }

        // Build graph
        Graph graph(overlapped.size() + n_old_seam_nodes + 2);
        int s = overlapped.size() + n_old_seam_nodes, t = overlapped.size() + n_old_seam_nodes + 1;
        int old_sean_node_index = overlapped.size();
        for (int i = 0; i < overlapped.size(); ++ i) {

            auto xy = overlapped[i];
            int index = xy.second * w + xy.first;
            int m_s = pixel(xy.first, xy.second).distance(patch->pixel(xy.first, xy.second));
            for (int d = 0; d < 4; ++ d) {
                int a = xy.first + dx[d], b = xy.second + dy[d];
                int neighbor_index = b * w + a;
                
                if (in_range(a, b) and origin[neighbor_index]) {
                    
                    if (origin[neighbor_index] == patch) {
                        graph.add_edge(i, t, Graph::inf_flow);
                    } 
                    
                    else {
                        
                        if (overlapped_index[neighbor_index] == -1) {
                            graph.add_edge(s, i, Graph::inf_flow);
                        } 
                        
                        else if (d < 2) { // `add_edge` is bi-directional
                            
                            if (origin[index] != origin[neighbor_index] and origin[index]->in_range(a, b) and
                                origin[neighbor_index]->in_range(a, b)) { // Old seam node
                                int m_t = data[neighbor_index].distance(patch->pixel(a, b));
                                graph.add_edge(old_sean_node_index, i, m_s + m_t);
                                graph.add_edge(old_sean_node_index, overlapped_index[neighbor_index], m_s + m_t);
                                int old_m_s = origin[index]->pixel(xy.first, xy.second).distance(origin[neighbor_index]->pixel(xy.first, xy.second));
                                int old_m_t = origin[index]->pixel(a, b).distance(origin[neighbor_index]->pixel(a, b));
                                graph.add_edge(old_sean_node_index, t, old_m_s + old_m_t);
                                ++ old_sean_node_index;
                            } 
                            
                            else {
                                int m_t = data[neighbor_index].distance(patch->pixel(a, b));
                                graph.add_edge(i, overlapped_index[neighbor_index], m_s + m_t);
                            }
                        }
                    }
                }
            }
        }
        // cout << " > " << overlapped.size() << " overlapped pixels" << endl;
        
        // Min-cut and overwrite
        // cout << " > Running min-cut algorithm ... " << endl;
       
      
        
        auto decisions = graph.min_cut(s, t);
        
ofstream dump("mincut.txt"); 

 dump <<"Overlapped size :"<<overlapped.size();
 
 for(int i=0;i<overlapped.size(); ++i)
 {
     auto xy = overlapped[i];
  dump << xy.first<<","<<xy.second<<endl;
}

dump.close();



        for (int i = 0; i < overlapped.size(); ++ i) {
            
            if (decisions[i]) { // Belongs to the new patch
                auto xy = overlapped[i];
                int index = xy.second * w + xy.first;
                  origin[index] = patch;
                        
                Pixel meow(255,255,0); 
                data[index]=meow;

               
              
            }
        }
    
    }
   
   
   
void applyover(const shared_ptr<Patch> &patch) {
        cout << " > Applying a new patch at (" << patch->x << ", " << patch->y << ")" << endl;
        int x_begin = max(patch->x, 0);
        int y_begin = max(patch->y, 0);
        int x_end = min(patch->x_end(), w);
        int y_end = min(patch->y_end(), h);

        // Fill non-overlapped area first
        static int dx[4] = { 0, +1,  0, -1};
        static int dy[4] = {+1,  0, -1,  0};
        int n_old_seam_nodes = 0;
        vector<pair<int, int>> overlapped;
        vector<int> overlapped_index(w * h, -1);

        for (int y = y_begin; y < y_end; ++ y) {
            for (int x = x_begin; x < x_end; ++ x) {
                int index = y * w + x;
                
                if (not origin[index]) {
                    origin[index] = patch;
                    data[index] = patch->pixel(x, y);
                } 
                
                else {
                    overlapped_index[index] = overlapped.size();
                    overlapped.emplace_back(x, y);
                    
                    for (int d = 0; d < 2; ++ d) {
                        int a = x + dx[d], b = y + dy[d];
                        int neighbor_index = b * w + a;
                        
                        if (in_range(a, b) and origin[neighbor_index] and origin[neighbor_index] != origin[index]) {
                            ++ n_old_seam_nodes;
                        }
                    }
                }
            }
        }

        // Build graph
        Graph graph(overlapped.size() + n_old_seam_nodes + 2);
        int s = overlapped.size() + n_old_seam_nodes, t = overlapped.size() + n_old_seam_nodes + 1;
        int old_sean_node_index = overlapped.size();
        for (int i = 0; i < overlapped.size(); ++ i) {

            auto xy = overlapped[i];
            int index = xy.second * w + xy.first;
            int m_s = pixel(xy.first, xy.second).distance(patch->pixel(xy.first, xy.second));
            for (int d = 0; d < 4; ++ d) {
                int a = xy.first + dx[d], b = xy.second + dy[d];
                int neighbor_index = b * w + a;
                
                if (in_range(a, b) and origin[neighbor_index]) {
                    
                    if (origin[neighbor_index] == patch) {
                        graph.add_edge(i, t, Graph::inf_flow);
                    } 
                    
                    else {
                        
                        if (overlapped_index[neighbor_index] == -1) {
                            graph.add_edge(s, i, Graph::inf_flow);
                        } 
                        
                        else if (d < 2) { // `add_edge` is bi-directional
                            
                            if (origin[index] != origin[neighbor_index] and origin[index]->in_range(a, b) and
                                origin[neighbor_index]->in_range(a, b)) { // Old seam node
                                int m_t = data[neighbor_index].distance(patch->pixel(a, b));
                                graph.add_edge(old_sean_node_index, i, m_s + m_t);
                                graph.add_edge(old_sean_node_index, overlapped_index[neighbor_index], m_s + m_t);
                                int old_m_s = origin[index]->pixel(xy.first, xy.second).distance(origin[neighbor_index]->pixel(xy.first, xy.second));
                                int old_m_t = origin[index]->pixel(a, b).distance(origin[neighbor_index]->pixel(a, b));
                                graph.add_edge(old_sean_node_index, t, old_m_s + old_m_t);
                                ++ old_sean_node_index;
                            } 
                            
                            else {
                                int m_t = data[neighbor_index].distance(patch->pixel(a, b));
                                graph.add_edge(i, overlapped_index[neighbor_index], m_s + m_t);
                            }
                        }
                    }
                }
            }
        }
       
       
             ofstream dump("Graph.txt");

            for(int i=0; i<graph.edges.size();i++)
            {
           dump << graph.edges[i].v<<":";
           dump << graph.edges[i].next<< " capacity:-";
           dump << graph.edges[i].capacity<<endl;
            
            }
            

          dump.close();
        
        auto decisions = graph.min_cut(s, t);

        

        for (int i = 0; i < overlapped.size(); ++ i) {
            
            
                auto xy = overlapped[i];
                int index = xy.second * w + xy.first;
                origin[index] = patch;

               Pixel meow(255,255,0); 
               data[index]=meow; 
          
        }

    }
};




