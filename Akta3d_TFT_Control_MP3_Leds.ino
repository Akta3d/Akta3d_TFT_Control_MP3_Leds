#include <Akta3d_TFT_Colors.h>
#include <Akta3d_TFT_Utils.h>
#include <Akta3d_TFT_Button.h>
#include <Akta3d_TFT_Color_Picker.h>
#include <Akta3d_TFT_MP3_Control.h>
#include <Adafruit_GFX.h>
#include <MCUFRIEND_kbv.h>
MCUFRIEND_kbv _tft;
#include <TouchScreen.h>
#define MINPRESSURE 10
#define MAXPRESSURE 1000
#define TFT_ORIENTATION  LANDSCAPE

// ALL Touch panels and wiring is DIFFERENT
// copy-paste results from TouchScreen_Calibr_native.ino
const int XP=7,XM=A1,YP=A2,YM=6; //240x320 ID=0x9341
const int TS_LEFT=914,TS_RT=224,TS_TOP=228,TS_BOT=921;

Akta3d_Screen_Settings _screenSettings {XP, XM, YP, YM, TS_LEFT,TS_RT,TS_TOP,TS_BOT, TFT_ORIENTATION, MINPRESSURE, MAXPRESSURE};

TouchScreen _ts = TouchScreen(XP, YP, XM, YM, 300/*283*/);

// define buttons
Akta3d_TFT_Button _chooseColor1Button, _chooseColor2Button;
Akta3d_TFT_Button *buttons[] = {
  &_chooseColor1Button, 
  &_chooseColor2Button, 
  NULL
};

// colors selection
uint16_t _selectedColor1 = AK_PURPLE; 
void (*cbOnCancelColor1)() = &drawInterface;
void (*cbOnSelectColor1)(HSV) = &onSelectColor1;

uint16_t _selectedColor2 = AK_BLUE; 
void (*cbOnCancelColor2)() = &drawInterface;
void (*cbOnSelectColor2)(HSV) = &onSelectColor2;
Akta3d_TFT_Color_Picker _colorPicker(&_tft);

Akta3d_TFT_MP3_Control _mp3Control(&_tft, 0);

#define VIEW_MAIN_GUI 0
#define VIEW_SELECT_COLOR1 1
#define VIEW_SELECT_COLOR2 2
uint16_t _currentView = VIEW_MAIN_GUI;

void drawInterface() {
  Serial.println("drawInterface");
  _currentView = VIEW_MAIN_GUI;
  
  _tft.fillScreen(AK_BACKGROUND);

  uint16_t lineHeight = 23;
/*
 * 1  : line
 * 1  : empty
 * 1  : btn outline
 * 1  : btn empty
 * 16 : btn txt
 * 1  : btn empty
 * 1  : btn outline 
 * 1  : empty
 * 1  : line
 */

  uint16_t mp3Height = _mp3Control.getInterfaceHeight();
  
  // line 1 
  uint16_t lineTextY = 4 + mp3Height;  
  uint16_t lineBtnX = 280;
  uint16_t lineBtnY = 2 + mp3Height;
  uint16_t lineBtnW = 40;
  uint16_t lineBtnH = 18;
  uint16_t lineBottomLineY = 23 + mp3Height;
  
  _tft.setCursor(1, lineTextY);
  _tft.setTextColor(_selectedColor1);
  _tft.setTextSize(2);
  _tft.print("Color 1");

  _chooseColor1Button.initButtonUL(&_tft, lineBtnX, lineBtnY, lineBtnW, lineBtnH, AK_COLOR_BTN_OUTLINE, AK_COLOR_BTN_FILL, AK_COLOR_BTN_TEXT, "Color", 1);
  _chooseColor1Button.drawButton();

  _tft.drawFastHLine(0, lineBottomLineY, 320, AK_COLOR_LINE);

  // line 2
  lineTextY += lineHeight;  
  lineBtnY += lineHeight;
  lineBottomLineY += lineHeight;  
  _tft.setCursor(1, lineTextY);
  _tft.setTextColor(_selectedColor2);
  _tft.setTextSize(2);
  _tft.print("Color 2");

  _tft.drawFastHLine(0, lineBottomLineY, 320, AK_COLOR_LINE);
        
  _chooseColor2Button.initButtonUL(&_tft, lineBtnX, lineBtnY, lineBtnW, lineBtnH, AK_COLOR_BTN_OUTLINE, AK_COLOR_BTN_FILL, AK_COLOR_BTN_TEXT, "Color", 1);
  _chooseColor2Button.drawButton();

  _mp3Control.drawInterface();
}

void onSelectColor1(HSV color) {
  _selectedColor1 = Akta3d_TFT_Utils::hsvToHex(color);
  drawInterface();
}
void onSelectColor2(HSV color) {
  _selectedColor2 = Akta3d_TFT_Utils::hsvToHex(color);
  drawInterface();
}

void setup(void) {
    Serial.begin(9600);
    
    Serial.println("");
    Serial.println("");
    Serial.println("--- START SETUP ---");

    // init TFT
    uint16_t ID = _tft.readID();       
    Serial.print("TFT ID = 0x");
    Serial.println(ID, HEX);
    if (ID == 0xD3D3) ID = 0x9486; // write-only shield
    _tft.begin(ID);

    // set orientation
    Serial.print("TFT ORIENTATION = ");
    if(TFT_ORIENTATION == PORTRAIT) {
      Serial.println("PORTRAIT");
    } else {
      Serial.println("LANDSCAPE");      
    }
    _tft.setRotation(TFT_ORIENTATION);   
     
    // draw interface
    drawInterface();
    
    Serial.println("--- END SETUP ---");
}

void loop(void)
{  
    Akta3d_Touch_Info touchInfo = Akta3d_TFT_Utils::getTouchInfo(&_tft, &_ts, _screenSettings);

    switch(_currentView) {
      case VIEW_MAIN_GUI:
        // update all buttons
        Akta3d_TFT_Button::updateButtonList(buttons, touchInfo);
    
        if (_chooseColor1Button.justReleased()) {      
            //Serial.println("_chooseColor1Button touch");        
            _currentView = VIEW_SELECT_COLOR1;
            _colorPicker.display(Akta3d_TFT_Utils::hexToHsv(_selectedColor1), cbOnCancelColor1, cbOnSelectColor1);
        }
        if (_chooseColor2Button.justReleased()) {
            //Serial.println("_chooseColor2Button touch");
            _currentView = VIEW_SELECT_COLOR2;
            _colorPicker.display(Akta3d_TFT_Utils::hexToHsv(_selectedColor2), cbOnCancelColor2, cbOnSelectColor2);
        } 

        _mp3Control.update(touchInfo);
      break;
      
      case VIEW_SELECT_COLOR1:      
      case VIEW_SELECT_COLOR2:
        _colorPicker.update(touchInfo);
      break;    
    }
    

}
