#include <Servo.h>
#include <LiquidCrystal.h>

// Entrance sensor
const int entranceTrig = 9;
const int entranceEcho = 8;

// Parking slot 1
const int slot1Trig = 11;
const int slot1Echo = 10;

// Parking slot 2
const int slot2Trig = 13;
const int slot2Echo = 12;

// COMPONENT PINS:
const int servoPin = 7;
const int buzzerPin = 2;

const int green1 = 5;
const int green2 = 3;

// LCD 16x2
LiquidCrystal lcd(A0, A1, A2, A3, A4, A5);

Servo gate;

// Vehicle detection distance in centimeters
const int detectionDistance = 20;

// Servo angles
const int gateOpenAngle = 90;
const int gateClosedAngle = 0;

// Gate timing
const unsigned long minimumOpenTime = 1500;
const unsigned long maximumOpenTime = 8000;

// LCD welcome message duration
const unsigned long welcomeDuration = 2000;

// sistemos būsenos:
enum GateState {
  GATE_CLOSED,
  GATE_OPEN
};

GateState gateState = GATE_CLOSED;

// Time when gate was opened
unsigned long gateOpenedAt = 0;

// Prevent repeated detection of the same vehicle
bool carHandled = false;

// LCD MESSAGE:
unsigned long messageStartedAt = 0;
bool welcomeMessageActive = false;

// Remember last LCD content
String lastLine1 = "";
String lastLine2 = "";

// Atstumo matavimai
long getDistance(int trigPin, int echoPin) {
  // Make sure trigger is LOW
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);

  // Send 10 microsecond ultrasonic pulse
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);

  // Measure echo:
  long duration = pulseIn(echoPin, HIGH, 30000);

  // No echo received:
  if (duration == 0) return 999;

  // Convert microseconds to centimeters:
  return duration / 58;
}

// LCD DISPLAY:
void printLine(int row, String text) {
  // Limit text to 16 characters
  if (text.length() > 16) text = text.substring(0, 16);

  // Fill remaining characters with spaces
  while (text.length() < 16) text += " ";

  lcd.setCursor(0, row);
  lcd.print(text);
}

void updateLCD(String line1, String line2) {
  // Update LCD only when text changes
  if (line1 != lastLine1 || line2 != lastLine2) {
    printLine(0, line1);
    printLine(1, line2);

    lastLine1 = line1;
    lastLine2 = line2;
  }
}


// GATE CONTROL:
void openGate() {
  gate.write(gateOpenAngle);
  gateState = GATE_OPEN;
  gateOpenedAt = millis();
}


void closeGate() {
  gate.write(gateClosedAngle);
  gateState = GATE_CLOSED;
}

void setup() {
  pinMode(entranceTrig, OUTPUT);
  pinMode(entranceEcho, INPUT);

  pinMode(slot1Trig, OUTPUT);
  pinMode(slot1Echo, INPUT);

  pinMode(slot2Trig, OUTPUT);
  pinMode(slot2Echo, INPUT);

  pinMode(green1, OUTPUT);
  pinMode(green2, OUTPUT);

  pinMode(buzzerPin, OUTPUT);

  gate.attach(servoPin);
  closeGate();

  lcd.begin(16, 2);
  updateLCD("SMART PARKING", "System Ready");
  delay(2000);
}

void loop() {
  long distance1 = getDistance(slot1Trig, slot1Echo);
  
  // Small delay prevents ultrasonic interference
  delay(30);

  long distance2 = getDistance(slot2Trig, slot2Echo);
  delay(30);

  long entranceDistance = getDistance(entranceTrig, entranceEcho);

  bool occupied1 = distance1 < detectionDistance;
  bool occupied2 = distance2 < detectionDistance;

  int freePlaces = 0;

  if (!occupied1) freePlaces++;

  if (!occupied2) freePlaces++;

  digitalWrite(green1, !occupied1);
  digitalWrite(green2, !occupied2);

  // ENTRANCE DETECTION:
  bool carDetected = entranceDistance < detectionDistance;

  unsigned long currentTime = millis();

  // WELCOME MESSAGE TIMER:
  if (welcomeMessageActive) {
    if (currentTime - messageStartedAt >= welcomeDuration) {
      welcomeMessageActive = false;
    }
  }
  
  // atidaromi vartai:
  if (gateState == GATE_OPEN) {
    bool minimumTimePassed = currentTime - gateOpenedAt >= minimumOpenTime;
    bool maximumTimePassed = currentTime - gateOpenedAt >= maximumOpenTime;

    if (maximumTimePassed ||(minimumTimePassed && !carDetected)) {
      closeGate();
      
      // Gate cycle finished:
      carHandled = true;
    }

    // =============================
    // LCD WHILE GATE IS OPEN
    // =============================
    if (welcomeMessageActive) {
      updateLCD("WELCOME!", String("Vacant: ") + freePlaces);
    } else {
      updateLCD("GATE OPEN", String("Vacant: ") + freePlaces);
    }
  } else {
    // Reset detection only after the vehicle leaves the entrance:
    if (!carDetected) carHandled = false;

    // nėra laisvų vietų:
    if (freePlaces == 0) {
      updateLCD("PARKING FULL", "No vacant spaces");
      
      // Sound buzzer only once for the detected vehicle:
      if (carDetected && !carHandled) {
        tone(buzzerPin, 500, 500);
        carHandled = true;
      }
    } else {
      // Vehicle detected and this vehicle has not been handled yet:
      if (carDetected && !carHandled) {
        updateLCD("WELCOME!", String("Vacant: ") + freePlaces);

        tone(buzzerPin, 1000, 200); // Short confirmation sound
        openGate(); // opens gate

        // Start welcome message timer:
        welcomeMessageActive = true;
        messageStartedAt = currentTime;

        carHandled = true; // prevents repeated opening
      } else if (!welcomeMessageActive) {
        updateLCD("VACANT SPACES", String("Available: ") + freePlaces);
      }
    }
  }
 
  delay(50); // Small delay before next loop
}
