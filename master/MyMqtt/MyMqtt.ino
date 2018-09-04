

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <Servo.h>
#include <LedControl.h>
/*
  pub:NodeToAnd
  sub:AndToNode
    PyToNode

	2017bug:
	如何解决getword的问题
*/
const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "119.23.227.254";

Servo myser;
Servo myser2;



WiFiClient espClient;
PubSubClient client(espClient);
long lastMsg = 0;
int value = 0;
char msgfromAnd[50];
char msgtoAnd[50];
//byte msgfromPy[200];

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
uint8_t step = 20;
uint8_t neckLR = 90;	//这是一个坑，注意测试修改
uint8_t	neckUD = 90;
byte *msgfromPy;
int lenthofpy;
bool flag = false;

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
	B00000000,
	B10000001,
	B01100110,
	B00000000,
	B00011000,
	B00111100,
	B01111110,
	B11111111,

};
byte sad[8]{
	B00000000,
	B00100100,
	B01000010,
	B10000001,
	B00000000,
	B00111100,
	B01000010,
	B10000001,

};

void setup() {
  Serial.begin(115200);
  setup_wifi();
  
  client.setServer(mqtt_server, 1883);
  client.setCallback(Receive);

  myser.attach(wir1);
  myser2.attach(wir2);

  lc.shutdown(0, false);//
  lc.setIntensity(0, 2);//
  lc.clearDisplay(0);//   �

  

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
    delete msgfromPy;
	  msgfromPy = new byte[300];
	int i = 0;
    for ( i = 0; i < length; i++) {
      msgfromPy[i] = (byte)payload[i];
      Serial.print(msgfromPy[i]);
      Serial.print(" ");
    }
    lenthofpy = length;
    Serial.println();

  }
//可能是arduino处理数据要点延迟，不可能！
//可能是client的东西重复了
}

void reconnect() {
  while (!client.connected()) {
    Serial.print("Attempting MQTT connection...");
    if (client.connect("ESP8266Client")) {
      Serial.println("connected");
      //client.publish(outTopic, "hello world");
      client.subscribe("AndToNode",1); //这仅仅是一个订阅而已，订阅了之前设定的主题
     client.subscribe("PyToNode",1);
   
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
  //encodeJson(); 
  client.publish("NodeToAnd", msgtoAnd); 
  if (BeControlled == 1)
  {
	  control();
    flag = true;
  }	  
  else flag = false; 
  


}
void FaceControl(byte *face)
{
	lc.clearDisplay(0);
	DrawFaceByColumn(face);
	//blinkTime = random(5, 180);
}
void control()
{

	myser.write(neckUD);
	myser2.write(neckLR);

	
	switch (facecon)
	{//感觉像是嵌入式开发
	case 1: {
 // lc.clearDisplay(0);
		DrawFaceByColumn(heart);
		break;
	}case 2: {
    //lc.clearDisplay(0);
		DrawFaceByColumn(happy);
			break;
	}case 3: {
   // lc.clearDisplay(0);
		DrawFaceByColumn(sad);
			break;
	}case 4: {
   // lc.clearDisplay(0);
		DrawFaceByColumn(angry);
			break;
	}case 5: {
    //lc.clearDisplay(0);
		DrawFaceByColumn(snicker);
			break;
	}case 6: {
		//lc.clearDisplay(0);
		WriteWordsColumn();
		break;
	}
	default: {
    LinkUP();
		break;
	}	
	}

	if (neckcon)
		controlneck();

  

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
	if (neckcon == 1)
		neckLR -= step;
	else if (neckcon == 2 )
		neckLR += step;
	if (neckcon == 3 )
		neckUD -= step;
	else if (neckcon == 4 )
		neckUD += step;

   neckcon = 0;

}

void WriteWordsColumn()
{
	//书写接收到的字，注意衔接
	int n;
	int row=0;
	for (n = 0; n < lenthofpy; n++)
	{
		row = n % 8;
		//if (row == 0)
			delay(300);
		lc.setColumn(0, row,msgfromPy[n]);
	}
	//delete msgfromPy;
}
void LinkUP()
{
	DrawFaceByColumn(snicker);
}


