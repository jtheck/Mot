/**************************************************************************
 Two-screen Hourglass and eyeball example.
  Ft. Single & Dual SSD1306 OLED Screens,
    Onboard Tilt, External IMU MPU6050 Tilt
 **************************************************************************/
//MM PROJECT Eyeballs
//#define MM_IS_UNO
//#define MM_HAS_1SCREEN
#define MM_HAS_2SCREENS


#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

//#include <ArduinoSound.h>

#define SCREEN_WIDTH 128 // OLED display width, in pixels
#define SCREEN_HEIGHT 64 // OLED display height, in pixels

// #define VIRTUAL_WIDTH 256 // double wide
// #define VIRTUAL_HEIGHT 64

// ARDUINO NANO 33 BLE
#define OLED_RESET     -1 // Reset pin # (or -1 if sharing Arduino reset pin)
#define SCREEN_ADDRESS 0x3C ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);

#ifdef MM_HAS_2SCREENS
#define SCREEN2_ADDRESS 0x3D ///< See datasheet for Address; 0x3D for 128x64, 0x3C for 128x32
Adafruit_SSD1306 display2(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#endif

struct Eye {
  int x;
  int y;
  int lidUpper;
  int lidLower;
  int irisRadius;
  int pupilRadius;
  int brow;
  int lines[15];
};


struct Eye eye1 = {
    .x = SCREEN_WIDTH/2,
    .y = SCREEN_HEIGHT/2,
    .lidUpper = 12,
    .lidLower = SCREEN_HEIGHT - 1,
    .irisRadius = 24,
    .pupilRadius = 9,
    .brow = 1,
    // .lines = {} 
  };

#ifdef MM_HAS_2SCREENS
struct Eye eye2 = {
    .x = SCREEN_WIDTH/2,
    .y = SCREEN_HEIGHT/2,
    .lidUpper = 9,
    .lidLower = SCREEN_HEIGHT - 3,
    .irisRadius = 24,
    .pupilRadius = 8,
    .brow = -3,
    // .lines = {} 
  };
#endif


struct Keyframe {
  int time;
  int state;
};

const int frameCt = 6;
struct Animation {
  unsigned long startTime;
  int currentFrame;
  struct Keyframe keyframes[frameCt];
};

struct Gesture {
  // int 

};

struct Animation testAnim = {
  .startTime = 0,
  .currentFrame = 0,
  .keyframes = {
    {2, 9},
    {40, 19},
    {50, 60},
    // {55, SCREEN_HEIGHT - 12},
    {60, 19},
    {175, 12},
    {4255, 9}
  }
};

struct Timing {
  unsigned long frameStart;
  unsigned long framePrev;
};
struct Timing timer = {0, 0};



void setup() {
  Serial.begin(9600);
  while(!Serial);
  
  randomSeed(analogRead(0));
  
 
  if(!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    // Serial.println(F("SSD1306 allocation failed"));
    // for(;;); // Don't proceed, loop forever
  }
  display.clearDisplay();
  initEye(&eye1);

  
  #ifdef MM_HAS_2SCREENS
  if(!display2.begin(SSD1306_SWITCHCAPVCC, SCREEN2_ADDRESS)) {
    // Serial.println(F("SSD1306 allocation failed"));
    // for(;;); // Don't proceed, loop forever
  }
  display2.clearDisplay();
  initEye(&eye2);
  #endif

  if (Serial) Serial.println("Begin!");
}

void loop() {
  // Timing
  timer.frameStart = millis();
  timer.framePrev = timer.frameStart;


  // Serial.print((String)elapsed+" v "+testAnim.keyframes[testAnim.currentFrame].time+" frame "+testAnim.currentFrame);
  int elapsed = timer.frameStart - testAnim.startTime;
  if (elapsed > testAnim.keyframes[testAnim.currentFrame].time){
    eye1.lidUpper = testAnim.keyframes[testAnim.currentFrame].state;
    if (testAnim.currentFrame < frameCt-1){
      testAnim.currentFrame++;
    } else {
      testAnim.currentFrame = 0;
      testAnim.startTime = timer.frameStart;
    }
  }




  display.clearDisplay(); // Clear the display buffer
  #ifdef MM_HAS_2SCREENS
  display2.clearDisplay();
  #endif
    

  drawEye(eye1, &display);
  #ifdef MM_HAS_2SCREENS
  drawEye(eye2, &display2);
  #endif 


  display.display(); // Show the display buffer on the screen
  #ifdef MM_HAS_2SCREENS
  display2.display();
  #endif
}





void initEye(struct Eye* eye){
  for (int i=0; i<15; i++){
    eye->lines[i] = random(360);
  }
}


void drawEye(struct Eye eye, Adafruit_SSD1306 *disp){
  disp->drawCircle(eye.x, eye.y, eye.irisRadius, 1); 
  disp->drawCircle(eye.x, eye.y, eye.irisRadius+1, 1);
  disp->fillCircle(eye.x, eye.y, eye.pupilRadius, 1);

  for (int i=0; i<15; i++){
    disp->drawLine(eye.x, eye.y, eye.x+cos(eye.lines[i])*eye.irisRadius, eye.y+sin(eye.lines[i])*eye.irisRadius, 1);
  }

  for (int i=0; i<eye.lidUpper; i++){
    disp->drawFastHLine(0, i, SCREEN_WIDTH, 0);
  }
  disp->drawFastHLine(0, eye.lidUpper, SCREEN_WIDTH, 1);
  
  for (int i=eye.lidLower; i<SCREEN_HEIGHT; i++){
    disp->drawFastHLine(0, i, SCREEN_WIDTH, 0); 
  }
  disp->drawFastHLine(0, eye.lidLower, SCREEN_WIDTH, 1);

  disp->drawLine(0, 5+eye.brow, SCREEN_WIDTH, 5-eye.brow, 1);
}