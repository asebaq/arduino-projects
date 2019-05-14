/*
 Toothbrush Wear Simulator Project
 4th year Students, Biomedical Department, Faculty of Engineering, Minia University
 Work of: Embeded Team
 */
#include <LiquidCrystal.h>
#include <Keypad.h>
#include <ctype.h>

#define STEPS 1620

// Keypad #rows = 4
const byte ROWS = 4;
// Keypad #columns = 4
const byte COLS = 4; 

// Define the Keymap grid
char keys[ROWS][COLS] = {
    {'1', '2', '3', 'A'},
    {'4', '5', '6', 'B'},
    {'7', '8', '9', 'C'},
    {'*', '0', '#', 'D'}};

// Configure keypad pin connection with Arduino.
// Connect keypad ROW0, ROW1, ROW2 and ROW3 to these Arduino pins.
byte rowPins[ROWS] = {6, 7, 8, 9};
// Connect keypad COL0, COL1 and COL2 to these Arduino pins.
byte colPins[COLS] = {10, 11, 12, 13};

// Create the Keypad and map the pins to the grid
Keypad kpd = Keypad(makeKeymap(keys), rowPins, colPins, ROWS, COLS);

// Configure the LCD pins connection with Arduino.
const int rs = A0, en = A1, d4 = A2, d5 = A3, d6 = A4, d7 = A5; 
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);

// Configure Stepper motor pins
int stepperPin1 = 3;
int stepperPin2 = 4;

// Variable definitions
unsigned int numberOfCycle;
unsigned int delayInSec;
char key;
volatile boolean state = false;
const int buttonSwitch = 2;

unsigned int flag = 0;
unsigned int temp = 0;
bool isInterrupted = false;

void setup() {
  // Configure pin mode
  pinMode(stepperPin1, OUTPUT);
  pinMode(stepperPin2, OUTPUT);
  digitalWrite(stepperPin1, LOW);
  digitalWrite(stepperPin2, LOW);

  pinMode(buttonSwitch, INPUT);
  attachInterrupt(digitalPinToInterrupt(2), ISR_button, RISING);
  // Using a 16*2 LCD display
  lcd.begin(16, 2);
  lcd.print("Toothbrush Wear");  
  lcd.setCursor(0, 1);
  lcd.print("BME 4th Year");
  delay(2000);
  lcd.clear();
}

void loop() {
  key = kpd.getKey(); //storing pressed key value in a char

  // Page flipping system
  switch (flag)
  {
  case 0:
    // 1st page
    numberOfCycle = EnterCycles();
    break;
  case 1:
    // 2nd page
    delayInSec = EnterSpeed();
    break;
  case 2:
    // 3rd page
    RunStepper(numberOfCycle, delayInSec);
    break;
  case 3:
    // 4th and final page
    HasFinished();
    break;
  }
}

// Function for the stepper to rotate #n of cycle with s amount of delay in second 
void RunStepper(int num, int ds) {
  long d = (1000*ds)/(3.0*num);
 
  int i = 1;
  while (i <= num && !isInterrupted)
  {
    lcd.clear();
    lcd.setCursor(0, 0); // set the cursor to column 0, line 0
    lcd.print("Cycle #");
    lcd.setCursor(6, 0); // set the cursor to column 0, line 1
    lcd.print(i);
    for (int j = 0; j < STEPS; j++)
    {
      digitalWrite(stepperPin2, HIGH);
      delayMicroseconds(d);
      digitalWrite(stepperPin2, LOW);
      delayMicroseconds(d);
      // Interrupt
      if (state == true)
      {
        isInterrupted = true;
        break;
      }
    }
    if (isInterrupted == true) {
      HasInterrupted();
      break;
    }
    i++;
  }
  flag++;
}

int EnterCycles()
{
  int digit = 0;
  temp = 0;
  lcd.clear();
  while (key != '*')
  {
    key = kpd.getKey(); //storing pressed key value in a char
    if (isdigit(key)){
      digit = CharToDigit(key);
      temp = (temp * 10) + digit;
    }
    if (key == '#')
    {
      lcd.clear();
      temp = 0;
    }
    lcd.setCursor(0, 0); // set the cursor to column 0, line 0
    lcd.print("Enter cycles:");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 1
    lcd.print(temp);
  }
  flag++;
  return temp;
}

int EnterSpeed()
{
  int digit = 0;
  temp = 0;
  lcd.clear();
  while (key != '*')
  {
    key = kpd.getKey();
    if (isdigit(key))
    {
      digit = CharToDigit(key);
      temp = (temp * 10) + digit;
    }
    if (key == '#')
    {
      lcd.clear();
      temp = 0;
    }
    lcd.setCursor(0, 0); // set the cursor to column 0, line 0
    lcd.print("Enter time: ");
    lcd.setCursor(0, 1); // set the cursor to column 0, line 1
    lcd.print(temp);
  }
  flag++;
  return temp;
}

void HasFinished()
{
  lcd.clear();
  while (key != '*')
  {
    key = kpd.getKey();
    lcd.setCursor(0, 0);
    lcd.print("Finished.");
  }
  flag = 0;
  temp = 0;
  numberOfCycle = 0;
  delayInSec = 0;
}

void HasInterrupted()
{
  flag = -1;
  temp = 0;
  isInterrupted = false;
  numberOfCycle = 0;
  delayInSec = 0;
  state = false;
  digitalWrite(stepperPin1, LOW);
  digitalWrite(stepperPin2, LOW);
  lcd.clear();

  while (key != '*')
  {
    key = kpd.getKey();
    lcd.setCursor(0, 0);
    lcd.print("Interrupt.");
  }
}

void ISR_button()
{
  state = true;
}

int CharToDigit(char character)
{
  char c = character;
  int n = character - '0';
  return n;
}
