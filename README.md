# WaterCounter
Measure water consumption by means of Arduino board

Note that sketch provides only following functions, available through COM-terminal:
w - to get current warm water value;
c - to get current cold water value;
s - to preset current values;
d - to set data acquiring interval;
t - to adjust treshold of pulse counting trigger;
l - to adjust one step increment for values;
x - to cross-toggle warm and cold water pins; 
v - toggle on/off measurement data transmission*;
*Ttransmission format are following ...0xFF 0xXX 0xYY 0xFF 0xXX 0xYY 0xFF..., where 0xFF - markup symbol, 0xXX - 8 bit measurement from warm channel, and 0xYY - 8 bit measurement from cold channel.
