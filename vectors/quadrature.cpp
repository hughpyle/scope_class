#include "quadrature.h"

/*
 * Quadrature using recurrence relation from section 5.4 of Numerical Recipes: The Art of Scientific Computing:
 * cos(n*ang) = 2 cos(ang) cos((n-1) * ang) - cos((n-2)*ang)
 * sin(n*ang) = 2 cos(ang) sin((n-1) * ang) - sin((n-2)*ang)
 *
 * https://brainwagon.org/2008/01/07/sines-and-cosines-of-the-times/
 */

Quadrature::Quadrature(uint16_t start_degrees, float points_per_cycle)
{
    // Initialize for a given number of points per cycle
    // (e.g. points_per_cycle = freq / sample_rate)

    float start_angle = 2.0 * PI * start_degrees / 360;
    float delta_angle = 2.0 * PI / points_per_cycle;
    quad_cm =  2.0 * cosf(delta_angle);

    quad_c2 = cosf(start_angle - 2.0 * delta_angle);
    quad_c1 = cosf(start_angle - 1.0 * delta_angle);
    quad_cos = cosf(start_angle);
    quad_s2 = sinf(start_angle - 2.0 * delta_angle);
    quad_s1 = sinf(start_angle - 1.0 * delta_angle);
    quad_sin = sinf(start_angle);
}


void Quadrature::step()
{
    // step the quadrature recurrence
    // cos(n*ang) = 2 cos(ang) cos((n-1) * ang) - cos((n-2)*ang)
    // sin(n*ang) = 2 cos(ang) sin((n-1) * ang) - sin((n-2)*ang)
    quad_cos = quad_cm * quad_c1 - quad_c2;
    quad_sin = quad_cm * quad_s1 - quad_s2;
    // Save for next time around
    quad_c2 = quad_c1;
    quad_c1 = quad_cos;
    quad_s2 = quad_s1;
    quad_s1 = quad_sin;
}


float Quadrature::cos()
{
    return quad_cos;
}


float Quadrature::sin()
{
    return quad_sin;
}


void quad_arc(uint16_t center_x, uint16_t center_y, uint16_t radius, uint16_t start_degrees, uint16_t end_degrees, uint16_t points_per_circle)
{
    // draw circular arc centered at x,y with given radius,
    // with the starting and end points defined.
    Quadrature arc(start_degrees, points_per_circle);
    int x = arc.cos() * radius + center_x;
    int y = arc.sin() * radius + center_y;
    moveto(x, y);
    uint16_t count = points_per_circle * 360 / (end_degrees - start_degrees);
    for(uint16_t i=0; i<=count; i++)
    {
        int xx, yy;
        arc.step();
        xx = arc.cos() * radius + center_x;
        yy = arc.sin() * radius + center_y;
        line(x, y, xx, yy);
        x = xx;
        y = yy;
    }
}


void quad_circle(uint16_t center_x, uint16_t center_y, uint16_t radius)
{
    uint16_t points = max(radius/4, 10);
    quad_arc(center_x, center_y, radius, 0, 360, points);
}
