#include <WiFi.h>

WiFiClient client;

// Left motor pin
const int pinBO1 = 1;
const int pinBO2 = 2;
const int pinBO3 = 3;

// Right motor pin
const int pinAO1 = 4;
const int pinAO2 = 5;
const int pinAO3 = 6;

const int pinLED = 13;

const int pinEchoResponse = 8;
const int pinEchoTrigger = 9;
float speed = 50;
boolean go = true;
boolean wentBackward = false;

IPAddress serverIP(192, 168, 0, 102);
const int serverPort = 1234;

char ssid[] = "EdisonWiFI";
char wifiPassword[] = "00000000";

void setup() {
  connectWifi(ssid, wifiPassword);

  if (client.connect(serverIP, serverPort)) {
    client.println("Edison connected");
  }
  
  pinMode(pinAO1, OUTPUT);
  pinMode(pinAO2, OUTPUT);
  pinMode(pinAO3, OUTPUT);

  pinMode(pinBO1, OUTPUT);
  pinMode(pinBO2, OUTPUT);
  pinMode(pinBO3, OUTPUT);

  pinMode(pinLED, OUTPUT);

  pinMode(pinEchoTrigger, OUTPUT);
  pinMode(pinEchoResponse, INPUT);
  
  flashLED(10, 50);
}

void loop() {//mapping algorithm
  unsigned int distance = IR();
  
  float accelerate = 1.15;  
  client.println(distance);
  if(go){    
    if (speed < 100){
      speed = speed * accelerate;
    }    
    forward((int)speed,50);
  }
  else{
    if(speed > 50){
      speed = speed / accelerate;
    }
    reverse((int)speed,50);
    if(speed == 50){
      wentBackward = true;
    }
    
  }
  if (distance == 1) {
    go = false;
  }
  if (wentBackward){
    forward(50,50);//move a little forward so you don't hit the exact same spot
    delay(100);
    go = true;
    wentBackward = false;
  }
}

void connectWifi(char ssid[], char wifiPassword[]) {
  int status = WL_IDLE_STATUS;
  while (status != WL_CONNECTED) {
    status = WiFi.begin(ssid, wifiPassword);
    delay(10000);
  }
}

unsigned int IR(){
 unsigned int x = digitalRead(7);
  //flashLED(5,70);
  return x;
}

// Sends a ping and returns in cm how far an obstacle is
unsigned long ping() {
  // Give a short LOW pulse beforehand to ensure a clean HIGH pulse:
  digitalWrite(pinEchoTrigger, LOW);
  delayMicroseconds(2);
  digitalWrite(pinEchoTrigger, HIGH);
  delayMicroseconds(5);
  digitalWrite(pinEchoTrigger, LOW);

  unsigned long duration = pulseIn(pinEchoResponse, HIGH);
  unsigned long distance = duration / 58.2;

  delay(100);
  return distance;
}

void flashLED(int numTimes, int duration) {
  for (int i = 1; i <= numTimes; i++) {
    digitalWrite(pinLED, HIGH);
    delay(duration);
    digitalWrite(pinLED, LOW);
    delay(duration);
  }
}

void forward(int speedR, int speedL) {
  digitalWrite(pinAO1, HIGH);
  digitalWrite(pinAO2, LOW);
  digitalWrite(pinBO1, HIGH);
  digitalWrite(pinBO2, LOW);

  int  sigR = map(speedR, 1 , 100, 0, 255);
  int  sigL = map(speedL, 1 , 100, 0, 255);
  analogWrite(pinAO3, sigL);//right (0-255)
  analogWrite(pinBO3, sigR);//left (0-255)
}

void reverse(int speedR, int speedL) {
  digitalWrite(pinAO1, LOW);
  digitalWrite(pinAO2, HIGH);
  digitalWrite(pinBO1, LOW);
  digitalWrite(pinBO2, HIGH);

  int  sigR = map(speedR, 1 , 100, 0, 255);
  int  sigL = map(speedL, 1 , 100, 0, 255);
  analogWrite(pinAO3, sigL);//left (0-255)
  analogWrite(pinBO3, sigR);//left (0-255)
}

// POSITIVE degrees will turn to the RIGHT
// NEGATIVE degrees will turn to the LEFT
void turn(int degrees) {
  //time for 360
  int t = 10;
  int state[] = {digitalRead(pinAO1), digitalRead(pinAO2), digitalRead(pinBO1), digitalRead(pinBO2)};
  brake();

  if (degrees > 0) {
    digitalWrite(pinAO1, HIGH);
    digitalWrite(pinAO2, LOW);
    digitalWrite(pinBO1, LOW);
    digitalWrite(pinBO2, HIGH);
  } else {
    digitalWrite(pinAO1, LOW);
    digitalWrite(pinAO2, HIGH);
    digitalWrite(pinBO1, HIGH);
    digitalWrite(pinBO2, LOW);
  }

  analogWrite(pinAO3, 255);
  analogWrite(pinBO3, 255);

  delay(abs(degrees * t));

  digitalWrite(pinAO1, state[0]);
  digitalWrite(pinAO2, state[1]);
  digitalWrite(pinBO1, state[2]);
  digitalWrite(pinBO2, state[3]);

  analogWrite(pinAO3, 255);
  analogWrite(pinBO3, 255);
}

void brake() {
  analogWrite(pinAO3, 0);
  analogWrite(pinBO3, 0);
}

