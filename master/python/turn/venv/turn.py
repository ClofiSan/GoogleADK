#读取文件
GetWord = "I AM ZLJ"#是一个字符串格式的，之后是终端传输


def GetOneLetter(n):
    One = []
    AsciiFront = open("FONT.BIN","rb")
    AsciiByte = AsciiFront.read(1024)#读进去的byte全都成了十进制的数据
    AsciiFront.close()
    offset = ord(GetWord[n])*8
    for i in range(8):
        One.append(hex(AsciiByte[offset+i]))
    return One

SendByte = []
for i in range(len(GetWord)):
    SendByte.append(GetOneLetter(i))
for i in range(len(SendByte)):
    for j in SendByte[i]:
        print(hex(eval(j)),end='')
        print(",",end='')

