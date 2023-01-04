# define NOP __asm__("nop")
// LED pattern
unsigned int pattern = 0b0001000100010010; // RED | RED | RED | GR
unsigned int maskPattern[4] = {0xFFF0,0xFF0F,0xF0FF,0x0FFF};
// Sensor variables
int presentSensorId = 0; // Initial value
int nextSensorId = 1; // Initial value
int sensorCount = 4;
// Time constants
const long greenDelay = 5000; // 10 sec
const long yellowDelay = 2000; // 4 sec
// Sensor Pins
int sensorPin[4] = {A0,A1,A2,A3};
// SHR control
int SER = 2;
int RST = 3;
int inClk = 4;
int outClk = 5; 
//*************************************************************************
void init_Port(){
 pinMode(sensorPin[0],INPUT);
 pinMode(sensorPin[1],INPUT);
 pinMode(sensorPin[2],INPUT);
 pinMode(sensorPin[3],INPUT);
}
//*************************************************************************
void init_Light(){
 pinMode(SER,OUTPUT);
 pinMode(RST,OUTPUT);
 pinMode(inClk,OUTPUT);
 pinMode(outClk,OUTPUT);
 digitalWrite(SER,LOW);
 digitalWrite(RST,LOW);
 digitalWrite(inClk,LOW);
 digitalWrite(outClk,LOW);
 delay(500);
 
digitalWrite(inClk,HIGH);
delay(10);
 
digitalWrite(outClk,HIGH);
delay(10);
 
digitalWrite(inClk,LOW);
NOP;
 
digitalWrite(outClk,LOW);
digitalWrite(RST,HIGH);
}
//*************************************************************************
void write_LED(byte val1, byte val2){
digitalWrite(outClk,LOW);
NOP;
 
digitalWrite(inClk,LOW);
NOP;
 
shiftOut(SER, inClk, MSBFIRST, val2);
shiftOut(SER, inClk, MSBFIRST, val1);
NOP;
 
digitalWrite(outClk,HIGH);
NOP;
NOP;
NOP; 
 
digitalWrite(outClk,LOW);
NOP;
 
digitalWrite(inClk,LOW);
NOP; 
}
void displaySignal(){
write_LED(byte(pattern & 0xFF),byte(pattern >> 8)); 
}
//*************************************************************************
void green2yellowUpdate(){ // Green to yellow
 unsigned int musk_presentSensorId;
 unsigned int musk_nextSensorId;
pattern = 0b0001000100010001; // Default make all red then change
 
musk_presentSensorId = maskPattern[presentSensorId];
musk_nextSensorId = maskPattern[nextSensorId];
pattern = pattern & musk_presentSensorId; 
pattern = pattern | (0b0100 << presentSensorId*4); // Load yellow pattern
pattern = pattern & musk_nextSensorId; 
pattern = pattern | (0b0100 << nextSensorId*4); // Load yellow pattern
 }
//*************************************************************************
void yellow2greenUpdate(){ // Yellow to green rest are RED
unsigned int musk_nextSensorId;
pattern = 0b0001000100010001; // Default make all red then change
 
musk_nextSensorId = maskPattern[nextSensorId];
pattern = pattern & musk_nextSensorId; 
pattern = pattern | (0b0010 << nextSensorId*4); // Load green pattern
}
//*************************************************************************
void sensorIdUpdate(){
 int I;
 int busyFlag = 0; // reset value 
 // Checking next sensors
 for (I=presentSensorId;I<sensorCount;I++){
 
 if ((I!=presentSensorId) && (digitalRead(sensorPin[I]) == LOW)){
 nextSensorId = I;
 busyFlag = 1;
 break;
 }
}
// Checking previous sensors
 if (busyFlag == 0){ // No busy road found
 
// Checking previous sensors
 for (I=0;I<presentSensorId;I++){
 if ((I!=presentSensorId) && (digitalRead(sensorPin[I]) == LOW)){
 nextSensorId = I;
 busyFlag = 1;
 break;
 }
 }
}
// If nothing detected, regular update
 if (busyFlag == 0){
 nextSensorId = (presentSensorId + 1) % sensorCount;
 }
 }
//*************************************************************************

void setup() {
Serial.begin(9600);
Serial.println(" ------- Automatic Traffic Ligh Control based on Density --------");
 
// SHR Init
 init_Light();
}
//*************************************************************************
void loop() {
 displaySignal();
 delay(greenDelay);
 sensorIdUpdate();
 green2yellowUpdate();
 displaySignal();
 delay(yellowDelay);
 yellow2greenUpdate();
 presentSensorId = nextSensorId; // Update sensor id
 sensorIdUpdate(); // Now update for next sensor (1) from busy road / (2) regular
}
