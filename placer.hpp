#pragma once

#define STB_IMAGE_IMPLEMENTATION
#define STB_IMAGE_WRITE_IMPLEMENTATION
#include "stb_image.h"
#include "stb_image_write.h"

#include "dft.hpp"
#include "image.hpp"

int Random(int min, int max)
{
    return min + ( std::rand() % ( max - min + 1 ) );
}

class Placer {
public:
    static void init(const std::shared_ptr<Canvas> &canvas, const std::shared_ptr<Image> &texture) {
       
        int i=0;

        for (int y = 0; y < canvas->h; y += Random(texture->h / 3, texture->h * 2 / 3)) {
            for (int x = 0; x < canvas->w; x += Random(texture->w / 3, texture->w * 2 / 3)) {
                auto patch = std::make_shared<Patch>(texture, x, y);
                canvas->apply(patch);
            }
        }
    }

     static void initseg(const std::shared_ptr<Canvas> &canvas, const std::shared_ptr<Image> &texture) {
       
        int i=0;

        for (int y = 0; y < canvas->h; y += Random(texture->h / 3, texture->h * 2 / 3)) {
            for (int x = 0; x < canvas->w; x += Random(texture->w / 3, texture->w * 2 / 3)) {
                auto patch = std::make_shared<Patch>(texture, x, y);
                canvas->applyseg(patch);

                if(i==1)
                {
                 break;
                } 
                 i++;
            }
            if(i==1)
                {
                 break;
                }
        }
    }


     static void initover(const std::shared_ptr<Canvas> &canvas, const std::shared_ptr<Image> &texture) {
       
         int i=0;
        for (int y = 0; y < canvas->h; y += Random(texture->h / 3, texture->h * 2 / 3)) {
            for (int x = 0; x < canvas->w; x += Random(texture->w / 3, texture->w * 2 / 3)) {
                auto patch = std::make_shared<Patch>(texture, x, y);
                canvas->applyover(patch);
                if(i==1)
                {
                 break;
                }
                 i++;
            }
            if(i==1)
                {
                 break;
                }
        }
    }

    static void random(const std::shared_ptr<Canvas> &canvas, const std::shared_ptr<Image> &texture) {
        auto patch = std::make_shared<Patch>(texture, Random(0, texture->w - 1), Random(0, texture->h - 1));
        canvas->apply(patch);
    }

  
};