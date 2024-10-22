#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

// Initialize the LCD
LiquidCrystal_I2C lcd(0x3F, 16, 2);

// Buzzer pin
const int buzzerPin = 3;

// 7-Segment display pins (ABCDEFG and DP)
const int segmentPins[8] = {4, 5, 6, 7, 8, 9, 10, 11}; // DP is the last pin (index 7)

// Struct to hold note information
struct Note {
  int melody;
  float duration;
  char name;
  bool isSharpOrFlat;
  int octave;
};

// Array of Note structs for the melody
Note notes[] = {
  {NOTE_AS4, 2, 'A', true, 4}, {NOTE_AS4, 1, 'A', true, 4}, {NOTE_C5, 1, 'C', false, 5}, {NOTE_AS4, 1, 'A', true, 4}, 
  {NOTE_A4, 0.75, 'A', false, 4}, {NOTE_G4, 0.25, 'G', false, 4}, {NOTE_F4, 2, 'F', false, 4}, {NOTE_AS3, 1.5, 'A', true, 3}, 
  {NOTE_C4, 0.5, 'C', false, 4}, {NOTE_D4, 1, 'D', false, 4}, {NOTE_AS3, 1, 'A', true, 3}, {NOTE_F4, 3, 'F', false, 4}, 
  {REST, 1, 'R', false, -1}, // "남산이 영을 넘어 바라보이고" / "삼천리 이강산에 꽃을 피우려"
  
  {NOTE_G4, 1.5, 'G', false, 4}, {NOTE_F4, 0.5, 'F', false, 4}, {NOTE_DS4, 2, 'D', true, 4}, {NOTE_AS4, 1, 'A', true, 4}, 
  {NOTE_A4, 0.75, 'A', false, 4}, {NOTE_G4, 0.25, 'G', false, 4}, {NOTE_F4, 2, 'F', false, 4}, {NOTE_F4, 1.5, 'F', false, 4}, 
  {NOTE_G4, 0.5, 'G', false, 4}, {NOTE_A4, 1, 'A', false, 4}, {NOTE_AS4, 1, 'A', true, 4}, {NOTE_C5, 3, 'C', false, 5}, 
  {NOTE_F4, 1, 'F', false, 4}, // "뒤로는 관악산이 높이 솟았네" / "군세고 지혜롭게 모여들었네"
  
  {NOTE_D5, 2, 'D', false, 5}, {NOTE_D5, 1, 'D', false, 5}, {NOTE_C5, 1, 'C', false, 5}, {NOTE_AS4, 1, 'A', true, 4}, 
  {NOTE_A4, 1, 'A', false, 4}, {NOTE_G4, 2, 'G', false, 4}, {NOTE_C5, 2, 'C', false, 5}, {NOTE_C5, 1, 'C', false, 5}, 
  {NOTE_AS4, 1, 'A', true, 4}, {NOTE_A4, 1, 'A', false, 4}, {NOTE_G4, 1, 'G', false, 4}, {NOTE_F4, 1, 'F', false, 4}, 
  {NOTE_D4, 0.75, 'D', false, 4}, {NOTE_C4, 0.25, 'C', false, 4}, // "강수구 비치는 노들강변" / "선미가 특한 말 길려온 정"
  
  {NOTE_AS3, 2, 'A', true, 3}, {REST, 0.5, 'R', false, -1}, {NOTE_AS4, 0.5, 'A', true, 4}, {NOTE_AS4, 0.5, 'A', true, 4}, 
  {NOTE_A4, 0.5, 'A', false, 4}, {NOTE_G4, 2, 'G', false, 4}, {REST, 1, 'R', false, -1}, {NOTE_G4, 1, 'G', false, 4}, 
  {NOTE_F4, 2, 'F', false, 4}, {NOTE_G4, 1, 'G', false, 4}, {NOTE_A4, 1, 'A', false, 4}, {NOTE_AS4, 3, 'A', true, 4}, 
  {REST, 1, 'R', false, -1}, // "대숲결 속에 우뚝 솟은 중앙" / "세계를 누리위해 몸 바친 중앙"

  {NOTE_D4, 0.75, 'D', false, 4}, {NOTE_E4, 0.25, 'E', false, 4}, {NOTE_F4, 0.5, 'F', false, 4}, {REST, 0.5, 'R', false, -1}, 
  {NOTE_F4, 2, 'F', false, 4}, {NOTE_DS4, 0.75, 'D', true, 4}, {NOTE_F4, 0.25, 'F', false, 4}, {NOTE_G4, 0.5, 'G', false, 4}, 
  {REST, 0.5, 'R', false, -1}, {NOTE_G4, 2, 'G', false, 4}, {NOTE_F4, 1.5, 'F', false, 4}, {NOTE_AS4, 0.5, 'A', true, 4}, 
  {NOTE_AS4, 1, 'A', true, 4}, {NOTE_C5, 3, 'C', false, 5}, {NOTE_D5, 1, 'D', false, 5}, {NOTE_D5, 2, 'D', false, 5}, // "의에 죽고 찬에 삶이 모교의 정신"
  
  {NOTE_D5, 2, 'D', false, 5}, {NOTE_D5, 0.5, 'D', false, 5}, {NOTE_D5, 0.5, 'D', false, 5}, {NOTE_C5, 0.5, 'C', false, 5}, 
  {NOTE_D5, 0.5, 'D', false, 5}, {NOTE_DS5, 3, 'D', true, 5}, {NOTE_DS5, 0.75, 'D', true, 5}, {NOTE_DS5, 0.25, 'D', true, 5}, 
  {NOTE_F4, 1.5, 'F', false, 4}, {NOTE_G4, 0.5, 'G', false, 4}, {NOTE_A4, 1, 'A', false, 4}, {NOTE_C5, 1, 'C', false, 5}, 
  {NOTE_AS4, 3, 'A', true, 4}, {REST, 1, 'R', false, -1}  // "예빛내세 우리중앙대학교"

};

// State variables
volatile bool isPlaying = false; // Use volatile for variables accessed in ISRs
int currentNote = 0;

// Function prototypes
void togglePlayPause();
void playNote();
void indicatePause();
void showNoteOn7Segment(char note, bool isSharpOrFlat);

void setup() {
  // Initialize the LCD
  lcd.init();
  lcd.backlight();
  
  // Set up the push button interrupt on pin 2
  pinMode(2, INPUT_PULLUP);
  attachInterrupt(digitalPinToInterrupt(2), togglePlayPause, FALLING); // Trigger on falling edge

  // Set up the buzzer
  pinMode(buzzerPin, OUTPUT);
  
  // Set up the 7-segment display
  for (int i = 0; i < 8; i++) {
    pinMode(segmentPins[i], OUTPUT);
  }
}

void loop() {
  // If playing, play the melody
  if (isPlaying) {
    playNote();
  }
  else { 
    indicatePause();
  }
}

void indicatePause() {
    lcd.setCursor(0,0);
    lcd.print("PAUSED       ");
    lcd.setCursor(0,1);
    lcd.print("             ");
}

void playNote() {
  if (currentNote < sizeof(notes) / sizeof(Note)) {
    int noteDuration = 400 * notes[currentNote].duration;
    tone(buzzerPin, notes[currentNote].melody, noteDuration);
    
    // Display note on 7-segment and LCD
    showNoteOn7Segment(notes[currentNote].name, notes[currentNote].isSharpOrFlat);
    lcd.setCursor(0, 0);
    lcd.print("Playing: ");
    lcd.setCursor(9, 0);
    lcd.print(notes[currentNote].name);
    if (notes[currentNote].octave != -1) {
      lcd.print(notes[currentNote].octave);
    }
    else {
      lcd.print("EST");
    }
    if (notes[currentNote].isSharpOrFlat) {
      lcd.print("# "); // Display sharp symbol on the LCD
    }
    else {
      lcd.print("  ");
    }
    lcd.setCursor(0, 1);
    lcd.print("Freq: ");
    lcd.setCursor(6, 1);
    if(notes[currentNote].melody == 0) lcd.print("  ");
    lcd.print(notes[currentNote].melody);
    lcd.print(" Hz");
    
    // Pause for the note duration plus 30% to distinguish notes
    int pauseBetweenNotes = noteDuration * 1.250;
    delay(pauseBetweenNotes);
    
    // Stop the tone playing
    noTone(buzzerPin);
    
    // Move to the next note
    currentNote++;
  } else {
    // Reset the melody
    currentNote = 0;
    isPlaying = true;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Melody Player");
    lcd.setCursor(0, 1);
    lcd.print("Press Button");
    delay(1000);
  }
}

// Interrupt Service Routine (ISR) to toggle play/pause
void togglePlayPause() {
  isPlaying = !isPlaying; // Toggle play/pause state
  if (!isPlaying) {
    noTone(buzzerPin); // Stop the buzzer immediately if paused
  }
}

void showNoteOn7Segment(char note, bool isSharpOrFlat) {
  // Define segment values for each note (A-G)
  // Segment values: {A, B, C, D, E, F, G, DP}
  byte segments[7][7] = {
    {1, 1, 1, 0, 1, 1, 1}, // A
    {0, 0, 1, 1, 1, 1, 1}, // B
    {0, 0, 0, 1, 1, 0, 1}, // C
    {0, 1, 1, 1, 1, 0, 1}, // D
    {1, 0, 0, 1, 1, 1, 1}, // E
    {1, 0, 0, 0, 1, 1, 1}, // F
    {1, 0, 1, 1, 1, 1, 1}  // G
  };

  int index = note - 'A';
  if (index >= 0 && index < 7) {
    // Set segment pins according to the letter
    for (int i = 0; i < 7; i++) {
      digitalWrite(segmentPins[i], segments[index][i]);
    }
    // Set the DP (dot) pin for sharp or flat indication
    digitalWrite(segmentPins[7], isSharpOrFlat ? HIGH : LOW);
  }
  else{
    for(int i = 0; i < 8; i++){
      digitalWrite(segmentPins[i], LOW);
    }
  }
}
