#探索mqtt库
#这里其实根本不需要用到node-red，因为python上是直接传输数据到ser上的，所以node-red最多是一次控制
#这里知识基于python控制，py接收就要一直把程序开着处理了
#也就是说，node-red只要控制py的开关就好了
#py本身的数据传输是独立的
#除非要从py中知道消息做出可视化来
import paho.mqtt.client as mqtt
import paho.mqtt.publish as publish
import time
#import json

PubTopic = 'PyMsg'
SubTopic = 'NodesTell'
#InitPayload = {"Hello":"Hello,I am python~"}
#PubPayload = json.dumps(InitPayload)
PubPayload = "Hello,I am python~"
MQTTHOST = "123.206.127.199"
MQTTPORT = 1883
mqttClient = mqtt.Client()  #创建一个client的类

def mqtt_connect():
    mqttClient.connect(MQTTHOST,MQTTPORT,60)#最后一个参数（没写完）是本地ip
    mqttClient.loop_start()#后台启动一个线程，释放主线程，让主线程去做其他的事情

def on_publish(topic,payload,qos):
    mqttClient.publish(topic,payload,qos) #发送的主题，实际被发送的消息，发送的质量

def on_message_come(Client,userdata,msg):
    print(msg.topic+" "+":"+str(msg.payload))

def on_subscribe():
    mqttClient.subscribe(SubTopic,2)#主题和质量 可以元组-列表的格式然后订阅多个主题
    mqttClient.on_message = on_message_come #接收主题之后的回调函数，可以直接打印出来

def main():
    mqtt_connect()
    while(1):
        on_publish(PubTopic,PubPayload,2)
        time.sleep(5)
        on_subscribe()
    #问题是这个地方是否有循环？
    #while True:
    #    pass



if __name__ == '__main__':
    main()