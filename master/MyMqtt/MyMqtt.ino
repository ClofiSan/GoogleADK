

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <LedControl.h>
/*
  pub:NodeToAnd
  sub:AndToNode
    PyToNode
*/
const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "123.206.127.199";

Servo myser;
Servo myser2;


WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;
char msgfromAnd[50];
char msgtoAnd[20];
byte msgfromPy[200];

byte
normalFace[][8] = {    // Eye animation frames
	{
		B00111100,         // Fully open eye
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00111100 }
		,
		{
			B00000000,
			B01111110,
			B11111111,
			B11111111,
			B11111111,
			B11111111,
			B01111110,
			B00111100 }
			,
			{
				B00000000,
				B00000000,
				B00111100,
				B11111111,
				B11111111,
				B11111111,
				B00111100,
				B00000000 }
				,
				{
					B00000000,
					B00000000,
					B00000000,
					B00111100,
					B11111111,
					B01111110,
					B00011000,
					B00000000 }
					,
					{
						B00000000,         // Fully closed eye
						B00000000,
						B00000000,
						B00000000,
						B10000001,
						B01111110,
						B00000000,
						B00000000 }
};
uint8_t
blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 },	//sizeof = 7 
blinkTime = 100;					//其实是眨眼的周期

uint8_t
mood = 1,
TouchReact = 15,
BeControlled = 0,
neckcon = 0,
facecon = 0;

//舵机设定
#define wir1 D7
#define wir2 D8
const uint8_t step = 10;
uint8_t neckLR = 0;	//这是一个坑，注意测试修改
uint8_t	neckUD = 0;

#define CLK		  D2     
#define CS        D3    
#define DIN       D1      
LedControl lc = LedControl(DIN, CLK, CS, 1);

byte heart[8]{

		B00000000,
		B01100110,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00111100,
		B00011000,
};
byte snicker[8]{

	B00000000,
	B11100111,
	B00000000,
	B00000000,
	B11111111,
	B11011011,
	B01011010,
	B00111100,
};
byte happy[8]{
	B0000000,
	B0110011,
	B0110011,
	B0000000,
	B0000000,
	B0100010,
	B0011100,
	B0000000,
};
byte angry[8]{

};
byte sad[8]{

};

void setup() {
  Serial.begin(115200);
  setup_wifi();//连接wifi
  client.setServer(mqtt_server, 1883);
  //只想intopic中的那个消息指针
  client.setCallback(Receive);
  myser.attach(wir1);
  myser2.attach(wir2);

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
	int i = 0;
    for ( i = 0; i < length; i++) {
      msgfromPy[i] = (byte)payload[i];
      Serial.print(msgfromPy[i]);
      Serial.print(" ");
    }
	msgfromPy[i] = '\0';
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
  if (BeControlled == 1)
  {
	  LinkUP();
	  control();
  }	  


}
void control()
{
	//整个控制的环节
	//舵机的控制输出
	//neckLR和neckUD需要初始化
	myser.write(neckLR);
	myser2.write(neckUD);
	//控制face其实就是一个分支

	//没有输入控制信号的时候应该是一张normalface
	DrawFaceByColumn(normalFace[(blinkIndex[JudgeBlinkTime()])]);
	if (--blinkTime == 0) blinkTime = random(5, 180);

	
	switch (facecon)
	{//感觉像是嵌入式开发
	case 1: {
		lc.clearDisplay();
		DrawFaceByColumn(heart);
		delay(2000);
		blinkTime = random(5, 180);
		break;
	}case 2: {
		DrawFaceByColumn(snicker);
		delay(2000);
		blinkTime = random(5, 180);
			break;
	}case 3: {
		DrawFaceByColumn(happy);
		delay(2000);
		blinkTime = random(5, 180);
			break;
	}case 4: {
		DrawFaceByColumn(angry);
		delay(2000);
		blinkTime = random(5, 180);
			break;
	}case 5: {
		DrawFaceByColumn(sad);
		delay(2000);
		blinkTime = random(5, 180);
			break;
	}
	default: {
		break;
	}	
	}
	

}
uint8_t JudgeBlinkTime()
{
	if (blinkTime < sizeof(blinkIndex))
		return blinkIndex[blinkTime];
	else return 0;

}
void DrawFaceByColumn(byte *face)
{
	for (int i = 0; i<8; i++)
	{
		lc.setColumn(0, i, face[i]);
	}
}
void controlneck()
{
	//控制舵机
	//neck == random(0,180)
	//需要测试才能知道左右
	
	if (neckcon == 1&& neckLR!=0)
		neckLR -= step;
	else if (neckcon == 2&& neckLR != 180)
		neckLR += step;
	if (neckcon == 3 && neckUD != 0)
		neckUD -= step;
	else if (neckcon == 4 && neckUD != 180)
		neckUD += step;

}

void WriteWordsColumn()
{
	//书写接收到的字，注意衔接
	int n;
	int row=0;
	for (n = 0; n < sizeof(msgfromPy); n++)
	{
		row = n % 8;
		if (row == 0)
			delay(100);
		lc.setColumn(0, row,msgfromPy[n]);
	}
}
void LinkUP()
{
	//关于衔接的函数，防止交互出现问题
	   
}


