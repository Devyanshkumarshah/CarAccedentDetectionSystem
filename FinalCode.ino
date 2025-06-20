#include <TinyGPS.h>
#include <LiquidCrystal.h>


LiquidCrystal lcd(4, 5, 6, 7, 8, 9);

const int buzzerPin = 3;
const int analogVibrationSensorPin = A0;

int triggerPin = 10; // Trigger pin for ultrasonic sensor
int echoPin = 11;    // Echo pin for ultrasonic sensor

TinyGPS gps; // TinyGPS object for GPS parsing

long lat, lon; // Variables to store latitude and longitude
bool vibration_Status = LOW; // Flag for accident-level vibration
long duration, distance; // Variables for ultrasonic sensor readings

void setup() {
  // Set up pin modes
  pinMode(buzzerPin, OUTPUT); // Buzzer pin as output (for PWM)
  // The digital vibration_sensor pin (12) is no longer directly used for input.
  // Instead, we'll read from analogVibrationSensorPin (A0).
  pinMode(triggerPin, OUTPUT); // Ultrasonic trigger pin as output
  pinMode(echoPin, INPUT);     // Ultrasonic echo pin as input

  // Start serial communication for GPS and debugging
  Serial.begin(9600);
  
  // Initialize LCD
  lcd.begin(16, 2);
  lcd.clear();

  // Initial display messages on LCD
  lcd.print("ACCIDENT DETECTION");
  lcd.setCursor(3, 1); // For a 16x2 LCD, rows are 0 and 1
  lcd.print("SYSTEM");
  delay(1000); // Display for 1 second
}

void loop() {
  // Turn on the buzzer briefly when the vehicle starts (initial indication)
  analogWrite(buzzerPin, 255); // Max volume
  delay(200);
  analogWrite(buzzerPin, 0); // Turn off after brief buzz

  lcd.clear();
  lcd.print("Vehicle Started");
  delay(1000); // Display for 1 second

  // --- CONTINUOUS OPERATION LOOP ---
  // The 'while(1);' that was here previously was a critical bug
  // that stopped the program. It has been removed.
  // The loop() function will now continuously execute.

  // Read analog vibration sensor value
  int vibrationValue = analogRead(analogVibrationSensorPin);

  // Map the raw analog sensor value (0-1023) to a PWM intensity (0-255)
  // You might need to adjust the 'in_min' and 'in_max' of the map function
  // based on the actual output range of your specific analog vibration sensor.
  // For example, if your sensor rarely goes below 100, set in_min to 100.
  // If a very low vibration should produce no sound, set a threshold.
  int buzzerIntensity = map(vibrationValue, 0, 1023, 0, 255);

  // Apply a minimum threshold: if vibration is too low, turn off the buzzer
  // This prevents constant faint buzzing from minor vibrations/noise.
  const int MIN_VIBRATION_THRESHOLD_ANALOG = 50; // Adjust this value (0-1023)
  if (vibrationValue < MIN_VIBRATION_THRESHOLD_ANALOG) {
    buzzerIntensity = 0; // Turn off buzzer
  }

  // Write the calculated PWM intensity to the buzzer pin
  analogWrite(buzzerPin, buzzerIntensity);

  delay(50); // Small delay to prevent too rapid readings and allow sound to play

  // Perform ultrasonic distance measurement
  ultrasonic();

  // Determine if it's an "accident" based on a higher vibration threshold and distance
  // This 'vibration_Status' is now derived from the analog reading
  const int ACCIDENT_VIBRATION_THRESHOLD_ANALOG = 700; // Adjust this value (0-1023)
  vibration_Status = (vibrationValue >= ACCIDENT_VIBRATION_THRESHOLD_ANALOG);

  // Check for accident conditions: significant vibration and close object
  if ((vibration_Status == HIGH) && (distance < 5)) { // distance < 5 (cm)
    delay(100);
    // If accident detected, blast the buzzer at full volume
    analogWrite(buzzerPin, 255);
    delay(500); // Keep buzzer on for a short period to alert

    lcd.clear();
    lcd.print("Accident Detected!");
    lcd.setCursor(3, 1);
    lcd.print("Sending Msg...");
    delay(1000); // Display for 1 second

    // Send SMS via GSM module
    Serial.println("AT+CMGF=1"); // Sets the GSM Module in text Mode
    delay(500); // Give module time to process command
    // Replace with your emergency contact number
    Serial.println("AT+CMGS=\"+918709586243\"\r"); 
    delay(500); // Give module time to process command
    Serial.println("ACCIDENT DETECTED!");
    Serial.println("Please check the location immediately.");
    // End SMS with Ctrl+Z (ASCII 26)
    Serial.write(26); 
    delay(3000); // Wait for SMS to be sent

    // Enter continuous GPS reading loop after accident
    // This 'while(1)' ensures GPS coordinates are repeatedly sent until reset
    while (1) {
      lcd.clear();
      lcd.print("Getting GPS Coords");
      lcd.setCursor(0, 1);
      lcd.print("Sending...");
      gps_read(); // Read and print GPS data
      delay(5000); // Delay before next GPS read to avoid spamming serial
    }
  }
}

// Function to read and print GPS data
void gps_read() {
  // Read available serial data and feed it to TinyGPS parser
  while (Serial.available()) {
    byte gpsChar = Serial.read();
    if (gps.encode(gpsChar)) {
      // If a full GPS sentence is processed, get position
      gps.get_position(&lat, &lon);

      // Print latitude and longitude
      Serial.println("--- GPS Position ---");
      Serial.print("Latitude: ");
      Serial.println((lat * 0.000001), 8); // Convert to float with 8 decimal places
      Serial.print("Longitude: ");
      Serial.println((lon * 0.000001), 8); // Convert to float with 8 decimal places
      Serial.println("--------------------");
    }
  }
}

// Function to perform ultrasonic distance measurement
void ultrasonic() {
  // Clear the trigger pin by setting it LOW for a short period
  digitalWrite(triggerPin, LOW);
  delayMicroseconds(2);

  // Set the trigger pin HIGH for 10 microseconds to send a pulse
  digitalWrite(triggerPin, HIGH);
  delayMicroseconds(10);
  digitalWrite(triggerPin, LOW);

  // Measure the duration of the pulse on the echo pin
  duration = pulseIn(echoPin, HIGH);

  // Calculate distance in centimeters (speed of sound ~343 m/s or 0.0343 cm/microsecond)
  // Distance = (duration of echo * speed of sound) / 2 (because sound travels to object and back)
  // (duration in microseconds / 2) / 29.1 results in cm
  distance = (duration / 2) / 29.1;
}
