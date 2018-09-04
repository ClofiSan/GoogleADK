#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include <SPI.h>
#include <LedControl.h>
#include <Servo.h>
/*
pub:NodeToAnd
sub:AndToNode
	PyToNode
*/
//#include "WaveHC.h"
//#include "WaveUtil.h"
/*
2018/7/19  bug锟斤拷未锟斤拷傻墓锟斤拷艿锟斤拷锟斤拷锟斤拷

1.这个是什么其实我也不知道www

*/

//wifi锟借定锟斤拷mqtt锟斤拷锟斤拷锟斤拷锟斤拷锟借定
const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "119.23.227.254";
int lenthofpy;
WiFiClient espClient;
PubSubClient client(espClient);
//long lastMsg = 0;
//int value = 0;
char msgfromAnd[60];
char msgtoAnd[40];
byte *msgfromPy;

uint8_t
BeControlled = 0, 
neckcon = 0,
facecon = 0;

#define CLK     D2     
#define CS        D3    
#define DIN       D1      
LedControl lc = LedControl(DIN, CLK, CS, 1);


#define wir1 D7
#define wir2 D8
uint8_t step = 10;
uint8_t neckLR = 90; //锟斤拷锟斤拷一锟斤拷锟接ｏ拷注锟斤拷锟斤拷锟斤拷薷锟�
uint8_t neckUD = 90;


byte
sadFace[][8] = {    
	{
		B00100100,        
		B01000010,
		B10000001,
		B00111100,
		B01111110,
		B11111111,
		B11111111,
		B11111111
	},
	{
		B00100100,
		B01000010,
		B10000001,
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B11111111
	},

	{
		B00100100,
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B01111110,
		B11111111,
		B11111111 }
		,
	{
		B00100100,
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B00000000,
		B01111110,
		B11111111
		},
	{
		B00100100,         
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B00000000,
		B00000000,
		B11111111 }

};

byte
normalFace[][8] = {    
	{
		B00111100,         
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
		B00000000,         
		B00000000,
		B00000000,
		B00000000,
		B10000001,
		B01111110,
		B00000000,
		B00000000 }
};

byte
happyFace[][8] = {    
	{
		B00111100,        
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
		,
	{
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
			,
	{
		B00000000,
		B00000000,
		B01111110,
		B11111111,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
				,
	{
		B00000000,
		B00000000,
		B00000000,
		B01111110,
		B11111111,
		B00000000,
		B10000001,
		B01111110 }
		,
	{
		B00000000,             
		B00000000,
		B00000000,
		B01111110,
		B10000001,
		B00000000,
		B10000001,
		B01111110 }
};

byte
botherFace[][8] = {    
	{
		B10000001,        
		B01100110,
		B00000000,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00111100 }
		,
	{
		B10000001,
		B01100110,
		B00000000,
		B11111111,
		B11111111,
		B11111111,
		B01111110,
		B00000000 }
	,
	{
		B10000001,
		B01100110,
		B00000000,
		B11111111,
		B11111111,
		B01111110,
		B00000000,
		B00000000 }
				,
	{
		B10000001,
		B01100110,
		B00000000,
		B11111111,
		B01111110,
		B00000000,
		B00000000,
		B00000000 }
					,
	{
		B10000001,         
		B01100110,
		B00000000,
		B10000001,
		B01111110,
		B00000000,
		B00000000,
		B00000000 }

};
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


Servo myser;
Servo myser2;


uint8_t
blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, //sizeof = 7 
blinkTime = 100,          //锟斤拷实锟斤拷眨锟桔碉拷锟斤拷锟斤拷
gazeTime = 75,            //锟斤拷一锟斤拷锟桔撅拷锟狡讹拷锟斤拷时锟戒倒锟斤拷时//注锟斤拷锟斤拷锟斤拷
gazeMove = 50;            //锟斤拷锟斤拷锟斤拷锟斤拷锟绞憋拷锟�
int8_t
gazeX = 3, gazeY = 3,       //锟斤拷锟斤拷锟斤拷锟斤拷锟�
newX = 3, newY = 3,         //锟铰碉拷锟斤拷锟斤拷锟斤拷锟斤拷锟�
dX = 0, dY = 0;


int TouchNum = 0;
int TouchReact = 15;
byte mood = 1;
uint8_t istouch = 0;
//const byte vibration PROGMEM = ;//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷 
//const int TouchLevel PROGMEM = 512;//锟斤拷锟斤拷锟斤拷位锟侥等硷拷
//#define vibration D5
long PreMillis = 0;
#define analog A0

const int decay = 30000;  //衰锟斤拷时锟斤拷       

unsigned long checkMillis, touchMillis, nowMillis; //锟斤拷锟斤拷时锟斤拷亩锟斤拷锟�
												   //锟斤拷锟狡诧拷锟街的憋拷锟斤拷
												   //锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟侥诧拷锟斤拷
												   //SdReader card;    // 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷息
												   //FatVolume vol;    // 锟斤拷锟斤拷锟斤拷锟斤拷锟较�
												   //FatReader root;   //锟斤拷锟侥硷拷系统锟斤拷锟斤拷目录锟斤拷息
												   //WaveHC wave;      //  锟斤拷锟斤拷锟斤拷锟斤拷锟侥讹拷锟斤拷一锟斤拷只锟斤拷锟斤拷一锟斤拷

												   //uint8_t dirLevel; // //锟侥硷拷/目录锟斤拷锟狡碉拷锟斤拷锟斤拷锟斤拷锟斤拷 
												   //dir_t dirBuf;     //  锟侥硷拷锟侥讹拷目录锟侥的伙拷锟斤拷锟斤拷
												   //FatReader f;      // 锟斤拷锟斤拷锟斤拷锟节诧拷锟脚碉拷锟斤拷锟斤拷


void setup()
{
	Serial.begin(115200);
	
	setup_wifi();
	client.setServer(mqtt_server, 1883);
	client.setCallback(Receive);

	
	pinMode(D6, INPUT);
	randomSeed(analogRead(analog));
	myser.attach(wir1);
	myser2.attach(wir2);



	//LedControl锟侥匡拷亩锟斤拷锟�
	lc.shutdown(0, false);//锟节碉拷模式
	lc.setIntensity(0, 2);//锟斤拷锟斤拷LED锟斤拷锟斤拷
	lc.clearDisplay(0);   //锟斤拷锟斤拷锟侥�

						  //wifi锟斤拷mqtt锟斤拷锟借定
						  //setup_wifi();//锟斤拷锟斤拷wifi
						  //client.setServer(mqtt_server, 1883);
						  //client.setCallback(Receive); //只锟斤拷intopic锟叫碉拷锟角革拷锟斤拷息指锟斤拷

						  //Wavehc锟斤拷锟斤拷瓒�
						  //锟斤拷锟絪d锟斤拷锟斤拷始锟斤拷失锟斤拷
						  /*  if (!card.init(true)) {

						  }
						  //锟斤拷始锟斤拷锟斤拷锟斤拷锟叫讹拷
						  uint8_t part;
						  for (part = 0; part < 5; part++) {
						  if (vol.init(card, part))
						  break;
						  }
						  if (part == 5) {
						  sdErrorCheck();
						  while (1);
						  }
						  //锟津开革拷目录失锟斤拷
						  if (!root.openRoot(vol)) {
						  }
						  //锟斤拷印锟斤拷锟斤拷锟斤拷锟侥硷拷
						  root.ls(LS_R | LS_FLAG_FRAGMENTED);//锟斤拷锟斤拷锟斤拷

						  */
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
		for (i = 0; i < length; i++) {
			msgfromPy[i] = (byte)payload[i];
			Serial.print(msgfromPy[i]);
			Serial.print(" ");
		}
		lenthofpy = length;
		Serial.println();

	}
}
void decodeJson(char msg[]) {
	//锟斤拷锟斤拷AndToNode
	//锟斤拷谓锟斤拷锟斤拷锟酵拷亩锟斤拷锟斤拷锟斤拷训锟斤拷锟绞叫匆伙拷锟斤拷锟斤拷锟�
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
	Serial.println(facecon);
}
void encodeJson() {
	//装锟斤拷json
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root1 = jsonBuffer.createObject();
	root1["mood"] = mood;
	root1["TouchReact"] = TouchReact;
	root1.printTo(msgtoAnd);
}
void reconnect() {
	while (!client.connected()) {
		Serial.print("Attempting MQTT connection...");
		if (client.connect("ESP8266Client")) {
			Serial.println("connected");
			//client.publish(outTopic, "hello world");
			client.subscribe("AndToNode", 1); //锟斤拷锟斤拷锟斤拷锟揭伙拷锟斤拷锟斤拷亩锟斤拷眩锟斤拷锟斤拷锟斤拷锟街帮拷瓒拷锟斤拷锟斤拷锟�
			client.subscribe("PyToNode", 1);
		}
		else {
			Serial.print("failed, rc=");
			Serial.print(client.state());
			Serial.println(" try again in 5 seconds");
			delay(5000);
		}
	}
}

void GazeAprh(int8_t x, int8_t y)
{
	//锟斤拷setled锟劫度匡拷锟杰伙拷锟斤拷
	lc.setLed(0, y, 7 - x, false);
	lc.setLed(0, y + 1, 7 - x, false);
	lc.setLed(0, y, 7 - (x + 1), false);
	lc.setLed(0, y + 1, 7 - (x + 1), false);

}
void SetNewGaze() {
	if (mood == 0)
	{
		newX = random(0, 7);
		newY = random(5, 7);
	}
	else if (mood == 1)
	{
		newX = random(7);
		newY = random(7);
	}
	else if (mood == 2)
	{
		newX = random(7);
		newY = random(4);
	}
	else if (mood == 3)
	{
		newX = random(7);
		newY = random(3, 7);
	}
}
void MoveGaze()
{
	if (--gazeTime <= gazeMove) {
		//瞳锟阶碉拷锟借定锟角从碉拷锟斤拷锟酵硷拷锟斤拷锟斤拷诳锟�2*2
		//锟斤拷锟斤拷胤锟揭拷锟酵拷椎木锟轿伙拷锟斤拷锟斤拷锟斤拷咏锟斤拷碌锟轿伙拷锟�
		//newX - (dX * gazeTime / gazeMove) 锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟接斤拷锟斤拷
		GazeAprh(newX - (dX * gazeTime / gazeMove), newY - (dY * gazeTime / gazeMove));
		if (gazeTime == 0) {
			gazeX = newX;
			gazeY = newY;
			do {
				//new锟斤拷位锟矫固讹拷锟斤拷预锟矫碉拷锟斤拷锟疥范围锟斤拷锟斤拷 也锟斤拷锟斤拷一锟斤拷圆
				//锟斤拷瞥锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷贫锟斤拷锟斤拷瓒拷锟斤拷碌亩锟斤拷锟�
				//random锟侥诧拷锟斤拷锟斤拷实锟斤拷锟斤拷圈锟斤拷锟斤拷锟斤拷
				//锟斤拷为3锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟剿空筹拷锟斤拷锟斤拷2*2锟斤拷锟斤拷锟斤拷锟�
				//每锟街憋拷锟斤拷锟統锟斤拷围锟斤拷锟斤拷一锟斤拷锟斤拷
				SetNewGaze();
				dX = newX - 3;
				dY = newY - 3;
			} while ((dX * dX + dY * dY) >= 10);

		}
	}
	else {
		//锟桔撅拷锟斤拷锟斤拷锟斤拷时锟斤拷
		GazeAprh(gazeX, gazeY);
		MoveNeck();

	}

}
/*void DrawFaceByRow(byte *face)
{
for (int i = 0; i<8; i++)
{
lc.setRow(0, i, face[i]);
}

}*/
void DrawFaceByColumn(byte *face)
{
	for (int i = 0; i<8; i++)
	{
		lc.setColumn(0, i, face[i]);
		//Serial.println("i am drawing");
	}
}
void BlinkFace()
{
	//要锟斤拷锟斤拷值锟斤拷锟斤拷荩锟斤拷锟斤拷锟剿筹拷锟绞碉拷锟秸ｏ拷鄣锟叫э拷锟斤拷锟揭诧拷锟斤拷前锟斤拷锟揭伙拷锟斤拷锟剿筹拷锟饺ワ拷懦锟斤拷锟斤拷椋�
	//也锟斤拷锟斤拷要锟斤拷锟斤拷index锟斤拷顺锟斤拷锟斤拷
	//锟斤拷一锟斤拷锟斤拷锟斤拷锟斤拷全锟街碉拷顺锟津画筹拷位图锟斤拷锟较撅拷锟斤拷锟斤拷锟斤拷锟絣oop锟斤拷锟斤拷模锟揭诧拷锟斤拷锟剿抵灰伙拷纬锟斤拷殖锟揭伙拷锟酵硷拷秃锟斤拷锟�
	//锟节讹拷锟斤拷锟斤拷锟绞憋拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟揭伙拷锟斤拷碌锟绞憋拷锟�
	lc.clearDisplay(0);
	if (mood == 0)
		DrawFaceByColumn(sadFace[(blinkIndex[JudgeBlinkTime()])]);
	else if (mood == 1)
		DrawFaceByColumn(normalFace[(blinkIndex[JudgeBlinkTime()])]);
	else if (mood == 2)
		DrawFaceByColumn(happyFace[(blinkIndex[JudgeBlinkTime()])]);
	else if (mood == 3)
		DrawFaceByColumn(botherFace[(blinkIndex[JudgeBlinkTime()])]);

	if (--blinkTime == 0) blinkTime = random(5, 180);
}
uint8_t JudgeBlinkTime()
{
	if (blinkTime < sizeof(blinkIndex))
		return blinkIndex[blinkTime];
	else return 0;

}
void MoveNeck()
{
	//锟狡讹拷锟斤拷锟�
	int8_t y2 = 7 - gazeX;
	int8_t x2 = gazeY;
	myser.write(y2 * 10); //
	myser2.write(180 - (x2 * 10));

}
void CheckTouch()
{

	//锟斤拷锟斤拷约锟斤拷欠癖淮锟斤拷锟�

  istouch  = digitalRead(D6);
	if (istouch)
	{
		TouchNum++;
		if (TouchNum % 200 == 0) {
			TouchReact++;
      
		}
		if (TouchNum >= 1000)
			TouchNum = 0;

	}
	if (TouchReact <= 10) mood = 0;
	else if (TouchReact <= 20) mood = 1;
	else if (TouchReact <= 30) mood = 2;
	else if (TouchReact > 30) mood = 3;

	nowMillis = millis();
	if (nowMillis - PreMillis > decay) {

		PreMillis = nowMillis;
		TouchReact--;
		if (TouchReact < 0) TouchReact = 0;
		if (TouchReact == 30) TouchReact = 15;
		if (TouchReact > 40) TouchReact = 40;
	}
	  Serial.print("touchnum ");
      Serial.print(TouchNum);
      
      Serial.print("mood ");
      Serial.print(mood);
      Serial.print("TouchReact ");
      Serial.println(TouchReact);
}

void loop()
{
	if (!client.connected()) {
		reconnect();
	}
	client.loop();
	encodeJson();
	client.publish("NodeToAnd", msgtoAnd);
	if (!BeControlled)
	{
		BlinkFace();
		MoveGaze();
		touchMillis = millis();
		while (millis() - touchMillis < 40)
		{
			istouch = 0;
			CheckTouch();
		}
	}
	else
	{
		control();
	}
	
	
   
	/*if (!client.connected()) {
	reconnect();//锟斤拷锟斤拷client锟斤拷锟较ｏ拷同时锟斤拷锟斤拷锟斤拷氐锟斤拷锟斤拷锟�
	}
	client.loop();
	encodeJson();
	client.publish("NodeToAnd", msgtoAnd);*/

}

void FaceControl(byte *face)
{
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
void controlneck()
{
	//控制舵机
	//neck == random(0,180)
	//需要测试才能知道左右
	if (neckcon == 1)
		neckLR -= step;
	else if (neckcon == 2)
		neckLR += step;
	if (neckcon == 3)
		neckUD -= step;
	else if (neckcon == 4)
		neckUD += step;

	neckcon = 0;

}
void WriteWordsColumn()
{
	//书写接收到的字，注意衔接
	int n;
	int row = 0;
	for (n = 0; n < lenthofpy; n++)
	{
		row = n % 8;
			delay(100);
		lc.setColumn(0, row, msgfromPy[n]);
	}

	
}
void LinkUP()
{
	//这里原本是用来衔接的
	DrawFaceByColumn(snicker);
}

/*
* print error message and halt if SD I/O error, great for debugging!
*/
/*
void error_P(const char *str) {
//锟斤拷锟揭诧拷锟�
PgmPrint("Error: ");
SerialPrint_P(str);
sdErrorCheck();
while (1);
}
void sdErrorCheck(void) {
//锟斤拷一锟斤拷锟角癸拷锟斤拷Sd锟斤拷锟斤拷锟斤拷锟斤拷
if (!card.errorCode()) return;
PgmPrint("\r\nSD I/O error: ");
Serial.print(card.errorCode(), HEX);
PgmPrint(", ");
Serial.println(card.errorData(), HEX);
while (1);
}
void PlayVoice(FatReader &dir)
{
FatReader file;
while (dir.readDir(dirBuf) > 0) {    //锟斤拷始锟斤拷锟侥硷拷锟斤拷锟斤拷锟斤拷锟叫碉拷锟侥硷拷

// 锟斤拷锟斤拷锟斤拷锟絛ir锟斤拷锟斤拷wav锟斤拷直锟斤拷锟斤拷锟斤拷
if (!DIR_IS_SUBDIR(dirBuf) && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
continue;
}

Serial.println();            //锟铰碉拷一锟斤拷

for (uint8_t i = 0; i < dirLevel; i++) {
Serial.write(' ');       // 为锟剿讹拷锟侥硷拷锟侥斤拷锟斤拷
}
if (!file.open(vol, dirBuf)) {        // 锟斤拷锟斤拷薹锟斤拷锟�
//      error("file.open failed");          // something went wrong
}

if (file.isDir()) {                   // 锟津开碉拷锟角凤拷锟斤拷一锟斤拷锟铰碉拷锟侥硷拷锟斤拷锟捷癸拷锟斤拷募锟�
printEntryName(dirBuf);
Serial.println();
dirLevel += 2;

PlayVoice(file);
dirLevel -= 2;
}
else {
//锟斤拷锟斤拷锟斤拷锟絛ir
printEntryName(dirBuf);
if (!wave.create(file)) {            // 锟叫讹拷锟角凤拷锟斤拷wav
putstring(" Not a valid WAV");     // ok skip it
return;
}
else {
Serial.println();
wave.play();
uint8_t n = 0;
while (wave.isplaying) {
//锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷时锟津串匡拷锟斤拷示
putstring(".");
if (!(++n % 32))Serial.println();
delay(100);
}
sdErrorCheck();                    // everything OK?
if (wave.errors)Serial.println(wave.errors);
}
}
}
}
void playcomplete(char *name) {
//锟斤拷锟斤拷锟斤拷锟斤拷一锟斤拷锟斤拷锟斤拷
playfile(name);
while (wave.isplaying) {

}

}
void playfile(char *name) {

if (wave.isplaying) {   //锟斤拷锟斤拷诓锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷锟斤拷幕锟斤拷锟酵ｏ拷锟斤拷锟�
wave.stop();
}

if (!f.open(root, name)) {  //锟斤拷锟斤拷锟绞э拷艿幕锟斤拷锟紾G
Serial.print("can't open file");
Serial.println (name);
return;
}

if (!wave.create(f)) {  //锟叫讹拷锟角凤拷锟斤拷wav
Serial.println(name);
Serial.println("Not a WAV");
}

// 锟斤拷锟斤拷
wave.play();
}*/

 
