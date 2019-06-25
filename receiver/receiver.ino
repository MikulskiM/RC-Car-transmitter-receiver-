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

This code receive 1 channels and prints the value on the serial monitor
*/

#define TEST 0  /* 1 - pozwolisz na wykonanie dodatkowych instrukcji pomagających w testach (wyświetlanie stanu na serial 
                        monitorze itp.)
                   0 - brak testowych instrukcji
                */

#include <SPI.h>
#include <nRF24L01.h>
#include <RF24.h>

#define ENABLE12_PWM_PIN 3 // 1,5V do zasilanie sterujacego silnika
#define IN1_PIN 4 // 1 silnik (naped)
#define IN2_PIN 5 // 1 silnik (naped)
#define IN3_PIN 6 // 2 silnik (sterowanie)
#define IN4_PIN 7 // 2 silnik (sterowanie)


const uint64_t pipeIn = 0xE8E8F0F0E1LL;     //this code has to be the same in both transmitter and receiver
RF24 radio(9, 10);  //CSN and CE pins

// The sizeof this struct should not exceed 32 bytes
struct Received_data {
  byte ch1; // zmienna opisująca skręt ( wychylenie gałki potencjometra )
  byte ch2; // napędowa zmienna ( 0 = stop; 1 = jedź_przód; -1 = jedź_tył )
};

int ch1_value = 127;
int ch2_value = 0;
Received_data received_data;


/**************************************************/

void setup()
{
  #if TEST == 1
  Serial.begin(9600);
  #endif
  //We reset the received values
  received_data.ch1 = 127;
  received_data.ch2 = 0;
 
  //Once again, begin and radio configuration
  radio.begin();
  radio.setAutoAck(false);
  radio.setDataRate(RF24_250KBPS);  
  radio.openReadingPipe(1,pipeIn);
  
  //We start the radio comunication
  radio.startListening();

  pinMode(IN1_PIN, OUTPUT);   // wyjścia silnika napędowego
  pinMode(IN2_PIN, OUTPUT);   // wyjścia silnika napędowego
  pinMode(IN3_PIN, OUTPUT);   // wyjścia silnika skręcającego
  pinMode(IN4_PIN, OUTPUT);   // wyjścia silnika skręcającego
  pinMode(ENABLE12_PWM_PIN, OUTPUT);
  analogWrite (ENABLE12_PWM_PIN, 50); /* 77 to mniej wiecej 1,5V (sprawdź inne wartości, od 0 do 255)
                                        zmienna odpowiadająca za szybkość skręcania                      */
}

// -------------------------------------------------

unsigned long last_Time = 0;

//We create the function that will read the data every time
void receive_the_data()
{
  while ( radio.available() ) {
  radio.read(&received_data, sizeof(Received_data));
  last_Time = millis(); //Here we receive the data
}
}

// -------------------------------------------------

void loop()
{
  //Receive the radio data
  receive_the_data();

  ch1_value = map(received_data.ch1,0,255,0,255);
  ch2_value = received_data.ch2;
  if(ch2_value == 5){
    #if TEST == 1
    Serial.println("jazda do przodu");
    #endif
    digitalWrite(IN4_PIN, LOW);
    digitalWrite(IN3_PIN, HIGH);
  }
  else if(ch2_value == 10){
    #if TEST == 1
    Serial.println("jazda do tyłu");
    #endif
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, HIGH);
  }
  else{
    #if TEST == 1
    Serial.println("silnik wyłączony");
    #endif
    digitalWrite(IN3_PIN, LOW);
    digitalWrite(IN4_PIN, LOW);
  }
  
  #if TEST == 1
  Serial.println(ch1_value);
  #endif
  
  if(ch1_value < 120){
    #if TEST == 1
    Serial.println("skręca w lewo");
    #endif
    digitalWrite(IN2_PIN, LOW);
    digitalWrite(IN1_PIN, HIGH);
  }
  else if(ch1_value > 134){
    #if TEST == 1
    Serial.println("skręca w prawo");
    #endif
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, HIGH);
  }
  else{
    #if TEST == 1
    Serial.println("nie skręca");
    #endif
    digitalWrite(IN1_PIN, LOW);
    digitalWrite(IN2_PIN, LOW);
  }
  #if TEST == 1
  delay(500); // używaj delay przy obserwacji działania na serial monitorze
  #endif
}
