// DisplayPins
int clockPin = 9;      
int dataPin  = 7;      
int latchPin = 8;       

const int DISPLAY_NUM[] = { 
  0b11000000,   // 0 
  0b11111001,   // 1
  0b10100100,   // 2
  0b10110000    // 3
};

// LimitPins
const int limitFloor1Pin = 2;    
const int limitFloor2Pin = 3;    
const int limitFloor3Pin = 4;   

// UltrasonicPins
const int trigPin = 5;
const int echoPin = 6;

// ButtonPins
const int outbtnCall1Pin = A0;   
const int outbtnCall2Pin = A1;   
const int outbtnCall3Pin = A2;    

const int inbtnCall1Pin = A3;   
const int inbtnCall2Pin = A4;   
const int inbtnCall3Pin = A5; 

// MotorPins 
const int motorUpPin = 12;
const int motorDownPin = 13;


// Lift States Enum definiton
enum ElevatorState {
  IDLE,         
  MOVING_UP,
  MOVING_DOWN,
  WAITING        
};
ElevatorState currentState = IDLE;

// Memory to store floor requests
bool insideRequests[3]  = {false, false, false}; 
bool outsideRequests[3] = {false, false, false}; 

// Door Open time tracker
unsigned long doorOpenTime = 0;          
const unsigned long doorWaitTime = 3000; // 3 secs door open time

int currentFloor = 1; 
int currentDirection = 0; 
int lastDisplayedFloor = -1; 


// SetupLoop
void setup() {
  Serial.begin(9600);

  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(limitFloor1Pin, INPUT);
  pinMode(limitFloor2Pin, INPUT);
  pinMode(limitFloor3Pin, INPUT);

  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  pinMode(outbtnCall1Pin, INPUT);
  pinMode(outbtnCall2Pin, INPUT);
  pinMode(outbtnCall3Pin, INPUT);
  pinMode(inbtnCall1Pin, INPUT);
  pinMode(inbtnCall2Pin, INPUT);
  pinMode(inbtnCall3Pin, INPUT);

  pinMode(motorUpPin, OUTPUT);
  pinMode(motorDownPin, OUTPUT);
  
  Serial.println("Starting Setup in 4 secs...");
  delay(4000); 
  Serial.println("Setup Running");

  // Display 0 for homing sequence
  digitalWrite(latchPin, LOW);
  shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[0]); 
  digitalWrite(latchPin, HIGH);
  
  // Perform homing sequence
  while (digitalRead(limitFloor1Pin) == LOW && digitalRead(limitFloor2Pin) == LOW && digitalRead(limitFloor3Pin) == LOW) {
    Serial.println("Homing: Moving to nearby lower station");
    digitalWrite(motorDownPin, HIGH);
    currentState = MOVING_DOWN;
  }
  
  Serial.println("Lift reached a station. Homing complete.");
  digitalWrite(motorUpPin, LOW);
  digitalWrite(motorDownPin, LOW);
  currentState = IDLE;  
}

// Function to handle lift inside buttons
void handleInsideButton(int floorIndex) {
  if (insideRequests[floorIndex] == false) {
    insideRequests[floorIndex] = true;
    Serial.print("Inside request logged : Floor ");
    Serial.println(floorIndex + 1);
  }
}

// Function to handle lift outside buttons
void handleOutsideButton(int floorIndex) {
  if (outsideRequests[floorIndex] == false) {
    outsideRequests[floorIndex] = true;
    Serial.print("Outside request logged : Floor ");
    Serial.println(floorIndex + 1);
  }
}

// Function to update the 7 segment display 
void updateDisplay() {
  if (lastDisplayedFloor != currentFloor){
    digitalWrite(latchPin, LOW);
    if (currentFloor >= 0 && currentFloor <= 3) {
        shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[currentFloor]); 
    }
    digitalWrite(latchPin, HIGH);
    lastDisplayedFloor = currentFloor;
    
    Serial.print("Display updated to ");
    Serial.println(currentFloor);
  }
}

// Function to get the ultrasonic distance reading from lift
int getDistance() {
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  long duration = pulseIn(echoPin, HIGH, 30000); 
  if (duration == 0) return 999; 
  return duration * 0.034 / 2;
}

// Function to check if lift empty 
bool isElevatorEmpty(){
  int detectionCount = 0;
  int thresholdDist = 8;
  
  // Take 5 quick readings instead of freezing for 1 second
  for (int i = 0; i < 5; i++) {
    if (getDistance() <= thresholdDist) {
      detectionCount++;
    }
  }
  
  // if only 2 detection count passed we return false
  return (detectionCount < 3);
}

void loop() {
  
  // check for button presses
  if (digitalRead(outbtnCall1Pin) == HIGH) handleOutsideButton(0);
  if (digitalRead(outbtnCall2Pin) == HIGH) handleOutsideButton(1);
  if (digitalRead(outbtnCall3Pin) == HIGH) handleOutsideButton(2);

  if (digitalRead(inbtnCall1Pin) == HIGH) handleInsideButton(0);
  if (digitalRead(inbtnCall2Pin) == HIGH) handleInsideButton(1);
  if (digitalRead(inbtnCall3Pin) == HIGH) handleInsideButton(2);

  // check for limit activations
  if (digitalRead(limitFloor1Pin) == HIGH) currentFloor = 1;
  else if (digitalRead(limitFloor2Pin) == HIGH) currentFloor = 2;
  else if (digitalRead(limitFloor3Pin) == HIGH) currentFloor = 3;
  updateDisplay();

  if (currentState != WAITING) {
    // If we are at a floor that is requested in our memory
    if (
        (insideRequests[currentFloor - 1] == true || outsideRequests[currentFloor - 1] == true )
        && 
        (digitalRead(limitFloor1Pin) == HIGH || digitalRead(limitFloor2Pin) == HIGH || digitalRead(limitFloor3Pin) == HIGH)
    ) {
      
      Serial.print("Arrived at Floor: ");
      Serial.println(currentFloor);

      // clear the requests
      insideRequests[currentFloor - 1] = false; 
      outsideRequests[currentFloor - 1] = false;
      
      currentState = WAITING;                  //change to waiting 
      doorOpenTime = millis();                 //start the timer 
      
      // Stop the motors
      digitalWrite(motorUpPin, LOW);
      digitalWrite(motorDownPin, LOW);
      
      Serial.println("Doors Opening...");
    }
  }

  // Check if our 3 seconds are up
  if (currentState == WAITING) {
    if (millis() - doorOpenTime >= doorWaitTime) {
      Serial.println("Doors Closed.");
      currentState = IDLE; // We are done waiting, go back to IDLE to decide next move
    }
  }

  // SCAN ALGORITHM
  if (currentState == IDLE) {

    bool requestAbove = false;
    bool requestBelow = false;

    // Check if elevator empty and clear the insideRequests if empty
    bool elevatorEmpty = isElevatorEmpty();
    if (elevatorEmpty == true ){
      for (int i = 0; i < 3; i++) insideRequests[i] = false;
    }

    for (int i = currentFloor; i < 3; i++) { 
      if (outsideRequests[i] == true || (insideRequests[i] == true && elevatorEmpty == false)) {
        requestAbove = true;
      }
    }
    for (int i = currentFloor - 2; i >= 0; i--) { 
      if (outsideRequests[i] == true || (insideRequests[i] == true && elevatorEmpty == false)) {
        requestBelow = true;
      }
    }

    if (currentDirection == 1 || currentDirection == 0) { 
      if (requestAbove) {
        currentDirection = 1;
        currentState = MOVING_UP;
      } else if (requestBelow) {
        currentDirection = -1; 
        currentState = MOVING_DOWN;
      } else {
        currentDirection = 0; 
      }
    } 
    else if (currentDirection == -1) { 
      if (requestBelow) {
        currentDirection = -1;
        currentState = MOVING_DOWN;
      } else if (requestAbove) {
        currentDirection = 1; 
        currentState = MOVING_UP;
      } else {
        currentDirection = 0; 
      }
    }
  }

  // Motor pins are only updated if we aren't WAITING
  if (currentState == MOVING_UP) {
    digitalWrite(motorUpPin, HIGH);
    digitalWrite(motorDownPin, LOW);
  } 
  else if (currentState == MOVING_DOWN) {
    digitalWrite(motorUpPin, LOW);
    digitalWrite(motorDownPin, HIGH);  
  } 
  else if (currentState == IDLE || currentState == WAITING) { 
    digitalWrite(motorUpPin, LOW);
    digitalWrite(motorDownPin, LOW);   
  }
}
