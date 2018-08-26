/*
  Winkey

  ボード上のスイッチが押されると、windows側にwindowsキーが
  押されたこととして返ります。
  参考
  http://eucalyn.hatenadiary.jp/entry/original-keyboard-toc

*/

#include <Keyboard.h>

const int switchpin = 3;
const int ledpin = 11;

uint32_t timer = 0;
uint8_t blink = 0;  // Lチカ用

uint8_t currentstate = LOW;
uint8_t laststate = LOW;

uint8_t pressedCount = 0;

// 時間計測用構造体
struct TimeInfo
{
  uint32_t Now = 0;
  uint32_t Last = 0;

  uint32_t Delta = 0;

  void Update()
  {
    Now = millis();
    Delta = Now - Last;
    Last = Now;
  }

  void Reset()
  {
    Now = Last = millis();
    Delta = 0;
  }
};

TimeInfo info;

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
  pinMode(switchpin, INPUT_PULLUP);  // スイッチ検出用のピン

  pinMode(ledpin, OUTPUT);
  Serial.begin(9600);
  info.Reset();
  Keyboard.begin();

//  pressedCount = 0;
}

// the loop function runs over and over again forever
void loop() {
  currentstate = checkSwitch();
  if(laststate != currentstate){
    if(currentstate == HIGH){
      digitalWrite(LED_BUILTIN, HIGH);
      Keyboard.press(KEY_LEFT_GUI); // windowsだとこれがwindowsキー
      Serial.println("on");
    }
    else{
      digitalWrite(LED_BUILTIN, LOW);
      Keyboard.release(KEY_LEFT_GUI);
      Serial.println("off");
    }
    laststate = currentstate;
  }

#if 01
  LED_Blink();
#endif
}

// スイッチ押されたかチェック
// チャタリング対策処理入
uint8_t checkSwitch()
{
  if(!digitalRead(switchpin)){
    // 適当に16サイクルカウント
    if(pressedCount == 0xff || pressedCount >= 16){
      return HIGH;
    }
    pressedCount++;
  }
  else{
    pressedCount = 0;
  }
  return LOW;
}

// LEDチカチカ
void LED_Blink()
{
  info.Update();
  timer += info.Delta;
  if(timer >= (blink ? 1000 : 2000)){
    timer = 0;
    blink = blink ^ HIGH;
    digitalWrite(ledpin, blink);
//    String str = blink ? "on" : "off";
//    Serial.println(str);
  }
}