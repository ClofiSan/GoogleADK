

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include "LedControl.h"

const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "123.206.127.199";
const char* outTopic = "NodesTell";
const char* inTopic = "PyMsg";
WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
char msg[50];
int value = 0;


#define DIN D1
#define CLK D2
#define CS  D3 
LedControl lc = LedControl(DIN, CLK, CS, 1);
unsigned long delaytime = 100;


void setup() {
	Serial.begin(115200);
	setup_wifi();//����wifi
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);  //ֻ��intopic�е��Ǹ���Ϣָ��

	lc.shutdown(0, false);
	lc.setIntensity(0, 8);//����0-15
	lc.clearDisplay(0);//�����Ļ
}
void WriteMyWordsByrow(byte* words) {
	int n = 0;
	int row = 0;
	while (n<sizeof(recieve))
	{
		row = n % 8;
		lc.setRow(0, row, recieve[n++]);
		delay(delaytime);
	}
}
void WriteMyWords(byte* words) {
	int n = 0;
	int row = 0;
	while (n<sizeof(words))
	{
		row = n % 8;
		if (row == 0)
			delay(delaytime);
		lc.setRow(0, row, words[n++]);

	}
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
void callback(char* topic, byte* payload, unsigned int length) {
	//���յĺ�����topicΪ���ն��ĵ����⣬payloadΪ���ȣ����һ��Ϊpayload�ĳ���
	Serial.print("Message arrived [");
	Serial.print(topic);
	Serial.print("] ");
	for (int i = 0; i < length; i++) {
		recieve[i] = (byte)payload[i];
		Serial.print(recieve[i]);
		Serial.print(" ");
	}
	Serial.println();

}
void reconnect() {
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		if (client.connect("ESP8266Client")) {
			Serial.println("connected");
			//client.publish(outTopic, "hello world");
			client.subscribe(inTopic, 1);//�������һ�����Ķ��ѣ�������֮ǰ�趨������
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			delay(5000);
		}
	}
}
void loop() {
	byte recieve[1000];
	if (!client.connected()) {
		reconnect();//����client���ϣ�ͬʱ������ص�����
	}
	client.loop();//ΪʲôҪloop�����ָ���ž���client������֮��һֱ�����ŵ�
	//client.publish(outTopic, "Hi,python,I am arduino!");
	//WriteMyWords();
	WriteMyWords();
	
}


