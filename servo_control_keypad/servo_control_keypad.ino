#include <Wire.h> 
#include <LiquidCrystal_I2C.h>

LiquidCrystal_I2C lcd(16, 2);

#include <Servo.h>
Servo myservo;

#include <Keypad.h>
const byte ROWS = 4; //four rows
const byte COLS = 3; //three columns
char keys[ROWS][COLS] = {
  {1,2,3},
  {4,5,6},
  {7,8,9},
  {'*','0','#'}
};
byte rowPins[ROWS] = {5, 4, 3, 2}; //connect to the row pinouts of the keypad
byte colPins[COLS] = {8, 7, 6}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), rowPins, colPins, ROWS, COLS );

int state = 0;
int key;
long set_jumlah, jumlah_ikan;

#define START 0
#define RUN 1
#define STOP 2

#define E18 10
bool hold_ikan;
bool dt_ikan;

const char BINTANG = 42; // nilai BINTANG
const char PAGAR = 35;

int pos = 0;

String line1, line2;

void setup(){
  pinMode(E18, INPUT_PULLUP);
  myservo.attach(9);  
  Serial.begin(9600);
  
  lcd.autoAddress();
  lcd.begin();
 
  Serial.println("system mulai");
}
  
void loop(){
  key = keypad.getKey();
  if (key) {
    baca_key(); 
  }
  update_lcd();
}
void baca_key(){
  switch(state){
    case START:
      if ( key == 48 ) key=0;
      if ( key < 10){
        set_jumlah = set_jumlah * 10 + key;
      }
        Serial.println("set jml ikan");
        Serial.println(set_jumlah);
        switch(key){
          case PAGAR:
            Serial.println("system RUN");
            buka_servo();
            state = RUN;
          break;
          case BINTANG:
            set_jumlah  = 0; jumlah_ikan = 0;
          break;
        }
    break;
    
    case RUN:
        buka_servo();
        Serial.println("system run");
        Serial.println(set_jumlah);
    break;
    
    case STOP:
      switch(key){
        case PAGAR:
          Serial.println("system stanby");
          set_jumlah = 0;
          state = START;
          set_jumlah  = 0; jumlah_ikan = 0;
        break;
      }
    break;
  }
}
void update_lcd(){
  get_text();
  lcd.setCursor(0,0);
  lcd.print(line1);
  lcd.setCursor(0,1);
  lcd.print(line2);
}

void get_text(){
  switch(state){
    case START:
      line1 = "Set Jumlah ikan";
      line2 = "Set: " +String(set_jumlah);
    break;

    case RUN:
      baca_sensor();
      line1 = "System Running..");
      line2 = "Jml: " + String(jumlah_ikan);
    break;

    case STOP:
      line1 = "System Stop.....");
      line2 = "jml: " + String(jumlah_ikan);
    break;
  }
}

void buka_servo(){
  Serial.println("servo di buka");
  for (pos = 0; pos <= 180; pos += 1) { // goes from 0 degrees to 180 degrees
    // in steps of 1 degree
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void tutup_servo(){
  Serial.println("servo di tutup");
  for (pos = 180; pos >= 0; pos -= 1) { // goes from 180 degrees to 0 degrees
    myservo.write(pos);              // tell servo to go to position in variable 'pos'
    delay(15);                       // waits 15ms for the servo to reach the position
  }
}

void baca_sensor(){
  dt_ikan = digitalRead(E18);
  if(dt_ikan == 0){
    if(hold_ikan == false){
      jumlah_ikan = jumlah_ikan + 1;
      hold_ikan = true;
      Serial.println(jumlah_ikan);
    }
  }else{
    hold_ikan = false;
  }
  if( jumlah_ikan >= set_jumlah){
    Serial.println("stop hitung ikan");
    tutup_servo();
    state = STOP;
  }
}
