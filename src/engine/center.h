#ifndef __CENTER_H__
#define __CENTER_H__
#include "piece.h"

class Center : public Piece {
public:
    Center() {
        this->centerInit();
    };
    static void createAxis(int n, double* target);
    ~Center() {}

    void init(int n);
    void render();
};

#endif