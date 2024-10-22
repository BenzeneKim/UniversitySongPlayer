#include <Arduino.h>
#include <Wire.h>
#include <LiquidCrystal_I2C.h>
#include "pitches.h"

// Initialize the LCD
LiquidCrystal_I2C lcd(0x27, 16, 2);

// Buzzer pin
const int buzzerPin = 8;

// 7-Segment display pins (ABCDEFG and DP)
const int segmentPins[8] = {3, 4, 5, 6, 7, 8, 9, 10}; // DP is the last pin (index 7)


// Complete melody and durations from the score
int melody[] = {
  NOTE_D4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, // "남산이 영을 넘어 바라보이고"
  NOTE_D4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, // "삼천리 이강산에 꽃을 피우려"
  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, // "뒤로는 관악산이 높이 솟았네"
  NOTE_F4, NOTE_E4, NOTE_D4, NOTE_E4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4, // "군세고 지혜롭게 모여들었네"
  NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_F4, NOTE_G4, // "강수구 비치는 노들강변"
  NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, // "선미가 특한 말 길려온 정"
  NOTE_B4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, NOTE_D4, // "대숲결 속에 우뚝 솟은 중앙"
  NOTE_E4, NOTE_F4, NOTE_E4, NOTE_D4, NOTE_C4, NOTE_D4, NOTE_E4, NOTE_D4, // "세계를 누리위해 몸 바친 중앙"
  NOTE_B4, NOTE_C5, NOTE_D5, NOTE_C5, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4, // "의에 죽고 찬에 삶이 모교의 정신"
  NOTE_D4, NOTE_E4, NOTE_G4, NOTE_A4, NOTE_B4, NOTE_A4, NOTE_G4, NOTE_E4  // "예빛내세 우리중앙대학교"
};

// Note durations: 4 = quarter note, 8 = eighth note, etc.
int noteDurations[] = {
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4, 
  4, 4, 4, 8, 8, 4, 4, 4  
};

// Note names corresponding to the melody
char noteNames[] = {
  'D', 'E', 'G', 'A', 'B', 'A', 'G', 'E', // "남산이 영을 넘어 바라보이고"
  'D', 'E', 'G', 'A', 'G', 'A', 'B', 'A', // "삼천리 이강산에 꽃을 피우려"
  'B', 'C', 'D', 'C', 'B', 'A', 'G', 'E', // "뒤로는 관악산이 높이 솟았네"
  'F', 'E', 'D', 'E', 'C', 'D', 'E', 'D', // "군세고 지혜롭게 모여들었네"
  'G', 'A', 'B', 'A', 'G', 'E', 'F', 'G', // "강수구 비치는 노들강변"
  'G', 'A', 'B', 'A', 'B', 'C', 'D', 'C', // "선미가 특한 말 길려온 정"
  'B', 'G', 'A', 'B', 'A', 'G', 'E', 'D', // "대숲결 속에 우뚝 솟은 중앙"
  'E', 'F', 'E', 'D', 'C', 'D', 'E', 'D', // "세계를 누리위해 몸 바친 중앙"
  'B', 'C', 'D', 'C', 'B', 'A', 'G', 'E', // "의에 죽고 찬에 삶이 모교의 정신"
  'D', 'E', 'G', 'A', 'B', 'A', 'G', 'E'  // "예빛내세 우리중앙대학교"
};

// Array indicating whether each note is sharp (1) or not (0)
bool isSharpOrFlat[] = {
  0, 0, 0, 0, 0, 0, 0, 0, // "남산이 영을 넘어 바라보이고"
  0, 0, 0, 0, 0, 0, 0, 0, // "삼천리 이강산에 꽃을 피우려"
  0, 0, 0, 0, 0, 0, 0, 0, // "뒤로는 관악산이 높이 솟았네"
  0, 0, 0, 0, 0, 0, 0, 0, // "군세고 지혜롭게 모여들었네"
  0, 0, 0, 0, 0, 0, 0, 0, // "강수구 비치는 노들강변"
  0, 0, 0, 0, 0, 0, 0, 0, // "선미가 특한 말 길려온 정"
  0, 0, 0, 0, 0, 0, 0, 0, // "대숲결 속에 우뚝 솟은 중앙"
  0, 0, 0, 0, 0, 0, 0, 0, // "세계를 누리위해 몸 바친 중앙"
  0, 0, 0, 0, 0, 0, 0, 0, // "의에 죽고 찬에 삶이 모교의 정신"
  0, 0, 0, 0, 0, 0, 0, 0  // "예빛내세 우리중앙대학교"
};

// State variables
volatile bool isPlaying = false; // Use volatile for variables accessed in ISRs
int currentNote = 0;

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
  
  // Welcome message
  lcd.setCursor(0, 0);
  lcd.print("Melody Player");
  lcd.setCursor(0, 1);
  lcd.print("Press Button");
}

void loop() {
  // If playing, play the melody
  if (isPlaying) {
    playNote();
  }
}

void playNote() {
  if (currentNote < sizeof(melody) / sizeof(int)) {
    int noteDuration = 1000 / noteDurations[currentNote];
    tone(buzzerPin, melody[currentNote], noteDuration);
    
    // Display note on 7-segment and LCD
    showNoteOn7Segment(noteNames[currentNote], isSharpOrFlat[currentNote]);
    lcd.setCursor(0, 0);
    lcd.print("Playing: ");
    lcd.setCursor(9, 0);
    lcd.print(noteNames[currentNote]);
    if (isSharpOrFlat[currentNote]) {
      lcd.print("#"); // Display sharp symbol on the LCD
    }
    lcd.setCursor(0, 1);
    lcd.print("Freq: ");
    lcd.setCursor(6, 1);
    lcd.print(melody[currentNote]);
    lcd.print(" Hz");
    
    // Pause for the note duration plus 30% to distinguish notes
    int pauseBetweenNotes = noteDuration * 1.30;
    delay(pauseBetweenNotes);
    
    // Stop the tone playing
    noTone(buzzerPin);
    
    // Move to the next note
    currentNote++;
  } else {
    // Reset the melody
    currentNote = 0;
    isPlaying = false;
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Melody Player");
    lcd.setCursor(0, 1);
    lcd.print("Press Button");
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
  byte segments[7][8] = {
    {1, 1, 1, 0, 1, 1, 1, 0}, // A
    {0, 0, 1, 1, 1, 1, 1, 0}, // B
    {1, 0, 0, 1, 1, 1, 0, 0}, // C
    {1, 1, 1, 1, 1, 0, 1, 0}, // D
    {1, 0, 0, 1, 1, 1, 1, 0}, // E
    {1, 0, 0, 0, 1, 1, 1, 0}, // F
    {0, 1, 1, 1, 1, 0, 1, 0}  // G
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
}
