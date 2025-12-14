#include "U8glib.h"
#include <DHT.h>

// Logo
const unsigned char Temp_Logo [] PROGMEM = {
	0x01, 0xc0, 0x02, 0x00, 0x02, 0xb0, 0x02, 0x80, 0x02, 0xb0, 0x02, 0x80, 0x02, 0xa0, 0x02, 0xa0, 
	0x04, 0x90, 0x09, 0x88, 0x0b, 0x08, 0x0b, 0xa8, 0x09, 0xc8, 0x04, 0x10, 0x03, 0xe0, 0x00, 0x00
};
const unsigned char Humidity_Logo [] PROGMEM = {
	0x00, 0x00, 0x03, 0x00, 0x04, 0x80, 0x08, 0x40, 0x10, 0x20, 0x20, 0xf0, 0x27, 0x9e, 0x46, 0x06, 
	0x44, 0x92, 0x44, 0x22, 0x44, 0x42, 0x24, 0x92, 0x16, 0x06, 0x0f, 0x9e, 0x00, 0xf0, 0x00, 0x00
};

// DHT
const int DHT_PIN = 13;
DHT dht(DHT_PIN, DHT11);

// Light
const int LIGHT_PIN = A0;
const int LUMINOSITY_THRESHOLD = 500;

// Button
const int BUTTON_UP = 7;
const int BUTTON_DOWN = 4;
const int BUTTON_SELECT = 2;
int button_select_clicked = 0;
int button_up_clicked = 0;
int button_down_clicked = 0;

// Screen
const int NUM_ITEMS = 7; 
const int MAX_ITEM_LENGTH = 20;
int screen_value = 0;
int item_selected = 1; 
char buffer[20];

char menu_items [NUM_ITEMS] [MAX_ITEM_LENGTH] = {  
  { " " },
  { "Dashboard" }, 
  { "Temperature" }, 
  { "Humidity" }, 
  { "Light" },
  { "Alarm" },
  { " " }
 };

U8GLIB_SSD1306_128X64 u8g(U8G_I2C_OPT_DEV_0 | U8G_I2C_OPT_NO_ACK | U8G_I2C_OPT_FAST);

// Buzzer
const int BUZZER = 12;

// Temperature Threshold 
float temperature_threshold = 30.0;

// Rotary Encoder
const int CLK = 8;
const int DT = 9;
int current_state_CLK;
int last_state_CLK;


void setup() {
  // Start
  Serial.begin(9600);

  // Pin
  pinMode(LIGHT_PIN, INPUT);
  pinMode(BUTTON_UP, INPUT_PULLUP);
  pinMode(BUTTON_DOWN, INPUT_PULLUP);
  pinMode(BUTTON_SELECT, INPUT_PULLUP);
  pinMode(BUZZER, OUTPUT);

  // Initialization 
  dht.begin();
  u8g.setColorIndex(1); 
}


void loop() {
  // Values
  float humidity = dht.readHumidity();
  float temperature = dht.readTemperature(false);
  int luminosity = analogRead(LIGHT_PIN);

  // Buzzer
  if(temperature > temperature_threshold) {
    tone(BUZZER, 200, 100);
  }

  // Screen Change
  if(digitalRead(BUTTON_SELECT) == LOW && button_select_clicked == 0) {
    button_select_clicked = 1;
    screen_value = (screen_value + 1) % 2;
  }
  if(digitalRead(BUTTON_SELECT) == HIGH && button_select_clicked == 1) {
    button_select_clicked = 0;
  }
  
  //  Menu Screen
  if (screen_value == 0) { 
      if ((digitalRead(BUTTON_UP) == LOW) && (button_up_clicked == 0)) { 
        item_selected = max(1, item_selected - 1);
        button_up_clicked = 1; 
      }
      else if ((digitalRead(BUTTON_DOWN) == LOW) && (button_down_clicked == 0)) { 
        item_selected = min(NUM_ITEMS-2, item_selected + 1); 
        button_down_clicked = 1;
      } 
      if ((digitalRead(BUTTON_UP) == HIGH) && (button_up_clicked == 1)) { 
        button_up_clicked = 0;
      }
      if ((digitalRead(BUTTON_DOWN) == HIGH) && (button_down_clicked == 1)) { 
        button_down_clicked = 0;
      }
  }
  
  // Screen Display
  u8g.firstPage();
  do {
    if(screen_value == 0) {      
      // Menu
      u8g.setFont(u8g_font_7x14);
      u8g.drawStr(30, 15, menu_items[item_selected - 1]); 
       
      u8g.setFont(u8g_font_7x14B);    
      u8g.drawStr(8, 15+20+2, "->");
      u8g.drawStr(30, 15+20+2, menu_items[item_selected]);   

      u8g.setFont(u8g_font_7x14);     
      u8g.drawStr(30, 15+20+20+2+2, menu_items[item_selected + 1]);   
 
    } else if(screen_value == 1) {
      if(item_selected == 1) {
        // Dashboard
        u8g.setFont(u8g_font_7x14); 
        u8g.drawBitmapP(8, 8, 16/8, 16, Temp_Logo);  
        dtostrf(temperature, 1, 1, buffer);
        sprintf(buffer, "%s%s", buffer, " C"); 
        u8g.drawStr(30, 20, buffer);
        u8g.drawBitmapP( 9, 28, 16/8, 16, Humidity_Logo);
        dtostrf(humidity, 1, 1, buffer);
        sprintf(buffer, "%s%s", buffer, " %"); 
        u8g.drawStr(30, 42, buffer);
        if(luminosity < LUMINOSITY_THRESHOLD) {
          u8g.drawStr(30, 62, "Bright");
        } else {
          u8g.drawStr(30, 62, "Dark");
        }
      } else if (item_selected == 2) {
        // Temperature
        u8g.drawBitmapP( 10, 10, 16/8, 16, Temp_Logo);
        u8g.setFont(u8g_font_7x14);    
        u8g.drawStr(30, 22, "Temperature");
        dtostrf(temperature, 1, 1, buffer);
        sprintf(buffer, "%s%s", buffer, " C"); 
        u8g.drawStr(15, 44, buffer);        
      } else if (item_selected == 3) {
        // Humidity
        u8g.drawBitmapP( 10, 10, 16/8, 16, Humidity_Logo);
        u8g.setFont(u8g_font_7x14);    
        u8g.drawStr(30, 24, "Humidity");
        dtostrf(humidity, 1, 1, buffer);
        sprintf(buffer, "%s%s", buffer, " %"); 
        u8g.drawStr(15, 44, buffer);
      } else if (item_selected == 4) {
        // Light
        u8g.setFont(u8g_font_7x14);    
        u8g.drawStr(10, 15, "Light");
        sprintf(buffer, "%d", luminosity);
        u8g.drawStr(10, 35, buffer);
        if(luminosity < LUMINOSITY_THRESHOLD) {
          u8g.drawStr(10, 55, "Bright");
        } else {
          u8g.drawStr(10, 55, "Dark");
        }
      } else if (item_selected == 5) {
        // Rotary Encoder
        u8g.setFont(u8g_font_7x14);    
        u8g.drawStr(10, 15, "Alarm");
        dtostrf(temperature, 1, 1, buffer);
        sprintf(buffer, "%s%s", buffer, " C");
        u8g.drawStr(10, 35, buffer);
        
        // Update Value
        current_state_CLK = digitalRead(CLK);
        if (current_state_CLK != last_state_CLK && current_state_CLK == 1) {
          if (digitalRead(DT) != current_state_CLK) {
            temperature_threshold = temperature_threshold - 0.1;
          } else {
            temperature_threshold = temperature_threshold + 0.1;
          }
        }  
        last_state_CLK = current_state_CLK;
      }
    } 
  } while(u8g.nextPage());
}