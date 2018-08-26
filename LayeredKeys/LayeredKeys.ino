/*
  Layered Keys

  Fnキーを押すことでレイヤーを切り替え、同じキーにレイヤーごとに違う機能を割り当てます。
  参考
  http://eucalyn.hatenadiary.jp/entry/original-keyboard-toc

*/

#include <Keyboard.h>

const uint8_t SWITCH_PIN = 3;  // レイヤーごとに機能切り替えるスイッチ
const uint8_t LAYER_PIN = 6;   // レイヤー切り替え用スイッチ

enum LAYER{
  LAYER0,
  LAYER1,
};

enum class KEY_TYPE{
  SWITCH,
  LAYER,
  MAX,
};

class KeyInfo
{

  uint8_t mPinIndex = 0xff;
  // 現在のピンの状態
  uint8_t mCurrentState = LOW;
  // 最後のピンの状態
  uint8_t mLastState = LOW;
  // キーが実際押されているカウント(チャタリング対策用)
  uint8_t mPressedCount = 0;

  // スイッチ押されたかチェック
  // チャタリング対策処理入
  uint8_t CheckSwitch()
  {
    if(!digitalRead(mPinIndex)){
      // 適当に16サイクルカウント 短すぎず長すぎずのカウント
      if(mPressedCount == 0xff || mPressedCount >= 16){
        return HIGH;
      }
      mPressedCount++;
    }
    else{
      mPressedCount = 0;
    }
    return LOW;
  }
public:
  KeyInfo() = delete;
  KeyInfo(uint8_t pinindex)
  : mPinIndex(pinindex){}

  void Initialize()
  {
    pinMode(mPinIndex, INPUT_PULLUP);
  }

  // 前回から状態が変わったかチェック
  bool CheckChangeState()
  {
    mCurrentState = CheckSwitch();
    if(mLastState != mCurrentState){
      mLastState = mCurrentState;
      return true;
    }
    return false;
  }

  uint8_t GetCurrentState()
  {
    return mCurrentState;
  }

};

uint8_t CurrentLayer = LAYER0;
uint8_t LastLayer = LAYER0;

KeyInfo KeyInfoList[(uint8_t)KEY_TYPE::MAX] = {KeyInfo(SWITCH_PIN), KeyInfo(LAYER_PIN)};

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
//  pinMode(SWITCH_PIN, INPUT_PULLUP);  // スイッチ検出用のピン

  for(uint8_t i = 0; i < static_cast<uint8_t>(KEY_TYPE::MAX); i++){
    KeyInfoList[i].Initialize();
  }

  Serial.begin(9600);
  Keyboard.begin();

}

// the loop function runs over and over again forever
void loop() {
  
  // レイヤーの切り替え
  if(KeyInfoList[(uint8_t)KEY_TYPE::LAYER].CheckChangeState()){
    uint8_t state = KeyInfoList[(uint8_t)KEY_TYPE::LAYER].GetCurrentState();
    if(state == HIGH){
      // 0と1の切り替えしかないのでこれでOK
      CurrentLayer = CurrentLayer ^ LAYER1;
    }
  }

  // レイヤーが1ならLED光らせる
  if(LastLayer != CurrentLayer){
    LastLayer = CurrentLayer;
    digitalWrite(LED_BUILTIN, CurrentLayer);
  }

  if(KeyInfoList[(uint8_t)KEY_TYPE::SWITCH].CheckChangeState()){
    uint8_t state = KeyInfoList[(uint8_t)KEY_TYPE::SWITCH].GetCurrentState();
    switch(CurrentLayer){
    case LAYER0:
      if(state == HIGH){
        Keyboard.press(KEY_LEFT_GUI); // windowsだとこれがwindowsキー
//        Serial.println("on");
      }
      else{
        Keyboard.release(KEY_LEFT_GUI);
//        Serial.println("off");
      }
      break;
    case LAYER1:
      if(state == HIGH){  // エクスプローラを開くショートカット
        Keyboard.press(KEY_LEFT_GUI);
        Keyboard.press('e');
        Keyboard.releaseAll();
      }
      break;
    }
  }


}

