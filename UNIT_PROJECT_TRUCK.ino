/* Created by Richard Pu

Truck game for TEJ3M1-02|Thornhill SS

richardpu6@gmail.com
Truck Game is made possible by open source projects: https://docs.google.com/document/d/1fYKcz8IDffW3OJzqPxNy-y38uEFWh4Md5hfnoGhZaYI/edit?usp=sharing
Version 1.5
*/

#include <LiquidCrystal_I2C.h>  // Library for LCD  
#include <ezButton.h> // Library for joystick switch 

LiquidCrystal_I2C lcd(0x27, 16, 2);  

// Pin definitions
int yValue = 0;
int bValue = 0;
ezButton joyButton(13);

#define VRY_PIN  A3
#define UP_THRESHOLD    400
#define DOWN_THRESHOLD  800
#define COMMAND_UP     0x04
#define COMMAND_DOWN   0x08
#define COMMAND_NO     0x00
int command = COMMAND_NO;

const int potPin = 1;
const int potMax = 1024; 

const int speakPin = 3;
const int ledPin = 2;

const int randSeedPin = 0; // Analog pin that is not connected

// Game Variables
const int maxStepDuration = 300; 
const int minStepDuration = 150; 

unsigned long startTime = 0; // timer for score
bool gameStarted = false; //True or false depending on game state

//glyphs for game
const int nGlyphs = 6;
byte glyphs[nGlyphs][8] = {
  { B00000, B01110, B11111, B01010, B00000, B00000, B00000, B00000 }, // Car up
  { B00000, B00000, B00000, B00000, B01110, B11111, B01010, B00000 }, // Car down
  { B00000, B11110, B11111, B01010, B00000, B00000, B00000, B00000 }, // Truck up
  { B00000, B00000, B00000, B00000, B11110, B11111, B01010, B00000 }, // Truck down
  { B10101, B01110, B01110, B10101, B00000, B00000, B00000, B00000 }, // Crash up
  { B00000, B00000, B00000, B10101, B01110, B01110, B10101, B00000 }  // Crash down
};

const int nCarPosition = 4;
const char BLANK=32;
char car2glyphs[nCarPosition][2] = {
  {1,BLANK},{2,BLANK},{BLANK,1},{BLANK,2}
};
char truck2glyphs[nCarPosition][2] = {
  {3,BLANK},{4,BLANK},{BLANK,3},{BLANK,4}
};
char crash2glyphs[nCarPosition][2] = {
  {5,BLANK},{6,BLANK},{BLANK,5},{BLANK,6}
};

const int roadLen = 15; //length of screen not counting car
int road[roadLen]; 
char line_buff[2+roadLen];
int road_index;
int car_pos;
const int maxRoadPos = 3*nCarPosition;
int step_duration;

int crash; // true if crashed
unsigned int crashtime; // millis() when crashed

// Menu variables
unsigned long lastInstructionTime = 0;
const unsigned long instructionDelay = 1500;
int instructionStep = 0;

// Control mode
bool useButtons = false;  // True if using buttons to control the car

// Setup
void setup() {
  noTone(speakPin);  // Make sure piezo is silent

  lcd.init();  // Initialize the LCD
  lcd.backlight();  // Turn on the backlight
  lcd.noBlink();  // Disable cursor blinking
  lcd.clear();  // Clear the screen
  lcd.setCursor(0, 0);  // Set cursor position (0, 0)
  lcd.print("Truck Game");  // Display the game title
  lcd.setCursor(3, 1);  // Move cursor to (3, 1)
  lcd.print("Starting...");  // Display starting message

  pinMode(speakPin, OUTPUT);  // Set speaker pin as output  
  pinMode(ledPin, OUTPUT); // set led to output
  delay(2000);  // Wait 2 seconds
  lcd.clear();  // Clear the screen
}

// Main game loop
void loop() {
  joyButton.loop();
  bValue = joyButton.getState();
  command = COMMAND_NO;
  if (!gameStarted) {  // If the game has not started yet
    unsigned long currentMillis = millis();

    if (currentMillis - lastInstructionTime >= instructionDelay) {
      lastInstructionTime = currentMillis;
      instructionStep++;
      if (instructionStep > 3) {
        instructionStep = 0;
      }

      displayInstructions(instructionStep); //switch between instructions
    }

    // Start the game if joystick is pressed
    if (joyButton.isPressed()) {
      startGame();
    }
  } else {  // If the game is running
    gameLoop();
  }
}

// Display instructions on the LCD
void displayInstructions(int step) {
  lcd.clear();  // Clear the screen
  
  switch (step) {
    case 0:
      lcd.setCursor(0, 0);
      lcd.print("Press joy");
      lcd.setCursor(0, 1);
      lcd.print("stick to start");
      break;
    case 1:
      lcd.setCursor(0, 0);
      lcd.print("Try and avoid");
      lcd.setCursor(0, 1);
      lcd.print("oncoming traffic");
      break;
    case 2:
      lcd.setCursor(0, 0);
      lcd.print("Use joystick or");
      lcd.setCursor(0, 1);
      lcd.print("wheel to move");
      break;
    case 3:
      lcd.setCursor(0, 0);
      lcd.print("Tap joystick");
      lcd.setCursor(0, 1);
      lcd.print("to change mode");
      break;
  }
}

// Start the game
void startGame() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Starting!");
  delay(1000);
  lcd.clear();
  
  crash = false;  // Ensure crash status is reset
  crashtime = 0;  // Reset the crash time
  road_index = 0;
  step_duration = maxStepDuration;
  line_buff[1 + roadLen] = '\0'; // null terminate it
  randomSeed(analogRead(randSeedPin));
  for (int i = 0; i < nGlyphs; i++) {
    lcd.createChar(i + 1, glyphs[i]);
  }
  for (int i = 0; i < roadLen; i++) {
    road[i] = -1;
  }
  analogWrite(speakPin, 0); 
  getSteeringWheel();
  drawRoad();

  startTime = millis();  // Record the start time when the game starts
  
  gameStarted = true;
}

// Main game loop
void gameLoop() {
  unsigned long now = millis();

  // Switch control mode if joystick is pressed
  if (joyButton.isPressed()) {
    useButtons = !useButtons;  // Toggle control mode
    delay(500);  // Debounce time to avoid rapid switching
  }

  if (!crash) {
    getSteeringWheel();
    crash = (car_pos == road[road_index]);
  }

  if (crash) {
    if (crashtime == 0) {  // Only set crashtime when the crash occurs
      crashtime = now;
      drawRoad();
      lcd.setCursor(2, 0);
      lcd.print("Crashed after");
      
      unsigned long gameDuration = (now - startTime) / 1000;  // Calculate game duration
      lcd.setCursor(2, 1);
      lcd.print(gameDuration);  // Display time in seconds
      lcd.print(" seconds.");
    }

    if ((now - crashtime) < 250) {
      analogWrite(speakPin, random(255)); // white noise
    } else {
      analogWrite(speakPin, 0); 
      delay(3000); 
      gameStarted = false;
      return;
    }
    delay(10);
  } else {
    int prev_pos = road[(road_index - 1) % roadLen];
    int this_pos = random(maxRoadPos);
    while (abs(this_pos - prev_pos) < 2) { // don't jam the road
      this_pos = random(maxRoadPos);
    }
    road[road_index] = this_pos;
    road_index = (road_index + 1) % roadLen;
    drawRoad();
    delay(step_duration);
    if (step_duration > minStepDuration) {
      step_duration--; // go faster
    }
  }
}

void getSteeringWheel() {
  if (useButtons) {
    digitalWrite(ledPin, HIGH);
    yValue = analogRead(VRY_PIN);
    if (yValue < UP_THRESHOLD)
    command = command | COMMAND_UP;
    else if (yValue > DOWN_THRESHOLD)
      command = command | COMMAND_DOWN;
    if (command & COMMAND_UP && car_pos > 0) {
      car_pos--;
    }
    if (command & COMMAND_DOWN && car_pos < nCarPosition - 1) {
      car_pos++;
    }
  } else {
    digitalWrite(ledPin, LOW);
    car_pos = map(analogRead(potPin), 0, 1024, 0, nCarPosition);
  }
}

void drawRoad() {
  for (int i = 0; i < 2; i++) {
    if (crash) {
      line_buff[0] = crash2glyphs[car_pos][i];
    } 
    else {
      line_buff[0] = car2glyphs[car_pos][i];
    }
    for (int j = 0; j < roadLen; j++) {
      int pos = road[(j + road_index) % roadLen];
      line_buff[j + 1] = (pos >= 0 && pos < nCarPosition) ? truck2glyphs[pos][i] : BLANK;
    }
    lcd.setCursor(0, i);
    lcd.print(line_buff);
  }
}