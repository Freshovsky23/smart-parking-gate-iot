#include <LiquidCrystal.h> // Include library for LCD display
LiquidCrystal lcd(2, 3, 4, 5, 6, 7); // Create LCD object and assign pins: RS, E, D4, D5, D6, D7

// Pin definitions – using readable names instead of hardcoded numbers
#define echo 9       // Ultrasonic sensor receiver pin (receives the signal)
#define trig 8       // Ultrasonic sensor transmitter pin (sends the signal)
#define buzzer A5    // Buzzer pin
#define greenLED 11  // Green LED pin (PWM) – indicates the gate is open
#define redLED 10    // Red LED pin (PWM) – indicates the gate is closed
#define button 13    // Button pin – the driver presses it after entering the parking lot
#define whiteLED 12  // White LED pin – turns on when a car is in range

// Global variables – storing the state of the program between loop() executions
bool isGateOpen = false;              // Is the gate currently open? At start: no
int currentState = -1;                // Current LCD state: -1=none, 0=welcome, 1=entering
bool previousButtonState = HIGH;      // Previous button reading (HIGH = not pressed, due to INPUT_PULLUP)
bool buttonBeingPressed = false;      // Has the button been registered and not yet released?

void setup() {
  lcd.begin(20, 4);          // Initialize LCD – 20 columns, 4 rows
  Serial.begin(9600);        // Start serial communication (for debugging)
  pinMode(trig, OUTPUT);     // Trig sends the signal – set as output
  pinMode(echo, INPUT);      // Echo receives the signal – set as input
  pinMode(buzzer, OUTPUT);   // Buzzer plays sound – set as output
  pinMode(greenLED, OUTPUT); // Green LED – set as output
  pinMode(redLED, OUTPUT);   // Red LED – set as output
  pinMode(whiteLED, OUTPUT); // White LED – set as output
  pinMode(button, INPUT_PULLUP); // Button as input with an internal pull-up resistor
                                 // INPUT_PULLUP means: not pressed = HIGH, pressed = LOW

  analogWrite(redLED, 255); // At startup, turn the red LED on fully – gate is closed
}

void loop()
{
  checkRange(2, 15);  // Check if a car is within 2-15 cm from the sensor
  checkButton();      // Check if the driver pressed the button after entering
}

void showWelcomeScreen()
// Displays the welcome screen when no one is at the gate
{
  lcd.clear();                        // Clear the entire LCD screen
  lcd.setCursor(0, 0);                // Set cursor to: column 0, row 0
  lcd.print("Welcome to parking");    // Display text in row 0
  lcd.setCursor(0, 1);                // Move to row 1
  lcd.print("Heilman industries");    // Display text in row 1
  lcd.setCursor(0, 2);                // Move to row 2
  lcd.print("Waiting for");           // Display text in row 2
  lcd.setCursor(0, 3);                // Move to row 3
  lcd.print("a vehicle");             // Display text in row 3
  lcd.blink();                        // Turn on the blinking cursor on the LCD

  digitalWrite(whiteLED, LOW); // Turn off the white LED – no one is there
  digitalWrite(buzzer, LOW);   // Ensure the buzzer is off
}

long measureDistance()
// Measures the distance in centimeters using the HC-SR04 ultrasonic sensor
// Principle of operation: send a short pulse, measure how long the echo takes to return
{
  long duration;
  digitalWrite(trig, LOW);        // Ensure trig is off before measuring
  delayMicroseconds(2);           // Wait 2 microseconds for stability
  digitalWrite(trig, HIGH);       // Send ultrasonic pulse
  delayMicroseconds(10);          // The pulse must last exactly 10 microseconds
  digitalWrite(trig, LOW);        // End the pulse

  duration = pulseIn(echo, HIGH); // Measure the time (in microseconds) for the echo to return
  delay(100);                     // Short pause before the next measurement
  return (duration / 58);         // Convert time to centimeters (speed of sound: ~340m/s, round trip)
}

void checkRange(int a, int b)
// Checks if the measured distance falls within the range (a, b) centimeters
// If yes and the gate is closed – opens the gate and lets the car in
{
  long distance = measureDistance(); // Get current distance from the sensor

  if (distance > a && distance < b && isGateOpen == false)
  // Condition: car is close (2-15 cm) AND the gate is still closed
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Please enter"); // Inform the driver they can enter
    
    // Request for double click split into two rows (fits exactly 20 chars)
    lcd.setCursor(0, 1);
    lcd.print("After entering press");
    lcd.setCursor(0, 2);
    lcd.print("the button 2 times");
    
    lcd.blink();                  // Turn on the blinking cursor

    digitalWrite(whiteLED, HIGH); // Turn on the white LED – car detected

    turnOffLED(redLED);           // Smoothly fade out the red LED (animation)
    turnOnLED(greenLED);          // Smoothly fade in the green LED (animation)

    digitalWrite(buzzer, HIGH);   // Play a beep for 1 second
    delay(1000);
    digitalWrite(buzzer, LOW);    // Turn off the buzzer

    isGateOpen = true; // Remember that the gate is now open
    currentState = 1;  // Set state: car is entering
    Serial.println("GATE_OPENED");
  }
  else if (isGateOpen == false && currentState != 0)
  // Condition: gate is closed AND the screen is not yet showing the welcome message
  // (currentState != 0 prevents continuous refreshing of the welcome screen)
  {
    showWelcomeScreen(); // Display the welcome screen
    currentState = 0;    // Remember that we are showing the welcome screen
  }
}

void checkButton()
// Handles the button that the driver presses after entering the parking lot
// Uses falling edge detection – responds only at the MOMENT of a press,
// not just the fact that the button is held down
{
  bool currentButtonState = digitalRead(button); // Read the current state of the button

  // If the button was released – remove the lock
  // This allows the next press to be detected as a new event
  if (currentButtonState == HIGH)
  {
    buttonBeingPressed = false; // Reset flag – button is free to be used again
  }

  // Detect falling edge: previously HIGH (released), now LOW (pressed)
  // We also check the flag – was this press already handled?
  bool justPressed = false;
  if (previousButtonState == HIGH && currentButtonState == LOW && !buttonBeingPressed)
  {
    delay(50); // Debouncing: wait 50ms for the mechanical contacts to stabilize
               // (mechanical buttons "bounce" for a fraction of a second when pressed)
    if (digitalRead(button) == LOW) // Confirm it's a real press, not a bounce
    {
      justPressed = true;           // Yes – button genuinely pressed
      buttonBeingPressed = true;    // Block further detections until the button is released
    }
  }

  if (isGateOpen == true && justPressed)
  // Close the gate only if: gate is open AND button was just pressed
  {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Closing the gate..."); // Inform about closing
    lcd.blink();                      // Turn on the blinking cursor
    delay(1500);                      // Wait 1.5 seconds (time for the gate to close)

    digitalWrite(whiteLED, LOW); // Turn off the white LED – car has entered

    turnOffLED(greenLED);        // Smoothly fade out the green LED (animation)
    turnOnLED(redLED);           // Smoothly fade in the red LED (animation)

    isGateOpen = false; // Remember that the gate is closed
    currentState = -1;  // Reset state – loop() will return to the welcome screen shortly
    Serial.println("GATE_CLOSED");
  }

  previousButtonState = currentButtonState; // Remember the state for the next loop() iteration
}

void turnOnLED(int color)
// Smoothly brightens the LED passed as an argument (uses PWM)
// The entire fade-in takes about 1.3 seconds (256 steps x 5ms)
{
  for (int i = 0; i <= 255; i++) // From 0 (off) to 255 (full brightness)
  {
    analogWrite(color, i); // Set LED brightness to value i (0-255)
    delay(5);              // Wait 5ms before the next step
  }
}

void turnOffLED(int color)
// Smoothly dims the LED passed as an argument (uses PWM)
// The entire fade-out takes about 1.3 seconds (256 steps x 5ms)
{
  for (int i = 255; i >= 0; i--) // From 255 (full brightness) down to 0 (off)
  {
    analogWrite(color, i); // Set LED brightness to value i (0-255)
    delay(5);              // Wait 5ms before the next step
  }
}