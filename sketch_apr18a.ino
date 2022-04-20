#include <LiquidCrystal.h>

/*
Written by: Amirhosein Soleimanian
Date: April, 2022
For: Mr.Kohres
About: A game where user have to test their memory by guessing the right color!
*/



const int LED_YELLOW  = 10; // Yellow LED pin
const int LED_BLUE    = 11; // Blue LED pin
const int LED_GREEN   = 12; // Green LED pin
const int LED_RED     = 13; // Red LED pin

LiquidCrystal lcd(9, 8, 5, 4, 3, 2); // LCD pin definitions

const int BUTTON_YELLOW = 2;  // Yellow button input pin
const int BUTTON_BLUE   = 3;  // Blue button input pin
const int BUTTON_GREEN  = 4;  // Green button input pin
const int BUTTON_RED    = 5;  // Red button input pin

const int PIEZO         = 0;  // The pin the piezo is connected to

const int STATE_START_GAME        = 0;  // Initial state
const int STATE_PICK_RND_SEQUENCE = 1;  // Pick a random sequence of LEDs
const int STATE_SHOW_RND_SEQUENCE = 2;  // Show the randomly selected sequence of LED flashes
const int STATE_READ_PLAYER_GUESS = 3;  // Read the player's guess
const int STATE_VERIFY_GUESS      = 4;  // Check the guess against the random sequence
const int STATE_GUESS_CORRECT     = 5;  // Player guessed correctly
const int STATE_GUESS_INCORRECT   = 6;  // Player guessed incorrectly

const int MAX_DIFFICULTY_LEVEL    = 10;  // Maximum difficulty level (LED flash sequence length)

// Array used to store the generated random sequence
int randomSequence[MAX_DIFFICULTY_LEVEL];

// Array used to store the player's guess
int playerGuess[MAX_DIFFICULTY_LEVEL];
// A counter to record the number of button presses made by the player
int numButtonPresses;

// The current state the game is in
int currentState;
int nextState;

// The difficulty level (1..MAX_DIFFICULTY_LEVEL)
// (Do not set to zero!)
int difficultyLevel;

void setup() {
  // Initialise input buttons
    for(int i=BUTTON_YELLOW; i<=BUTTON_RED; i++) {
        pinMode(i, INPUT);
    }
    // Initialise LEDs
    for(int i=LED_YELLOW; i<=LED_RED; i++) {
        pinMode(i, OUTPUT);
    }
    // Set initial difficulty level to 1 random flash && put game in start state
    difficultyLevel = 1;
    currentState = STATE_START_GAME;
    nextState = currentState;
    numButtonPresses = 0;
    lcd.begin(16, 2);
    lcd.print("   Welcome to   ");
    lcd.setCursor(0,1);
    lcd.print("   Guess Cool   ");
    delay(1000);
    lcd.clear();
}

void loop() {
    // Run the state machine controlling the game's states
    switch( currentState ) {
        case STATE_START_GAME:
        for (int count = 5; count >= 0; count--)
        {
            lcd.clear();
            lcd.setCursor(0,0);
            lcd.print("  Starting in:  ");
            lcd.setCursor( 0, 1 ); 
            lcd.print("       "+String(count)+"      ");
            delay(1000);
        }
        nextState = STATE_PICK_RND_SEQUENCE; 
        break;
        case STATE_PICK_RND_SEQUENCE:
        generateRndSequence();
        nextState = STATE_SHOW_RND_SEQUENCE;
        break; 
        case STATE_SHOW_RND_SEQUENCE:
        showRndSequence();  // Display the rnd sequence to the player
        nextState = STATE_READ_PLAYER_GUESS;
        break;
        case STATE_READ_PLAYER_GUESS:
        readPlayerGuess();  // Poll buttons and record each button press
        // If all inputs have been made then verify the guess
        if( numButtonPresses >= difficultyLevel ) {
            numButtonPresses = 0;
            nextState = STATE_VERIFY_GUESS;
        }
        break;
        case STATE_VERIFY_GUESS:
        // Check player's guess against the generated random sequence
        if( verifyGuess() ) {
            nextState = STATE_GUESS_CORRECT;
        } else {
            nextState = STATE_GUESS_INCORRECT;
        }
        break;
        case STATE_GUESS_CORRECT:
        // Player was right. Increase difficulty level and goto start game
        soundCorrectGuess();
        difficultyLevel++;
        lcd.clear();
        lcd.print("    Correct!    ");
        lcd.setCursor(0,1);
        lcd.print("   Next Round!  ");
        delay(1500);
        if( difficultyLevel > MAX_DIFFICULTY_LEVEL )
            difficultyLevel = MAX_DIFFICULTY_LEVEL;
        nextState = STATE_START_GAME;
        break;
        case STATE_GUESS_INCORRECT:
        // Player was wrong. Sound buzzer, show correct sequence, set difficulty level to 1 and re-start game
        lcd.clear();
        lcd.print("WRONGGGGGGGGGGG!");
        lcd.setCursor(0,1);
        lcd.print("   Try Again!   ");
        delay(1500);  
        soundBuzzer();
        showRndSequence();
        difficultyLevel = 1;
        nextState = STATE_START_GAME;
        break;
    }
    currentState = nextState;
}


// Generate a random sequence of LED pin numbers
void generateRndSequence() {
    for(int i=0; i<difficultyLevel; i++) {
        randomSequence[i] = rndLEDPin();
    }
}

// Show random sequence
void showRndSequence() {
    for(int i=0; i<difficultyLevel; i++) {
        flashLED(randomSequence[i], true);  
    }
}

// Read a button press representing a guess from player
void readPlayerGuess() {
    if( digitalRead(BUTTON_YELLOW) == HIGH ) {
        playerGuess[numButtonPresses] = LED_YELLOW;
        numButtonPresses++;
        flashLED(LED_YELLOW, true);
        blockUntilRelease(BUTTON_YELLOW);
    } else if( digitalRead(BUTTON_BLUE) == HIGH ) {
        playerGuess[numButtonPresses] = LED_BLUE;
        numButtonPresses++;
        flashLED(LED_BLUE, true);
        blockUntilRelease(BUTTON_BLUE);
    }else if( digitalRead(BUTTON_GREEN) == HIGH ) {
        playerGuess[numButtonPresses] = LED_GREEN;
        numButtonPresses++;
        flashLED(LED_GREEN, true);
        blockUntilRelease(BUTTON_GREEN);
    }else if( digitalRead(BUTTON_RED) == HIGH ) {
        playerGuess[numButtonPresses] = LED_RED;
        numButtonPresses++;
        flashLED(LED_RED, true);
        blockUntilRelease(BUTTON_RED);

    }
}

void blockUntilRelease(int buttonNumber) {
    while( digitalRead(buttonNumber) == HIGH )
        ;
}
// Compare the guess with the random sequence and return true if identical
bool verifyGuess() {
    bool identical = true;
    for(int i=0; i<difficultyLevel; i++) {
        if( playerGuess[i] != randomSequence[i] ) {
        identical = false;
        break;
        }
    }
    return identical;
}

// Sound the buzzer for incorrect guess
void soundBuzzer() {
    tone(PIEZO, 100, 2000);
    delay(2300);  
}

void soundCorrectGuess() {
    tone(PIEZO, 700, 100);
    delay(100);
    tone(PIEZO, 800, 100);
    delay(100);
    tone(PIEZO, 900, 100);
    delay(100);
    tone(PIEZO, 1000, 100);
    delay(100);
    tone(PIEZO, 1100, 100);
    delay(100);
    tone(PIEZO, 1200, 100);
    delay(100);
}
// Flash the LED on the given pin
void flashLED(int ledPinNum, bool playSound) {
    digitalWrite(ledPinNum, HIGH);
    if( playSound )
        playSoundForLED(ledPinNum);
    delay(1000);
    digitalWrite(ledPinNum, LOW);
    delay(500);
}

void playSoundForLED(int ledPinNum) {
    int pitch = 0;
    switch(ledPinNum) {
        case LED_YELLOW: pitch = 200; break;
        case LED_BLUE: pitch = 250; break;
        case LED_GREEN: pitch = 300; break;
        case LED_RED: pitch = 350; break;
    }
    tone(PIEZO, pitch, 800);
}
// Get a random LED pin number
int rndLEDPin() {
    return random(LED_YELLOW, LED_RED + 1);
}
