/*
 *  Radio Controlled Car project made by Marek Mikulski
 * 
    moduł bezprzewodowy NRF24 -> NANO
    
    GND    ->   GND
    Vcc    ->   3.3V
    CE     ->   D9
    CSN    ->   D10
    CLK    ->   D13
    MOSI   ->   D11
    MISO   ->   D12

This code transmits 1 channels with data from pins A0 POTENTIOMETER
*/

#define TEST 0  /* 1 - pozwolisz na wykonanie dodatkowych instrukcji pomagających w testach (wyświetlanie stanu na serial 
                        monitorze itp.)
                   0 - brak testowych instrukcji
                */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

//#define FRONT_BUTTON 8  // pin przycisku odpowiedzialnego za jadę do przodu
//#define BACK_BUTTON 7   // pin przycisku odpowiedzialnego za jadę do tyłu
#define FRONT_SWITCH 8  // ON when steering switch is at "go forward" position
//#define BACK_SWITCH 7 // I won't use it. LOW signal on FRONT_SWITCH will do the same job
#define ACCELERATION_BUTTON 7 // when pushed provides acceleration on power motor

const uint64_t my_radio_pipe = 0xE8E8F0F0E1LL; // this code has to be the same in both transmitter and receiver

RF24 radio(9, 10);

// The sizeof this struct should not exceed 32 bytes
struct Data_to_be_sent {
  byte ch1;  // zmienna opisująca wychylenie gałki potencjometra (skręt kierownicy)
  byte ch2;  // zmienna napędowa (( 0 = stop; 1 = jedź_przód; -1 = jedź_tył )
};

Data_to_be_sent sent_data;

void setup()
{
  pinMode(FRONT_SWITCH, INPUT_PULLUP);
  pinMode(ACCELERATION_BUTTON, INPUT_PULLUP);
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);
  radio.openWritingPipe(my_radio_pipe);  
  sent_data.ch1 = 127;
  sent_data.ch2 = 0;

  #if TEST == 1
  Serial.begin(9600);
  #endif
}

/**************************************************/


void loop()
{
  /*If your channel is reversed, just swap 0 to 255 by 255 to 0 below
  EXAMPLE:
  Normal:    data.ch1 = map( analogRead(A0), 0, 1024, 0, 255);
  Reversed:  data.ch1 = map( analogRead(A0), 0, 1024, 255, 0);  */

  #if TEST == 1
  delay(500); // używaj delay przy obserwacji działania na serial monitorze
  #endif
  
  sent_data.ch1 = map( analogRead(A0), 0, 1024, 0, 255);
  
  #if TEST == 1
  Serial.print("sygnał z potencjometru <0, 255>: ");
  Serial.println(sent_data.ch1);
  #endif

  if(digitalRead(FRONT_SWITCH) == LOW && digitalRead(ACCELERATION_BUTTON) == LOW){

    #if TEST == 1
    Serial.println("jazda do przodu");
    #endif
    sent_data.ch2 = 5;
  }
  else if(digitalRead(FRONT_SWITCH) == HIGH && digitalRead(ACCELERATION_BUTTON) == LOW){
    #if TEST == 1
    Serial.println("jazda do tyłu");
    #endif
    sent_data.ch2 = 10;
  }
  else{
    sent_data.ch2 = 0;
    #if TEST == 1
    Serial.println("nie jedzie");
    #endif
  }
/*
  if(digitalRead(FRONT_SWITCH) == HIGH)
  Serial.print("front_switch HIGH\t");
  else
  Serial.print("front_switch LOW\t");
  if(digitalRead(ACCELERATION_BUTTON) == HIGH)
  Serial.print("acceleration_button HIGH\t");
  else
  Serial.print("acceleration_button LOW\t");
  */
  radio.write(&sent_data, sizeof(Data_to_be_sent));
}
