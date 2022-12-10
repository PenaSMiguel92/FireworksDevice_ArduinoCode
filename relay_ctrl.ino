//const int INPUTS[] = {13,12,11,10,9,8,7,6};
const int SER_Pin = 13;
const int SRCLK_Pin = 12;
const int RCLK_Pin = 11;
const int modeSelectPin = 8;
const int modeExecutePin = 7;
bool modeSelectDebounce = false;
bool modeExecuteDebounce = false;
int currentMode = 0;
const int maxMode = 4;
byte outputByte = B00000000;
unsigned long st_time_select = micros();
unsigned long st_time_exec = micros();

byte firingModes[] = {
  B00000000, //All at once
  B00110011, //Every other two
  B10101010, //Every other one
  B11111110, //One by one, from right to left.
  B01111111 //One by one, from left to right.
};


void setup() {
  // put your setup code here, to run once:
  //for (int i=0;i<8;i++)
  //{
  pinMode(SER_Pin,OUTPUT);
  pinMode(SRCLK_Pin,OUTPUT);
  pinMode(RCLK_Pin,OUTPUT);

  pinMode(modeSelectPin,INPUT);
  pinMode(modeExecutePin,INPUT);
  
  Serial.begin(9600);
  //INPUTS[i],OUTPUT);
  //}

}

void loop() {
  // put your main code here, to run repeatedly:
  //outputByte<<1;
  if ((digitalRead(modeSelectPin)==HIGH) && !modeSelectDebounce){
    selectMode();
    modeSelectDebounce = true;
    st_time_select = micros();
  }
  if ((digitalRead(modeExecutePin)==HIGH) && !modeExecuteDebounce){
    modeExecute();
    setAllToOff();
    modeExecuteDebounce = true;
    st_time_exec = micros();
  }
  if ((modeSelectDebounce) && ((micros() - st_time_select) > 250))
  {
    modeSelectDebounce = false;
  }
  if ((modeExecuteDebounce) && ((micros() - st_time_exec) > 250))
  {
    modeExecuteDebounce = false;
  }
}

void send_data(byte data)
{
  digitalWrite(RCLK_Pin, HIGH);
  digitalWrite(SRCLK_Pin,LOW);
  shiftOut(SER_Pin, SRCLK_Pin,LSBFIRST, data);
  digitalWrite(RCLK_Pin,LOW);
}
void alternateBits()
{
  outputByte = firingModes[currentMode];
  send_data(outputByte);
  delay(2500);
  send_data(~outputByte);
  delay(2500);
}
void shiftLeftToRight()
{
  for (int i=0;i<8;i++)
  {
    
    outputByte = firingModes[currentMode] >> i;
    send_data(outputByte);
    delay(2500);
  }
}
void shiftRightToLeft()
{
  for (int i=0;i<8;i++)
  {
    
    outputByte = firingModes[currentMode] << i;
    send_data(outputByte);
    delay(2500);    
  }
}

void selectMode()
{
  currentMode++;
  if (currentMode>maxMode)
  {
    currentMode = 0;
  }
  //outputByte = firingModes[currentMode];
}
void modeExecute()
{
  switch(currentMode)
  {
    case 0:      
      alternateBits();
      break;
    case 1:
      alternateBits();
      break;
    case 2:
      alternateBits();
      break;
    case 3:
      shiftRightToLeft();
      break;
    case 4:
      shiftLeftToRight();
      break; 
  }
  
}
void setAllToOff()
{
  outputByte = B11111111;
  send_data(outputByte);
}
