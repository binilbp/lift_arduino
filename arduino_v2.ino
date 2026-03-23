
const int motorUpPin = 12;        
const int motorDownPin = 13;    
bool UP = true;  

void setup() {
  Serial.begin(9600);

  pinMode(motorUpPin, OUTPUT);
  pinMode(motorDownPin, OUTPUT);
  Serial.println("Starting motor check program in 3 secs");
  delay(3000); 
  
  Serial.println("Motor auto changes rotating direction every 4 secs");
  
}


void loop() {
  //UP
  if (UP) {
    digitalWrite(motorUpPin, HIGH);
    digitalWrite(motorDownPin, LOW);
    Serial.println("Motor rotating upwards for 4 secs");
    delay(4000);
    UP = !UP;
  }
  //DOWN 
  else { 
    digitalWrite(motorUpPin, LOW);
    digitalWrite(motorDownPin, HIGH);   
    Serial.println("Motor rotating downwards for 4 secs");
    delay(4000);
    UP = !UP;
  }

  //motor cool off  
  digitalWrite(motorUpPin, LOW);
  digitalWrite(motorDownPin, LOW);
  delay(500);
}
