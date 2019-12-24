//including the needed header
#include <ESP32Lib.h>
#include <Ressources/Font6x8.h>
#include <Ressources/CodePage437_8x19.h>
#include <GfxWrapper.h>
#include <Fonts/FreeMonoBoldOblique24pt7b.h>
#include <Fonts/FreeSerif9pt7b.h>
#include <Fonts/TomThumb.h>

#define BUTTON0 34
#define BUTTON1 0
#define BUTTON2 35

//pin configuration
#define REDPIN 14
#define GREENPIN 19
#define BLUEPIN 27
#define HSYNCPIN 32
#define VSYNCPIN 33

//VGA Device using an interrupt to unpack the pixels from 4bit to 16bit for the I²S
//This takes some CPU time in the background but is able to fit a frame buffer in the memory
VGA3Bit vga;
GfxWrapper<VGA3Bit> gfx(vga, 320, 240);

// shows title screen
void show_title_screen() {
  vga.setTextColor(vga.RGB(255, 255, 255));
  vga.setCursor(140, 120);
  vga.print("Gameduino");
}

// shows options screen
void show_options_screen() {
  
}

// shows about screen
void show_about_screen() {
  
}

byte prev_state0 = 1, prev_state1 = 1, prev_state2 = 1;
int button_pressed = 0;
void setup()
{
  Serial.begin(115200);
  //enabling double buffering
  vga.setFrameBufferCount(2);
  
  //create screen
  vga.init(vga.MODE640x480.custom(320, 240), REDPIN, GREENPIN, BLUEPIN, HSYNCPIN, VSYNCPIN);

  // initialize buttons
  pinMode(BUTTON0, INPUT_PULLUP); 
  pinMode(BUTTON1, INPUT_PULLUP);
  pinMode(BUTTON2, INPUT_PULLUP);
  
  //setting the font
  vga.setFont(CodePage437_8x19);

  //start title sequence
  show_title_screen();

  while (1) {
    byte curr_state = digitalRead(BUTTON0);
    if (prev_state0 == 1 && curr_state == 0) {
      break;
    }
    prev_state0 = curr_state;
    delay(10);
  }
  
  
}

///draws a bouncing balls
void balls()
{
  Serial.println("in balls");
  //some basic gravity physics
  static VGA3BitI::Color c[4] = {vga.RGB(0, 255, 0), vga.RGB(0, 255, 255), vga.RGB(255, 0, 255), vga.RGB(255, 255, 0)};
  static float y[4] = {50, 50, 50, 50};
  static float x[4] = {20, 20, 20, 20};
  static float vx[4] = {.1, -0.7, .5, -.3};
  static float vy[4] = {0, 1, 2, 3};
  static unsigned long lastT = 0;
  unsigned long t = millis();
  float dt = (t - lastT) * 0.001f;
  lastT = t;
  const int r = 10;
  for (int i = 0; i < 4; i++)
  {
    int rx = r;
    int ry = r;
    vy[i] += -9.81f * dt * 100;
    x[i] += vx[i];
    y[i] += vy[i] * dt;
    //check for boundaries and bounce back
    if (y[i] < r && vy[i] < 0)
    {
      vy[i] = 200 + i * 10;
      ry = y[i];
    }
    if (x[i] < r && vx[i] < 0)
    {
      vx[i] = -vx[i];
      rx = x[i];
    }
    if (x[i] >= vga.xres - r && vx[i] > 0)
    {
      vx[i] = -vx[i];
      rx = vga.xres - x[i];
    }
    //draw a filled ellipse
    vga.fillCircle(x[i], vga.yres - y[i] - 1, r, c[i]);
    vga.circle(x[i], vga.yres - y[i] - 1, r, 0);
  }
}

//mainloop
void loop()
{
  //draw a background
  for (int y = 0; y * 10 < vga.yres; y++)
    for (int x = 0; x * 10 < vga.xres; x++)
      vga.fillRect(x * 10, y * 10, 10, 10, (x + y) & 1 ? vga.RGB(255, 0, 0) : vga.RGB(255, 255, 255));
  //text position
  Serial.println("in loop");
  vga.setCursor(2, 2);
  //black text color no background color
  vga.setTextColor(vga.RGB(0, 0, 0));
  //show the remaining memory
  vga.print(vga.xres);
  vga.print("x");
  vga.println(vga.yres);
  vga.print("free memory: ");
  vga.print((int)heap_caps_get_free_size(MALLOC_CAP_DEFAULT));
  //draw bouncing balls
  balls();
  //show the backbuffer (only needed when using backbuffering)
  vga.show();
}
