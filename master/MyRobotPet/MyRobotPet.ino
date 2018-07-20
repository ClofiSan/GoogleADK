#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <ArduinoJson.h>
#include "SPI.h"
#include "LedControl.h"
#include "Servo\src\Servo.h"

//#include "WaveHC.h"
//#include "WaveUtil.h"
/*
	2018/7/19  bug和未完成的功能的情况：
	1.声音可能做不了了
	2.控制的衔接
	3,新的表情的设计

*/

//wifi设定和mqtt订阅主题设定
const char* ssid = "Napoleon";
const char* password = "19980909qaz";
const char* mqtt_server = "123.206.127.199";
const char* outTopic = "NodesTell";
const char* inTopic = "PyMsg";
WiFiClient espClient;
PubSubClient client(espClient);
char msg[50];
byte recieve[100];

//LED控制的引脚
#define CLK		  D2     
#define CS        D3    
#define DIN       D1      
LedControl lc = LedControl(DIN, CLK, CS, 1);

//舵机设定
#define wir1 D7
#define wir2 D8
const uint8_t step = 10;
uint8_t neckLR = 0;	//这是一个坑，注意测试修改
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
blinkTime = 100,					//其实是眨眼的周期
gazeTime = 75,						//下一次眼睛移动的时间倒计时//注视周期
gazeMove = 50;						//眼球滚动的时间
int8_t
gazeX = 3, gazeY = 3,				//眼球的坐标
newX = 3, newY = 3,					//新的眼球的坐标
dX = 0, dY = 0;						


int TouchNum = 0; 
int TouchReact = 15;
byte mood = 1;
//const byte vibration PROGMEM = ;//触摸传感器的引脚 
//const int TouchLevel PROGMEM = 512;//触摸电位的等级
#define vibration D6
long PreMillis = 0;        
#define analog A0

const int decay = 30000;  //衰退时间       

unsigned long checkMillis,touchMillis,nowMillis; //各种时间的定义

//控制的部分,也即json格式传输的msg
uint8_t 
BeControlled = 0, //查看是否被控制
neckcon = 0,
facecon = 0,

						 //控制部分的表情


//下面是声音的部分
//SdReader card;    // 整个卡的信息
//FatVolume vol;    // 保存分区信息
//FatReader root;   //（文件系统）根目录信息
//WaveHC wave;      //  操作声音的对象，一次只能有一个

//uint8_t dirLevel; // //文件/目录名称的缩进级别 
//dir_t dirBuf;     //  文件阅读目录的的缓冲区
//FatReader f;      // 我们正在播放的声音


void setup()
{
	pinMode(D6, INPUT);
	randomSeed(analogRead(analog));
	myser.attach(wir1);
	myser2.attach(wir2);

	//LedControl的库的定义
	lc.shutdown(0, false);//节点模式
	lc.setIntensity(0, 5);//设置LED亮度
	lc.clearDisplay(0);	  //清空屏幕

	//wifi和mqtt的设定
	setup_wifi();//连接wifi
	client.setServer(mqtt_server, 1883);
	client.setCallback(callback);  //只想intopic中的那个消息指针

	//Wavehc库的设定
	//如果sd卡初始化失败
/*	if (!card.init(true)) {

	}
	//初始化分区判定
	uint8_t part;
	for (part = 0; part < 5; part++) {
		if (vol.init(card, part))
			break;
	}
	if (part == 5) {
		sdErrorCheck();
		while (1);
	}
	//打开根目录失败
	if (!root.openRoot(vol)) {
	}
	//打印出所有文件
	root.ls(LS_R | LS_FLAG_FRAGMENTED);//？？？

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
	//接收的函数，topic为接收订阅的主题，payload为长度，最后一个为payload的长度
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
	//装载json
	DynamicJsonBuffer jsonBuffer;//动态json对象
	JsonObject& root1 = jsonBuffer.createObject(); //创建一个新的对象
	root1["Humidity"] = humidity; //key为Humidity的value为humidity
	root1["Temperature"] = temperature;
	//  root1.prettyPrintTo(Serial);
	root1.printTo(msg);//把json格式的转换为字符串传入msg中
}
void decodeJson(char msg[100]) {
	//解析json
	DynamicJsonBuffer jsonBuffer;
	JsonObject& root = jsonBuffer.parseObject(msg);//解析一个字符串对象
	float temp = root["Temperature"];
	//将key为temperature对应的value存入temp中
	float hum = root["Humidity"];

	Serial.println(temp);
	Serial.println(hum);

}
//以下是非控制部分
void MqttAndJson()
{
	//执行mqtt相关的部分

}
void controlneck()
{
	//控制舵机
}
void controlface()
{
	//控制表情的部分
}
void getwords()
{
	//从mqtt中接收到的字
	//平时的时候这个key为0
}
void GazeAprh(int8_t x,int8_t y)
{
	//用setled速度可能会慢
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
		//瞳孔的设定是从点阵的图像中挖空2*2
		//这个地方要让瞳孔的旧位置慢慢接近新的位置
		//newX - (dX * gazeTime / gazeMove) 这样就慢慢接近了
		GazeAprh(newX - (dX * gazeTime / gazeMove), newY - (dY * gazeTime / gazeMove));
		if (gazeTime == 0) {
			gazeX = newX;
			gazeY = newY;
			do {
				//new的位置固定在预置的坐标范围里面 也就是一个圆
				//设计出了眼珠的随机移动，设定出新的东西
				//random的参数其实是眼圈的限制
				//因为3正好是算上了空出来的2*2的坐标的
				//每种表情的y范围都不一样的
				SetNewGaze();
				dX = newX - 3;
				dY = newY - 3;
			} while ((dX * dX + dY * dY) >= 10);

		}
	}
	else {
		//眼睛不动的时候
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
	//要求呈现的内容：按照顺序实现眨眼的效果，也就是按照一定的顺序去放出数组，
	//也就是要按照index的顺序来
	//第一步，按照全局的顺序画出位图，毕竟这个是在loop里面的，也就是说只要一次呈现出一张图就好了
	//第二步如果时间耗完就随机获得一个新的时间
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
//移动舵机
myser.write(gazeX * 10);
myser2.write(gazeY * 10);

}
*/

void CheckTouch()
{
	//检测自己是否被触碰
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
//这段也是
PgmPrint("Error: ");
SerialPrint_P(str);
sdErrorCheck();
while (1);
}
void sdErrorCheck(void) {
	//这一段是关于Sd卡操作的
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
	while (dir.readDir(dirBuf) > 0) {    //开始读文件，读所有的文件

										 // 如果不是dir或者wav就直接跳过
		if (!DIR_IS_SUBDIR(dirBuf) && strncmp_P((char *)&dirBuf.name[8], PSTR("WAV"), 3)) {
			continue;
		}

		Serial.println();            //新的一样

		for (uint8_t i = 0; i < dirLevel; i++) {
			Serial.write(' ');       // 为了读文件的交互
		}
		if (!file.open(vol, dirBuf)) {        // 如果无法打开
											  //      error("file.open failed");          // something went wrong
		}

		if (file.isDir()) {                   // 打开的是否是一个新的文件，递归打开文件
			printEntryName(dirBuf);
			Serial.println();
			dirLevel += 2;                      
												
			PlayVoice(file);                         
			dirLevel -= 2;
		}
		else {
			//如果不是dir
			printEntryName(dirBuf);              
			if (!wave.create(file)) {            // 判定是否是wav
				putstring(" Not a valid WAV");     // ok skip it
				return;
			}
			else {
				Serial.println();                  
				wave.play();                       
				uint8_t n = 0;
				while (wave.isplaying) {
					//播放声音的时候串口显示
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
	//持续播放一段声音
	playfile(name);
	while (wave.isplaying) { 
							 
	}
	
}
void playfile(char *name) {
	
	if (wave.isplaying) {		//如果在播放其他声音的话就停下来
		wave.stop(); 
	}
	
	if (!f.open(root, name)) {	//如果打开失败的话就GG
		Serial.print("can't open file");
		Serial.println (name);
		return;
	}
	
	if (!wave.create(f)) {	//判定是否是wav
		Serial.println(name);
		Serial.println("Not a WAV");
	}

	// 播放
	wave.play();
}*/
