#include <ESP8266WiFi.h>
#include <WiFiUDP.h>
#include <Ticker.h>

//////////////////////
// Port Definitions //
//////////////////////
#define  ACTIVE_LED 14
#define  COMMAND_LED    12
#define  SWITCH    13
#define  NONE16   16
#define  NONE5     5
#define  NONE4     4
//////////////////////
// WiFi Definitions //
//////////////////////
#define MAX_CONNECT 1
const char APName[] = "********";
const char APPass[] = "12345678";      // 8 charctor
boolean alreadyConnected[MAX_CONNECT] ;
WiFiServer server(23);
WiFiClient client[MAX_CONNECT];
/////////////////////
// Data            //
/////////////////////
byte Count;
byte SwIn;
byte SwData;
boolean Acitve;
boolean Command;

void SdCardRead(void)
{
byte SdCardCommnad[] = {0x7E,0xFF,0x06,0x09,0x00,0x00,0x02,0xef};
  for ( byte i = 0 ; i < sizeof(SdCardCommnad) ; i++ )
  {
    Serial.write(SdCardCommnad[i]);
  }
  delay(200);
}
void PlayTrack(word Track)
{
 byte PlayTackCommand[] = {0x7E,0xFF,0x06,0x03,0x00,0x00,0x01,0xef};
  PlayTackCommand[5] =(byte)(Track >> 8);
  PlayTackCommand[6] =(byte)(Track);
  for ( byte i = 0 ; i < sizeof(PlayTackCommand) ; i++ )
  {
    Serial.write(PlayTackCommand[i]);
  }
  delay(200);
} 

/////////////////////////////////////////
/////// SetUp ///////////////////////////
/////////////////////////////////////////
void setup() 
{
  pinMode( ACTIVE_LED  , OUTPUT);
  pinMode( COMMAND_LED , OUTPUT);
  pinMode( SWITCH , INPUT);
  pinMode( NONE16 , OUTPUT);
  pinMode( NONE5  , OUTPUT);
  pinMode( NONE4  , OUTPUT);


  WiFi.softAPConfig(IPAddress(192, 168, 0, 1),       // ip
                    IPAddress(192, 168, 0, 1),        // gateway
                    IPAddress(255, 255, 255, 0)  );  //sub net mask

  WiFi.softAP((char *)&APName[0] , APPass );
 
  server.begin();

  Serial.begin(9600);
  SdCardRead();
  delay(2000);
  PlayTrack(1);
}



/////////////////////////////////////////
/////// Main Loop ///////////////////////
/////////////////////////////////////////
void loop(void) 
{
  delay(100);
  if(++ Count >= 10) Count = 1;

  if(digitalRead(SWITCH))
  {
    SwIn = true;
  }
  else
  {
    SwIn = false;
  }
  
  Command = SwIn;
  
  if(SwIn != SwData)
  {
      SwData = SwIn;
      if(!SwData)
      {
          PlayTrack(Count);
          
      }
      Command = SwData;
   }
  for (byte i = 0; i < MAX_CONNECT; i++)
  {
    if (!client[i])
    {
      client[i] = server.available();
    }
    else
    {
      if (client[i].status() == CLOSED)
      {
        client[i].stop();
        alreadyConnected[i] = false;
      }
      else
      {
        Acitve = true;
        if (!alreadyConnected[i]) // when the client sends the first byte, say hello:
        {
          alreadyConnected[i] = true;
        }

        if (client[i].available())
        {
           Count = client[i].read();
           PlayTrack(Count);
        }
      }
    }
  }
  if (  Acitve  )  digitalWrite(ACTIVE_LED, HIGH);
  else             digitalWrite(ACTIVE_LED, LOW );
  if (  Command )  digitalWrite(COMMAND_LED, HIGH);
  else             digitalWrite(COMMAND_LED, LOW );
}

