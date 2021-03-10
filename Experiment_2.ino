////////////////////////////////////////////////////////////////////////

////////////////////////////////////////////////////////////////////////

#include "Arduino.h"   // adds the libraries 
#include "SoftwareSerial.h"   //allows us to treat pins as RX, TX pins
#include "DFRobotDFPlayerMini.h"   // easier to send commands to Mp3

SoftwareSerial speaker(10, 11); // RX, TX
DFRobotDFPlayerMini myDFPlayer;
void printDetail(uint8_t type, int value);

int bluetoothTx = 0;
int bluetoothRx = 1;
SoftwareSerial bluetooth(bluetoothTx, bluetoothRx);

#define BAUDRATE 57600
#define DEBUGOUTPUT 0



#define powercontrol 10

// checksum variableX
byte generatedChecksum = 0;
byte checksum = 0; 
int payloadLength = 0;
byte payloadData[64] = {
  0};
byte poorQuality = 0;
byte attention = 0;
byte meditation = 0;

// system variables
long lastReceivedPacket = 0;
boolean bigPacket = false;

//////////////////////////
// Microprocessor Setup //n
//////////////////////////
void setup() {
  Serial.begin(BAUDRATE);           // transmission of bluetooth
  speaker.begin(BAUDRATE);           // transmission of mp3
  bluetooth.begin(BAUDRATE);        // bluetooth transmission

  myDFPlayer.volume(10);  //Set volume value. From 0 to 
  myDFPlayer.play(1);  //Play the first mp3

}

////////////////////////////////
// Read data from Serial UART //
////////////////////////////////

byte ReadOneByte() {
  int ByteRead;
  bluetooth.listen();
  while(!bluetooth.available());
  ByteRead = bluetooth.read();
  
  //Serial.print("Hello");

#if DEBUGOUTPUT  
  Serial.print((char)ByteRead);   // echo the same byte out the USB serial (for debug purposes)
#endif

  return ByteRead;
}


///////////////
void loop() {

  // Look for sync bytes
  if(ReadOneByte() == 170) {
    if(ReadOneByte() == 170) {
    
      payloadLength = ReadOneByte();
      if(payloadLength > 169)                      //Payload length can not be greater than 169
          return;

      generatedChecksum = 0;        
      for(int i = 0; i < payloadLength; i++) {  
        payloadData[i] = ReadOneByte();            //Read payload into memory
        generatedChecksum += payloadData[i];
      }   

      checksum = ReadOneByte();                      //Read checksum byte from stream      
      generatedChecksum = 255 - generatedChecksum;   //Take one's compliment of generated checksum

        if(checksum == generatedChecksum) {    

        poorQuality = 200;
        attention = 0;
        meditation = 0;

        for(int i = 0; i < payloadLength; i++) {    // Parse the payload
          switch (payloadData[i]) {
          case 2:
            i++;            
            poorQuality = payloadData[i];
            bigPacket = true;            
            break;
          case 4:
            i++;
            attention = payloadData[i];                        
            break;
          case 5:
            i++;
            meditation = payloadData[i];
            break;
          case 0x80:
            i = i + 3;
            break;
          case 0x83:
            i = i + 25;      
            break;
          default:
            break;
          } // switch
        } // for loop

#if !DEBUGOUTPUT

        // *** Add your code here ***

        if(bigPacket) {      //prints out values 
          if(poorQuality == 0)
            Serial.print("fuck ");
          else
          Serial.print("PoorQuality: ");
          Serial.print(poorQuality, DEC);
          Serial.print(" Attention: ");
          Serial.print(attention, DEC);
          Serial.print(" Time since last packet: ");
          Serial.print(millis() - lastReceivedPacket, DEC);
          lastReceivedPacket = millis();
          Serial.print("\n");

          switch(attention / 10) {
          case 0:
          myDFPlayer.play(1);
            break;
          case 1:
          myDFPlayer.play(1);

            break;
          case 2:

          myDFPlayer.play(2);

            break;
          case 3:
          myDFPlayer.play(3);              
            
            break;
          case 4:
          myDFPlayer.play(4);
           
            break;
          case 5:
          myDFPlayer.play(5);
             
            break;
          case 6:
          myDFPlayer.play(6);              
           
            break;
          case 7:
          myDFPlayer.play(7);
           
            break;    
          case 8:
          myDFPlayer.play(8);

            break;
          case 9:
          myDFPlayer.play(9);

            break;
          case 10:
          myDFPlayer.play(10);

            break;           
          }                     
  }
#endif        
        bigPacket = false;        
      }
      else {
        // Checksum Error
      }  // end if else for checksum
    } // end if read 0xAA byte
  } // end if read 0xAA byte
}
