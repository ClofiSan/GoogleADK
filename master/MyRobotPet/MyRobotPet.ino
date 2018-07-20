#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "SPI.h"
#include "LedControl.h"
#include "Servo\src\Servo.h"

//#include "WaveHC.h"
//#include "WaveUtil.h"
/*
	2018/7/19  bug��δ��ɵĹ��ܵ������
	1.����������������
	2.���Ƶ��ν�
	3,�µı�������

*/

//wifi�趨��mqtt���������趨
const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "123.206.127.199";
const char* outTopic = "NodesTell";
const char* inTopic = "PyMsg";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
byte recieve[100];

//LED���Ƶ�����
#define CLK		  D2     
#define CS        D3    
#define DIN       D1      
LedControl lc = LedControl(DIN, CLK, CS, 1);

//����趨
#define wir1 D7
#define wir2 D8
const uint8_t step = 10;
uint8_t neckLR = 0;	//����һ���ӣ�ע������޸�
uint8_t	neckUD = 0;

byte
sadFace[][8] = {    // Eye animation frames
	{
		B00100100,         // Fully open sad eye
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
		B00100100,         // Fully closed sad eye
		B01000010,
		B10000001,
		B00000000,
		B00000000,
		B00000000,
		B00000000,
		B11111111 }

};

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

byte
happyFace[][8] = {    // Eye animation frames
	{
		B00111100,         // Fully open happy eye
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
						B00000000,         // Fully closed happy eye      
						B00000000,
						B00000000,
						B01111110,
						B10000001,
						B00000000,
						B10000001,
						B01111110 }
};

byte 
botherFace[][8] = {    // Eye animation frames
	{
		B10000001,         // Fully open annoyed eye
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
						B10000001,         // Fully closed annoyed eye
						B01100110,
						B00000000,
						B10000001,
						B01111110,
						B00000000,
						B00000000,
						B00000000 }

};


Servo myser;
Servo myser2;


uint8_t 
blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 },	//sizeof = 7 
blinkTime = 100,					//��ʵ��գ�۵�����
gazeTime = 75,						//��һ���۾��ƶ���ʱ�䵹��ʱ//ע������
gazeMove = 50;						//���������ʱ��
int8_t
gazeX = 3, gazeY = 3,				//���������
newX = 3, newY = 3,					//�µ����������
dX = 0, dY = 0;						


int TouchNum = 0; 
int TouchReact = 15;
byte mood = 1;
//const byte vibration PROGMEM = ;//���������������� 
//const int TouchLevel PROGMEM = 512;//������λ�ĵȼ�
#define vibration D6
long PreMillis = 0;        
#define analog A0

const int decay = 30000;  //˥��ʱ��       

unsigned long checkMillis,touchMillis,nowMillis; //����ʱ��Ķ���

//���ƵĲ���,Ҳ��json��ʽ�����msg
uint8_t 
BeControlled = 0, //�鿴�Ƿ񱻿���
neckcon = 0,
facecon = 0,

						 //���Ʋ��ֵı���


//�����������Ĳ���
//SdReader card;    // ����������Ϣ
//FatVolume vol;    // ���������Ϣ
//FatReader root;   //���ļ�ϵͳ����Ŀ¼��Ϣ
//WaveHC wave;      //  ���������Ķ���һ��ֻ����һ��

//uint8_t dirLevel; // //�ļ�/Ŀ¼���Ƶ��������� 
//dir_t dirBuf;     //  �ļ��Ķ�Ŀ¼�ĵĻ�����
//FatReader f;      // �������ڲ��ŵ�����


void setup()
{
	pinMode(D6, INPUT);
	randomSeed(analogRead(analog));
	myser.attach(wir1);
	myser2.attach(wir2);

	//LedControl�Ŀ�Ķ���
	lc.shutdown(0, false);//�ڵ�ģʽ
	lc.setIntensity(0, 5);//����LED����
	lc.clearDisplay(0);	  //�����Ļ

	//wifi��mqtt���趨
	setup_wifi();//����wifi
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);  //ֻ��intopic�е��Ǹ���Ϣָ��

	//Wavehc����趨
	//���sd����ʼ��ʧ��
/*	if (!card.init(true)) {

	}
	//��ʼ�������ж�
	uint8_t part;
	for (part = 0; part < 5; part++) {
		if (vol.init(card, part))
			break;
	}
	if (part == 5) {
		sdErrorCheck();
		while (1);
	}
	//�򿪸�Ŀ¼ʧ��
	if (!root.openRoot(vol)) {
	}
	//��ӡ�������ļ�
	root.ls(LS_R | LS_FLAG_FRAGMENTED);//������

*/
}

void setup_wifi() {

	delay(10);
	//Serial.println();
	//Serial.print("Connecting to ");
	//Serial.println(ssid);

	WiFi.begin(ssid, password);

	while (WiFi.status() != WL_CONNECTED) {
		//delay(500);
		//Serial.print(".");
	}
	//Serial.println("");
	//Serial.println("WiFi connected");
	//Serial.println("IP address: ");
	//Serial.println(WiFi.localIP());
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
void encodeJson() {
	//װ��json
	DynamicJsonBuffer jsonBuffer;//��̬json����
	JsonObject& root1 = jsonBuffer.createObject(); //����һ���µĶ���
	root1["Humidity"] = humidity; //keyΪHumidity��valueΪhumidity
	root1["Temperature"] = temperature;
	//  root1.prettyPrintTo(Serial);
	root1.printTo(msg);//��json��ʽ��ת��Ϊ�ַ�������msg��
}
void decodeJson(char msg[100]) {
	//����json
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(msg);//����һ���ַ�������
	float temp = root["Temperature"];
	//��keyΪtemperature��Ӧ��value����temp��
	float hum = root["Humidity"];

	Serial.println(temp);
	Serial.println(hum);

}
//�����Ƿǿ��Ʋ���
void MqttAndJson()
{
	//ִ��mqtt��صĲ���

}
void controlneck()
{
	//���ƶ��
}
void controlface()
{
	//���Ʊ���Ĳ���
}
void getwords()
{
	//��mqtt�н��յ�����
	//ƽʱ��ʱ�����keyΪ0
}
void GazeAprh(int8_t x,int8_t y)
{
	//��setled�ٶȿ��ܻ���
	lc.setLed(0, x, y, false);
	lc.setLed(0, x+1, y, false);
	lc.setLed(0, x, y+1, false);
	lc.setLed(0, x+1, y+1, false);

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
		//ͫ�׵��趨�Ǵӵ����ͼ�����ڿ�2*2
		//����ط�Ҫ��ͫ�׵ľ�λ�������ӽ��µ�λ��
		//newX - (dX * gazeTime / gazeMove) �����������ӽ���
		GazeAprh(newX - (dX * gazeTime / gazeMove), newY - (dY * gazeTime / gazeMove));
		if (gazeTime == 0) {
			gazeX = newX;
			gazeY = newY;
			do {
				//new��λ�ù̶���Ԥ�õ����귶Χ���� Ҳ����һ��Բ
				//��Ƴ������������ƶ����趨���µĶ���
				//random�Ĳ�����ʵ����Ȧ������
				//��Ϊ3�����������˿ճ�����2*2�������
				//ÿ�ֱ����y��Χ����һ����
				SetNewGaze();
				dX = newX - 3;
				dY = newY - 3;
			} while ((dX * dX + dY * dY) >= 10);

		}
	}
	else {
		//�۾�������ʱ��
		GazeAprh(gazeX, gazeY);
		//MoveNeck();

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
	}
}
void BlinkFace()
{
	//Ҫ����ֵ����ݣ�����˳��ʵ��գ�۵�Ч����Ҳ���ǰ���һ����˳��ȥ�ų����飬
	//Ҳ����Ҫ����index��˳����
	//��һ��������ȫ�ֵ�˳�򻭳�λͼ���Ͼ��������loop����ģ�Ҳ����˵ֻҪһ�γ��ֳ�һ��ͼ�ͺ���
	//�ڶ������ʱ������������һ���µ�ʱ��
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
/*
void MoveNeck()
{
//�ƶ����
myser.write(gazeX * 10);
myser2.write(gazeY * 10);

}
*/

void CheckTouch()
{
	//����Լ��Ƿ񱻴���
	if (digitalRead(D6))
	{
		TouchNum++;
		if (TouchNum % 200 == 0) {
			TouchReact++;
			//PlayTouchSong();
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

}
void loop()
{
	BlinkFace();
	MoveGaze();

	touchMillis = millis();
	while (millis() - touchMillis < 40)
		CheckTouch();

}

/*
* print error message and halt if SD I/O error, great for debugging!
*/
/*
void error_P(const char *str) {
//���Ҳ��
PgmPrint("Error: ");
SerialPrint_P(str);
sdErrorCheck();
while (1);
}
void sdErrorCheck(void) {
	//��һ���ǹ���Sd��������
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
	while (dir.readDir(dirBuf) > 0) {    //��ʼ���ļ��������е��ļ�

										 // �������dir����wav��ֱ������
		if (!DIR_IS_SUBDIR(dirBuf) && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
			continue;
		}

		Serial.println();            //�µ�һ��

		for (uint8_t i = 0; i < dirLevel; i++) {
			Serial.write(' ');       // Ϊ�˶��ļ��Ľ���
		}
		if (!file.open(vol, dirBuf)) {        // ����޷���
											  //      error("file.open failed");          // something went wrong
		}

		if (file.isDir()) {                   // �򿪵��Ƿ���һ���µ��ļ����ݹ���ļ�
			printEntryName(dirBuf);
			Serial.println();
			dirLevel += 2;                      
												
			PlayVoice(file);                         
			dirLevel -= 2;
		}
		else {
			//�������dir
			printEntryName(dirBuf);              
			if (!wave.create(file)) {            // �ж��Ƿ���wav
				putstring(" Not a valid WAV");     // ok skip it
				return;
			}
			else {
				Serial.println();                  
				wave.play();                       
				uint8_t n = 0;
				while (wave.isplaying) {
					//����������ʱ�򴮿���ʾ
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
	//��������һ������
	playfile(name);
	while (wave.isplaying) { 
							 
	}
	
}
void playfile(char *name) {
	
	if (wave.isplaying) {		//����ڲ������������Ļ���ͣ����
		wave.stop(); 
	}
	
	if (!f.open(root, name)) {	//�����ʧ�ܵĻ���GG
		Serial.print("can't open file");
		Serial.println (name);
		return;
	}
	
	if (!wave.create(f)) {	//�ж��Ƿ���wav
		Serial.println(name);
		Serial.println("Not a WAV");
	}

	// ����
	wave.play();
}*/
