// Display pin connections
int clockPin = 9;      
int dataPin  = 7;      
int latchPin = 8;       

// 7-segment patterns for numbers 0-8 (common anode, MSBFIRST: DP,G,F,E,D,C,B,A)
const byte DISPLAY_NUM[] = { 
  0b11000000,   // 0
  0b11111001,   // 1
  0b10100100,   // 2
  0b10110000,   // 3
  0b10011001,   // 4
  0b10010010,   // 5
  0b10000010,   // 6
  0b11111000,   // 7
  0b10000000    // 8
};

void setup() {
  pinMode(latchPin, OUTPUT);
  pinMode(clockPin, OUTPUT);
  pinMode(dataPin, OUTPUT);
  Serial.begin(9600);
  Serial.println("Display test: counting 1-8");
}

void loop() {
  // Simple for loop: show numbers 1 through 8
  for (int i = 1; i <= 8; i++) {
    Serial.print("Showing: ");
    Serial.println(i);
    
    // Update 7-segment display via 74LS595
    digitalWrite(latchPin, LOW);                          // Prepare to send data
    shiftOut(dataPin, clockPin, MSBFIRST, DISPLAY_NUM[i]); // Send segment pattern
    digitalWrite(latchPin, HIGH);                         // Latch and display
    
    delay(1000); // Wait half a second before next number
  }
}
