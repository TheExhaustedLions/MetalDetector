/*
  Metal Detector Code Explanation:

  This Arduino code is designed to operate a metal detector circuit. The metal detector circuit
  consists of a coil that generates electromagnetic pulses to induce eddy currents in nearby metallic
  objects. The induced currents affect the charge on a capacitor, and this change is measured to detect
  the presence of metal.

  Circuit Components:
  - Coil: Connected to pin A0, generates pulses to induce eddy currents.
  - Capacitor: Connected to pin A1, its charge is affected by induced currents.
  - Buzzer: Connected to pin 12, serves as an indicator for metal detection.

  Operational Overview:
  1. The code begins by setting up the necessary pin modes and initializing variables.
  2. The main loop performs multiple measurements to accurately determine the capacitor charge.
  3. Each measurement involves resetting the capacitor, applying pulses to the coil, and reading
     the capacitor charge using the analogRead function.
  4. The minimum and maximum values are tracked and subtracted to remove spikes in the measurements.
  5. The code maintains a running sum and calculates an average sum to identify significant changes
     in the capacitor charge over time.
  6. The buzzer is controlled based on the difference between the current measurement and the average sum.
     - If the difference is positive, the buzzer is turned on briefly.
     - If the difference is negative, the buzzer is turned off briefly.
     - The duration of the buzzer activation is inversely proportional to the magnitude of the difference.
     - A long duration indicates a small change, and a short duration indicates a large change.
  7. The code includes safeguards to prevent false positives and to handle situations where the measurements
     exhibit prolonged skipping or remain relatively constant.

  Note: Adjustments to the circuit parameters and code may be necessary for optimal metal detection based on
  the specific requirements of the application.

  - Process
 * Reset Capacitor .
 * Give Pulses .
 * Read charge on Capacitor .
 * Determine Buzzer State according to detecting changes over time .
 * Remove Spikes .
 * Responsing Changes in measurement for more accuracy .
 
 */
 
// Define constants for better readability
const byte NUM_PULSES = 12;
const byte PULSE_PIN = A0;
const byte CAPACITOR_PIN = A1;
const byte BUZZER_PIN = 11;
const byte LED_PIN = 12;

const int NUM_MEASUREMENTS = 256;

// Variables for running calculations
long int runningSum = 0;
long int skipCount = 0;
long int difference = 0;
long int flashPeriod = 0;
long unsigned int prevFlashTimestamp = 0;

void setup() {
  // Set up pin modes
  pinMode(PULSE_PIN, OUTPUT);
  digitalWrite(PULSE_PIN, LOW);
  pinMode(CAPACITOR_PIN, INPUT);
  pinMode(BUZZER_PIN, OUTPUT);
  digitalWrite(BUZZER_PIN, LOW);
  pinMode(LED_PIN, OUTPUT);
  digitalWrite(LED_PIN, LOW);
}

// Function to reset the capacitor
void resetCapacitor() {
  pinMode(CAPACITOR_PIN, OUTPUT);
  digitalWrite(CAPACITOR_PIN, LOW);
  delayMicroseconds(20);
  pinMode(CAPACITOR_PIN, INPUT);
}

// Function to apply pulses to the coil
void applyPulses() {
  for (int i = 0; i < NUM_PULSES; i++) {
    digitalWrite(PULSE_PIN, HIGH);
    delayMicroseconds(3);
    digitalWrite(PULSE_PIN, LOW);
    delayMicroseconds(3);
  }
}

// Function to measure the charge on the capacitor
void measureCapacitor(int& minValue, int& maxValue, long unsigned int& sum) {
  int value = analogRead(CAPACITOR_PIN);
  minValue = min(value, minValue);
  maxValue = max(value, maxValue);
  sum += value;
}

// Function to determine the state of the buzzer
byte determineBuzzerState(long unsigned int timestamp) {
  byte buzzerState = 0;
  if (timestamp < prevFlashTimestamp + 12) { // look
    if (difference > 0) buzzerState = 0; // look
    if (difference < 0) buzzerState = 1; // look
  }
  if (timestamp > prevFlashTimestamp + flashPeriod) { // look
    if (difference > 0) buzzerState = 0; // look
    if (difference < 0) buzzerState = 1; // look
    prevFlashTimestamp = timestamp;
  }
  if (flashPeriod > 1000) buzzerState = 0; // look
  return buzzerState;
}

// Function to update the state of the buzzer
void updateBuzzer(byte buzzerState) {
  if (buzzerState == 0 ) {
    digitalWrite(BUZZER_PIN, LOW);
    digitalWrite(LED_PIN, LOW);
  } else if (buzzerState == 1) {
    digitalWrite(BUZZER_PIN, HIGH);
    digitalWrite(LED_PIN, HIGH);
  }
}

void loop() {
  int minVal = 2000;
  int maxVal = 0;
  long unsigned int measurementSum = 0;

  // Perform multiple measurements
  for (int i = 0; i < NUM_MEASUREMENTS + 2; i++) {
    resetCapacitor();
    applyPulses();
    measureCapacitor(minVal, maxVal, measurementSum);

    long unsigned int timestamp = millis(); // look
    byte buzzerState = determineBuzzerState(timestamp);
    updateBuzzer(buzzerState);
  }

  // look
  // Subtract minimum and maximum values to remove spikes
  measurementSum -= minVal;
  measurementSum -= maxVal;

  // Process measurements
  if (runningSum == 0) runningSum = measurementSum << 6; // look
  long int avgSum = (runningSum + 32) >> 6; // look
  difference = measurementSum - avgSum; // look

  // Calculate flash period for the buzzer
  if (difference == 0) {
    flashPeriod = 1000000; // look 
  } else {
    flashPeriod = avgSum / (2 * abs(difference)); // look
  }
}
