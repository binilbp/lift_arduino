// display settings
int clockPin = 9;      
int dataPin  = 7;      
int latchPin = 8;       

const int DISPLAY_NUM[] = { 
  202,   // Number 0 in binary: 11000000
  249,   // Number 1 in binary: 11111001
  164,   // Number 2 in binary: 10100100
  176,   // Number 3 in binary: 10110000
};



const int limitFloor1Pin = 2;    
const int limitFloor2Pin = 3;    
const int limitFloor3Pin = 4;   
   

int limitDoorOpenPin = 5;
int limitDoorClosedPin = 6;

int btnOpenDoorPin = A0;
int btnCloseDoorPin = A1;
const int btnCall1Pin = A2;   
const int btnCall2Pin = A3;   
const int btnCall3Pin = A4;    
    

 
int motorDoorOpenPin = 10;
int motorDoorClosePin = 11;

const int motorUpPin = 12;        
const int motorDownPin = 13;      

// variable to track lift state
enum ElevatorState {
  IDLE,         //door close
  MOVING_UP,
  MOVING_DOWN,  
  BOARDING      //door open
};
ElevatorState currentState = IDLE;

int currentFloor = 1; // default starting assumption
int targetFloor = 0;  // 0 means no active target

//variables handle door open timings
unsigned long doorOpenedTime = 0;
const unsigned long doorOpenDuration = 5000; // Stay open for 5 seconds


void setup() {
  Serial.begin(9600);

  pinMode(btnOpenDoorPin, INPUT);
  pinMode(btnCloseDoorPin, INPUT);
  pinMode(motorDoorOpenPin, OUTPUT);
  pinMode(motorDoorClosePin, OUTPUT);
  
  pinMode(limitDoorOpenPin, INPUT);
  pinMode(limitDoorClosedPin, INPUT);


  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  pinMode(limitFloor1Pin, INPUT);
  pinMode(limitFloor2Pin, INPUT);
  pinMode(limitFloor3Pin, INPUT);

  
  pinMode(btnCall1Pin, INPUT);
  pinMode(btnCall2Pin, INPUT);
  pinMode(btnCall3Pin, INPUT);


  pinMode(motorUpPin, OUTPUT);
  pinMode(motorDownPin, OUTPUT);
  Serial.println("Starting Setup in 4 secs");
  delay(4000); //initial delay for setup ease
  

  Serial.println("Setup Runing");

  //homing sequence: find a floor if floating
  while (digitalRead(limitFloor1Pin) == LOW && digitalRead(limitFloor2Pin) == LOW && digitalRead(limitFloor3Pin) == LOW) {
    Serial.println("Moving to nearby lower station");
    digitalWrite(motorDownPin, HIGH);
    digitalWrite(latchPin, LOW);
    shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[0]); 
    digitalWrite(latchPin, HIGH);
    currentState = MOVING_DOWN; //set state as moving down
  }
  
  Serial.println("Lift reached a station");
  // ensure outputs are low after homing
  digitalWrite(motorUpPin, LOW);
  digitalWrite(motorDownPin, LOW);
  currentState = IDLE;  //set state as idle

}

void loop() {
  //get the current millis timing
  unsigned long currentMillis = millis();
  

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


  // recieve floor requests only when IDLE
  if (currentState == IDLE) {
    if (digitalRead(btnCall1Pin)){
      targetFloor = 1;
      Serial.println("Floor 1 button pressed");
    }
    else if (digitalRead(btnCall2Pin)){
      targetFloor = 2;
      Serial.println("Floor 2 button pressed");
    }
    else if (digitalRead(btnCall3Pin)){
      targetFloor = 3;
      Serial.println("Floor 3 button pressed");
    }
  }


  // MAIN LOGIC
  switch (currentState) {
    case IDLE:
      // manual Door Open
      if (digitalRead(btnOpenDoorPin) == HIGH) {
        digitalWrite(motorDoorOpenPin, HIGH);
        digitalWrite(motorDoorClosePin, LOW);
        doorOpenedTime = currentMillis; // start the stopwatch
        currentState = BOARDING;
        Serial.println("Mannually opening door now");
      }
      // process target floor
      else if (targetFloor != 0) {
        if (targetFloor == currentFloor) {
          // already on same floor, open the doors.
          Serial.println("Already on same floor, opening door now");
          digitalWrite(motorDoorOpenPin, HIGH);
          digitalWrite(motorDoorClosePin, LOW);
          doorOpenedTime = currentMillis;
          currentState = BOARDING;
          targetFloor = 0; 
        } else if (targetFloor > currentFloor) {
          Serial.println("Lift moving UP");
          currentState = MOVING_UP;
        } else {
          Serial.println("Lift moving DOWN");
          currentState = MOVING_DOWN;
        }
      }
      break;

    case BOARDING:
      // stop opening motor if fully open
      if (digitalRead(limitDoorOpenPin) == HIGH) {
        digitalWrite(motorDoorOpenPin, LOW);
        Serial.println("Door fully open");
      }

      // automatically close doors after 5 seconds OR if close button is pressed
      if ((currentMillis - doorOpenedTime >= doorOpenDuration) || (digitalRead(btnCloseDoorPin) == HIGH)) {
        digitalWrite(motorDoorOpenPin, LOW); // ensure open motor is off
        digitalWrite(motorDoorClosePin, HIGH); // start closing
        Serial.println("Closing the door");
      }

      // stop closing motor and return to IDLE when fully closed
      if (digitalRead(limitDoorClosedPin) == HIGH) {
        digitalWrite(motorDoorClosePin, LOW);
        currentState = IDLE; 
        Serial.println("Door fully close");
      }
      break;

    case MOVING_UP:
      // if reached target floor
      if (currentFloor == targetFloor) {
        Serial.println("Reached target floor");
        targetFloor = 0;
        
        // open door after reaching the floor
        digitalWrite(motorDoorOpenPin, HIGH);
        digitalWrite(motorDoorClosePin, LOW);
        doorOpenedTime = currentMillis;
        currentState = BOARDING; 
        Serial.println("Automatic door opening now");
      }
      break;

    case MOVING_DOWN:
      // if reached target floor
      if (currentFloor == targetFloor) {
        Serial.println("Reached target floor");
        targetFloor = 0;
        
        // start opening doors immediately
        digitalWrite(motorDoorOpenPin, HIGH);
        digitalWrite(motorDoorClosePin, LOW);
        doorOpenedTime = currentMillis;
        currentState = BOARDING; 
      }
      break;
    
    default:
      // for safety
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

// helper Function for the 7-Segment Display
void updateDisplay() {
  digitalWrite(latchPin, LOW);
  if (currentFloor >= 0 && currentFloor <= 3) {
      shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[currentFloor]); 
  }
  digitalWrite(latchPin, HIGH);
}