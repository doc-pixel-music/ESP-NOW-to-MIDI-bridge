/*
  Here is the code to turn an ESP32-S3 into a dongle that receive wireless message from 
  other board via ESP-NOW (exemple with the Thereminou : https://github.com/doc-pixel-music/Thereminou/tree/main/Thereminou_ESP_NOW_esp32)
  It act like a USB MIDI device called 'ESP-NOW_bridge'

  Many thanks to Rui Santos (https://RandomNerdTutorials.com/esp-now-esp32-arduino-ide/)
  
  Permission is hereby granted, free of charge, to any person obtaining a copy
  of this software and associated documentation files.
  
  The above copyright notice and this permission notice shall be included in all
  copies or substantial portions of the Software.

  DOC PIXEL Corp. (https://doc-pixel.com/)
*/

#include <esp_now.h>
#include <WiFi.h>
#include <Control_Surface.h>

USBMIDI_Interface MIDI; // Instantiate the MIDI over USB interface
// USBSerialMIDI_Interface MIDI = 115200;
SET_USB_MIDI_DEVICE_NAME("ESP-NOW_bridge"); // Customize the name of the device

// Structure example to send data
// Must match the receiver structure
typedef struct struct_message {
  byte channel; //midi channel to use
  byte type;  // 1 : Note ; 2 : Controle Change; 3 : Program
  byte address; // Address of note, cc or program change (0-127)
  byte value; // Velocity on type note or Controle Change value
  bool keypress; // Note ON or OFF (only for Type 1)
} struct_message;

// Create a struct_message called myData
struct_message myData;

// Create a structure to hold the readings from each board
struct_message board1;
struct_message board2;
struct_message board3;
struct_message board4;
struct_message board5;
struct_message board6;
struct_message board7;

// Create an array with all the structures
struct_message boardsStruct[7] = {board1, board2, board3, board4, board5, board6, board7};

// callback function that will be executed when data is received
void OnDataRecv(const uint8_t * mac, const uint8_t *incomingData, int len) {
  memcpy(&myData, incomingData, sizeof(myData));
  /*
  Serial.print("Bytes received: ");
  Serial.println(len);
  Serial.print("Channel: ");
  Serial.println(myData.channel);
  Serial.print("Type: ");
  Serial.println(myData.type);
  Serial.print("Address: ");
  Serial.println(myData.address);
  Serial.print("Value: ");
  Serial.println(myData.value);
  Serial.println();
  */

  MIDIAddress address {myData.address, Channel(myData.channel)-1}; // First Channel start with 0

  if(myData.type == 1){ //Type note
    if(myData.keypress == 1){
      MIDI.sendNoteOn(address, myData.value);
    } else {
      MIDI.sendNoteOff(address, myData.value);
    }
  }else if(myData.type == 2){ // Type Control Change
    MIDI.sendControlChange(address, myData.value);
  }else{ // Type Program change
    // MIDI.sendProgramChange(address, myData.value);
  }
}
 
void setup() {
  // Initialize Serial Monitor
  Serial.begin(115200);

  // initialize the MIDI interface
  // Control_Surface.begin();
  MIDI.begin();
  
  // Set device as a Wi-Fi Station
  WiFi.mode(WIFI_STA);

  // Init ESP-NOW
  if (esp_now_init() != ESP_OK) {
    Serial.println("Error initializing ESP-NOW");
    return;
  }
  
  // Once ESPNow is successfully Init, we will register for recv CB to
  // get recv packer info
  esp_now_register_recv_cb(esp_now_recv_cb_t(OnDataRecv));

}
 
void loop() {

}