// display settings
int clockPin = 9;      
int dataPin  = 7;      
int latchPin = 8;       

const int DISPLAY_NUM[] = { 
  0b11000000,   // Number 0 in binary: 11000000
  0b11111001,   // Number 1 in binary: 11111001
  0b10100100,   // Number 2 in binary: 10100100
  0b10110000    // Number 3 in binary: 10110000
};

// limit pins
const int limitFloor1Pin = 2;    
const int limitFloor2Pin = 3;    
const int limitFloor3Pin = 4;   

// ultrasonic sensor pins
const int trigPin = 5;
const int echoPin = 6;

// buttons outside lift
const int outbtnCall1Pin = A0;   
const int outbtnCall2Pin = A1;   
const int outbtnCall3Pin = A2;    

// buttons inside lift (Fixed names)
const int inbtnCall1Pin = A3;   
const int inbtnCall2Pin = A4;   
const int inbtnCall3Pin = A5; 

// motor pins
const int motorUpPin = 12;        
const int motorDownPin = 13;      

// variable to track lift state
enum ElevatorState {
  IDLE,         
  MOVING_UP,
  MOVING_DOWN,  
};
ElevatorState currentState = IDLE;

int currentFloor = 1; // default starting assumption
int targetFloor = 0;  // 0 means no active target
bool isInsideRequest = false; // Tracks if the current target is from inside
int lastDisplayedFloor = -1; // help avoid unnecessary display output 

// setup loop
void setup() {
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(limitFloor1Pin, INPUT);
  pinMode(limitFloor2Pin, INPUT);
  pinMode(limitFloor3Pin, INPUT);

  // setup ultrasonic pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  // setup outside buttons
  pinMode(outbtnCall1Pin, INPUT);
  pinMode(outbtnCall2Pin, INPUT);
  pinMode(outbtnCall3Pin, INPUT);

  // setup inside buttons
  pinMode(inbtnCall1Pin, INPUT);
  pinMode(inbtnCall2Pin, INPUT);
  pinMode(inbtnCall3Pin, INPUT);

  pinMode(motorUpPin, OUTPUT);
  pinMode(motorDownPin, OUTPUT);
  
  Serial.println("Starting Setup in 4 secs...");
  delay(4000); 
  
  Serial.println("Setup Running");

  // homing sequence: output 0 to display
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[0]); 
  digitalWrite(latchPin, HIGH);
  
  // find a floor if floating
  while (digitalRead(limitFloor1Pin) == LOW && digitalRead(limitFloor2Pin) == LOW && digitalRead(limitFloor3Pin) == LOW) {
    Serial.println("Moving to nearby lower station");
    digitalWrite(motorDownPin, HIGH);
    currentState = MOVING_DOWN;
  }
  
  Serial.println("Lift reached a station");
  digitalWrite(motorUpPin, LOW);
  digitalWrite(motorDownPin, LOW);
  currentState = IDLE;  
}

// Helper Function to get Ultrasonic Distance
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2; // Calculate distance in cm
  return distance;
}

// helper Function for the 7-Segment Display
void updateDisplay() {
  if (lastDisplayedFloor != currentFloor){
    digitalWrite(latchPin, LOW);
    if (currentFloor >= 0 && currentFloor <= 3) {
        shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[currentFloor]); 
    }
    digitalWrite(latchPin, HIGH);

    Serial.print("Display updating to show Floor: ");
    Serial.println(currentFloor);
    lastDisplayedFloor = currentFloor;
  }
}

// main loop
void loop() {
  // update currentFloor and display
  if (digitalRead(limitFloor1Pin)){
    if (currentFloor != 1) {Serial.println("limit floor 1 triggered");}
    currentFloor = 1;
  }
  else if (digitalRead(limitFloor2Pin)){
    if (currentFloor != 2) {Serial.println("limit floor 2 triggered");}
    currentFloor = 2;
  } 
  else if (digitalRead(limitFloor3Pin)){
    if (currentFloor != 3) {Serial.println("limit floor 3 triggered");}
    currentFloor = 3;
  } 
  updateDisplay();

  // Receive floor requests only when IDLE and no current target exists
  if (currentState == IDLE && targetFloor == 0) {
    
    // Check OUTSIDE buttons
    if (digitalRead(outbtnCall1Pin)){
      targetFloor = 1;
      isInsideRequest = false;
      Serial.println("Floor 1 OUTSIDE button pressed");
    }
    else if (digitalRead(outbtnCall2Pin)){
      targetFloor = 2;
      isInsideRequest = false;
      Serial.println("Floor 2 OUTSIDE button pressed");
    }
    else if (digitalRead(outbtnCall3Pin)){
      targetFloor = 3;
      isInsideRequest = false;
      Serial.println("Floor 3 OUTSIDE button pressed");
    }
    // Check INSIDE buttons
    else if (digitalRead(inbtnCall1Pin)){
      targetFloor = 1;
      isInsideRequest = true;
      Serial.println("Floor 1 INSIDE button pressed");
    }
    else if (digitalRead(inbtnCall2Pin)){
      targetFloor = 2;
      isInsideRequest = true;
      Serial.println("Floor 2 INSIDE button pressed");
    }
    else if (digitalRead(inbtnCall3Pin)){
      targetFloor = 3;
      isInsideRequest = true;
      Serial.println("Floor 3 INSIDE button pressed");
    }
  }

  // MAIN LOGIC
  switch (currentState) {
    case IDLE:
      // process target floor
      if (targetFloor != 0) {
        if (targetFloor == currentFloor) {
          Serial.println("Already on same floor, nothing to do");
          targetFloor = 0; 
        } else {
          // If the request came from inside, check for ghosts before moving
          if (isInsideRequest) {
            int distance = getDistance();
            if (distance <= 0 || distance >= 15) {
               Serial.println("Ghost call detected! No one inside. Canceling request.");
               targetFloor = 0; 
               break; // Exit the switch, stay IDLE
            }
          }

          // If we passed the ghost check (or it's an outside request), start moving
          if (targetFloor > currentFloor) {
            Serial.println("Lift moving UP");
            currentState = MOVING_UP;
          } else {
            Serial.println("Lift moving DOWN");
            currentState = MOVING_DOWN;
          }
        }
      }
      break;

    case MOVING_UP:
    case MOVING_DOWN:
      // if reached target floor
      if (currentFloor == targetFloor) {
        Serial.println("Reached target floor");
        targetFloor = 0;
        currentState = IDLE; 
      }
      break;
    
    default:
      currentState = IDLE;
      break;
  }

  // motor logic 
  if (currentState == MOVING_UP) {
    digitalWrite(motorUpPin, HIGH);
    digitalWrite(motorDownPin, LOW);
  } 
  else if (currentState == MOVING_DOWN) {
    digitalWrite(motorUpPin, LOW);
    digitalWrite(motorDownPin, HIGH);  
  } 
  else { // IDLE
    digitalWrite(motorUpPin, LOW);
    digitalWrite(motorDownPin, LOW);   
  }
}