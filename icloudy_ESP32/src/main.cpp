#include <Arduino.h>
#include <Adafruit_NeoPixel.h>
#include <SoftwareSerial.h>
#define PIN 4       // Chân điều khiển LED WS2812
#define NUM_LEDS (8)  // Số lượng LED trong dải
String buff = "";
String data = "";

Adafruit_NeoPixel strip = Adafruit_NeoPixel(NUM_LEDS, PIN, NEO_GRB + NEO_KHZ800);

SoftwareSerial TempSerial(5,6); //pin 5: RX, pin 6: TX
// HardwareSerial MainSerial(0);
bool daNhanDuoc = false;
// void checkSerial() {
//   if (TempSerial.available() > 0) {
    
//     char dataBuffer[30];  
//     int len = TempSerial.readBytesUntil('\n', dataBuffer, sizeof(dataBuffer) - 1);
//     dataBuffer[len] = '\0';
//     data = String(dataBuffer);

//   }  
// }
void checkSerial() {
  static String lastData = "";

  if (TempSerial.available() > 0) {
    char dataBuffer[30];  
    int len = TempSerial.readBytesUntil('\n', dataBuffer, sizeof(dataBuffer) - 1);
    dataBuffer[len] = '\0';  // Kết thúc chuỗi
    String currentData = String(dataBuffer);

    // Chỉ cập nhật nếu dữ liệu mới khác dữ liệu cuối cùng
    if (currentData != lastData) {
      data = currentData; // Cập nhật dữ liệu mới
      lastData = currentData; // Lưu lại dữ liệu cuối cùng
    }
  }  
}
void rainbow(int wait) {
  static long firstPixelHue = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= wait) {
      lastUpdate = millis();

      for (int i = 0; i < strip.numPixels(); i++) {
          int pixelHue = firstPixelHue + (i * 65536L / strip.numPixels());
          strip.setPixelColor(i, strip.gamma32(strip.ColorHSV(pixelHue)));
      }
      strip.show();

      firstPixelHue += 256;
      if (firstPixelHue >= 3 * 65536) {
          firstPixelHue = 0;
      }
  }
}

void theaterChaseRainbow(int wait) {
  static int firstPixelHue = 0;
  static int a = 0;
  static int b = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= wait) {
      lastUpdate = millis();

      strip.clear();
      for (int c = b; c < strip.numPixels(); c += 3) {
          int hue = firstPixelHue + c * 65536L / strip.numPixels();
          uint32_t color = strip.gamma32(strip.ColorHSV(hue));
          strip.setPixelColor(c, color);
      }
      strip.show();

      b++;
      if (b > 2) {
          b = 0;
          a++;
          firstPixelHue += 65536 / 90;
          if (a >= 30) {
              a = 0;
          }
      }
  }
}

void StartFunction(uint32_t timedelay, int red, int green, int blue) {
  // Sáng dần màu xanh dương
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255));
    }
    strip.show();
    delay(timedelay); // Độ trễ giữa mỗi bước sáng dần
  }
  
  // Tắt dần màu xanh dương
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255));
      checkSerial();
    }
    strip.show();
    delay(timedelay); // Độ trễ giữa mỗi bước tắt dần
  }
}
void EndFunction(uint32_t timedelay, int red, int green, int blue) {
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255)); // Màu xanh dương
      checkSerial();
    }
    strip.show();
    delay(20); // Độ trễ giữa mỗi bước sáng dần
  }
  
  // Tắt dần 
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255)); // Giảm độ sáng màu xanh dương
    }
    strip.show();
    delay(20); // Độ trễ giữa mỗi bước tắt dần
  }
}
byte colorTable[] = {
  0,0,0,0,0,0,0,0,0 , 10 , 50, 100, 255, 100 , 50, 10,0,0,0,0,0,0,0,0,
};
void ThinkingFunction(uint32_t timedelay, int red, int green, int blue) {
  static int j = 0;
  static int step = 1;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= timedelay) {
      lastUpdate = millis();

      strip.clear();
      for (int i = strip.numPixels() - 1; i >= 0; i--) {
          int R = (red == 1) ? colorTable[i + j] : 0;
          int G = (green == 1) ? colorTable[i + j] : 0;
          int B = (blue == 1) ? colorTable[i + j] : 0;
          strip.setPixelColor(i, strip.Color(R, G, B));
      }
      strip.show();

      j += step;
      if (j > 16 || j < 0) {
          step = -step;
          j += step;
      }
  }
}

void ListeningFunction(uint32_t timedelay, int red, int green, int blue) {
  static int blueValue = 30;
  static bool increasing = true;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= timedelay) {
      lastUpdate = millis();

      if (increasing) {
          blueValue += 5;
          if (blueValue >= 255) {
              blueValue = 255;
              increasing = false;
          }
      } else {
          blueValue -= 5;
          if (blueValue <= 30) {
              blueValue = 30;
              increasing = true;
          }
      }

      for (int i = 0; i < strip.numPixels(); i++) {
          strip.setPixelColor(i, strip.Color(red, green, blueValue));
      }
      strip.show();
  }
}

void AnwserFunction(uint32_t timedelay, int red, int green, int blue) {
  static int i = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= timedelay) {
      lastUpdate = millis();

      strip.clear();
      for (int j = (strip.numPixels()) / 2 - i - 1; j <= (strip.numPixels()) / 2 + i; j++) {
          strip.setPixelColor(j, strip.Color(red, green, blue));
      }
      strip.show();

      i++;
      if (i > (strip.numPixels()) / 2) {
          i = 0;
      }
  }
}
void ThinkingDoneFunction(uint32_t timedelay, int red, int green, int blue){
  for (int brightness = 0; brightness <= 255; brightness += 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255));
      checkSerial();
      if(data!= "ThinkingDoneFunction") return;
    }
    strip.show();
    delay(timedelay); // Độ trễ giữa mỗi bước sáng dần
  }
  delay(200);
  // Tắt dần màu xanh dương
  for (int brightness = 255; brightness >= 0; brightness -= 5) {
    for (int i = 0; i < strip.numPixels(); i++) {
      strip.setPixelColor(i, strip.Color(red * brightness / 255, green * brightness / 255, blue * brightness / 255));
      checkSerial();
      if(data!= "ThinkingDoneFunction") return;
    }
    strip.show();
    delay(timedelay); // Độ trễ giữa mỗi bước tắt dần
  }
  delay(0.2);
}

void TurnLeft(uint32_t timedelay, int red, int green, int blue) {
  static int i = 0;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= timedelay) {
      lastUpdate = millis();

      strip.clear();
      for (int j = 0; j <= i; j++) {
          strip.setPixelColor(j, strip.Color(red, green, blue));
      }
      strip.show();

      i++;
      if (i >= strip.numPixels()) {
          i = 0;
      }
  }
}

void TurnRight(uint32_t timedelay, int red, int green, int blue) {
  static int i = strip.numPixels() - 1;
  static unsigned long lastUpdate = 0;

  if (millis() - lastUpdate >= timedelay) {
      lastUpdate = millis();

      strip.clear();
      for (int j = strip.numPixels() - 1; j >= i; j--) {
          strip.setPixelColor(j, strip.Color(red, green, blue));
      }
      strip.show();

      i--;
      if (i < 0) {
          i = strip.numPixels() - 1;
      }
  }
}

void setup() {
  Serial.begin(115200);
  TempSerial.begin(9600);
  strip.begin(); // Khởi động dải LED
  strip.show(); // Tắt tất cả các đèn LED
}

void loop() {
  checkSerial();
  if(data == "StartFunction"){
    int r_start = 0;
    int g_start = 64;
    int b_start = 255;
    int delay_start = 10;
    int count = 0;
    while(count < 3){
      StartFunction(delay_start,r_start,g_start,b_start);
      count++;
    }
    data = "";
  }
  else if(data == "ThinkingFunction"){
    int r_thinking = 0;
    int g_thinking = 0;
    int b_thinking = 1;
    int delay_thinking = 50;
    ThinkingFunction(delay_thinking, r_thinking, g_thinking, b_thinking);
  }
  else if(data == "ListeningFunction"){
    int r_listen = 0;
    int g_listen = 50;
    int b_listen = 255;
    int delay_listen = 10;
    ListeningFunction(delay_listen, r_listen, g_listen, b_listen);
  }
  else if(data == "ThinkingDoneFunction"){
    int r_thinkingdone = 0;
    int g_thinkingdone = 20;
    int b_thinkingdone = 255;
    int delay_thinkingdone = 20;
    int count = 0;
    while(count < 2){
      ThinkingDoneFunction(delay_thinkingdone, r_thinkingdone, g_thinkingdone, b_thinkingdone);
      count++;
    }
    data = "";
  
  }
  else if(data == "EndFunction"){
    int r_end = 100;
    int g_end = 0;
    int b_end = 0;
    int delay_end = 10;
    int count = 0;
    while(count < 1){
      EndFunction(delay_end,r_end,g_end,b_end);
      count++;
    }
    data = "";
  }
  else if(data == "AnswerFunction"){
    int r_answer = 0;
    int g_answer = 0;
    int b_answer = 200;
    int delay_answer = 200;
    AnwserFunction(delay_answer, r_answer, g_answer, b_answer);    
  }
  else if(data == "RainBow"){
    rainbow(10);
  }
  else if(data == "theaterChaseRainbow"){
    theaterChaseRainbow(10);
  }
  else if(data == "TurnLeft"){
    int r_left = 0;
    int g_left = 130;
    int b_left = 255;
    int delay_left = 70;
    TurnLeft(delay_left, r_left, g_left, b_left);
  }
  else if(data == "TurnRight"){
    int r_right = 0;
    int g_right = 130;
    int b_right = 255;
    int delay_right = 70;
    TurnRight(delay_right, r_right, g_right, b_right);
  }
}

