#这里其实根本不需要用到node-red，因为python上是直接传输数据到ser上的，所以node-red最多是一次控制
#这里知识基于python控制，py接收就要一直把程序开着处理了
#也就是说，node-red只要控制py的开关就好了
#py本身的数据传输是独立的
#除非要从py中知道消息做出可视化来
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time
import json
import struct as s
GetWord = "Hello"
SendByte = []
Initfmt = 'B'
PubTopic = "PyToNode"
SubTopic = "AndToPy"
#InitPayload = {"Hello":"Hello,I am python~"}
#PubPayload = json.dumps(InitPayload)
PubPayload = []
MQTTHOST = "123.206.127.199"
MQTTPORT = 1883
mqttClient = mqtt.Client()


def GetFmt():
    fmt = ""
    for i in range(len(GetWord)):
        for j in range(8):
            fmt += Initfmt
    return fmt
def mqtt_connect():
    mqttClient.connect(MQTTHOST,MQTTPORT,60)
    mqttClient.loop_start()
def on_publish(topic,payload,qos):
    mqttClient.publish(topic,payload,qos)
def on_message_come(Client,userdata,msg):
    print(msg.topic+" "+":"+str(msg.payload))
def on_subscribe():
    mqttClient.subscribe("/WordsFromAndroid",1)
    mqttClient.on_message = on_message_come
def GetOneLetter(n):
    One = []
    AsciiFront = open("FONT.BIN","rb")
    AsciiByte = AsciiFront.read(1024)#读进去的byte全都成了十进制的数据
    AsciiFront.close()
    offset = ord(GetWord[n])*8
    for i in range(8):
        SendByte.append(AsciiByte[offset+i])
    #    One.append(AsciiByte[offset+i])
    #return One
def main():
    #传输格式应该是什么呢？其实只要是一个全都是byte的数组就好了
    #到时候在arduino里面设置一个全局变量传进去就好了
    #SendByte2 = []
    for i in range(len(GetWord)):
        #SendByte2 =
         GetOneLetter(i)
    Format = GetFmt()
    AfterPack = s.pack(Format, *SendByte)
    mqtt_connect()
    while 1:
        on_publish(PubTopic,AfterPack,2)
       # time.sleep(10)
       # on_publish(PubTopic,AfterPack,1)
        time.sleep(5)


if __name__ == '__main__':
    main()