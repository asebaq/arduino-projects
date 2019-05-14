//sensors declration:
int mq5Value = 0;
int pirRead = 0;
int pirState = LOW;
const int flameMin = 0;
const int flameMax = 1024;

// pins declration:
int tempPin = A0;
int flamePin = A1;
int mq5Pin = A2;
int GLPin = 13;
int RLPin = 8;
int BLPin = 11;
int pirPin = 50;
int YLPin = 6;

void setup() {
  pinMode(pirPin, INPUT);
  pinMode(GLPin, OUTPUT);
  pinMode(RLPin, OUTPUT);
  pinMode(YLPin, OUTPUT);
  Serial.begin(9600);
}

void loop() {
  // read the sensor on analog A0:
  int tempValue = analogRead(tempPin);
  int flameRead = analogRead(flamePin);
  int mq5Read = analogRead(mq5Value);
  pirRead = digitalRead(pirPin);
  float cel = tempValue * 500 / 2048;
  
  //flame sensor setup:
  int range = map(flameRead, flameMin, flameMax, 0, 3);
  switch (range) {
    case 0:    // A fire closer than 1.5 feet.
      Serial.println("Near Fire");
      digitalWrite(GLPin, HIGH);
      break;
    case 1:    // A fire between 1-3 feet.
      Serial.println("Distant Fire");
      break;
    case 2:    // No fire detected.
      Serial.println("No Fire");
      digitalWrite(GLPin, LOW);
      break;
  }
  //temperature sensor setup:
  if (cel >= 50 || cel <= 0){
    digitalWrite(BLPin, HIGH);
  }
  Serial.print("Temperature = ");
  Serial.print(cel);
  Serial.print(" C");
  Serial.println();
  

  //gas sensor setup:
  Serial.println(mq5Read, DEC);
  if (mq5Read > 65) {
    digitalWrite(RLPin, HIGH);
  }
  else {
    digitalWrite(RLPin, LOW);
  }
  // pir sensor setup:
  if (pirRead == HIGH) {
    digitalWrite(YLPin, HIGH);
    if (pirState == LOW) {
      Serial.println("Motion Detected !!!");
      pirState = HIGH;
    }
  } else {
    digitalWrite(YLPin, LOW);
    if (pirState == HIGH) {
      Serial.println("Motion Ended.");
      pirState = LOW;
    }
  }
  delay(1000);
}
