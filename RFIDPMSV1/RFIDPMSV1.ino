#include <SPI.h>
#include <MFRC522.h>
#include <Servo.h>

#define SS_PIN    10
#define RST_PIN   9
#define SERVO_PIN A5
#define BUTTON_PIN 2  // Change this to the pin where your button is connected

MFRC522 rfid(SS_PIN, RST_PIN);
Servo servo;

byte authorizedUIDs[][4] = {
  {0x3A, 0xC9, 0x6A, 0xCB},
  {0x30, 0x01, 0x8B, 0x15},
  {0x03, 0x7D, 0x37, 0xFB},
  {0xD3, 0x08, 0x09, 0xFB},
  {0x23, 0x71, 0x32, 0xFB},
  {0xC3, 0x91, 0x25, 0xFB},
  {0xC3, 0xBA, 0x1E, 0xFB}
};

int angle = 0; // the current angle of servo motor
int buttonState = 0;
int lastButtonState = 0;

void setup() {
  Serial.begin(9600);
  SPI.begin(); // init SPI bus
  rfid.PCD_Init(); // init MFRC522
  servo.attach(SERVO_PIN);
  pinMode(BUTTON_PIN, INPUT_PULLUP);

  Serial.println("Tap RFID/NFC Tag on reader or press the button to control the servo");
}

void loop() {
  if (rfid.PICC_IsNewCardPresent()) { // new tag is available
    if (rfid.PICC_ReadCardSerial()) { // NUID has been read

      if (isAuthorized(rfid.uid.uidByte)) {
        Serial.println("Authorized Tag");
        changeServo(90); // turn servo to 90 degrees for authorized card
      } else {
        Serial.print("Unauthorized Tag with UID:");
        for (int i = 0; i < rfid.uid.size; i++) {
          Serial.print(rfid.uid.uidByte[i] < 0x10 ? " 0" : " ");
          Serial.print(rfid.uid.uidByte[i], HEX);
        }
        Serial.println();
      }

      rfid.PICC_HaltA(); // halt PICC
      rfid.PCD_StopCrypto1(); // stop encryption on PCD
    }
  }

  // Check the button state
  buttonState = digitalRead(BUTTON_PIN);

  // If the button is pressed, change servo angle to 0 degrees
  if (buttonState == LOW && lastButtonState == HIGH) {
    changeServo(0);
  }

  lastButtonState = buttonState;
}

bool isAuthorized(byte* uid) {
  for (int i = 0; i < sizeof(authorizedUIDs) / sizeof(authorizedUIDs[0]); i++) {
    if (memcmp(uid, authorizedUIDs[i], sizeof(authorizedUIDs[i])) == 0) {
      return true;
    }
  }
  return false;
}

void changeServo(int targetAngle) {
  // change angle of servo motor
  servo.write(targetAngle);
  Serial.print("Rotate Servo Motor to ");
  Serial.print(targetAngle);
  Serial.println("°");
}
