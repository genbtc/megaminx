#ifndef __CENTER_H__
#define __CENTER_H__
#include "piece.hpp"

class Center : public Piece {
public:
    Center() {
        this->centerInit();
    }
    ~Center() = default;
    void createAxis(int n, double* target);
    void init(int n);
    void render() const;
};

#endif
