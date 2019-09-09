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
    for (uint16_t i = 0; i < h; i++)
    {
        outY = y0++;
        _out();
        pixel_delay();
    }
}


void line_horiz(int x0, int y0, uint16_t w)
{
    moveto(x0, y0);
    for (uint16_t i = 0; i < w; i++)
    {
        outX = x0++;
        _out();
        pixel_delay();
    }
}


void lineto(int x1, int y1)
{
    line(outX, outY, x1, y1);
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

	while (1)
	{
		if (x0 == x1 && y0 == y1)
			break;

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
uint16_t size_str(const char *str , uint8_t scale)
{
  // return pixel (DAC count) length of given string at current scale
  char c;
  uint16_t w = 0;
  while ((c = *str++)) {
    int asc = c - 0x20;               // row in file is ascii code c - 32
    w += hershey_simplex[asc][1];             // width of character is first entry in row
  }
  return scaling(w, scale);
}


uint16_t draw_str(const uint16_t x, const uint16_t y, const char * str, uint8_t scale)
{
  uint16_t xx = x;
  uint16_t yy = max(y, vSpace(scale) / 4);
  // draw text string, return ending x position (DAC count)
  char c;
  while ((c = *str++)) {
    xx += draw_char(xx, yy, c, scale);
  }
  return xx;
}


static inline uint16_t _draw_char(const uint16_t x, const uint16_t y, const char c, const uint8_t scale)
{
	uint16_t ox = x;
	uint16_t oy = y;
	uint8_t pen_down = 0;

	if (c < 0x20)
		return 0;

    const int asc = c - 0x20;
    const uint8_t count = hershey_simplex[asc][0];  // no. vertices in character
    const uint8_t wid = hershey_simplex[asc][1];  // width of character

	for (uint8_t i = 0 ; i < count ; i++)
	{
        const int8_t px = hershey_simplex[asc][2 * i + 2];
        const int8_t py = hershey_simplex[asc][2 * i + 3];

		if (px == -1 && py == -1)
		{
			pen_down = 0;
			continue;
		}

		const uint8_t nx = x + scaling(px, scale);
		const uint8_t ny = y + scaling(py, scale);

		if (pen_down)
			line(ox, oy, nx, ny);

		pen_down = 1;
		ox = nx;
		oy = ny;
	}

	return scaling(wid, scale);
}


uint16_t draw_char(const uint16_t x, const uint16_t y, const char c, const uint8_t scale)
{
  return _draw_char(x, y, c, scale);
}


uint16_t
draw_char_big(
	uint16_t x,
	uint16_t y,
	uint8_t c
)
{
	return _draw_char(x, y, c, 3);
}


uint16_t
draw_char_med(
	uint16_t x,
	uint16_t y,
	uint8_t c
)
{
	return _draw_char(x, y, c, 2);
}

uint16_t
draw_char_small(
	uint16_t x,
	uint16_t y,
	uint8_t c
)
{
	return _draw_char(x, y, c, 1);
}

*/
