#ifndef _circles_h_
#define _circles_h_

#include <Arduino.h>
#include "vector.h"

/*
 * Class for calculating the points along an arc
 */
class Quadrature {

public:
    // Make a new arc
    Quadrature(float start_degrees=0.0, float points_per_cycle=360, float radius=1.0);

    // Move by one step
    void step();

    // public
    float quad_cos, quad_sin;
    // Getters
    inline float cos() { return quad_cos; }
    inline float sin() { return quad_sin; }

protected:
    float quad_cm;
    float quad_c1, quad_c2;
    float quad_s1, quad_s2;

};


/*
 * Draw an arc
 */
void quad_arc(uint16_t center_x, uint16_t center_y, uint16_t radius, uint16_t start_degrees, uint16_t end_degrees, uint16_t points_per_circle);

/*
 * Draw a full circle
 */
void quad_circle(uint16_t center_x, uint16_t center_y, uint16_t radius);

#endif
