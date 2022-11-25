#define NumPairs 3
#define NumLEDPins 2
#define CHECK_PIN_FREQ 100
#define PIN_IO_DELAY 50
#define ANALOG_HIGH 1000 //>1000 seems to be a valid signal connect
#define pulseRight
#define RAND_MAX NumPairs - 1
#define LOCK_RELAY_PIN A5
#define LOCK_RELAY_INIT LOW

int brightnessCounter = 1;
bool onOff = true;
bool gameWon = false;
bool winSequenceDone = false;

int winConditions = 0;

typedef struct {
  int red;
  int blue;
  int green;
  //char colorName[];
} color;

struct LED
{
  int rPin;
  int gPin;
  int bPin;
  color color;
};

typedef struct
{
  int inPin;
  byte outPin;
  struct LED rgb;
} pinPair;

pinPair pinPairs[NumPairs] = {
  //RED Pair
  {A2, 2, {5, 6, 7}},
  //GREEN Pair
  {A3, 3, {8, 9, 10}},
  //BLUE Pair
  {A4, 4, {11, 12, 13}}
};


color colors[] = {
  //RED
  {255, 0, 0},
  //BLUE
  {0, 0, 255},
  //GREEN
  {0, 255, 0}/*,
  //YELLOW
  {255, 255, 0},

  {255, 0, 255},

  {255, 255, 255},

  {0, 255, 255}*/
};


void setup() {
  randomSeed(analogRead(1));
  Serial.begin(9600);
  shuffleColors();
  for (int x = 0; x < NumPairs; x++) {
    //Serial.println("Initiated pin");
    pinPair initiatePin = pinPairs[x];
    pinMode(initiatePin.outPin, OUTPUT);

    //Add a color
    pinPairs[x].rgb.color = colors[x];
    //pinMode(initiatePin.inPin, INPUT);
  }

  //TODO SET ALL PINS OFF/LO/WHATEVER
  //SET RGB LED FLASH STATES
}

void loop() {
  // put your main code here, to run repeatedly:
  //Check pin states
  if (!gameWon) {
    delay(CHECK_PIN_FREQ);

    brightnessCounter++;
    onOff = !onOff;
    winConditions = 0;

    for (int x = 0; x < NumPairs; x++) {
      pinPair currentPin = pinPairs[x];
      //Serial.println("----------BEGIN-------------");
      //Serial.print("Reading Pin Pair: " );
      //Serial.print(currentPin.inPin);
      //Serial.print(", ");
      //Serial.println(currentPin.outPin);


      bool currentCheck = checkLoop(currentPin);
      if (currentCheck) {
        // gameWon = true;
        ledOn(currentPin.rgb);
        winConditions++;
      }
      else {
        gameWon = false;
        pulse(currentPin.rgb);
      }
      //Serial.print("Current State: ");
      //Serial.println(currentCheck ? "ON" : "OFF");
      //Serial.println("-----------END------------\n");
    }
    
    if (winConditions == NumPairs){
      gameWon = true;
      }
  }
  else if (!winSequenceDone) {
    winSequence();
    Serial.println("win sequence");
  }
}

bool checkLoop(pinPair checkPin) {

  bool success = false;
  //SET HIGHPIN
  digitalWrite(checkPin.outPin, HIGH);

  delay(PIN_IO_DELAY);

  //check if inputPin is High
  int readValue = analogRead(checkPin.inPin);

  if (readValue > ANALOG_HIGH) {
    //Serial.print("HIGH READ PIN: ");
    //Serial.println(checkPin.inPin);
    success =  true;
  }

  //TURN IT AT ALL OFF
  digitalWrite(checkPin.outPin, LOW);
  return success;
}

void winSequence() {
  pinMode(LOCK_RELAY_PIN, OUTPUT);
  analogWrite(LOCK_RELAY_PIN, !LOCK_RELAY_INIT);
  winSequenceDone = true;
}


void shuffleColors() {
  const int colorCount = sizeof colors / sizeof colors[0];
  for (int i = 0; i < colorCount; i++) {
    int n = random(0, colorCount);  // Integer from 0 to colorCount-1
    color temp = colors[n];
    colors[n] =  colors[i];
    colors[i] = temp;
  }
}

void ledOn(LED led) {
  int bright = brightnessCounter % 4 + 1;
  pinMode(led.rPin,   OUTPUT);
  pinMode(led.gPin, OUTPUT);
  pinMode(led.bPin,  OUTPUT);
  analogWrite(led.rPin, led.color.red);
  analogWrite(led.gPin, led.color.green );
  analogWrite(led.bPin, led.color.blue);
}

void ledOff(LED led) {
  analogWrite(led.rPin, LOW);
  analogWrite(led.gPin, LOW);
  analogWrite(led.bPin, LOW);
}

void pulse(LED led) {
  onOff ? ledOn(led) : ledOff(led);
}
