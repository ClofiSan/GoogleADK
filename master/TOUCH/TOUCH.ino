void setup() {
  // put your setup code here, to run once:
  pinMode(D6,INPUT);
  Serial.begin(115200);
  delay(10);
}
int num;
int num2;
void loop() {
  // put your main code here, to run repeatedly:
  int flag = digitalRead(D6);
  if(flag){
    num++;
    }
  Serial.println(num-200);
 
}
