#include "LedControl.h"
#include "Server.h"
//#include "WaveHC.h"
//#include "WaveUtil.h"
/*
  2018/7/18  bug和未完成的功能的情况：
  1.触摸传感器和touchnum的冲突
  2.声音和相关情绪的衔接
  3.收集声音

*/
//LED控制的引脚
#define CLK     D2     
#define CS        D3    
#define DIN       D1      
LedControl lc = LedControl(DIN, CLK, CS, 1);

//舵机设定
#define wir1
#define wir2
int pos = 0;

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


//Servo myser;
//Servo myser2;


uint8_t
blinkIndex[] = { 1, 2, 3, 4, 3, 2, 1 }, //sizeof = 7 
blinkTime = 100,          //其实是眨眼的周期
gazeTime = 75,            //下一次眼睛移动的时间倒计时//注视周期
gazeMove = 50;            //眼球滚动的时间
int8_t
gazeX = 3, gazeY = 3,       //眼球的坐标
newX = 3, newY = 3,         //新的眼球的坐标
dX = 0, dY = 0;           

int TouchNum = 15; 
byte mood = 3;
//const byte vibration PROGMEM = ;//触摸传感器的引脚 
//const int TouchLevel PROGMEM = 512;//触摸电位的等级
#define vibration D6
long PreMillis = 0;        
#define analog A0

const int decay = 30000;  //衰退时间       

unsigned long checkMillis,touchMillis,nowMillis; //各种时间的定义


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
  pinMode(D6,INPUT);
  randomSeed(analogRead(analog));
  //myser.attach(wir1);
  //myser2.attach(wir2);

  //LedControl的库的定义
  lc.shutdown(0, false);//节点模式
  lc.setIntensity(0, 5);//设置LED亮度
  lc.clearDisplay(0);   //清空屏幕


  //Wavehc库的设定
  //如果sd卡初始化失败


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
void DrawFace(byte *face)
{
  for (int i = 0; i<8; i++)
  {
    lc.setRow(0, i, face[i]);
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
    DrawFace(sadFace[(blinkIndex[JudgeBlinkTime()])]);
  else if (mood == 1)
    DrawFace(normalFace[(blinkIndex[JudgeBlinkTime()])]);
  else if (mood == 2)
    DrawFace(happyFace[(blinkIndex[JudgeBlinkTime()])]);
  else if (mood == 3)
    DrawFace(botherFace[(blinkIndex[JudgeBlinkTime()])]);

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
  }

  if (TouchNum <= 10) mood = 0;   
  else if (TouchNum <= 20) mood = 1;
  else if (TouchNum <= 30) mood = 2;
  else if (TouchNum > 30) mood = 3;

  nowMillis = millis();
  if (nowMillis - PreMillis > decay) {
  
    PreMillis = nowMillis;
    TouchNum--;
    if (TouchNum < 0) TouchNum = 0;
    if (TouchNum == 30) TouchNum = 15;
    if (TouchNum > 40) TouchNum = 40;
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
