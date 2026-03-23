  
// Ultrasonic pins
const int trigPin = 5;
const int echoPin = 6;


void setup() {
  Serial.begin(9600);

  
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
  delay(500);

}
