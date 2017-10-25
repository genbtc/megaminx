#ifndef __CENTER_H__
#define __CENTER_H__
#include "piece.h"

class Center : Piece
{
public:
    Center();
    virtual ~Center() = default;

    void init(int n);
	void render();
};

#endif