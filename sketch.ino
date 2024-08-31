#include <Adafruit_SSD1306.h>
#include <Wire.h>
#include <ArduinoJson.h>
#include "sprites.h" 

#define SCREEN_WIDTH 128 
#define SCREEN_HEIGHT 64 
#define OLED_RESET     -1 
#define SCREEN_ADDRESS 0x3C ///< see datasheet
Adafruit_SSD1306 display(SCREEN_WIDTH, SCREEN_HEIGHT, &Wire, OLED_RESET);
#define BUTTON_FEED 5
#define BUTTON_PLAY 6
#define BUTTON_OTHER 7


float health = 100;
float hunger = 100;
float happiness = 100;
float discipline=100;
float weight=1;
float age=0;
bool isAlive = true; 
float money = 100; 
bool isSleep = false;
float sleepometer = 100; 
float poopometer = 0;

//menu:
int selected = 0;
int entered = -1;

int gameState = 0;  

unsigned long previousMillis = 0; 
unsigned long animationStartTime = 0;

bool timerLock1 = false;

byte actionType = 0;
int8_t actionTimer = 2; 


int spriteX = 64;
bool movingRight = true;


void setup() {
  Serial.begin(115200);

  if (!display.begin(SSD1306_SWITCHCAPVCC, SCREEN_ADDRESS)) {
    Serial.println(F("SSD1306 allocation failed"));
    for (;;); // loop forever
  }

  display.clearDisplay();
  display.setTextSize(1);           
  display.setTextColor(SSD1306_WHITE);      
  display.setCursor(0,0); 
  display.fillScreen(WHITE);
  display.println("Start");
  display.display();
 
  pinMode(BUTTON_FEED, INPUT_PULLUP);
  pinMode(BUTTON_PLAY, INPUT_PULLUP);
  pinMode(BUTTON_OTHER, INPUT_PULLUP);
  gameState = 2;
}

void loop() {
  unsigned long currentMillis = millis(); 
 
  if (currentMillis - previousMillis >= 1000) {
    previousMillis = currentMillis;
    Serial.print(F("Game state: "));
    Serial.print(gameState);
    Serial.print(F(" Entered: "));
    Serial.println(entered);
    Serial.print(F(" Selected: "));
    Serial.println(selected);
    lifeSimulation();
    
  }
  drawMainScreen();


}


void drawMainScreen() {
  if (gameState==2) {
    display.clearDisplay();
    display.setCursor(0,0);
    display.print("Toka");
    display.print("  ");
    display.write(0x03);
    display.print(int(health));
    //display.setCursor(0, 20);
    display.print("  ");
    display.write(0x04);
    display.print(int(hunger));
    display.print("  ");
    display.write(0x02);
    display.print(int(happiness));
    display.print("  ");
    display.write(0x24);
    display.print(int(money));
 

    
    moveSprite(); 
    drawSprite(); 

    
    handleButtons();
  } 
  if (gameState==1) {displaymenu();}
  
  if (gameState==3) {actionScreen();}
  display.display();

}


void handleButtons() {
  if (digitalRead(BUTTON_OTHER) == LOW) {
    
    hunger += 20;
    if (hunger > 100) {
     hunger = 100;
    }
    
  } else if (digitalRead(BUTTON_PLAY) == LOW) {
    happiness += 20;
    if (happiness > 100) {
     happiness = 100;
    }
   
  } else if (digitalRead(BUTTON_FEED) == LOW) {
    Serial.println(F("Button menu"));
    gameState =1;
    delay(50);
  }
}


void lifeSimulation() {
  
  if (isAlive) { 
    if (!isSleep) { 
      hunger-=0.002;
      if (poopometer<=100) {poopometer+=0.001;}
      if (happiness>=0) {happiness-=0.0001;}
      health-=age*0.0001+poopometer*0.01;
    }
    else {
      if (poopometer<=100) {poopometer+=0.0005;}
      if (happiness>=0) {happiness-=0.00005;}
      health-=age*0.0001+poopometer*0.01;
    }
    age+=0.0001;
  if (hunger<=0 || health<=0) {
    hunger=0;
    health=0;
    isAlive=false;
  }

  }


  
  if (hunger <= 0) {
    Serial.println(F("Dead"));
    gameState = 3;
    actionType = 8;  
    isAlive = false;
   
  }

  
}

void displaymenu(void) {
  const int menuLength = 10; 
  const int visibleItems = 6; 

  if (digitalRead(BUTTON_OTHER) == LOW) {
    entered = -1;
    if (selected > 0) {
      selected--;
    } else {
      selected = menuLength - 1; 
    }
    delay(20);
  } else if (digitalRead(BUTTON_FEED) == LOW) {
    selected++;
    if (selected >= menuLength) {
      selected = 0; 
    }
    delay(20);
  } else if (digitalRead(BUTTON_PLAY) == LOW) {
    entered = selected;
  } else if (digitalRead(BUTTON_OTHER) == LOW) {
    entered = -1;
  }


  const char *options[10] = { 
    " Close Menu ", 
    " Stat ", 
    " Feed ", 
    " Wash ",
    " Heal ",
    " Sleep ",
    " Play ",
    " Setup",
    " Save",
    " Reset"
  };

  int startIndex;
  int endIndex;

  switch (entered) {
    case -1: 
      display.clearDisplay();
      display.setTextSize(1);
      display.setTextColor(SSD1306_WHITE);

      display.drawRoundRect(0, 0, 58, 11, 3, WHITE);
      display.setCursor(2, 2);
      display.println(F("Main Menu"));
      display.setCursor(0, 12);
      //display.println("");

      
      startIndex = max(0, selected - (visibleItems - 1));
      endIndex = min(menuLength - 1, startIndex + visibleItems - 1);

      for (int i = startIndex; i <= endIndex; i++) {
        if (i == selected) {
          display.write(0x10); 
          display.setTextColor(SSD1306_BLACK, SSD1306_WHITE);
          display.println(options[i]);
        } else {
          display.setTextColor(SSD1306_WHITE);
          display.println(options[i]);
        }
      }
      break;

    case 0: 
      gameState = 2;
      entered = -1;
      selected = 0;
      break;

    case 1: 
      display.clearDisplay();
      //display.setTextSize(1);             
      display.setTextColor(SSD1306_WHITE);
      display.setCursor(0, 0);
      display.println(F("Stat"));
      display.println("Name: Toka");
      display.setCursor(0, 20);
      display.write(0x03);
      display.print("Health: ");
      display.print(health);
      display.setCursor(0, 30);
      display.write(0x04);
      display.print("Hunger: ");
      display.print(hunger);
      display.setCursor(0, 40);
      display.write(0x02);
      display.print("Happiness: ");
      display.print(happiness);
      display.setCursor(0, 50);
      display.write(0x24);
      display.print("Money: ");
      display.print(money);
      break;

    case 2: 
      gameState = 3; 
      if (hunger <= 100) {
        actionTimer = 2; 
        actionType = 1; 
        hunger += 20.333;
        entered = -1;
        selected = 0;
      }
      else {
        actionTimer = 2; 
        actionType = 0; 
        entered = -1;
        selected = 0;
      }
      break;
      
      // entered = -1;
      // selected = 0;
  }
}



void actionScreen() {
  if (timerLock1==false) {
    animationStartTime = millis(); 
    timerLock1=true;
  }

    
    switch(actionType) {
      case 0: 
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.print("NO!");
        break;  
      case 1: 
        display.clearDisplay();
        display.setTextColor(SSD1306_WHITE);
        display.setCursor(0,0);
        display.print("Eating...");
        break;
      case 2: 
        // 
        break;
      case 3:

        break;
      case 4:

        break;
      case 5:

        break;
      case 6:

        break;
      case 7:

        break;
      case 8:
      
        break;
      // 
      default:
        gameState = 2;
        break;
    }

    // 
    if (millis() - animationStartTime >= actionTimer*1000) {
      gameState = 2; 
      timerLock1=false;
      }
  
}


void moveSprite() {
    
    if (movingRight) {
        spriteX++; 
        
        if (spriteX + 48 >= SCREEN_WIDTH) {
            movingRight = false; 
        }
    } else { 
        spriteX--; 
        
        if (spriteX <= 0) {
            movingRight = true; 
        }
    }
}


void drawSprite() {
    //display.clearDisplay();
    
    //display.display();
    if (movingRight) {
        display.drawBitmap(spriteX, 32, dinoWalk0, 48, 24, WHITE);
    } else {
        display.drawBitmap(spriteX, 32, dinoWalkInverted, 48, 24, WHITE);
    }
}
