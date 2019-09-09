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
    Quadrature(uint16_t start_degrees, float points_per_cycle);

    // Move by one step
    void step();

    // Getters
    float cos();
    float sin();

protected:
    float quad_cm;
    float quad_cos, quad_c1, quad_c2;
    float quad_sin, quad_s1, quad_s2;

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
