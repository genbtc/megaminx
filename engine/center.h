#ifndef __CENTER_H__
#define __CENTER_H__

class Center
{
public:
	Center();
	void init(int n);
    void setColor(double r, double g, double b);
    void initColor(int k);
	~Center();

	void render();

private:

	double _color[3];
	double _vertex[5][3];
};

#endif