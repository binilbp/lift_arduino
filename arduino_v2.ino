// Shift register pins
const int clockPin = 9;      
const int dataPin  = 7;      
const int latchPin = 8;       

// Ultrasonic pins
const int trigPin = 5;
const int echoPin = 6;

// Binary configurations for Common Anode display
const byte displayFive = 0b10010010; // Shows '5'
const byte displayBlank = 0b11111111; // Clears the display

void setup() {
  Serial.begin(9600);

  // Setup shift register pins
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  
  // Setup ultrasonic pins
  pinMode(trigPin, OUTPUT);
  pinMode(echoPin, INPUT);

  Serial.println("Starting 15cm Distance Test...");
}

void loop() {
  // 1. Send the ultrasonic ping
  digitalWrite(trigPin, LOW);
  delayMicroseconds(2);
  digitalWrite(trigPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(trigPin, LOW);
  
  // 2. Read the echo
  long duration = pulseIn(echoPin, HIGH);
  int distance = duration * 0.034 / 2; // Calculate distance in cm
  
  // Print to Serial Monitor for debugging
  Serial.print("Distance: ");
  Serial.print(distance);
  Serial.println(" cm");

  // 3. Update the display based on the reading
  digitalWrite(latchPin, LOW);
  
  // Check if someone is 15cm or closer (and ignore 0 which is usually an error)
  if (distance > 0 && distance <= 15) {
    // Target detected within 15cm! Push '5'
    shiftOut(dataPin, clockPin, MSBFIRST, displayFive);
    Serial.println("Target in range: Displaying 5");
  } else {
    // Target is further away or not detected. Blank the display.
    shiftOut(dataPin, clockPin, MSBFIRST, displayBlank);
  }
  
  digitalWrite(latchPin, HIGH);

  // Wait half a second before pinging again
  delay(500); 
}
