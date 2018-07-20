#读取文件
from struct import *
GetWord = "ABCDEFG"#是一个字符串格式的，之后是终端传输

def GetFmt():
    fmt = ""
    for i in range(len(GetWord)):
        for j in range(8):
            fmt += 'B'
    return fmt

def GetOneLetter(n):
#    One = []
    AsciiFront = open("FONT.BIN","rb")
    AsciiByte = AsciiFront.read(1024)#读进去的byte全都成了十进制的数据
    AsciiFront.close()
    offset = ord(GetWord[n])*8
    for i in range(8):
        SendByte.append(AsciiByte[offset+i])

SendByte = []
for i in range(len(GetWord)):
    GetOneLetter(i)
#print(len(SendByte))
Format = GetFmt()
AfterPack = pack(Format,*SendByte)
#for i in range(len(SendByte)):
#    print(SendByte[i])
for i in AfterPack:
    print(i)
