/*-----------------------------------------------------------
A sketch that will run your board as a digital water counter!
Use any COM-port terminal with following commands:
w - to get current warm water value;
c - to get current cold water value;
s - to preset current values;
d - to set data acquiring interval;
t - to adjust treshold of pulse counting trigger;
l - to adjust one step increment for values;
x - to cross-toggle warm and cold water pins; 
v - toggle on/off measurement data transmission*;
*Ttransmission format are following ...0xFF 0xXX 0xYY 0xFF 0xXX 0xYY 0xFF...,
where 0xFF - markup symbol, 0xXX - 8 bit measurement from warm channel,
and 0xYY - 8 bit measurement from cold channel.
Taranov Alex, 2015
-------------------------------------------------------------*/

#define SERIAL_SPEED 115200
#define DEFAULT_SAMPLE_PERIOD_US 1000
#define DEFAULT_WARM_PIN 0
#define DEFAULT_COLD_PIN 1
#define DEFAULT_LITERS_PER_STEP 10
#define DEFAULT_TRESHOLD 512

unsigned int m_samplePeriod = DEFAULT_SAMPLE_PERIOD_US; 
unsigned long m_timeStamp = 0;
int incomingSymbol = 0;
int m_treshold = DEFAULT_TRESHOLD;
bool m_sendDataFlag = false;
byte m_stepLiters = DEFAULT_LITERS_PER_STEP;

byte m_warmPin = DEFAULT_WARM_PIN;
byte m_coldPin = DEFAULT_COLD_PIN;

unsigned long m_warm = 0;
unsigned long m_cold = 0;

int m_oldC = 0;
int m_newC = 0;
int m_oldW = 0;
int m_newW = 0;
//----------------------------------------------------
void setup() {  
  Serial.begin(SERIAL_SPEED);  
}
//----------------------------------------------------
void loop() {
     
    if( (micros() - m_timeStamp) >= m_samplePeriod) {
      seekPulse();  
      m_timeStamp = micros();   
    }
    
      if(Serial.available()) {  
        incomingSymbol = Serial.read();   
      }     
      switch(incomingSymbol) {
        case 'w': Serial.print( getWarm()/1000.0, 3 );
                  Serial.println( " m3" );
                  break;
        case 'c': Serial.print( getCold()/1000.0, 3 );
                  Serial.println( " m3" );
                  break;
        case 's': makePreset();
                  break;
        case 'd': setSamplePeriod();
                  break;
        case 't': setTreshold();
                  break;
        case 'v': toggleDataSending();
                  break;
        case 'l': adjustStepLiters();
                  break;
        case 'x': toggleChannels();
                  break;                                   
      }
      incomingSymbol = 0;  
}
//----------------------------------------------------
void setWarm(unsigned long value) {
  m_warm = value; 
}
//----------------------------------------------------
void setCold(unsigned long value) {
  m_cold = value; 
}
//----------------------------------------------------
void updateWarm() {
  m_warm += m_stepLiters;
}
//----------------------------------------------------
void updateCold() {
  m_cold += m_stepLiters;
}
//----------------------------------------------------
unsigned long getWarm() {
  return m_warm; 
}
//----------------------------------------------------
unsigned long getCold() {
  return m_cold; 
}
//----------------------------------------------------
void seekPulse() {

  m_newW = analogRead(m_warmPin);
  m_newC = analogRead(m_coldPin);
  
  if((m_newW - m_oldW) > m_treshold)
    updateWarm();   
  if((m_newC - m_oldC) > m_treshold)
    updateCold();
  
  m_oldW = m_newW;
  m_oldC = m_newC;
  
  if(m_sendDataFlag) {
    byte out = (byte)(m_newW >> 2);
    if(out == 0xFF) {
      out = 0xFE;
    }
    Serial.write(out);
    out = (byte)(m_newC >> 2); 
    if(out == 0xFF) {
      out = 0xFE;
    }
    Serial.write(out);
    Serial.write(0xFF); // it is data markup symbol ...0xFF 0xXX 0xYY 0xFF 0xXX 0xYY 0xFF...
  }
}
//----------------------------------------------------
void makePreset()  {  
  bool wait = true;
  
  Serial.println("Send warm preset in *.XXX format, m3:");  
  while(wait)    
    if(Serial.available()) {  
        setWarm( (unsigned long)(Serial.parseFloat()*1000) );
        wait = false;   
    }

  wait = true;
  Serial.println("Send cold preset in *.XXX format, m3:");   
  while(wait)    
    if(Serial.available()) {  
        setCold( (unsigned long)(Serial.parseFloat()*1000) );
        wait = false;   
    }   
}
//----------------------------------------------------
void setTreshold()
{
  bool wait = true;
  Serial.println("Send treshold from 0 to 1023:");
  while(wait) {
    if(Serial.available()) { 
      m_treshold = (Serial.parseInt() % 1024);
      wait = false;
    }
  } 
}
//----------------------------------------------------
void setSamplePeriod()
{
  bool wait = true;
  Serial.println("Send sample period in us, not less than 250:");
  while(wait) {
    if(Serial.available()) {
      m_samplePeriod = Serial.parseInt();
      wait = false;
    }
  }  
}
//----------------------------------------------------
void toggleDataSending()
{
  m_sendDataFlag = !m_sendDataFlag; 
}
//----------------------------------------------------
void adjustStepLiters()
{
  bool wait = true;
  Serial.println("Send one step size in liters:");
  while(wait) {
    if(Serial.available()) {
      m_stepLiters = Serial.parseInt();
      wait = false;  
    }
  }     
}
//----------------------------------------------------
void toggleChannels()
{
  byte temp = m_warmPin;
  m_warmPin = m_coldPin;
  m_coldPin = temp; 
}


