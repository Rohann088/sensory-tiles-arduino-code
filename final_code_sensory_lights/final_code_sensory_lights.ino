#include <Adafruit_NeoPixel.h>

// Define pins for LEDs and Buttons
#define LED_PIN_1 2
#define BUTTON_PIN_1 36
#define LED_PIN_2 3
#define BUTTON_PIN_2 37
#define LED_PIN_3 4
#define BUTTON_PIN_3 38
#define LED_PIN_4 5
#define BUTTON_PIN_4 39
#define LED_PIN_5 6
#define BUTTON_PIN_5 40
#define LED_PIN_6 7
#define BUTTON_PIN_6 41
#define LED_PIN_7 8
#define BUTTON_PIN_7 42
//#define LED_PIN_8 9
//#define BUTTON_PIN_8 43
#define LED_PIN_9 10
#define BUTTON_PIN_9 44

#define SPEAKER_1 A0
#define SPEAKER_2 A3

// LED ring setup
#define NUM_LEDS 12
Adafruit_NeoPixel ledRings[] = {
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_1, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_2, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_3, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_4, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_5, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_6, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_7, NEO_GRB + NEO_KHZ800),
  Adafruit_NeoPixel(NUM_LEDS, LED_PIN_9, NEO_GRB + NEO_KHZ800),
};

// Button pins
int buttonPins[] = {BUTTON_PIN_1, BUTTON_PIN_2, BUTTON_PIN_3, BUTTON_PIN_4, BUTTON_PIN_5,
                    BUTTON_PIN_6, BUTTON_PIN_7, BUTTON_PIN_9};

// Notes for "Mary Had a Little Lamb" (frequencies in Hz)
int melody[] = {329, 293, 261, 293, 329, 329, 329, 293, 293, 293, 329, 349, 349,
                329, 293, 261, 293, 329, 329, 329, 329, 293, 293, 329, 293, 261};
int melodyLength = sizeof(melody) / sizeof(melody[0]);

uint32_t red = ledRings[0].Color(255, 0, 0); // Red color for incorrect response

int activeRing = -1;       // Current active LED ring
int melodyIndex = 0;       // Tracks current position in the melody
unsigned long noteEndTime; // Tracks when the current note playback ends
bool playingNote = false;  // Tracks if a note is currently playing

void setup() {
  // Initialize LED rings and buttons
  for (int i = 0; i < 8; i++) {
    ledRings[i].begin();
    ledRings[i].show(); // Clear LEDs
    pinMode(buttonPins[i], INPUT_PULLUP); // Use pull-up resistors for buttons
  }

  pinMode(SPEAKER_1, OUTPUT);
  pinMode(SPEAKER_2, OUTPUT);

  randomSeed(analogRead(0)); // Seed random generator
}

void loop() {
  static unsigned long lastActionTime = 0; // To handle delays without blocking
  static bool waitingForInput = false;    // Whether the game is waiting for user input

  if (!waitingForInput) {
    // Light up a random ring
    delay(500);
    activeRing = random(0, 8);
    ledRings[activeRing].fill(ledRings[activeRing].Color(255, 255, 255), 0, NUM_LEDS); // White light
    ledRings[activeRing].show();
    lastActionTime = millis();
    waitingForInput = true;
  }

  // Turn off LED after 1 second
  if (waitingForInput && millis() - lastActionTime > 1000) {
    ledRings[activeRing].clear();
    ledRings[activeRing].show();
  }

  // Handle user input
  int buttonPressed = getButtonPress();
  if (buttonPressed != -1) {
    if (buttonPressed == activeRing) {
      // Correct response
      playNoteFromMelody(); // Play the next note in the melody
      ledRings[buttonPressed].fill(ledRings[buttonPressed].Color(0, 255, 0), 0, NUM_LEDS); // Green light
      ledRings[buttonPressed].show();
      delay(500); // Show light briefly
      ledRings[buttonPressed].clear();
      ledRings[buttonPressed].show();
    } else {
      // Incorrect response
      playIncorrectSequence();
    }
    waitingForInput = false; // Reset the game state
  }

  // Update note playback
  updateNotePlayback();
}

// Function to check for a button press
int getButtonPress() {
  for (int i = 0; i < 8; i++) {
    if (digitalRead(buttonPins[i]) == LOW) { // Button pressed
      delay(50); // Simple debounce
      return i;
    }
  }
  return -1; // No button pressed
}

// Function to play the next note in the melody
void playNoteFromMelody() {
  int note = melody[melodyIndex];
  if (melodyIndex % 2 == 0) {
    tone(SPEAKER_1, note);
  } else {
    tone(SPEAKER_2, note);
  }
  noteEndTime = millis() + 500; // Set note duration to 500ms
  playingNote = true;

  melodyIndex = (melodyIndex + 1) % melodyLength; // Move to the next note, loop if necessary
}

// Function to stop note playback when the time is up
void updateNotePlayback() {
  if (playingNote && millis() > noteEndTime) {
    noTone(SPEAKER_1);
    noTone(SPEAKER_2);
    playingNote = false;
  }
}

// Function to play incorrect sequence
void playIncorrectSequence() {
  for (int i = 0; i < 8; i++) {
    // Light up one LED at a time
    ledRings[i].fill(red, 0, NUM_LEDS);
    ledRings[i].show();

    // Play one "womp" sound for each LED
    tone(SPEAKER_1, 200 - i * 30); // Descending frequency
    delay(300);                    // Duration of "womp"
    noTone(SPEAKER_1);

    // Turn off the current LED before lighting the next one
    ledRings[i].clear();
    ledRings[i].show();
    delay(100); // Short delay between LEDs
  }
}
