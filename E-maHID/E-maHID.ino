/*
  Layered Keys

  5keyの左手デバイス用スケッチ

*/

#include <Keyboard.h>


const uint8_t KEY_MAX = 5;

class KeyInfo
{

  uint8_t mPinIndex = 0xff;
  // 現在のピンの状態
  uint8_t mCurrentState = LOW;
  // 最後のピンの状態
  uint8_t mLastState = LOW;
  // キーが実際押されているカウント(チャタリング対策用)
  uint8_t mPressedCount = 0;
  // 検知時に押したとされるキーの定義
  uint8_t mKeyDefine = 0;

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
  KeyInfo(uint8_t pinindex, uint8_t keydefine)
  : mPinIndex(pinindex)
  , mKeyDefine(keydefine){}

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

  uint8_t GetKeyDefine()
  {
    return mKeyDefine;
  }

};

enum PIN{
  Pin5 = 5,
  Pin6 = 6,
  Pin7 = 7,
  Pin8 = 8,
  Pin9 = 9,
};

KeyInfo KeyInfoList[KEY_MAX] = {KeyInfo(PIN::Pin5, KEY_LEFT_ALT), KeyInfo(PIN::Pin6, KEY_LEFT_CTRL), KeyInfo(PIN::Pin7, KEY_LEFT_SHIFT),
                                KeyInfo(PIN::Pin8, 'z'), KeyInfo(PIN::Pin9, 's')};

// the setup function runs once when you press reset or power the board
void setup() {
  // initialize digital pin LED_BUILTIN as an output.
  pinMode(LED_BUILTIN, OUTPUT);
//  pinMode(SWITCH_PIN, INPUT_PULLUP);  // スイッチ検出用のピン

  for(uint8_t i = 0; i < KEY_MAX; i++){
    KeyInfoList[i].Initialize();
  }

//  Serial.begin(9600);
  Keyboard.begin();

}

// the loop function runs over and over again forever
void loop() {
  
  for(uint8_t i = 0; i < KEY_MAX; i++){
    // 前回と状態が変わってたら
    if(KeyInfoList[i].CheckChangeState()){
      KeyInfo& info = KeyInfoList[i];
      uint8_t state = info.GetCurrentState();
      if(state == HIGH){
        Keyboard.press(info.GetKeyDefine());
      }
      else{
        Keyboard.release(info.GetKeyDefine());
      }
    }
  }

}

