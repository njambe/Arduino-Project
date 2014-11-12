// Author : Nathan Jambe
// This code is intended to work with the prototype 0.2.1
// September 2014

#include <EtherCard.h>
#include <SPI.h>
#include <VirtualWire.h>

#define DEBUG      1    // Use to send data to serial port

/********* IO PORTS **********/
#define PIN_SCE    6
#define PIN_RESET  7
#define PIN_DC     5
#define PIN_SDIN   4
#define PIN_SCLK   3

#define LED_R      A5 
#define LED_Y      A4 
#define LED_G      A3 
  
#define BUT_UP     A0
#define BUT_DOWN   9 
#define BUT_CENTER A2
#define BUT_LEFT   10
#define BUT_RIGHT  A1

/****** NETWORK VALUES ******/
#define STATIC       1  // set to 1 to disable DHCP
#define IN_PORT      7777
#define OUT_PORT     7890
#define SERVER_IP    "192.168.1.98"
byte myip[4] =       {192, 168, 1, 80};
byte gwip[4] =       {192, 168, 1, 1};
static byte mymac[] = { 0x74,0x69,0x69,0x2D,0x30,0x31 }; // ethernet mac address

/****** SCREEN VALUES ******/
#define LCD_C     LOW
#define LCD_D     HIGH
#define LCD_X     84
#define LCD_Y     48

/******** FUNCTIONS *********/
#define LED_R_ON digitalWrite(LED_R, HIGH)
#define LED_R_OFF digitalWrite(LED_R, LOW)
#define LED_Y_ON digitalWrite(LED_Y, HIGH)
#define LED_Y_OFF digitalWrite(LED_Y, LOW)
#define LED_G_ON digitalWrite(LED_G, HIGH)
#define LED_G_OFF digitalWrite(LED_G, LOW)

#define CND_UP if (BUT_UP == HIGH)
#define CND_DOWN if (BUT_DOWN == HIGH)
#define CND_LEFT if (BUT_LEFT == HIGH)
#define CND_RIGHT if (BUT_RIGHT == HIGH)
#define CND_CENTER if (BUT_CENTER == HIGH)

#define WHILE_UP_PRESS      while (digitalRead(BUT_UP) == LOW)
#define WHILE_DOWN_PRESS    while (digitalRead(BUT_DOWN) == LOW)
#define WHILE_LEFT_PRESS    while (digitalRead(BUT_LEFT) == LOW)
#define WHILE_RIGHT_PRESS   while (digitalRead(BUT_RIGHT) == LOW)
#define WHILE_CENTER_PRESS  while (digitalRead(BUT_CENTER) == LOW)
#define WHILE_CENTER_NPRESS while (digitalRead(BUT_CENTER) == HIGH)

// Ethernet variables
byte Ethernet::buffer[700]; // tcp/ip send and receive buffer
volatile int nbsent = 0;
volatile int nbrecv = 0;

// Screen variables
int count=0;
char dat[4];
char disp_tab[]={
  '0','1','2','3','4','5','6','7','8','9'};
char buf[72]="A_simple_example_of_interfacing_with_the_84_x_48_pixel_Nokia_3310_LCD!!";

static const byte ASCII[][5] =
{  {    0x00, 0x00, 0x00, 0x00, 0x00    } // 20  
  ,{    0x00, 0x00, 0x5f, 0x00, 0x00    } // 21 !
  ,{    0x00, 0x07, 0x00, 0x07, 0x00    } // 22 "
  ,{    0x14, 0x7f, 0x14, 0x7f, 0x14    } // 23 #
  ,{    0x24, 0x2a, 0x7f, 0x2a, 0x12    } // 24 $
  ,{    0x23, 0x13, 0x08, 0x64, 0x62    } // 25 %
  ,{    0x36, 0x49, 0x55, 0x22, 0x50    } // 26 &
  ,{    0x00, 0x05, 0x03, 0x00, 0x00    } // 27 '
  ,{    0x00, 0x1c, 0x22, 0x41, 0x00    } // 28 (
  ,{    0x00, 0x41, 0x22, 0x1c, 0x00    } // 29 )
  ,{    0x14, 0x08, 0x3e, 0x08, 0x14    } // 2a *
  ,{    0x08, 0x08, 0x3e, 0x08, 0x08    } // 2b +
  ,{    0x00, 0x50, 0x30, 0x00, 0x00    } // 2c ,
  ,{    0x08, 0x08, 0x08, 0x08, 0x08    } // 2d -
  ,{    0x00, 0x60, 0x60, 0x00, 0x00    } // 2e .
  ,{    0x20, 0x10, 0x08, 0x04, 0x02    } // 2f /
  ,{    0x3e, 0x51, 0x49, 0x45, 0x3e    } // 30 0
  ,{    0x00, 0x42, 0x7f, 0x40, 0x00    } // 31 1
  ,{    0x42, 0x61, 0x51, 0x49, 0x46    } // 32 2
  ,{    0x21, 0x41, 0x45, 0x4b, 0x31    } // 33 3
  ,{    0x18, 0x14, 0x12, 0x7f, 0x10    } // 34 4
  ,{    0x27, 0x45, 0x45, 0x45, 0x39    } // 35 5
  ,{    0x3c, 0x4a, 0x49, 0x49, 0x30    } // 36 6
  ,{    0x01, 0x71, 0x09, 0x05, 0x03    } // 37 7
  ,{    0x36, 0x49, 0x49, 0x49, 0x36    } // 38 8
  ,{    0x06, 0x49, 0x49, 0x29, 0x1e    } // 39 9
  ,{    0x00, 0x36, 0x36, 0x00, 0x00    } // 3a :
  ,{    0x00, 0x56, 0x36, 0x00, 0x00    } // 3b ;
  ,{    0x08, 0x14, 0x22, 0x41, 0x00    } // 3c <
  ,{    0x14, 0x14, 0x14, 0x14, 0x14    } // 3d =
  ,{    0x00, 0x41, 0x22, 0x14, 0x08    } // 3e >
  ,{    0x02, 0x01, 0x51, 0x09, 0x06    } // 3f ?
  ,{    0x32, 0x49, 0x79, 0x41, 0x3e    } // 40 @
  ,{    0x7e, 0x11, 0x11, 0x11, 0x7e    } // 41 A
  ,{    0x7f, 0x49, 0x49, 0x49, 0x36    } // 42 B
  ,{    0x3e, 0x41, 0x41, 0x41, 0x22    } // 43 C
  ,{    0x7f, 0x41, 0x41, 0x22, 0x1c    } // 44 D
  ,{    0x7f, 0x49, 0x49, 0x49, 0x41    } // 45 E
  ,{    0x7f, 0x09, 0x09, 0x09, 0x01    } // 46 F
  ,{    0x3e, 0x41, 0x49, 0x49, 0x7a    } // 47 G
  ,{    0x7f, 0x08, 0x08, 0x08, 0x7f    } // 48 H
  ,{    0x00, 0x41, 0x7f, 0x41, 0x00    } // 49 I
  ,{    0x20, 0x40, 0x41, 0x3f, 0x01    } // 4a J
  ,{    0x7f, 0x08, 0x14, 0x22, 0x41    } // 4b K
  ,{    0x7f, 0x40, 0x40, 0x40, 0x40    } // 4c L
  ,{    0x7f, 0x02, 0x0c, 0x02, 0x7f    } // 4d M
  ,{    0x7f, 0x04, 0x08, 0x10, 0x7f    } // 4e N
  ,{    0x3e, 0x41, 0x41, 0x41, 0x3e    } // 4f O
  ,{    0x7f, 0x09, 0x09, 0x09, 0x06    } // 50 P
  ,{    0x3e, 0x41, 0x51, 0x21, 0x5e    } // 51 Q
  ,{    0x7f, 0x09, 0x19, 0x29, 0x46    } // 52 R
  ,{    0x46, 0x49, 0x49, 0x49, 0x31    } // 53 S
  ,{    0x01, 0x01, 0x7f, 0x01, 0x01    } // 54 T
  ,{    0x3f, 0x40, 0x40, 0x40, 0x3f    } // 55 U
  ,{    0x1f, 0x20, 0x40, 0x20, 0x1f    } // 56 V
  ,{    0x3f, 0x40, 0x38, 0x40, 0x3f    } // 57 W
  ,{    0x63, 0x14, 0x08, 0x14, 0x63    } // 58 X
  ,{    0x07, 0x08, 0x70, 0x08, 0x07    } // 59 Y
  ,{    0x61, 0x51, 0x49, 0x45, 0x43    } // 5a Z
  ,{    0x00, 0x7f, 0x41, 0x41, 0x00    } // 5b [
  ,{    0x02, 0x04, 0x08, 0x10, 0x20    } // 5c ¥
  ,{    0x00, 0x41, 0x41, 0x7f, 0x00    } // 5d ]
  ,{    0x04, 0x02, 0x01, 0x02, 0x04    } // 5e ^
  ,{    0x40, 0x40, 0x40, 0x40, 0x40    } // 5f _
  ,{    0x00, 0x01, 0x02, 0x04, 0x00    } // 60 `
  ,{    0x20, 0x54, 0x54, 0x54, 0x78    } // 61 a
  ,{    0x7f, 0x48, 0x44, 0x44, 0x38    } // 62 b
  ,{    0x38, 0x44, 0x44, 0x44, 0x20    } // 63 c
  ,{    0x38, 0x44, 0x44, 0x48, 0x7f    } // 64 d
  ,{    0x38, 0x54, 0x54, 0x54, 0x18    } // 65 e
  ,{    0x08, 0x7e, 0x09, 0x01, 0x02    } // 66 f
  ,{    0x0c, 0x52, 0x52, 0x52, 0x3e    } // 67 g
  ,{    0x7f, 0x08, 0x04, 0x04, 0x78    } // 68 h
  ,{    0x00, 0x44, 0x7d, 0x40, 0x00    } // 69 i
  ,{    0x20, 0x40, 0x44, 0x3d, 0x00    } // 6a j 
  ,{    0x7f, 0x10, 0x28, 0x44, 0x00    } // 6b k
  ,{    0x00, 0x41, 0x7f, 0x40, 0x00    } // 6c l
  ,{    0x7c, 0x04, 0x18, 0x04, 0x78    } // 6d m
  ,{    0x7c, 0x08, 0x04, 0x04, 0x78    } // 6e n
  ,{    0x38, 0x44, 0x44, 0x44, 0x38    } // 6f o
  ,{    0x7c, 0x14, 0x14, 0x14, 0x08    } // 70 p
  ,{    0x08, 0x14, 0x14, 0x18, 0x7c    } // 71 q
  ,{    0x7c, 0x08, 0x04, 0x04, 0x08    } // 72 r
  ,{    0x48, 0x54, 0x54, 0x54, 0x20    } // 73 s
  ,{    0x04, 0x3f, 0x44, 0x40, 0x20    } // 74 t
  ,{    0x3c, 0x40, 0x40, 0x20, 0x7c    } // 75 u
  ,{    0x1c, 0x20, 0x40, 0x20, 0x1c    } // 76 v
  ,{    0x3c, 0x40, 0x30, 0x40, 0x3c    } // 77 w
  ,{    0x44, 0x28, 0x10, 0x28, 0x44    } // 78 x
  ,{    0x0c, 0x50, 0x50, 0x50, 0x3c    } // 79 y
  ,{    0x44, 0x64, 0x54, 0x4c, 0x44    } // 7a z
  ,{    0x00, 0x08, 0x36, 0x41, 0x00    } // 7b {
  ,{    0x00, 0x00, 0x7f, 0x00, 0x00    } // 7c |
  ,{    0x00, 0x41, 0x36, 0x08, 0x00    } // 7d }
  ,{    0x10, 0x08, 0x08, 0x10, 0x08    } // 7e ←
  ,{    0x78, 0x46, 0x41, 0x46, 0x78    } // 7f →
};

void LcdCharacter(char character) {
  LcdWrite(LCD_D, 0x00);
  for (int index = 0; index < 5; index++) {
    LcdWrite(LCD_D, ASCII[character - 0x20][index]);
  }
  LcdWrite(LCD_D, 0x00);
}

void LcdClear(void) {
  for (int index = 0; index < LCD_X * LCD_Y / 8; index++) {
    LcdWrite(LCD_D, 0x00);
  }
}

void LcdInitialise(void) {
  pinMode(PIN_SCE, OUTPUT);
  pinMode(PIN_RESET, OUTPUT);
  pinMode(PIN_DC, OUTPUT);
  pinMode(PIN_SDIN, OUTPUT);
  pinMode(PIN_SCLK, OUTPUT);
  digitalWrite(PIN_RESET, LOW);
  digitalWrite(PIN_RESET, HIGH);
  LcdWrite(LCD_C, 0x21 );  // LCD Extended Commands.
  LcdWrite(LCD_C, 0xBF );  // Set LCD Vop (Contrast). 
  LcdWrite(LCD_C, 0x04 );  // Set Temp coefficent. //0x04
  LcdWrite(LCD_C, 0x14 );  // LCD bias mode 1:48. //0x13
  LcdWrite(LCD_C, 0x0C );  // LCD in normal mode.
  LcdWrite(LCD_C, 0x20 );
  LcdWrite(LCD_C, 0x0C );
}

void LcdString(char *characters) {
  while (*characters) {
    LcdCharacter(*characters++);
  }
}

void LcdWrite(byte dc, byte data) {
  digitalWrite(PIN_DC, dc);
  digitalWrite(PIN_SCE, LOW);
  shiftOut(PIN_SDIN, PIN_SCLK, MSBFIRST, data);
  digitalWrite(PIN_SCE, HIGH);
}

void gotoXY(int x, int y) {
  LcdWrite( 0, 0x80 | x);  // Column.
  LcdWrite( 0, 0x40 | y);  // Row.  

}

void inline ethernetInit(void) {
  if (ether.begin(sizeof Ethernet::buffer, mymac) == 0)
    LED_R_ON;
  #if STATIC
    ether.staticSetup(myip, gwip);
  #else
    if (!ether.dhcpSetup())
      LED_Y_ON;
  #endif
  
  ether.parseIp(ether.hisip, SERVER_IP);
  ether.udpServerListenOnPort(udpHandler, IN_PORT);
  ether.registerPingCallback(gotPinged);
}

// Performed actions while receive a UDP packet
void udpHandler(uint16_t dport, uint8_t * sip, const char * message, uint16_t len) {
  Serial.println("Packet received");
  gotoXY(0,5);
  LcdString("In : ");
  LcdString(itoa(++nbrecv, buf, 10));
}

void sendServer() {
  char data[22] = "Coucou";
  int len_data = 6;
  
  ether.sendUdp(data, len_data, IN_PORT, ether.hisip, OUT_PORT);
  gotoXY(0,4);
  LcdString("Out: ");
  LcdString(itoa(++nbsent, buf, 10));
}

// called when a ping comes in (replies to it are automatic)
static void gotPinged (byte* ptr) {
  Serial.println("pinged");
  gotoXY(0,5);
  LcdString("In : ");
  LcdString(itoa(++nbrecv, buf, 10));
}

void setup(void) {  
  // IO
  pinMode(BUT_UP, INPUT);
  pinMode(BUT_DOWN, INPUT);
  pinMode(BUT_LEFT, INPUT);
  pinMode(BUT_RIGHT, INPUT);
  pinMode(BUT_CENTER, INPUT);
  
  pinMode(LED_R, OUTPUT);
  pinMode(LED_Y, OUTPUT);
  pinMode(LED_G, OUTPUT);
  
  LED_R_ON;
  
  // Screen
  LcdInitialise();
  LcdClear();
  
  LED_R_OFF;
  LED_Y_ON;
  
  // Ethernet
  ethernetInit();
  
  #if DEBUG
    // Serial if debug mode enabled
    Serial.begin(9600);
    Serial.println("Server : UP");
  #endif
  
  gotoXY(0,0);
  LcdString("== SERVER ==");
  gotoXY(0,1);
  LcdString("Status: UP");
  
  LED_Y_OFF;
  LED_G_ON;
}

void inline io(void) {
  word len = ether.packetReceive(); // go receive new packets
  word pos = ether.packetLoop(len); // respond to incoming pings
}

void loop(void) {
  // MANDATORY !!!
  io();
  // END MANDATORY
  
  CND_UP {
    LED_Y_ON;
    sendServer();
    LED_Y_OFF;
    WHILE_UP_PRESS {  io();  }
  }
}

