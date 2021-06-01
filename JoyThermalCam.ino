/*************
 * LIBRARIES
 *************/
 
 // Joystick:
#include "Adafruit_seesaw.h"

// Oled display:
#include <SPI.h>
#include <Wire.h>
#include <Adafruit_GFX.h>
#include <Adafruit_SSD1306.h>

// IR sensor:
// #include <Wire.h>
#include <Adafruit_AMG88xx.h>

// lidar sensor:
#include "Adafruit_VL53L0X.h"

/****************
 * GLOBAL SETUP
 ****************/

// JOYSTICK AND BUTTONS GLOBAL SETUP:

Adafruit_seesaw ss;
#define BUTTON_RIGHT 6
#define BUTTON_DOWN  7
#define BUTTON_LEFT  9
#define BUTTON_UP    10
#define BUTTON_SEL   14
uint32_t button_mask = (1 << BUTTON_RIGHT) | (1 << BUTTON_DOWN) | 
                (1 << BUTTON_LEFT) | (1 << BUTTON_UP) | (1 << BUTTON_SEL);
#define IRQ_PIN   14
int last_x = 0, last_y = 0;

// OLED DISPLAY GLOBAL SETUP:

Adafruit_SSD1306 display = Adafruit_SSD1306(128, 32, &Wire);
#define BUTTON_A 15
#define BUTTON_B 32
#define BUTTON_C 14

// IR SENSOR GLOBAL SETUP:

Adafruit_AMG88xx amg;
float pixels[AMG88xx_PIXEL_ARRAY_SIZE];

// LIDAR SENSOR GLOBAL SETUP:
Adafruit_VL53L0X lox = Adafruit_VL53L0X();

// GENERAL PROGRAM GLOBAL VARIABLES:
const char* modes[] = {"distance", "temp.chang", "interval.t", "manual"};
const char* parameters[] = {"(mm) = ","(degC) = ","(ms) = ", "onClick[A]"};
int currentRow = 0;
int currentMode = 0;
int mm = 500;
int degC = 5;
int ms = 1000;
bool cameraActive = false;
int distRead;
int degCRead = amg.readThermistor();
int lastDegC = degCRead;
int timeRead = millis();
int lastTime = timeRead;
bool manualShutter = false;

// PROGRAM METHODS:

// indexUpdt to streamline the process of updating active menu item indices:
int indexUpdt(int currentVal, bool negative = false, int minval = 0, int maxval = 3, int stepval = 1, int nsteps = 1){
  if(negative == true){nsteps *= -1;}
  if(currentVal + stepval*nsteps > maxval){currentVal = minval;}
  else if(currentVal + stepval*nsteps < minval){currentVal = maxval;}
  else{currentVal += stepval*nsteps;}
  return currentVal;
}

int readInput(uint32_t buttons){
  //read joystick values:
  int x = ss.analogRead(2);
  int y = ss.analogRead(3);
  
  // interpret joystick values and button presses:
  if(x < 200 || (! (buttons & (1 << BUTTON_UP)) || !digitalRead(BUTTON_B))){ // up
    // update current row in menu
    currentRow = indexUpdt(currentRow,true);}
  else if(x > 823 || (! (buttons & (1 << BUTTON_DOWN)) || !digitalRead(BUTTON_C))){ // down
    // update current row in menu
    currentRow = indexUpdt(currentRow);}
  if(y < 200 || (! (buttons & (1 << BUTTON_LEFT)))){ // left
    // if the mode row is active, update the current mode
    if(currentRow == 0){currentMode = indexUpdt(currentMode,true);}
    // if the parameter row is active, update the current parameter
    if(currentRow == 1){
      if(parameters[currentMode] == "(mm) = "){mm=indexUpdt(mm,true,10,800,10);}
      else if(parameters[currentMode] == "(degC) = "){degC=indexUpdt(degC,true,1,80);}        
      else if(parameters[currentMode] == "(ms) = "){ms=indexUpdt(ms,true,0,60*1000,100);}}
    // if the parameter value row is active and the manualShutter is activated, record image
    if((currentRow == 2 || currentRow == 1) && manualShutter && cameraActive){snapPhoto();}
    // if the camera activation row is active, update the camera activation status
    if(currentRow == 3){cameraActive = !cameraActive; delay(100);}}
  else if(y > 823 || (! (buttons & (1 << BUTTON_RIGHT)) || ! (buttons & (1 << BUTTON_SEL)) || !digitalRead(BUTTON_A))){ // right
    // if the mode row is active, update the current mode
    if(currentRow == 0){currentMode = indexUpdt(currentMode);}
    // if the parameter row is active, update the current parameter
    if(currentRow == 1){
      if(parameters[currentMode] == "(mm) = "){mm = indexUpdt(mm,false,10,800,10);}
      else if(parameters[currentMode] == "(degC) = "){degC = indexUpdt(degC,false,1,80);}
      else if(parameters[currentMode] == "(ms) = "){ms = indexUpdt(ms,false,0,60*1000,100);}}
    // if the parameter value row is active and the manualShutter is activated, record image
    if((currentRow == 2 || currentRow == 1) && manualShutter && cameraActive){snapPhoto();}
    // if the camera activation row is active, update the camera activation status
    if(currentRow == 3){cameraActive = !cameraActive; delay(100);}}
   return x, y, currentRow, currentMode, mm, degC, ms;
}

void updateOled(){
  display.clearDisplay();
  
  if(currentRow == 0){display.print("<");}
  display.print("Trigger: ");
  display.print(modes[currentMode]);
  if(currentRow == 0){display.print(">");}
  display.println();
  
  if(currentRow == 1){display.print("<");}
  display.print("Param: ");
  display.print(parameters[currentMode]);
  if(parameters[currentMode] == "(mm) = "){display.print(mm);}
  else if(parameters[currentMode] == "(degC) = "){display.print(degC);}
  else if(parameters[currentMode] == "(ms) = "){display.print(ms);}
  if(currentRow == 1){display.print(">");}
  display.println();

  if(currentRow == 2){display.print("<");}
  display.print("Par.val: ");
  if(parameters[currentMode] == "(mm) = "){display.print(distRead);}
  else if(parameters[currentMode] == "(degC) = "){display.print(degCRead-lastDegC);}
  else if(parameters[currentMode] == "(ms) = "){display.print(timeRead-lastTime);}
  if(currentRow == 2){display.print(">");}
  display.println("");
  
  if(currentRow == 3){display.print("<");}
  display.print("Cam state: ");
  if(cameraActive == true){display.print("active");}
  else{display.print("inactive");}
  if(currentRow == 3){display.println(">");}
  
  display.setCursor(0,0);
  display.display();
}

void runCamera(){
  if(cameraActive == true){
    switch(currentMode){
    case 0:
      VL53L0X_RangingMeasurementData_t measure;
      Serial.println("Reading a measurement... ");
      lox.rangingTest(&measure, false); // pass in 'true' to get debug data printout!
      if (measure.RangeStatus != 4) {distRead=measure.RangeMilliMeter; distRead=measure.RangeMilliMeter;} 
      else {Serial.println(" out of range "); distRead=mm+1;}
      if(distRead <= mm){snapPhoto();}

      // if read distance < mm, record image
      // Serial.println("c0");
      // Serial.println(distRead);
      // Serial.println(mm);
      break;
    case 1:
      degCRead = amg.readThermistor();

      if((degCRead - lastDegC) >= degC){snapPhoto(); lastDegC=degCRead;}
      
      // if readDegC - prevDegC > degC parameter, record image
      // Serial.println("c1");
      // Serial.println(degCRead);
      // Serial.println(lastDegC);
      break;
      
    case 2:
      timeRead = millis();
      // Serial.println(timeRead-lastTime);
      if((timeRead-lastTime)>=ms){snapPhoto();lastTime=timeRead;}
      
      // prevtime = record time using millis, if prevtime + ms < currenttime using millis, then prevtime = currenttime and record image
      // Serial.println("c2");
      break;
    case 3: // the actual code controlling this has to be in the readInput() function
      // if the user presses Right or A or an equivalent, record image:
      //Serial.println("c3");
      break;
      }
    }
  }
  void snapPhoto(){
    //read all the pixels
    amg.readPixels(pixels);
    
    // print all the pixels (cycle through each pixel value)
    Serial.print("[");
    for(int i=1; i<=AMG88xx_PIXEL_ARRAY_SIZE; i++){
      if((i+7)%8 == 0){Serial.print("[");}
      Serial.print(pixels[i-1]);
      if(i%8 != 0){Serial.print(", ");}
      if(i%8 == 0){Serial.print("]");}
      if(i%8 == 0 && i != AMG88xx_PIXEL_ARRAY_SIZE){Serial.println(",");} 
      else if(i%8 == 0){Serial.println();}
    }
    Serial.println("]");
    Serial.println();
}
/****************
 * PROGRAM BODY
 ****************/

void setup() {
  Serial.begin(115200);
  while(!Serial) {
    delay(10);
  }

  // JOYSTICK SETUP:

  if(!ss.begin(0x49)){
    Serial.println("ERROR! seesaw not found");
    while(1);
  }
  ss.pinModeBulk(button_mask, INPUT_PULLUP);
  ss.setGPIOInterrupts(button_mask, 1);

  pinMode(IRQ_PIN, INPUT);
  
  // OLED DISPLAY SETUP:
  
  // SSD1306_SWITCHCAPVCC = generate display voltage from 3.3V internally
  display.begin(SSD1306_SWITCHCAPVCC, 0x3C); // Address 0x3C for 128x32
  
  // Clear the buffer.
  display.clearDisplay();
  display.display();
  
  // button pinModes:
  pinMode(BUTTON_A, INPUT_PULLUP);
  pinMode(BUTTON_B, INPUT_PULLUP);
  pinMode(BUTTON_C, INPUT_PULLUP);
  
  // Initialise Oled text display:
  display.setTextSize(1);
  display.setTextColor(SSD1306_WHITE);
  display.setCursor(0,0);
  display.print("<Trigger: ");
  display.print(modes[currentMode]);
  display.println(">");
  display.print("Param: ");
  display.println(parameters[currentMode]);
  display.println("Par.val: 500");
  display.println("Cam state: inactive");
  display.setCursor(0,0);
  display.display(); // actually display all of the above

  // IR SENSOR SETUP:
  Serial.println(F("AMG88xx pixels"));
  bool status;
    
  // default settings
  status = amg.begin();
  if (!status) {
    Serial.println("Could not find a valid AMG88xx sensor, check wiring!");
    while (1);
  }
  delay(100); // let sensor boot up

  // LIDAR SENSOR SETUP:
  if (!lox.begin()) {
    Serial.println(F("Failed to boot VL53L0X"));
    while(1);
  }
}

void loop() {
  uint32_t buttons = ss.digitalReadBulk(button_mask);
  int x, y = 0;
  x, y, currentRow, currentMode, mm, degC, ms = readInput(buttons);
  if(currentMode == 3){manualShutter=true;}else{manualShutter=false;}
  updateOled();
  runCamera();
  delay(100);
}
