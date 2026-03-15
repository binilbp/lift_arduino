// display settings
int clockPin = 9;      
int dataPin  = 7;      
int latchPin = 8;       

const int DISPLAY_NUM[] = { 
  0b11000000,   // Number 0 in binary: 11000000
  0b11111001,   // Number 1 in binary: 11111001
  0b10100100,   // Number 2 in binary: 10100100
  0b10110000   // Number 3 in binary: 10110000
};


const int limitFloor1Pin = 2;    
const int limitFloor2Pin = 3;    
const int limitFloor3Pin = 4;   

const int btnCall1Pin = A2;   
const int btnCall2Pin = A3;   
const int btnCall3Pin = A4;    

const int motorUpPin = 12;        
const int motorDownPin = 13;      

// variable to track lift state
enum ElevatorState {
  IDLE,         //door close
  MOVING_UP,
  MOVING_DOWN,  
};
ElevatorState currentState = IDLE;

int currentFloor = 1; // default starting assumption
int targetFloor = 0;  // 0 means no active target
int lastDisplayedFloor = -1; //help avoid unneccessary display output 


void setup() {
  Serial.begin(9600);

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
  // //output 0 to display
  // digitalWrite(latchPin, LOW);
  // shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[0]); 
  // digitalWrite(latchPin, HIGH);
  
  // while (digitalRead(limitFloor1Pin) == LOW && digitalRead(limitFloor2Pin) == LOW && digitalRead(limitFloor3Pin) == LOW) {
  //   Serial.println("Moving to nearby lower station");
  //   digitalWrite(motorDownPin, HIGH);
  //   currentState = MOVING_DOWN; //set state as moving down
  // }
  
  // Serial.println("Lift reached a station");
  // // ensure outputs are low after homing
  // digitalWrite(motorUpPin, LOW);
  // digitalWrite(motorDownPin, LOW);
  // currentState = IDLE;  //set state as idle

}


void loop() {
  // Count from 0 to 3
  for (int i = 0; i <= 3; i++) {
    
    Serial.print("Testing Display Floor: ");
    Serial.println(i);

    // 1. Pull latch LOW so the LEDs don't change while you are sending data
    digitalWrite(latchPin, LOW);
    
    // 2. Send the binary data from your array to the 74LS595
    shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[i]);
    
    // 3. Pull latch HIGH to lock the data and turn on the display
    digitalWrite(latchPin, HIGH);
    
    // 4. Wait for 2 seconds (2000 milliseconds) before showing the next number
    delay(2000); 
  }
}
//   //update currentFloor and display
//   if (digitalRead(limitFloor1Pin)){
//     if (currentFloor != 1) {Serial.println("limit floor 1 triggered");}
//     currentFloor = 1;
//   }
//   else if (digitalRead(limitFloor2Pin)){
//     if (currentFloor != 2) {Serial.println("limit floor 2 triggered");}
//     currentFloor = 2;
//   } 
//   else if (digitalRead(limitFloor3Pin)){
//     if (currentFloor != 3) {Serial.println("limit floor 3 triggered");}
//     currentFloor = 3;
//   } 
//   updateDisplay();


//   // recieve floor requests only when IDLE
//   if (currentState == IDLE) {
//     if (digitalRead(btnCall1Pin)){
//       targetFloor = 1;
//       Serial.println("Floor 1 button pressed");
//     }
//     else if (digitalRead(btnCall2Pin)){
//       targetFloor = 2;
//       Serial.println("Floor 2 button pressed");
//     }
//     else if (digitalRead(btnCall3Pin)){
//       targetFloor = 3;
//       Serial.println("Floor 3 button pressed");
//     }
//   }


//   // MAIN LOGIC
//   switch (currentState) {
//     case IDLE:
//       // process target floor
//       if (targetFloor != 0) {
//         if (targetFloor == currentFloor) {
//           // already on same floor, open the doors.
//           Serial.println("Already on same floor, nothing to do");
//           targetFloor = 0; 
//         } else if (targetFloor > currentFloor) {
//           Serial.println("Lift moving UP");
//           currentState = MOVING_UP;
//         } else {
//           Serial.println("Lift moving DOWN");
//           currentState = MOVING_DOWN;
//         }
//       }
//       break;

//     case MOVING_UP:
//       // if reached target floor
//       if (currentFloor == targetFloor) {
//         Serial.println("Reached target floor");
//         targetFloor = 0;
//         currentState = IDLE; 
//       }
//       break;

//     case MOVING_DOWN:
//       // if reached target floor
//       if (currentFloor == targetFloor) {
//         Serial.println("Reached target floor");
//         targetFloor = 0;
//         currentState = IDLE; 
//       }
//       break;
    
//     default:
//       // for safety
//       currentState = IDLE;
//       break;
//   }

//   // motor logic 
//   if (currentState == MOVING_UP) {
//     digitalWrite(motorUpPin, HIGH);
//     digitalWrite(motorDownPin, LOW);
//   } 
//   else if (currentState == MOVING_DOWN) {
//     digitalWrite(motorUpPin, LOW);
//     digitalWrite(motorDownPin, HIGH);  
//   } 
//   else { // IDLE
//     digitalWrite(motorUpPin, LOW);
//     digitalWrite(motorDownPin, LOW);   
//   }
// }

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
    //update the lastDisplayedFloor
    lastDisplayedFloor = currentFloor;
  }
}