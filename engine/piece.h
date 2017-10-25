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

    void setColor(double r, double g, double b)
    {
        setColor3(0, r, g, b);
    }
    void setColor3(int i, double r, double g, double b)
    {
        _color[i][0] = r;
        _color[i][1] = g;
        _color[i][2] = b;
    }
    
    void initColor(int k)
    {
        switch (k)
        {
        case 1:
            setColor(1.0, 1.0, 1.0);
            break;
        case 2:
            setColor(0.0, 0.0, 1.0);
            break;
        case 3:
            setColor(1.0, 0.0, 0.0);
            break;
        case 4:
            setColor(0.0, 0.5, 0.0);
            break;
        case 5:
            setColor(0.5, 0.0, 1.0);
            break;
        case 6:
            setColor(1.0, 1.0, 0.0);
            break;
        case 7:
            setColor(0.5, 0.5, 0.5);
            break;
        case 8:
            setColor(0.2, 0.5, 1.0);
            break;
        case 9:
            setColor(1.0, 0.4, 0.1);
            break;
        case 10:
            setColor(0.3, 1.0, 0.3);
            break;
        case 11:
            setColor(0.9, 0.4, 1.0);
            break;
        case 12:
            setColor(1.0, 1.0, 0.3);
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
                setColor3(i, 1.0, 1.0, 1.0);
                break;
            case 2:
                setColor3(i, 0.0, 0.0, 1.0);
                break;
            case 3:
                setColor3(i, 1.0, 0.0, 0.0);
                break;
            case 4:
                setColor3(i, 0.0, 0.5, 0.0);
                break;
            case 5:
                setColor3(i, 0.5, 0.0, 1.0);
                break;
            case 6:
                setColor3(i, 1.0, 1.0, 0.0);
                break;
            case 7:
                setColor3(i, 0.5, 0.5, 0.5);
                break;
            case 8:
                setColor3(i, 0.2, 0.5, 1.0);
                break;
            case 9:
                setColor3(i, 1.0, 0.4, 0.1);
                break;
            case 10:
                setColor3(i, 0.3, 1.0, 0.3);
                break;
            case 11:
                setColor3(i, 0.9, 0.4, 1.0);
                break;
            case 12:
                setColor3(i, 1.0, 1.0, 0.3);
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
                setColor3(i, 1.0, 1.0, 1.0);
                break;
            case 2:
                setColor3(i, 0.0, 0.0, 1.0);
                break;
            case 3:
                setColor3(i, 1.0, 0.0, 0.0);
                break;
            case 4:
                setColor3(i, 0.0, 0.5, 0.0);
                break;
            case 5:
                setColor3(i, 0.5, 0.0, 1.0);
                break;
            case 6:
                setColor3(i, 1.0, 1.0, 0.0);
                break;
            case 7:
                setColor3(i, 0.5, 0.5, 0.5);
                break;
            case 8:
                setColor3(i, 0.2, 0.5, 1.0);
                break;
            case 9:
                setColor3(i, 1.0, 0.4, 0.1);
                break;
            case 10:
                setColor3(i, 0.3, 1.0, 0.3);
                break;
            case 11:
                setColor3(i, 0.9, 0.4, 1.0);
                break;
            case 12:
                setColor3(i, 1.0, 1.0, 0.3);
                break;
            case 0:
            default:
                break;
            }
        }
    }
};

#endif