Tools Required
      • Arduino Uno (simulated in Proteus)
      • Ultrasonic Sensor (HC-SR04 or similar) 
      • Relay Module 
      • Vibration Sensor 
      • Display Board 
      • GSM Module 
      • GPS Module
      
Ultrasonic Sensor to Arduino Uno:
      . VCC to 5V pin.
      . GND to GND pin.
      . Trig to digital pin 10.
      . Echo to digital pin 11.
      
Relay Module to Arduino Uno:
      . VCC to 5V pin.
      . GND to GND pin.
      . IN1 to digital pin 4.

Vibration Sensor to Arduino Uno:
      . VCC to 5V pin.
      . GND to GND pin.
      . Signal to Analoge pin A0   // to read the data in analoge signal, Here we are using PWM. 

Display Board to Arduino Uno:
      . VCC to 5V pin.
      . GND to GND pin.
      . SDA to SDA(A4) pin.
      . SCL to SCL (A5) pin.

GPS Module to Arduino uno:
      . VCC to 5V pin.
      . GND to GND pin.
      . TX to digital pin 6 (RX).
      . RX to digital pin 7(TX).


Working:
1. The Ultrasonic Sensor measures the distance to an obstacle in front of it.
2. The Vibration Sensor detects measures the impact of collision 
3. The Arduino continuously reads data from the Ultrasonic Sensor and the Vibration Sensor.
4. If the Ultrasonic Sensor detects an object closer than a threshold distance (indicating a potential collision), and the Vibration Sensor detects an impact, the Arduino triggers the Relay Module to simulate an     accident detection.
5. Upon accident detection, the Arduino activates the GSM Module to send an emergency message with the GPS coordinates of the accident location to a pre-defined emergency contact number.

Result:
Hence the system i Designed will detect any type of accedent and and send the data of the accedent according to the danger level it will send a appropriate message to the provided mobile number. 
