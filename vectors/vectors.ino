/*
    Vector display driven by Teensy 3.6

    Sources:

    Trammell Hudson's vector code
    https://www.nycresistor.com/2012/09/03/vector-display/
    https://bitbucket.org/hudson/vectorscope/src/default/
    https://github.com/osresearch/vst/tree/master/teensyv

    Zike/MEZ teensy
    https://forum.pjrc.com/threads/44449-Vector-draw-examples-libs-using-T3-5-3-6-DAC

    TODO:
      lissajous
      spinny cube
      rotating text

*/

#include "hershey_font.h"
#include "teensydac.h"
#include "quadrature.h"
#include "vector.h"

const uint16_t  ch = 32;

uint8_t         fontScale = 9;



uint16_t xCenter = 0;
uint16_t yCenter = 0;


char title[] = "Brickyard Collaborative";
char   sig[] = "VECTOR DISPLAY";
char  caps[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZ" ;
char    lc[] = "abcdefghijklmnopqrstuvwxyz" ;
char  syma[] = {32, 33, 34, 35, 36, 37, 38, 39, 40, 41, 42, 43, 44, 45, 46, 47, 32}; // note  extra '32' (space); compiler bug?
char  symb[] = {48, 49, 50, 51, 52, 53, 54, 55, 56, 57, 58, 59, 60, 61, 62, 63, 64};

/*
 * ***************************************************
*/

void setup() {
    Serial.begin(9600);

    analogWriteResolution(dacRes);
    
    xCenter = pow(2, dacRes - 1);
    yCenter = xCenter;
    moveto(xCenter, yCenter);

    uint16_t backspace = max(size_string(title, fontScale) / 2, xCenter);
    while (millis() < 2000) {
        uint16_t x = xCenter - backspace;
        draw_string(title, x, yCenter, fontScale);
        draw_string(sig, x, yCenter - vSpace(fontScale), fontScale);
    }
}

/*
 * ***************************************************
*/

void loop() {
  
  // line_test();
  // box_test();
  // circle_test();

  test_machine();

  spiral_test();

  // text_test_1();
  // text_test_2();

}


/*
 * **************************************************************************************
*/

void draw_box(uint16_t x, uint16_t y, int16_t w, int16_t h) {
  // draw rectangle from lower left corner (x,y) w wide and h height  (all DAC counts)
  line(x, y, x + w, y);
  line(x + w, y, x + w, y + h);
  line(x + w, y + h, x, y + h);
  line(x, y + h, x, y);
}


/*
 * *******************************************************************
 * ************** DEMO TESTS *****************************************
 * *******************************************************************
*/

void line_test() {
  // test general, horizontal and vertical line drawing
  float  freq = .5;  // Hz
  float  amp = 2000; // counts
  for (int k = 1; k < 200; k++) {
    float phi = 2 * PI * freq * millis() / 1000 ;
    long xnew = 2048 -  (long) amp * cosf (phi);
    xnew = max(0, xnew);
    long ynew = 2048 +  (long) amp * sinf (phi);
    ynew = max(0, ynew);

    line(0, 0, xnew, ynew);
    line_vert(xnew, 0, ynew);
    line_horiz(0, ynew, xnew);


    line(4095, 4095, xnew, ynew);
    line_vert(xnew, ynew, 4095 - ynew); // requires finish > start
    line_horiz(xnew, ynew, 4095 - xnew); // requires finish > start
  }
}

/*
 * *******************************************************************
*/

void box_test() {
  // test box drawing
  int xc = 2047;
  int yc = 2047;
  for (int m = 0; m < 2; m++) {
    int dy = 40;
    int dx = 40;
    for (int ddx = dx; ddx < 4000; ddx += 40) {
      draw_box(xc - ddx / 2, yc - dy / 2, ddx, dy);
      dx = ddx;
    }
    for (int ddy = dy; ddy < 4000; ddy += 40) {
      draw_box(xc - dx / 2, yc - ddy / 2, dx , ddy);
      dy = ddy;
    }
    for (int sz = dy; sz > 0 ; sz -= 40) {
      draw_box(xc - sz / 2, yc - sz / 2, sz, sz);
    }
  }
}

/*
 * **************************************************************************************
*/

float spiral(uint16_t start_pos, uint16_t end_pos, uint16_t rotate_degrees)
{
    int center_x = 2047;
    int center_y = 2047;
    Quadrature arc(rotate_degrees, 180);
    float radius = 120 * expf((float)start_pos/480);
    int x = radius + center_x;
    int y = center_y;
    moveto(x, y);
    for(uint16_t i=0; i<start_pos; i+=2)
    {
        arc.step();
    }
    for(uint16_t i=start_pos; i<end_pos; i+=2)
    {
        radius = 120 * expf((float)i/480);
        int xx, yy;
        arc.step();
        xx = arc.cos() * radius + center_x;
        yy = arc.sin() * radius + center_y;
        line(x, y, xx, yy);
        x = xx;
        y = yy;
    }
    moveto(center_x, center_y);
    radius = 120 * expf((float)start_pos/480);
    return radius;
}


void circle_test() {
  // test circle drawing
  for (int m = 0;  m < 2; m++) {
    for (float r = 1; r < 2000; r *= 1.03) {
      quad_circle(2047, 2047, r);
    }
    for (float r = 2000; r > 1 ; r *= 0.97) {
      quad_circle(2047, 2047, r);
    }
  }
}


void spiral_test()
{
  // Draw spiral
  for(int rot=0; rot<3600; rot += 15)
  {
    int end = min(rot, 1500);
    if(end<1500)
    {
      float rad = 120 * expf((float)end/480);
      quad_circle(2047, 2047, rad);
    }
    spiral(0, end, -rot);
  }
  // open out the center as a circle
  for(int rot=15; rot<1500; rot += 15)
  {
    float rad = spiral(rot, 1500, -rot);
    quad_circle(2047, 2047, rad);
  }

}


/*
 * **************************************************************************************
*/

void text_test_1() {
  // display font really big
  uint8_t scale = 80;
  uint16_t xx = 2047;
  uint16_t yy = 2047 - vSpace(scale) / 4;
  char b[2];
  for (char c = 32; c < 127; c++)
  {
    String Q = String(c);
    Q.toCharArray(b, 2);
    xx = 2047 - size_string(b, scale) / 2 ;
    long start = millis();
    while (millis() - start < 80)
    {
      draw_character(c, xx, yy, scale);
    }
  }
}


/*
 * **************************************************************************************
*/

void text_test_2() {
  // display font list at various scales
  for (int m = 0; m < 2; m++) {
    for (float p = 2 ; p < 16; p *= 1.02) {
      uint8_t s = min(p, 8);
      uint16_t yy = 2247;
      uint16_t xx = 2047 ;
      xx = 2047 - size_string(caps, int(s)) / 2;
      draw_string(caps, xx, yy, s);

      yy = yy - vSpace(s);
      xx = 2047 - size_string(lc, int(s)) / 2;
      draw_string(lc, xx, yy, s);

      yy = yy - vSpace(s);
      xx = 2047 - size_string(syma, int(s)) / 2;
      draw_string(syma, xx, yy, s);

      yy = yy - vSpace(s);
      xx = 2047 - size_string(symb, int(s)) / 2;
      draw_string(symb, xx, yy, s);
    }
  }
}


/*
 * **************************************************************************************
*/

// see: https://github.com/hughpyle/machinesalem-arduino-projs/tree/master/writerscope
const int M = 1; /* moveto */
const int L = 2; /* lineto */
const int data[] = {
  M,357,-6,L,377,-6,L,377,-19,L,362,-35,L,332,-35,L,309,-11,L,309,81,L,357,81,L,357,-6,
  M,433,-6,L,452,-6,L,452,-20,L,437,-35,L,400,-35,L,384,-19,L,384,81,L,433,81,L,433,-6,
  M,508,-35,L,460,-35,L,460,81,L,508,81,L,508,-35,
  M,210,-6,L,279,-6,L,279,-20,L,265,-35,L,192,-35,L,163,-6,L,163,81,L,210,81,L,210,-6,
  M,289,32,L,269,13,L,218,13,L,218,34,L,241,34,L,241,54,L,218,54,L,218,67,L,232,81,L,269,81,L,289,62,L,289,32,
  M,87,-35,L,46,-35,L,17,-6,L,17,12,L,65,12,L,65,-6,L,87,-6,L,87,-35,
  M,143,-6,L,114,-35,L,94,-35,L,94,81,L,114,81,L,143,52,L,143,-6,
  M,87,54,L,65,54,L,65,40,L,17,40,L,17,52,L,46,81,L,87,81,L,87,54,
  M,-3,-79,L,-51,-79,L,-51,81,L,-3,81,L,-3,-79,
  M,-80,-6,L,-58,-6,L,-58,-20,L,-73,-35,L,-99,-35,L,-128,-6,L,-128,81,L,-80,81,L,-80,-6,
  M,-150,-80,L,-198,-80,L,-198,-49,L,-150,-49,L,-150,-80,
  M,-150,-35,L,-198,-35,L,-198,81,L,-150,81,L,-150,-35,
  M,-298,-6,L,-278,-6,L,-278,-19,L,-294,-35,L,-321,-35,L,-347,-9,L,-347,81,L,-298,81,L,-298,-6,
  M,-221,-35,L,-270,-35,L,-270,81,L,-221,81,L,-221,-35,
  M,-424,14,L,-447,14,L,-447,-6,L,-424,-6,L,-424,-35,L,-465,-35,L,-494,-6,L,-494,36,L,-424,36,L,-424,14,
  M,-368,-6,L,-397,-35,L,-416,-35,L,-416,81,L,-397,81,L,-368,52,L,-368,-6,
  M,-424,54,L,-489,54,L,-489,67,L,-474,81,L,-424,81,L,-424,54
};

void draw_machine(uint8_t scale)
{
    const uint16_t pts = sizeof(data)/sizeof(int);
    for(uint16_t point=0; point < pts; point += 3)
    {
        int instr = data[point];
        int tgt_x = data[point+1];
        int tgt_y = data[point+2];
        int x = -tgt_x * scale / 64 + 2047;
        int y = -tgt_y * scale / 64 + 2047;
        if(instr==M)
        {
            moveto(x, y);
        }
        else
        {
            lineto(x, y);
        }
    }
}

void test_machine()
{
    for(int i=0; i<256; i++)
    {
      draw_machine(i);
    }
    long start = millis();
    while (millis() - start < 3000)
    {
        draw_machine(255);
    }
    for(int i=256; i>0; i--)
    {
      draw_machine(i);
    }
}
