#ifndef _PIECE_H_
#define _PIECE_H_

class Piece
{

public:
    Piece()
    {
    }

    ~Piece() = default;
public:
    double _color[3][3];
    double _vertex[7][3];

    void setColor(int i, double r, double g, double b)
    {
        _color[i][0] = r;
        _color[i][1] = g;
        _color[i][2] = b;
    }
    
    void initColor(int k)
    {
        int i = 0;
        switch (k)
        {
        case 1:
            setColor(i, 1.0, 1.0, 1.0);
            break;
        case 2:
            setColor(i, 0.0, 0.0, 1.0);
            break;
        case 3:
            setColor(i, 1.0, 0.0, 0.0);
            break;
        case 4:
            setColor(i, 0.0, 0.5, 0.0);
            break;
        case 5:
            setColor(i, 0.5, 0.0, 1.0);
            break;
        case 6:
            setColor(i, 1.0, 1.0, 0.0);
            break;
        case 7:
            setColor(i, 0.5, 0.5, 0.5);
            break;
        case 8:
            setColor(i, 0.2, 0.5, 1.0);
            break;
        case 9:
            setColor(i, 1.0, 0.4, 0.1);
            break;
        case 10:
            setColor(i, 0.3, 1.0, 0.3);
            break;
        case 11:
            setColor(i, 0.9, 0.4, 1.0);
            break;
        case 12:
            setColor(i, 1.0, 1.0, 0.3);
            break;
        case 0:
        default:
            break;
        }
    }
    void initColor(int a, int b)
    {
        for (int i = 0; i < 2; ++i)
        {
            switch (i == 0 ? a : b)
            {
                //Which colors are these ?
            case 1:
                setColor(i, 1.0, 1.0, 1.0);
                break;
            case 2:
                setColor(i, 0.0, 0.0, 1.0);
                break;
            case 3:
                setColor(i, 1.0, 0.0, 0.0);
                break;
            case 4:
                setColor(i, 0.0, 0.5, 0.0);
                break;
            case 5:
                setColor(i, 0.5, 0.0, 1.0);
                break;
            case 6:
                setColor(i, 1.0, 1.0, 0.0);
                break;
            case 7:
                setColor(i, 0.5, 0.5, 0.5);
                break;
            case 8:
                setColor(i, 0.2, 0.5, 1.0);
                break;
            case 9:
                setColor(i, 1.0, 0.4, 0.1);
                break;
            case 10:
                setColor(i, 0.3, 1.0, 0.3);
                break;
            case 11:
                setColor(i, 0.9, 0.4, 1.0);
                break;
            case 12:
                setColor(i, 1.0, 1.0, 0.3);
                break;
            case 0:
            default:
                break;
            }
        }
    }
    void initColor(int a, int b, int c) {
        for (int i = 0; i < 3; ++i)
        {
            int k;
            if (i == 0)      k = a;
            else if (i == 1) k = b;
            else if (i == 2) k = c;
            else             k = 0;

            //Which colors are these ?
            switch (k)
            {
            case 1:
                setColor(i, 1.0, 1.0, 1.0);
                break;
            case 2:
                setColor(i, 0.0, 0.0, 1.0);
                break;
            case 3:
                setColor(i, 1.0, 0.0, 0.0);
                break;
            case 4:
                setColor(i, 0.0, 0.5, 0.0);
                break;
            case 5:
                setColor(i, 0.5, 0.0, 1.0);
                break;
            case 6:
                setColor(i, 1.0, 1.0, 0.0);
                break;
            case 7:
                setColor(i, 0.5, 0.5, 0.5);
                break;
            case 8:
                setColor(i, 0.2, 0.5, 1.0);
                break;
            case 9:
                setColor(i, 1.0, 0.4, 0.1);
                break;
            case 10:
                setColor(i, 0.3, 1.0, 0.3);
                break;
            case 11:
                setColor(i, 0.9, 0.4, 1.0);
                break;
            case 12:
                setColor(i, 1.0, 1.0, 0.3);
                break;
            case 0:
            default:
                break;
            }
        }
    }
};

#endif