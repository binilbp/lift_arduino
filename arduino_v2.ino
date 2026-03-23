const int limitFloor1Pin = 2;    
const int limitFloor2Pin = 3;    
const int limitFloor3Pin = 4;   

void setup() {
  Serial.begin(9600);
  
  pinMode(limitFloor1Pin, INPUT);
  pinMode(limitFloor2Pin, INPUT);
  pinMode(limitFloor3Pin, INPUT);

  Serial.println("Starting limit switches check up program in 3 secs");
  delay(3000); 
  Serial.println("Mannualy trigger each limiting switch");
}


void loop() {

  if (digitalRead(limitFloor1Pin)){
    Serial.println("limit floor 1 triggered");
  }
  else if (digitalRead(limitFloor2Pin)){
    Serial.println("limit floor 2 triggered");
  } 
  else if (digitalRead(limitFloor3Pin)){
    Serial.println("limit floor 3 triggered");
  } 
}