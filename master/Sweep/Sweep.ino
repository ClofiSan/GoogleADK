#include <Servo.h>
#include <ESP8266WiFi.h>
#include <PubSubClient.h>

const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "123.206.127.199";
const char* outTopic = "NodesTell";
const char* inTopic = "PyMsg";
WiFiClient espClient;
PubSubClient client(espClient);

#define wir D4
#define DigLim 180 //限制的角度

Servo servo;

int pos = 0;
int up = 5;
int down = 5;
void setup()
{
  servo.attach(wir);

}
void loop()
{
  //这样就限制住了转动的角度，并且可以改变step
  //接下来的难点在于要测试出up和down
  servo.write(pos);
  if(pos) 
    delay(15 * up); 
  if (pos <= (DigLim)) {
    //这里可以加入控制信号的触发条件
    pos += up;
  }
  else {
    pos = 0;
    delay(180*15);
  }
  //结果应该是一直动直到180不动
    
}

