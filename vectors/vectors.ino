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
      rotating text
      text in from serial port (including line breaks etc)
      bubbles
      switch function based on grounding one of the input pins 29/30/31/32
*/

#include <TimeLib.h>
#include "hershey_font.h"
#include "teensydac.h"
#include "quadrature.h"
#include "vector.h"


#define DIGITAL_FUNCTION_PIN_SINE     29
#define DIGITAL_FUNCTION_PIN_BOWDITCH 30
#define DIGITAL_FUNCTION_PIN_CUBE     31
#define DIGITAL_FUNCTION_PIN_CLOCK    32


const uint16_t  ch = 32;

uint8_t         fontScale = 9;



uint16_t xCenter = 2047;
uint16_t yCenter = 2047;


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
  // set the Time library to use Teensy 3.0's RTC to keep time
  setSyncProvider(getTeensy3Time);

  Serial.begin(115200);
/*  
while (!Serial);  // Wait for Arduino Serial Monitor to open
  delay(100);
  if (timeStatus()!= timeSet) {
    Serial.println("Unable to sync with the RTC");
  } else {
    Serial.println("RTC has set the system time");
  }
  */
  
    pinMode(DIGITAL_FUNCTION_PIN_SINE, INPUT_PULLUP);
    pinMode(DIGITAL_FUNCTION_PIN_BOWDITCH, INPUT_PULLUP);
    pinMode(DIGITAL_FUNCTION_PIN_CUBE, INPUT_PULLUP);
    pinMode(DIGITAL_FUNCTION_PIN_CLOCK, INPUT_PULLUP);
    
    analogWriteResolution(dacRes);    
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

  if (Serial.available()) {
    time_t t = processSyncMessage();
    if (t != 0) {
      Teensy3Clock.set(t); // set the RTC
      setTime(t);
    }
  }

  // line_test();
  box_test();
  circle_test();
  // function_test();
  bowditch_test();
  // clock_test();
  machine_test();
  spiral_test();
  text_test_1();
  // text_test_2();
  cube_test();
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
  int xc = xCenter;
  int yc = yCenter;
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
    Quadrature arc(rotate_degrees, 180);
    float radius = 120 * expf((float)start_pos/480);
    int x = radius + xCenter;
    int y = yCenter;
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
        xx = arc.cos() * radius + xCenter;
        yy = arc.sin() * radius + yCenter;
        line(x, y, xx, yy);
        x = xx;
        y = yy;
    }
    moveto(xCenter, yCenter);
    radius = 120 * expf((float)start_pos/480);
    return radius;
}


void circle_test() {
  // test circle drawing
  for (int m = 0;  m < 2; m++) {
    for (float r = 1; r < 2000; r *= 1.03) {
      quad_circle(xCenter, yCenter, r);
    }
    for (float r = 2000; r > 1 ; r *= 0.97) {
      quad_circle(xCenter, yCenter, r);
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
      quad_circle(xCenter, yCenter, rad);
    }
    spiral(0, end, -rot);
  }
  // open out the center as a circle
  for(int rot=15; rot<1500; rot += 15)
  {
    float rad = spiral(rot, 1500, -rot);
    quad_circle(xCenter, yCenter, rad);
  }

}


/*
 * **************************************************************************************
*/

void text_test_1() {
  // display font really big
  uint8_t scale = 80;
  uint16_t xx = xCenter;
  uint16_t yy = yCenter - vSpace(scale) / 4;
  char b[2];
  for (char c = 32; c < 127; c++)
  {
    String Q = String(c);
    Q.toCharArray(b, 2);
    xx = xCenter - size_string(b, scale) / 2 ;
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
      uint16_t yy = yCenter + 200;
      uint16_t xx = xCenter ;
      xx = xCenter - size_string(caps, int(s)) / 2;
      draw_string(caps, xx, yy, s);

      yy = yy - vSpace(s);
      xx = xCenter - size_string(lc, int(s)) / 2;
      draw_string(lc, xx, yy, s);

      yy = yy - vSpace(s);
      xx = xCenter - size_string(syma, int(s)) / 2;
      draw_string(syma, xx, yy, s);

      yy = yy - vSpace(s);
      xx = xCenter - size_string(symb, int(s)) / 2;
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
        int x = -tgt_x * scale / 64 + xCenter;
        int y = -tgt_y * scale / 64 + yCenter;
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

void machine_test()
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


/*
 * **************************************************************************************
*/

void bowditch_test()
{
    // Bowditch aka Lissajous figures
    int wavelen1 = 301;
    int wavelen2 = 200;
    
    Quadrature pendulum1(0, wavelen1, 2000);
    Quadrature pendulum2(0, wavelen2, 2000);
    moveto(pendulum1.cos() + xCenter, pendulum2.sin() + yCenter);
    long start = millis();
    while (millis() - start < 10000)
    {
        lineto(pendulum1.cos() + xCenter, pendulum2.sin() + yCenter);
        pendulum1.step();
        pendulum2.step();
    }
}


/*
 * **************************************************************************************
*/


void project(Point3 *vertices, Point2 *dest, uint8_t count)
{
  // Project from 3d to 2d (and scale for display)
  // For the trivial version, camera at Z=-inf, just copy across the x & y.
  // Add a fraction of 'z' for some perspective.
  for(uint8_t i=0; i<count; i++)
  {
    dest[i].x = vertices[i].x * (1000 + 20 * vertices[i].z) + xCenter;
    dest[i].y = vertices[i].y * (1000 + 20 * vertices[i].z) + yCenter;
  }
}

void cube_test()
{
    // rotating cube

    // default cube points are drawn in sequence
    // let's call them (x=left/right y=up/down z=front/back)
    //      ____        
    //     /    /     LUB  RUB        6   7
    //   .----./ |   LUF  RUF        4   5
    //   .    .  |
    //   .    . /     LDB    RDB      2   3
    //   .____./     LDF    RDF      0   1
    //
    // draw sequence is 
    // 0-4-5-1-3-7-6-2-0-1 3-2 5-7 6-4
    //
    Point3 v[8] = {
      //      x    y    z
      Point3(-1.0, -1.0, -1.0),
      Point3( 1.0, -1.0, -1.0),
      Point3(-1.0, -1.0,  1.0),
      Point3( 1.0, -1.0,  1.0),
      Point3(-1.0,  1.0, -1.0),
      Point3( 1.0,  1.0, -1.0),
      Point3(-1.0,  1.0,  1.0),
      Point3( 1.0,  1.0,  1.0),
    };
    Point2 p[8];

    // Three separate rotations
    Quadrature qx(0,  360, 0.05);
    Quadrature qy(0,  807, 0.03);
    Quadrature qz(0, 1023, 0.02);

    long start = millis();
    while (millis() - start < 50000)
    {
        // Project the 3d points onto the 2d space
        project(v, p, 8);
        // Draw the edges
        moveto(p[0].x, p[0].y);
        lineto(p[4].x, p[4].y);
        lineto(p[5].x, p[5].y);
        lineto(p[1].x, p[1].y);
        lineto(p[3].x, p[3].y);
        lineto(p[7].x, p[7].y);
        lineto(p[6].x, p[6].y);
        lineto(p[2].x, p[2].y);
        lineto(p[0].x, p[0].y);
        lineto(p[1].x, p[1].y);
        moveto(p[3].x, p[3].y);
        lineto(p[2].x, p[2].y);
        moveto(p[5].x, p[5].y);
        lineto(p[7].x, p[7].y);
        moveto(p[6].x, p[6].y);
        lineto(p[4].x, p[4].y);

        // Step the rotation amounts
        qx.step();
        qy.step();
        qz.step();
        float ax = qx.sin();
        float ay = qy.sin();
        float az = qz.sin();
        
        // Rotate each vertex by a small amount, around the center of the cube (which conveniently is 0,0)
        Matrix3 tx = {1, 0, 0, 0, cosf(ax), -sinf(ax), 0, sinf(ax), cosf(ax)};
        Matrix3 ty = {cosf(ay), 0, sinf(ay), 0, 1, 0, -sinf(ay), 0, cosf(ay)};
        Matrix3 tz = {cosf(az), -sinf(az), 0, sinf(az), cosf(az), 0, 0, 0, 1};
        for(uint8_t i=0; i<8; i++)
        {
            v[i].rotate(tx);
            v[i].rotate(ty);
            v[i].rotate(tz);
        }        
    }
}


/*
 * **************************************************************************************
*/

void function_test()
{
    // Sine wave
    int wavelen = 400;
    long start = millis();
    while (millis() - start < 10000)
    {
        Quadrature wave(0, wavelen);
        int v = xCenter;
        moveto(0, v);
        for(uint16_t x=0; x<1024; x++)
        {
            lineto(x << 2, wave.sin() * 2000 + yCenter);
            wave.step();
        }
    }

    // Square wave
    while (millis() - start < 20000)
    {
        Quadrature wave(0, wavelen);
        int v = 0;
        moveto(0, v);
        for(uint16_t x=0; x<1024; x++)
        {
            int w = (wave.sin() > 0) ? 4048 : 47;
            if(v==w)
            {
                lineto(x << 2, w);
            } else
            {
                moveto(x << 2, w);
            }
            v = w;
            wave.step();
        }
    }

    // Triangle
    while (millis() - start < 30000)
    {
        Quadrature wave(0, wavelen);
        int v = 47;
        moveto(0, v);
        for(uint16_t x=0; x<1024; x++)
        {
            v += (wave.sin() > 0) ? 16 : -16;
            lineto(x << 2, v);
            wave.step();
        }
    }
}


/*
 * **************************************************************************************
*/

time_t getTeensy3Time()
{
  return Teensy3Clock.get();
}

/*  code to process time sync messages from the serial port   */
#define TIME_HEADER  "T"   // Header tag for serial time sync message

unsigned long processSyncMessage() {
  unsigned long pctime = 0L;
  const unsigned long DEFAULT_TIME = 1357041600; // Jan 1 2013 

  if(Serial.find(TIME_HEADER)) {
     pctime = Serial.parseInt();
     return pctime;
     if( pctime < DEFAULT_TIME) { // check the value is a valid time (greater than Jan 1 2013)
       pctime = 0L; // return 0 to indicate that the time is not valid
     }
  }
  return pctime;
}

void draw_clock()
{
    // Draw the clock face
    uint16_t radf = 2000;  // clock face
    float rad0 = 0.95;  // outer end of ticks
    float rad1 = 0.925;  // inner end of 1-ticks
    float rad5 = 0.875;  // inner end of 5-ticks
    float rad15 = 0.8;  // inner end of 15-ticks
    Quadrature face(0, 360, radf);
    uint16_t ox = face.sin() + 2048;
    uint16_t oy = face.cos() + 2048;
    for(uint16_t i=0; i<360; i++)
    {
        face.step();
        float x = face.sin();
        float y = face.cos();
        uint16_t cx = x + 2048;
        uint16_t cy = y + 2048;
        line(ox, oy, cx, cy);
        if(i % 90 == 0)       // 15-minute marks
        {
            moveto(x * rad0 + 2048, y * rad0 + 2048);
            lineto(x * rad15 + 2048, y * rad15 + 2048);
            lineto(x * rad0 + 2048, y * rad0 + 2048);
            lineto(x * rad15 + 2048, y * rad15 + 2048);
        }
        else if(i % 30 == 0)  // 5-minute marks
        {
            moveto(x * rad0 + 2048, y * rad0 + 2048);
            lineto(x * rad5 + 2048, y * rad5 + 2048);          
            lineto(x * rad0 + 2048, y * rad0 + 2048);
        }
        else if(i % 6 == 0)  // 1-minute marks
        {
            moveto(x * rad0 + 2048, y * rad0 + 2048);
            lineto(x * rad1 + 2048, y * rad1 + 2048);
        }
        ox = cx;
        oy = cy;
    }
    // Draw the hands
    // Draw the seconds hand
}


void clock_test()
{
    long start = millis();
    while (millis() - start < 10000)
    {
        draw_clock();
    }
}


/*
 * **************************************************************************************
*/

void bubbles_test()
{
  // Draw a collection of bubbles.  They start small and then pop.
  Quadrature * bubbles[10];

  
}
