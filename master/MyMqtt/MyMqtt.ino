

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
/*
  pub:NodeToAnd
  sub:AndToNode
    PyToNode
*/
const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "123.206.127.199";


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;
char msgfromAnd[50];
char msgtoAnd[20];
byte msgfromPy[200];

uint8_t
mood = 1,
TouchReact = 15,
BeControlled = 0,
neckcon = 0,
facecon = 0;

void setup() {
  Serial.begin(115200);
  setup_wifi();//连接wifi
  client.setServer(mqtt_server, 1883);
  //只想intopic中的那个消息指针
  client.setCallback(Receive);
}

void setup_wifi() {

  delay(10);
  Serial.println();
  Serial.print("Connecting to ");
  Serial.println(ssid);

  WiFi.begin(ssid, password);

  while (WiFi.status() != WL_CONNECTED) {
    delay(500);
    Serial.print(".");
  }
  Serial.println("");
  Serial.println("WiFi connected");
  Serial.println("IP address: ");
  Serial.println(WiFi.localIP());
}
void decodeJson(char msg[]) {
  //解析AndToNode
  //如何解析不同的东西，难道格式写一样？？
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root = jsonBuffer.parseObject(msg);
  BeControlled = root["BeControlled"];
  neckcon = root["neckcon"];
  facecon = root["facecon"];

  Serial.print("BeControlled:");
  Serial.print(BeControlled);
  Serial.print("neckcon:");
  Serial.print(neckcon);
  Serial.print("facecon:");
  Serial.println (facecon);
}
void encodeJson() {
  //装载json
  DynamicJsonBuffer jsonBuffer;
  JsonObject& root1 = jsonBuffer.createObject();
  root1["mood"] = mood;
  root1["TouchReact"] = TouchReact;
  root1.printTo(msgtoAnd);
}
void Receive(char* topic, byte* payload, unsigned int length) {

  Serial.print("Message from  [");
  Serial.print(topic);
  Serial.print("] ");
  if (!strcmp(topic, "AndToNode"))
  {
    for (int i = 0; i < length; i++)
    {
      msgfromAnd[i] = payload[i];
    }
    decodeJson(msgfromAnd);
  }

  else {
    for (int i = 0; i < length; i++) {
      msgfromPy[i] = (byte)payload[i];
      Serial.print(msgfromPy[i]);
      Serial.print(" ");
    }
    Serial.println();

  }


}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      //client.publish(outTopic, "hello world");
      client.subscribe("AndToNode", 1); //这仅仅是一个订阅而已，订阅了之前设定的主题
      client.subscribe("PyToNode", 1);
    } else {
      Serial.print("failed, rc=");
      Serial.print(client.state());
      Serial.println(" try again in 5 seconds");
      delay(5000);
    }
  }
}
void loop() {

  if (!client.connected()) {
    reconnect();//先让client连上，同时订阅相关的主题
  }
  client.loop();
  encodeJson();
  client.publish("NodeToAnd", msgtoAnd);
  //为什么要loop，这个指令大概就是client在连接之后一直连接着的


}



