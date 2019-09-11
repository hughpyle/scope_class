/**
 * \file
 * Vector line drawing and fonts
 *
 */

#include "hershey_font.h"
#include "teensydac.h"
#include "vector.h"
#include "sin_table.h"


/** Slow scopes require time at each move; give them the chance */
/* #define CONFIG_SLOW_SCOPE */

int outX = 0;  // remember current point
int outY = 0;  //


void _out(void)
{
  writeDAC(outX, outY);
}


void moveto(int x, int y)
{
    if (outX == x && outY == y)
        return;

    outX = x;
    outY = y;
    _out();
}


static inline void pixel_delay(void)
{
#ifdef CONFIG_SLOW_SCOPE
    delayMicroseconds(5);
#endif
}


void line_vert(int x0, int y0, uint16_t h)
{
    moveto(x0, y0);
    while(h > 0)
    {
        outY = y0++;
        h--;
        _out();
        pixel_delay();
    }
}


void line_horiz(int x0, int y0, uint16_t w)
{
    moveto(x0, y0);
    while(w > 0)
    {
        outX = x0++;
        w--;
        _out();
        pixel_delay();
    }
}


void line(int x0, int y0, int x1, int y1)
{
	int dx;
	int dy;
	int sx;
	int sy;

	if (x0 == x1)
	{
		if (y0 < y1)
			line_vert(x0, y0, y1 - y0);
		else
			line_vert(x0, y1, y0 - y1);
    moveto(x1, y1);
		return;
	}

	if (y0 == y1)
	{
		if (x0 < x1)
			line_horiz(x0, y0, x1 - x0);
		else
			line_horiz(x1, y0, x0 - x1);
    moveto(x1, y1);
		return;
	}

	if (x0 <= x1)
	{
		dx = x1 - x0;
		sx = 1;
	} else {
		dx = x0 - x1;
		sx = -1;
	}

	if (y0 <= y1)
	{
		dy = y1 - y0;
		sy = 1;
	} else {
		dy = y0 - y1;
		sy = -1;
	}

	int err = dx - dy;

	moveto(x0, y0);
	while (!(x0 == x1 && y0 == y1))
	{
		int e2 = 2 * err;
		if (e2 > -dy)
		{
			err = err - dy;
      outX = (x0 += sx);
      _out();
		}
		if (e2 < dx)
		{
			err = err + dx;
			outY = (y0 += sy);
      _out();
		}

		pixel_delay();
	}
}


void lineto(int x1, int y1)
{
  line(outX, outY, x1, y1);
}


void
vector_rot_init(
	vector_rot_t * r,
	uint8_t theta
)
{
	r->theta = theta;
	r->sin_t = sin_lookup(theta);
	r->cos_t = cos_lookup(theta);
}


uint8_t
vector_rot_x(
	const vector_rot_t * r,
	int8_t x,
	int8_t y
)
{
	int32_t x2 = x;
	int32_t w;
	if (r->theta != 0)
	{
		int32_t y2 = y;

		w = (r->scale * (x2 * r->cos_t + y2 * r->sin_t)) / (32 * 256);
	} else {
		w = (r->scale * x2) / 64;
	}
	
	return w + r->cx;
}

uint8_t
vector_rot_y(
	const vector_rot_t * r,
	int8_t x,
	int8_t y
)
{
	int32_t x2 = x;
	int32_t y2 = y;
	int32_t z;

	if (r->theta != 0)
		z = (r->scale * (y2 * r->cos_t - x2 * r->sin_t)) / (32 * 256);
	else
		z = (r->scale * y2) / 64;

	return z + r->cy;
}
	

static inline int8_t
_trmm_scaling_TODO_(
	int8_t d,
	uint8_t scale
)
{
	if (scale == 0)
		return d / 4;
	if (scale == 1)
		return (d * 2) / 3;
	if (scale == 2)
		return d;
	if (scale == 3)
		return (d * 3) / 2;
	if (scale == 4)
		return d * 2;
	return d;
}



/* draw text with the hershey simplex font */

int draw_character(char c, int x, int y, int size)
{
	const hershey_char_t * const f = &hershey_simplex[c - ' '];
	int next_moveto = 1;

	for(int i = 0 ; i < f->count ; i++)
	{
		int dx = f->points[2*i+0];
		int dy = f->points[2*i+1];
		if (dx == -1)
		{
			next_moveto = 1;
			continue;
		}

		dx = (dx * size) * 3 / 4;
		dy = (dy * size) * 3 / 4;

		if (next_moveto)
			moveto(x + dx, y + dy);
		else
			lineto(x + dx, y + dy);

		next_moveto = 0;
	}

	return (f->width * size) * 3 / 4;
}


int draw_string(const char * s, int x, int y, int size)
{
	while(*s)
	{
		char c = *s++;
		x += draw_character(c, x, y, size);
	}
	return x;
}


int size_string(const char *s, int size)
{
    int extent = 0;
  	while(*s)
  	{
    		char c = *s++;
    		const hershey_char_t * const f = &hershey_simplex[c - ' '];
    		extent += (f->width * size) * 3 / 4;
  	}
  	return extent;
}


/*
 * ***************************************************
 */

uint16_t vSpace(uint16_t sc) {
  uint16_t charHeight = (21 + 7) ; // approximate
  return (sc * 1.25 * charHeight);
}


uint16_t scaling(uint16_t d, uint8_t scale) {
  return (d * scale);
}

/*
 ******
 */

void Point3::rotate(Matrix3 t)
{
    // Matrix multiply
    float xx = (x * t.m[0]) + (y * t.m[3]) + (z * t.m[6]);
    float yy = (x * t.m[1]) + (y * t.m[4]) + (z * t.m[7]);
    float zz = (x * t.m[2]) + (y * t.m[5]) + (z * t.m[8]);
    x = xx;
    y = yy;
    z = zz;
}
