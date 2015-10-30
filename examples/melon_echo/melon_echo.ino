// ----------------------------------
// Unified CAN driver example for Arduino and Teensy 3.1/3.2
//

#include <mcp_can_dfs.h>
#include <MelonCAN.h>
#include <FlexCAN.h>
#include <mcp_can.h>
#include <SPI.h>

MelonCAN flex(flex_controller);
MelonCAN mcp(mcp2515_controller, 10);

static CAN_message_t msg;
static CAN_message_t mcprx;;

uint32_t lastTime;

void setup()
{
    pinMode(SS, OUTPUT);
    Serial.begin(115200);
    delay(1000);
    
    
    flex.init(250000);
    mcp.init(250000);

    SPI.begin();
    
    lastTime = millis();
    srand(micros());
}

void loop()
{
    if(millis() - lastTime > 1000)
    {
        msg.len = (rand()%8)+1;
        msg.id = (rand()%256);
        Serial.print("\nFlex Out: ");
        Serial.print("ID: ");
        Serial.print(msg.id);
        Serial.print("  MSG: ");
        for(int i = 0 ; i < msg.len; i++)
        {
            msg.buf[i] = (rand()%16);
            Serial.print(msg.buf[i], HEX);
        }
        flex.write(msg);
        lastTime = millis();
    }
    if(mcp.available())
    {
        mcp.read(mcprx);
        Serial.print("\nMCP Read: ");
        Serial.print("ID: ");
        Serial.print(mcprx.id);
        Serial.print("  MSG: ");
        for(int i = 0; i < mcprx.len; i++)
        {
          Serial.print(mcprx.buf[i], HEX);
        }
        Serial.println();
    }
}







