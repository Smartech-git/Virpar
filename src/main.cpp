#include <Keypad.h>
#include <EEPROM.h>

const int ROW_NUM = 4; //four rows
const int COLUMN_NUM = 4; //three columns
byte ledPin = 10;
byte ledPin2 = 11;
byte THRESHOLD = 12;
bool AUTHctrl = true;
bool CHANGEPASSWORDctrl = true;
bool NEWPASSWORDctrl = false;
bool NEWPASSWORDKEY = true;
bool USBACTIVE = false;
int USBSIGNALPIN = A0;
byte KEY = 13;

 char keys[ROW_NUM][COLUMN_NUM] = {
  {'1','2','3', 'A'},
  {'4','5','6', 'B'},
  {'7','8','9', 'C'},
  {'*','0','#', 'D'}
};

byte pin_rows[ROW_NUM] = {9, 8, 7, 6}; //connect to the row pinouts of the keypad
byte pin_column[COLUMN_NUM] = {5, 4, 3, 2}; //connect to the column pinouts of the keypad

Keypad keypad = Keypad( makeKeymap(keys), pin_rows, pin_column, ROW_NUM, COLUMN_NUM );

String password = "1234"; // change your password here
String input_password;

void writeStringToEEPROM(byte addrOffset, String strToWrite) {
  byte len = strToWrite.length();
  EEPROM.write(addrOffset, len);

  for (int i = 0; i < len; i++) {
    EEPROM.write( addrOffset + 1 + i, strToWrite[i]);
  }
}

String readStringFromEEPROM(byte addrOffset) {
  byte newStrlen = EEPROM.read(addrOffset);
  char data[newStrlen + 1];

  for(int i=0; i < newStrlen; i++) {
    data[i] = EEPROM.read(addrOffset + 1 + i);
  }
  data[newStrlen] = '\0';
  return String(data);
}

void setup(){
  Serial.begin(9600);
  pinMode(ledPin,  OUTPUT);
  pinMode(ledPin2, OUTPUT);
  pinMode(THRESHOLD, OUTPUT);
  pinMode(KEY, OUTPUT);
  pinMode(USBSIGNALPIN, INPUT);
  password = readStringFromEEPROM(0);
  Serial.println(password);
  input_password.reserve(32); // maximum input characters is 32, change if needed
  digitalWrite(THRESHOLD, LOW);
  digitalWrite(KEY, HIGH);
  delay(250);
  digitalWrite(KEY, LOW);
  delay(250);
  digitalWrite(KEY, HIGH);
  delay(250);
  digitalWrite(KEY, LOW);
  delay(250);
}

void loop(){
  char key = keypad.getKey();
 
  while(USBACTIVE == true ) {
    delay(1000);
    int USBSIGNAL = analogRead(USBSIGNALPIN);
    if(USBSIGNAL <= 500) {
      digitalWrite(THRESHOLD, HIGH);
      Serial.print(USBSIGNAL);
      Serial.println("off");
    }
  } 
  
  if (key){
    Serial.println(key);

    //CHANGE OF PASSWORD
    if(CHANGEPASSWORDctrl == true) {
      if (key == 'C' ) {
        AUTHctrl = false;
        input_password = "";
      }else if(key == '#' && NEWPASSWORDKEY == true) {
        if(password == input_password) {
          Serial.println("input new password");
          digitalWrite(ledPin2, LOW);
          for(int i=0; i<=5; i++){
            digitalWrite(ledPin, HIGH);
            delay(50);
            digitalWrite(ledPin, LOW);
            delay(50);
          };
          input_password = "";
          NEWPASSWORDctrl = true;
          NEWPASSWORDKEY = false;
        } else {
          Serial.println("password incorrect");
          Serial.println(input_password);
          digitalWrite(ledPin, LOW);
          for(int i = 0; i<=5; i++){
            digitalWrite(ledPin2, HIGH);
            delay(50);
            digitalWrite(ledPin2, LOW);
            delay(50);
          }
        }
        input_password = "";
      } else if(NEWPASSWORDctrl == true && key == '#') {
          writeStringToEEPROM(0, input_password);
          Serial.print("new password is : ");
          Serial.println(readStringFromEEPROM(0));
          for(int i=0; i<=10; i++){
                digitalWrite(ledPin, HIGH);
                delay(50);
                digitalWrite(ledPin, LOW);
                delay(50);
              };
          input_password = "";
          digitalWrite(THRESHOLD, HIGH);
          Serial.println("changed");
      }else if(key == 'D') {
        int inputLength = input_password.length() - 1;
        input_password.remove(inputLength, 1);
        Serial.println(input_password);
      }else {
        input_password += key;
        Serial.println(input_password);
      }
    }
     
    //AUTHENTICATION
    if (AUTHctrl == true) {
      if(key == '*'){
        CHANGEPASSWORDctrl = false;
        input_password = "";
      }else if(key == '#') {
        if(password == input_password) {
          Serial.println("password is correct");
          digitalWrite(ledPin, HIGH);
          digitalWrite(ledPin2, LOW);
          digitalWrite(KEY, HIGH);
          delay(500);
          digitalWrite(KEY, LOW);
          USBACTIVE = true; 
        } else {
          Serial.println("password is incorrect, try again");
          digitalWrite(ledPin, LOW);
          digitalWrite(ledPin2, HIGH);
        }
        input_password = "";
       }else if(key == 'D') {
         int inputLength = input_password.length() - 1;
         input_password.remove(inputLength, 1);
         Serial.println(input_password);
       }else {
         input_password += key; // append new character to input password string
         Serial.println(input_password); 
       }
    }
  }
}