
// buttons outside lift
const int outbtnCall1Pin = A0;   
const int outbtnCall2Pin = A1;   
const int outbtnCall3Pin = A2;    

// buttons inside lift 
const int inbtnCall1Pin = A3;   
const int inbtnCall2Pin = A4;   
const int inbtnCall3Pin = A5;     

// setup loop
void setup() {
  Serial.begin(9600);

  // setup outside buttons
  pinMode(outbtnCall1Pin, INPUT);
  pinMode(outbtnCall2Pin, INPUT);
  pinMode(outbtnCall3Pin, INPUT);

  // setup inside buttons
  pinMode(inbtnCall1Pin, INPUT);
  pinMode(inbtnCall2Pin, INPUT);
  pinMode(inbtnCall3Pin, INPUT);
  
  Serial.println("Running switches check program in 3 secs ...");
  delay(3000); 
}


// main loop
void loop() {
  Serial.println("Press switch to check their working");
    
    // Check OUTSIDE buttons
  if (digitalRead(outbtnCall1Pin)){
    Serial.println("Floor 1 OUTSIDE button pressed");
  }
  else if (digitalRead(outbtnCall2Pin)){
    Serial.println("Floor 2 OUTSIDE button pressed");
  }
  else if (digitalRead(outbtnCall3Pin)){
    Serial.println("Floor 3 OUTSIDE button pressed");
  }
  // Check INSIDE buttons
  else if (digitalRead(inbtnCall1Pin)){
    Serial.println("Floor 1 INSIDE button pressed");
  }
  else if (digitalRead(inbtnCall2Pin)){
    Serial.println("Floor 2 INSIDE button pressed");
  }
  else if (digitalRead(inbtnCall3Pin)){
    Serial.println("Floor 3 INSIDE button pressed");
  }
}