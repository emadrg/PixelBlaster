#include "LedControl.h"
#include <LiquidCrystal.h>
#include <EEPROM.h>

// LCD declarations
const byte rs = 9;
const byte en = 8;
const byte d4 = 7;
const byte d5 = 6;
const byte d6 = 5;
const byte d7 = 4;
const int pwm_pin = 3;
LiquidCrystal lcd(rs, en, d4, d5, d6, d7);
int storedBrightness = 0;
int brightnessValue = 0;

const byte dinPin = 12;
const byte clockPin = 11;
const byte loadPin = 10;
const byte matrixSize = 8;
const int pinSW = 2;
const int pinX = A0;
const int pinY = A1;
const int buzzerPin = 13;


LedControl lc = LedControl(dinPin, clockPin, loadPin, 1);
int matrixBrightness = 2;  // Initial brightness level for the matrix
int lcdBrightness = 100; // Initial brightness level for the LCD
int xValue, yValue;
int minThreshold = 400;
int maxThreshold = 600;
bool joyMoved = false;
int row = 1;
int col = 0;

unsigned long lastDebounceTime = 0;
unsigned long debounceDelay = 50;
bool buttonState = HIGH;
bool lastButtonState = HIGH;
int playerRow = 3;
int playerCol = 3;
const int numObstacles = 5;  // Change this if you want to add more obstacles
int currentObstacleIndex = 0;

bool gameRunning = false;
bool difficultySelected = false;

// Number of items in the menu
int currentMenuItem = 0;  // Currently selected menu item

char currentDirection = 'N';
bool increaseBrightness = false;

int ledsShotObstacle = 0;


bool currentMatrix[matrixSize][matrixSize] = { 0 };

// I defined this matrix in order to go through the obstacles more easily,
// and also be able to add more in case I need to do so, without having to modify the rest of the code
bool obstacles[numObstacles][matrixSize][matrixSize] = {
  // Obstacle 1
  { { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 1, 1, 1, 1, 1, 1, 0 },
    { 0, 0, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 } },

  // Obstacle 2
  { { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 0, 1, 1, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 } },

  // Obstacle 3
  {
    { 0, 0, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 1, 1, 0, 0, 0, 0, 1, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 1, 0, 0, 0, 0, 1, 1 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 } },

  // Obstacle 4
  { { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 0, 1, 1, 1, 1, 0, 0 },
    { 0, 0, 1, 0, 0, 1, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 } },

  // Obstacle 5
  {
    { 0, 0, 1, 1, 1, 1, 0, 0 },
    { 0, 1, 0, 0, 0, 0, 1, 0 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 1, 0, 0, 0, 0, 0, 0, 1 },
    { 0, 1, 0, 0, 0, 0, 1, 0 },
    { 0, 0, 0, 0, 0, 0, 0, 0 } }

};

const bool sadFace[8][8] = {
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 0, 1, 0, 0, 1, 0, 0 },
  { 0, 0, 0, 0, 0, 0, 0, 0 },
  { 0, 0, 0, 1, 1, 0, 0, 0 },
  { 0, 1, 1, 1, 1, 1, 1, 0 },
  { 1, 1, 0, 0, 0, 0, 1, 1 },
  { 0, 0, 0, 0, 0, 0, 0, 0 }
};

unsigned long obstacleMoveInterval = 500;  // Adjust the interval as needed
unsigned long lastObstacleMoveTime = 0;

int difficulty = 1;  // Default difficulty level

int ledsShot = 0;

int index_settings = 0;

const int MENU_ITEMS = 5;
String main_menu[MENU_ITEMS] = { "Start game>", " <Settings> ", "<Game settings>", " <About>", "<Hall of Fame" };
String settings_menu[3] = { "LCD Brightness>", "<Matrix Brightness>", "<Back" };
const int SETTINGS_MENU_ITEMS = 3;
int currentSettingsItem = 0;
String difficulty_menu[4] = { "1>", "<2>", "<3>", "<Back" };
const int DIFFICULTY_MENU_ITEMS = 4;
const int GAME_SETTINGS_MENU_ITEMS = 4; 
String gameSettingsMenu[GAME_SETTINGS_MENU_ITEMS] = { "Enter Name>", "<Sound: On>", "<ResetHighscore>", "<Back" };
int currentGameSettingsItem = 0;
bool soundEnabled = true;  // Default sound setting

char playerName[4] = "AAA";  // 3-letter name with a null terminator
int letterIndex = 0;         // Index of the current letter being edited

const int EEPROM_MATRIX_BRIGHTNESS_ADDR = 100;  // Randomly chosen addresses
const int EEPROM_LCD_BRIGHTNESS_ADDR = 110;

const int ABOUT_LINES = 4;
String aboutText[ABOUT_LINES] = {
  "Pixel Blaster ",
  "Developed by Ema",
  "Github: emadrg",
  "Enjoy the game!"
};

const int EEPROM_HIGHSCORE_ADDR = 10;  // Address in EEPROM for high score

struct HighScore {
  char name[4];
  int score;
};

HighScore bestHighScore;

enum AppState {
  MainMenu,
  DifficultySelection,
  GamePlay,
  Settings,
  GameSettings
};


AppState currentState = MainMenu;


void setup() {

  pinMode(pinSW, INPUT_PULLUP);
  pinMode(buzzerPin, OUTPUT);
  pinMode(pwm_pin, OUTPUT);

  // Setting up the LCD, mentioning the number of columns and rows
  lcd.begin(16, 2);

  // Set initial values for LCD and matrix brightness from EEPROM

  lcdBrightness = EEPROM.read(EEPROM_LCD_BRIGHTNESS_ADDR);
  setLCDBrightness(lcdBrightness);  //Setting the initial LCD brightness to the last value saved in EEPROM


  matrixBrightness = EEPROM.read(EEPROM_MATRIX_BRIGHTNESS_ADDR);
  lc.setIntensity(0, matrixBrightness);  //Setting the initial matrix brightness to the last value saved in EEPROM

  // Set the initial LCD brightness to maximum
  analogWrite(pwm_pin, 255);

  //Setting uo the initial position for the player
  row = 7;
  col = 3;
  lc.shutdown(0, false);
  lc.clearDisplay(0);

  // Set the initial obstacle
  reset_current_obstacle(0);
  //Display the initial obstacle while doing the settings. Not necessary
  displayObstacle();
  // Display the player at the initial position
  lc.setLed(0, row, col, true);
  // Display the welcome message
  displayIntroMessage();
  // Initialize high scores
  initBestHighScore();
  Serial.begin(9600);
}



void loop() {

  switch (currentState) {
    case MainMenu:
      handleMainMenu();
      break;
    case DifficultySelection:
      startGame();
      break;
    case GamePlay:
      handleGame();
      break;
    case Settings:
      handleSettingsMenu();
      break;
  }
}

void soundBuzzer(int duration) {
  digitalWrite(buzzerPin, HIGH);  // Turn buzzer on
  delay(duration);                // Wait for the specified duration
  digitalWrite(buzzerPin, LOW);   // Turn buzzer off
}


//Function for deciding how to interpret the moves of the joystick. I 'recycled' this part of the code from one of my previous projects
bool decideJoyDirection() {
  xValue = analogRead(pinX);
  yValue = analogRead(pinY);

  if (xValue < minThreshold && !joyMoved) {  // left
    currentDirection = 'L';
    joyMoved = true;
  } else if (xValue > maxThreshold && !joyMoved) {  // right
    currentDirection = 'R';
    joyMoved = true;
  } else if (yValue < minThreshold && !joyMoved) {  // up
    currentDirection = 'U';
    joyMoved = true;
  } else if (yValue > maxThreshold && !joyMoved) {  // down
    currentDirection = 'D';
    joyMoved = true;
  }

  if (xValue > minThreshold && xValue < maxThreshold && yValue > minThreshold && yValue < maxThreshold) {
    joyMoved = false;  // reset joyMoved when joystick returns to center
  }
}

bool scrollMenu(int &index, int size) {
  decideJoyDirection();
  if (!joyMoved) {
    return false;
  }
  bool scrolled = false;
  switch (currentDirection) {
    case 'L':
      {
        if (index == 0) // This ensure that the menu doesn't loop back to the last option. 
        //This is just how I want my menus to work. Looping back would't be a problem, but it should be implemented accordingly to avoid displaying random stuff on the LCD
          break;
        index--;
        scrolled = true;
      }
      break;
    case 'R':
      {
        if (index == size - 1)
          break;
        index++;
        scrolled = true;
      }
      break;
    default:
      return false;
  }

  if (scrolled) {
    menuScrollSound();  // Play sound on scroll
  }

  // Reset currentDirection to avoid continuous scrolling
  currentDirection = 'N';

  lcd.clear();  // Clear the display before printing the new content
  lcd.setCursor(0, 0);

  return true;
}


void menuScrollSound() {
  if (!soundEnabled) return;  // Only play sound if sound is enabled

  // Play a short beep
  tone(buzzerPin, 1000, 50);
  delay(60);
  noTone(buzzerPin);
}


int index_main = 0;
void handleMainMenu() {
  // Display and scroll through the main menu
  if (scrollMenu(index_main, MENU_ITEMS)) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print(main_menu[index_main]);
  }

  // Check if the button is pressed to select a menu item. Depending on what was selected, the currentState changes
  if (buttonPressed()) {
    switch (index_main) {
      case 0:  // "Start Game" selected
        currentState = DifficultySelection;
        break;
      case 1:  // "Settings" selected
        currentState = Settings;
        handleSettingsMenu();
        currentState = MainMenu;
        break;
      case 2:  // "Game Settings" selected
        currentState = GameSettings;
        handleGameSettingsMenu();
        currentState = MainMenu;
        break;
      case 3:  // "About" selected
        about();
        break;
      case 4:  // "Hall of Fame" selected
        displayHighScores();
        break;
    }
  }
}



void startGame() {
  lcd.clear();

  while (!buttonPressed()) {
    if (scrollMenu(currentMenuItem, DIFFICULTY_MENU_ITEMS)) {
      lcd.setCursor(0, 0);
      lcd.print("Difficulty:");
      lcd.setCursor(0, 1);
      lcd.print(difficulty_menu[currentMenuItem]);
    }
  }

  if (currentMenuItem == DIFFICULTY_MENU_ITEMS - 1) {  // "Back" option
    currentState = MainMenu;                           // Go back to the main menu if the "Back" option was selcted
  } else {
    setDifficulty(currentMenuItem + 1);
    setupGameForPlay();       // Set up the game for the selected difficulty
    currentState = GamePlay;  // Change state to start the game
  }
}


void setupGameForPlay() {
  // Reset player position in order to start the game
  playerRow = 3;
  playerCol = 3;
  lc.setLed(0, playerRow, playerCol, true);  // Display the player at the initial position

  // Reset or setup the initial obstacle
  currentObstacleIndex = 0;
  reset_current_obstacle(currentObstacleIndex);
  displayObstacle();

  // Reset the variables relevant for the games
  ledsShot = 0;          //Total of LEDS that have been shot
  ledsShotObstacle = 0;  //LEDs shot from the current obstacle

  // Set the initial game state
  gameRunning = true;
  setObstacleMoveInterval();  // Set up the interval at which the LEDs dissaper, depending on the chosen difficulty

  lcd.clear();
  lcd.setCursor(0, 0);
}


void handleSettingsMenu() {

  while (true) {
    if (scrollMenu(currentSettingsItem, SETTINGS_MENU_ITEMS)) {
      lcd.clear();
      lcd.setCursor(0, 0);
      lcd.print(settings_menu[currentSettingsItem]);
    }

    if (buttonPressed()) {
      if (currentSettingsItem == SETTINGS_MENU_ITEMS - 1) {  // "Back" option
        currentState = MainMenu;                             // Return to the main menu
        break;                                               // Exit the loop
      }
      switch (currentSettingsItem) {
        case 0:
          LCDBrightnessAdjustment();  // Enter LCD brightness adjustment mode
          break;
        case 1:
          matrixBrightnessAdjustment();  // Enter matrix brightness adjustment mode
          break;
      }
    }
  }
}

void resetHighScore() {
    Serial.print("Highscore Reset!");
    memset(&bestHighScore, 0, sizeof(HighScore)); // Reset high score structure
    strcpy(bestHighScore.name, "AAA");
    bestHighScore.score = 0;
    EEPROM.put(EEPROM_HIGHSCORE_ADDR, bestHighScore); // Update EEPROM
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Highscore Reset!");
    delay(1000); // Show message for 1 second
}

void about() {
  lcd.clear();
  int currentLine = 0;

  while (true) {
    lcd.setCursor(0, 0);
    for (int i = 0; i < 2; i++) {  // The LCD display has 2 lines
      if (currentLine + i < ABOUT_LINES) {
        lcd.print(aboutText[currentLine + i]);
        if (i < 1) lcd.setCursor(0, i + 1);
      }
    }

    if (scrollAbout(currentLine)) {
      lcd.clear();
    }

    if (buttonPressed()) {  // Exit if button is pressed
      break;                // Exit the loop and return to the main menu
    }

    delay(100);  // Add a small delay to debounce and control input speed
  }

  lcd.clear();
}



bool scrollAbout(int &currentLine) {
  decideJoyDirection();

  if (!joyMoved) {
    return false;
  }

  bool scrolled = false;
  if (currentDirection == 'U' && currentLine > 0) {
    currentLine--;
    scrolled = true;
  } else if (currentDirection == 'D' && currentLine < ABOUT_LINES - 2) {
    currentLine++;
    scrolled = true;
  }

  joyMoved = false;        // Reset joyMoved after handling
  currentDirection = 'N';  // Reset direction

  return scrolled;
}


void setLCDBrightness(int brightness) {
  // Ensure the brightness value is within the 0-255 range
  brightness = max(0, min(brightness, 255));

  // Set the brightness of the LCD
  analogWrite(pwm_pin, brightness);
}


void LCDBrightnessAdjustment() {
  int lcdBrightness = EEPROM.read(EEPROM_LCD_BRIGHTNESS_ADDR); // Read the current brightness from EEPROM
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("LCD Brightness: ");

  while (true) {
    decideJoyDirection(); 

    if (joyMoved) {
      if (currentDirection == 'U') {
        lcdBrightness = min(255, lcdBrightness + 10);  // Increase brightness when moving the joystick up
      } else if (currentDirection == 'D') {
        lcdBrightness = max(0, lcdBrightness - 10);  // Decrease brightness when moving the joystick down
      }

     // setLCDBrightness(lcdBrightness); // Set new brightness
      //EEPROM.update(EEPROM_LCD_BRIGHTNESS_ADDR, lcdBrightness);  // Save the new brightness to EEPROM

      joyMoved = false;
      lcd.setCursor(0, 1);
      lcd.print(lcdBrightness);
      lcd.print("   ");  // Clear any remaining digits
      delay(100);  // Delay for debounce and smoother adjustment
    }

    if (buttonPressed()) {
    // Save the brightness setting when exiting
    setLCDBrightness(lcdBrightness);
    EEPROM.update(EEPROM_LCD_BRIGHTNESS_ADDR, lcdBrightness);
    break;  
  }
    }
  }



void matrixBrightnessAdjustment() {
  bool adjusting = true;
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Mat. Brightness: ");

  while (adjusting) {
    decideJoyDirection();  // Update currentDirection based on the joystick movement

    if (joyMoved) {  // Only adjust if there was a joystick movement
      if (currentDirection == 'U') {
        matrixBrightness = min(15, matrixBrightness + 1);  // Increase brightness for joystick moved up
      } else if (currentDirection == 'D') {
        matrixBrightness = max(0, matrixBrightness - 1);  // Decrease brightness for joystick moved down
      }

      lc.setIntensity(0, matrixBrightness);                            // Set new brightness
      EEPROM.update(EEPROM_MATRIX_BRIGHTNESS_ADDR, matrixBrightness);  // Save the new brightness to EEPROM

      joyMoved = false;
      delay(100);  // Delay for debounce and smoother adjustment
    }

    lcd.setCursor(0, 1);
    lcd.print(matrixBrightness);
    lcd.print("   ");  // Clear any remaining digits

    if (buttonPressed()) {
      adjusting = false;  // Exit adjustment mode after pressing the button
    }
  }
}


void handleGameSettingsMenu() {
    while (true) {
        if (scrollMenu(currentGameSettingsItem, GAME_SETTINGS_MENU_ITEMS)) {
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(gameSettingsMenu[currentGameSettingsItem]);
        }

        if (buttonPressed()) {
            if (currentGameSettingsItem == GAME_SETTINGS_MENU_ITEMS - 1) { // "Back" option
                currentState = MainMenu; // Return to the main menu when "Back" was selected
                break;
            }

            switch (currentGameSettingsItem) {
                case 0:
                    enterPlayerName(); // Handle Enter Name
                    break;
                case 1:
                    toggleSound(); // Handle Sound On/Off
                    break;
                case 2:
                    resetHighScore(); // Reset Highscore
                    break;
            }
            lcd.clear();
            lcd.setCursor(0, 0);
            lcd.print(gameSettingsMenu[currentGameSettingsItem]); // Redisplay the settings option after adjustment
        }
    }
}


void enterPlayerName() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Enter Name: ");
  lcd.setCursor(0, 1);
  lcd.print(playerName);

  int currentLetterIndex = 0;  // Index to keep track of which letter is being edited

  while (true) {
    decideJoyDirection();

    if (joyMoved) {
      if (currentDirection == 'R') {  // If the joystick was moved to the right, the letter "increases" (ex: it goes from A to B, to C etc)
        playerName[currentLetterIndex]++;
        if (playerName[currentLetterIndex] > 'Z') {
          playerName[currentLetterIndex] = 'A';  // Looping back to A
        }
      } else if (currentDirection == 'L') {  //If the joystick was moved to the left, the letter "decreases"
        playerName[currentLetterIndex]--;
        if (playerName[currentLetterIndex] < 'A') {
          playerName[currentLetterIndex] = 'Z';  // Looping back to Z
        }
      }

      lcd.setCursor(0, 1);
      lcd.print(playerName);
      joyMoved = false;  // Reset joyMoved after handling
    }

    if (buttonPressed()) {
      currentLetterIndex++;  // Move to next letter of the name when the button is pressed

      // Check if all letters have been updated
      if (currentLetterIndex >= 3) {
        currentState = MainMenu;  // Return to the main menu when all 3 letters have been set
        break;
      }

      // Move cursor to next letter position for editing
      lcd.setCursor(currentLetterIndex, 1);
    }

    delay(200);  // Add a delay to debounce and control input speed
  }
}


void toggleSound() {
  soundEnabled = !soundEnabled;  // Toggle the sound state
  gameSettingsMenu[1] = soundEnabled ? "Sound: On" : "Sound: Off";
}


//Function for deciding if the button was pressed
bool buttonPressed() {
  int reading = digitalRead(pinSW);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        return true;
      }
    }
  }

  lastButtonState = reading;
  return false;
}


void setDifficulty(int selectedDifficulty) {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Difficulty: " + String(selectedDifficulty));

  // Set the difficulty variable to the selected difficulty
  difficulty = selectedDifficulty;
  gameRunning = true;  // The game starts only after the difficulty was selected
}


void initBestHighScore() {
  EEPROM.get(EEPROM_HIGHSCORE_ADDR, bestHighScore);
  if (bestHighScore.score == 0xFFFF) {  // Check if EEPROM is uninitialized
    memset(&bestHighScore, 0, sizeof(HighScore));
    strcpy(bestHighScore.name, "AAA");
    bestHighScore.score = 0;
  }
}


void updateHighScores(const char *playerName, int playerScore) {
  if (playerScore > bestHighScore.score) {
    // Copy player name into struct
    strncpy(bestHighScore.name, playerName, sizeof(bestHighScore.name) - 1);
    bestHighScore.name[sizeof(bestHighScore.name) - 1] = '\0';  // Ensure null-termination
    bestHighScore.score = playerScore;

    // Save name to EEPROM
    for (int i = 0; i < sizeof(bestHighScore.name); ++i) {
      EEPROM.update(EEPROM_HIGHSCORE_ADDR + i, bestHighScore.name[i]);
    }

    // Save score to EEPROM
    int scoreAddr = EEPROM_HIGHSCORE_ADDR + sizeof(bestHighScore.name);
    EEPROM.update(scoreAddr, bestHighScore.score & 0xFF);
    EEPROM.update(scoreAddr + 1, (bestHighScore.score >> 8) & 0xFF);
  }
}



void displayHighScores() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Best High Score");
  lcd.setCursor(0, 1);
  lcd.print(bestHighScore.name);
  lcd.print(" ");
  lcd.print(bestHighScore.score);

  // Wait for a button press to return
  waitForButtonPress();
}


//Function that makes the player move on the matrix depending on the joystcik movement
void moveSegment(char direction) {

  lc.setLed(0, row, col, false);

  if (direction == 'U' && row > 0) {
    row--;
  } else if (direction == 'D' && row < matrixSize - 1) {  // Keeping in mind that the player can't move out of the matrix
    row++;
  } else if (direction == 'L' && col > 0) {
    col--;
  } else if (direction == 'R' && col < matrixSize - 1) {
    col++;
  }

  lc.setLed(0, row, col, true);
}



// Blinking the player LED in order to differenciate it from the obstacles/laser
void blinkLedPlayer(int row, int col, int interval) {
  static unsigned long previousMillis = 0;
  static bool ledState = false;

  unsigned long currentMillis = millis();

  if (currentMillis - previousMillis >= interval) {
    previousMillis = currentMillis;
    ledState = !ledState;  // Toggle the LED state

    lc.setLed(0, row, col, ledState);
  }
}


//Logic for deciding if a laser is shot, depending on the activity of the joystick button
//This is basically the buttonPressed function, with the exception that when the buttonState changes, it calls the shootLaser function
void handleLaserShoot() {
  int reading = digitalRead(pinSW);

  if (reading != lastButtonState) {
    lastDebounceTime = millis();
  }

  if ((millis() - lastDebounceTime) > debounceDelay) {
    if (reading != buttonState) {
      buttonState = reading;

      if (buttonState == LOW) {
        shootLaser(row, col);  // This is the difference I was talking about
      }
    }
  }

  lastButtonState = reading;
}



//Visual representation of the laser shooting. Here the obstacles are destroyed when shot, one LED at a time
void shootLaser(int row, int col) {
  for (int i = 1; i < 8; i++) {
    if (currentMatrix[row - i][col] == 1) {
      currentMatrix[row - i][col] = 0;
      lc.setLed(0, row - i, col, false);
      ledsShot++;
      ledsShotObstacle++;
      if (soundEnabled) {
        soundBuzzer(100);
      }
      if (ledsShot % 20 == 0) {
        increaseDifficulty();
      }
      return;
    }
    lc.setLed(0, row - i, col, true);
    delay(20);
    lc.setLed(0, row - i, col, false);
  }
}



void increaseDifficulty() {
  if (difficulty < 3) {  // 3 is the highest difficulty
    difficulty++;
    setObstacleMoveInterval();  // Adjust the obstacle move interval for the new difficulty
    difficultyIncreaseSound();  // Play the buzzer sound
  }
}


void difficultyIncreaseSound() {
  if (!soundEnabled) return;

  tone(buzzerPin, 440, 200); 
  delay(220);                 
  tone(buzzerPin, 523, 200);  
  delay(220);
  tone(buzzerPin, 659, 200);  
  delay(220);

  noTone(buzzerPin); 
}


//Function for displaying the obstacles one by one, on the matrix
void displayObstacle() {
  lc.clearDisplay(0);
  // Iterate through the currentMatrix and turn on LEDs
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (currentMatrix[i][j] == 1) {
        lc.setLed(0, i, j, true);  // Turn on the LED without clearing the display
      }
    }
  }
}


//Function for going through all the obstacles in the array. When a new obstacle is added, I don't need to modify this function since I used an array. I was a smart cookie for doing this B-)
void switchToNextObstacle() {
  if (ledsShotObstacle == 0) {
    gameRunning = false;
    updateHighScores(playerName, ledsShot);
    displayEndMessage();
    return;
  }

  ledsShotObstacle = 0;

  currentObstacleIndex = (currentObstacleIndex + 1) % numObstacles;
  reset_current_obstacle(currentObstacleIndex);
  displayObstacle();
}


//Function where the currentMatrix takes the value of each obstacle's matrix.
//Here the currentMatrix takes the values of each the matrix of each obstacle. I did this so that when the obstacles are destroyes, the modifications happen over this
//currentMatrix, not the ones where the obstacles are defined. If I hadn't used this, the declared matrixes would be destroyed.
void reset_current_obstacle(int index) {
  for (int i = 0; i < matrixSize; i++)
    for (int j = 0; j < matrixSize; j++) {
      currentMatrix[i][j] = obstacles[index][i][j];
    }
}

//This function allows me to control the contrast of my LCD using a potentiometer.
void setLCDcontrast() {
  analogWrite(pwm_pin, 255);
  // read the potentiometer on A0:
  int sensorReading = analogRead(A0);
  // map the result to 200 - 1000:
  int delayTime = map(sensorReading, 0, 1023, 200, 1000);
  delay(delayTime);
}

// Function to remove the leds from the obstacles one by one. This is used to add some difficulty to the game.
//The idea is to shoot as many LEDs from the obstacle as possible, but you have to be fast, since they dissapear one by one
void removeObstacleLeds() {
  const int maxLeds = matrixSize * matrixSize;
  int onLeds[maxLeds][2];  // Each element will store a pair of coordinates (row, col)
  int onLedsCount = 0;

  // Populate the array with the positions of 'on' LEDs
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (currentMatrix[i][j] == 1) {
        onLeds[onLedsCount][0] = i;  // Row
        onLeds[onLedsCount][1] = j;  // Col
        onLedsCount++;
      }
    }
  }

  // Check if there are any LEDs to turn off
  if (onLedsCount > 0) {
    // Randomly select an index
    int randomIndex = random(0, onLedsCount);
    int rowToRemove = onLeds[randomIndex][0];
    int colToRemove = onLeds[randomIndex][1];

    // Turn off the selected LED
    currentMatrix[rowToRemove][colToRemove] = 0;
    lc.setLed(0, rowToRemove, colToRemove, false);
  }
}


//This is the general function for playing the game, the place where I used all the little functions from above
void handleGame() {
  unsigned long currentTime = millis();

  //Printing the player's name on the first row during gameplay
  lcd.setCursor(0, 0);
  lcd.print("Now playing: ");
  lcd.print(playerName);

  if (currentTime - lastObstacleMoveTime >= obstacleMoveInterval) {
    removeObstacleLeds();  // Making the LEDs disappear
    lastObstacleMoveTime = currentTime;
  }

  // Check if the current obstacle is fully cleared
  bool currentObstacleCleared = true;
  for (int i = 0; i < matrixSize; i++) {
    for (int j = 0; j < matrixSize; j++) {
      if (currentMatrix[i][j] == 1) {
        currentObstacleCleared = false;
        break;
      }
    }
    if (!currentObstacleCleared) {
      break;
    }
  }

  // If the current obstacle is cleared, switch to the next one
  if (currentObstacleCleared) {
    switchToNextObstacle();
  }

  // Update player position and check shooting logic
  decideJoyDirection();
  if (joyMoved) {
    moveSegment(currentDirection);
    currentDirection = 'N';
  }
  handleLaserShoot();

  // Display the number of LEDs shot on the second row of the LCD
  lcd.setCursor(0, 1);
  lcd.print("LEDs Shot: " + String(ledsShot));
}



//This function displayes an intro message at the beginning of the game
void displayIntroMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Welcome!");
}

//Message to display when the game is over (the player lost)
void displayEndMessage() {
  lcd.clear();
  lcd.setCursor(0, 0);
  lcd.print("Game Over! Press button");
  lcd.setCursor(0, 1);
  lcd.print("to restart");

  gameOverSound();  // Play the game over sound

  displaySadFace();  // Display sad face on the LED matrix

  waitForButtonPress();
  resetGame();  // Reset the game after the button is pressed
}


void gameOverSound() {
  if (!soundEnabled) return;

  tone(buzzerPin, 523, 200);
  delay(250);
  tone(buzzerPin, 440, 200);
  delay(250);
  tone(buzzerPin, 349, 200);
  delay(250);
  tone(buzzerPin, 261, 400);

  // Turn off the tone
  noTone(buzzerPin);
}

void displaySadFace() {
  for (int row = 0; row < 8; row++) {
    for (int col = 0; col < 8; col++) {
      lc.setLed(0, row, col, sadFace[row][col]);
    }
  }
}




void waitForButtonPress() {
  // Wait for the button to be pressed and then released
  while (digitalRead(pinSW) == HIGH) {
    // Waiting for button press
  }
  delay(50);  // Debouncing delay for button press

  while (digitalRead(pinSW) == LOW) {
    // Waiting for button release
  }
  delay(50);  // Debouncing delay for button release
}



void setObstacleMoveInterval() {
  switch (difficulty) {
    case 1:
      obstacleMoveInterval = 500;  // Easy
      break;
    case 2:
      obstacleMoveInterval = 200;  // Medium
      break;
    case 3:
      obstacleMoveInterval = 50;  // Hard
      break;
  }
}


void resetGame() {
  // Reset game-specific variables
  gameRunning = false;
  difficultySelected = false;
  currentObstacleIndex = 0;
  ledsShot = 0;
  ledsShotObstacle = 0;

  // Re-initialize the initial game state or obstacle
  reset_current_obstacle(0);
  displayObstacle();

  // Reset the player position
  playerRow = 3;
  playerCol = 3;
  lc.setLed(0, playerRow, playerCol, true);

  // Set the state back to MainMenu
  currentState = DifficultySelection;
}