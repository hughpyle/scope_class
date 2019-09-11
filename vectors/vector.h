/**
 * \file
 * Vector line drawing and fonts
 *
 */

#ifndef _vector_h_
#define _vector_h_

#include <stdint.h>


void moveto(int x, int y);


void line_vert(int x0, int y0, uint16_t h);
void line_horiz(int x0, int y0, uint16_t w);

void line(int x0, int y0, int x1, int y1);
void lineto(int x1, int y1);


typedef struct
{
	// center of rotation
	uint8_t cx;
	uint8_t cy;

	// scale of vector, divided by 16
	int8_t scale;

	// Angle from 0 to 255 (== 2 Pi)
	uint8_t theta;

	// precomputed sin/cos
	int8_t sin_t;
	int8_t cos_t;
} vector_rot_t;


void
vector_rot_init(
	vector_rot_t * r,
	uint8_t angle
);


uint8_t
vector_rot_x(
	const vector_rot_t * r,
	int8_t x,
	int8_t y
);


uint8_t
vector_rot_y(
	const vector_rot_t * r,
	int8_t x,
	int8_t y
);


uint16_t vSpace(uint16_t sc);


int draw_character(char c, int x, int y, int size);
int draw_string(const char * s, int x, int y, int size);
int size_string(const char *s, int size);

void draw_char_rot(const vector_rot_t * const r, int x, int y, char val);


// 3d

class Point2
{
  public:
    float x, y;

    Point2(float xx=0, float yy=0)
      : x(xx), y(yy)
    {
    }
};

class Matrix3
{
  public:
    float m[9];
};

class Point3
{
  public:
    float x, y, z;

    Point3(float xx=0, float yy=0, float zz=0)
      : x(xx), y(yy), z(zz)
    {
    }

    void rotate(Matrix3 t);
};

#endif
