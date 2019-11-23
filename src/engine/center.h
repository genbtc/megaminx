#ifndef __CENTER_H__
#define __CENTER_H__
#include "piece.h"

class Center : public Piece {
public:
    Center() {
        this->centerInit();
    };
    ~Center() {}
    void createAxis(int n, double* target);
    void init(int n);
    void render() const;
};

#endif
