/*   Motor2Encoder
 *    Sample Arduino code to demostrate the usage of a BLDC/PMS motor as a rotary encoder
 *    it gives the possibility to create a human computer interface device with haptic feedback capability
 *   
 *   Created by Besenyei Máté (besenyeim)
 *   License: Creative Commons Zero
 *    No Rights Reserved
 *    
 *   Tested on STM32duino, on a "Blue Pill" board
 *   external parts and connections:
 *   3 half bridges made of complementary FETs, all gates are driven separately by MCU
 *   high sides are off when corresponding MCU pins are high
 *   low sides are off when corresponding MCU pins are low
 *   3 phase motor connected to half bridge outputs
 *   these outputs are also connected to MCU analog pins to provide voltage feedback
 */

//pin connection definitions:
#define PAH PB7   //phase A high side
#define PAL PB14  //phase A low side
#define PBH PB6   //phase B high side
#define PBL PB13  //phase B low side
#define PCH PB5   //phase C high side
#define PCL PB12  //phase C low side
#define PAF PA0   //phase A feedback
#define PBF PA1   //phase B feedback
#define PCF PA2   //phase C feedback

//try plotting and then tuning these values to refine sensing:
#define TRU 2400  //upper threshold, 2400 is my base value
#define TRL 1600  //lower threshold, 1600 is my base value

int fbbuf;    //variable to store temporary feedback data
int step;     //variable to store the current recorded position ranges from 1 to 6
bool haptic;  //status of haptic feedback function. enabled/disabled
int incomingByte;   //buffer for incoming serial data

/*
 *  switches off all half-bridge outputs. hi-Z mode
 */
void clear(){
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, LOW);

}

/* 
 *  drive the motor according to the current step
 *  used by haptic, override and testing functions
 */
void set_bridges(){
  switch(step){
    case 1:{  //A-BC
      digitalWrite(PAH, LOW);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);      
      break;
    }
    case 2:{  //AB-C
      digitalWrite(PAH, LOW);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, LOW);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);   
      break;
    }
    case 3:{  //B-AC
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBH, LOW);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);
      break;
    }
    case 4:{  //CB-A
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBH, LOW);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);
      break;
    }
    case 5:{  //C-AB
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);   
      break;
    }
    case 6:{  //AC-B
      digitalWrite(PAH, LOW);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);      
      break;
    }
  }
}

/*
 *  measures the motor all way it can, writes the results in one line to serial, with increasing level offset
 *  for testing and debugging
 *  nice graphs can be drawn with it
 */
void scan_all(){
//AB-C
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, LOW);
      digitalWrite(PAH, LOW); //put last to reduce adc delay 
      fbbuf = analogRead(PCF);  //read C
      digitalWrite(PAH, HIGH);  //switch off
      Serial.print(fbbuf);
      Serial.print(' ');
//AC-B
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);
      digitalWrite(PAH, LOW); //put last to reduce adc delay 
      fbbuf = analogRead(PBF);  //read B
      digitalWrite(PAH, HIGH);  //switch off
      Serial.print(fbbuf+3000);
      Serial.print(' ');
//BC-A
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, LOW);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);
      fbbuf = analogRead(PAF);  //read A
      digitalWrite(PBH, HIGH);  //switch off
      Serial.print(fbbuf+6000);
      Serial.print(' ');
//BA-C
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, LOW);
      digitalWrite(PBH, LOW);  //put last to reduce adc delay 
      fbbuf = analogRead(PCF);  //read C
      digitalWrite(PBH, HIGH);  //switch off
      Serial.print(fbbuf+9000);
      Serial.print(' ');
//CA-B
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);   
      fbbuf = analogRead(PBF);  //read B
      digitalWrite(PCH, HIGH);  //switch off
      Serial.print(fbbuf+12000);
      Serial.print(' ');
//CB-A
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);      
      fbbuf = analogRead(PAF);  //read A
      digitalWrite(PCH, HIGH);  //switch off
      Serial.print(fbbuf+15000);
      Serial.print(' ');

}

/*
 * Scan one of the channels
 * There is 6, but as 3 pairs. The pairs are mirrored to each other.
 */
int scan_one(int which){
  clear();
  delay(1); //measuremenet was noisy
  
  switch(which){
    case 1:{  //AB-C
      digitalWrite(PAH, LOW);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, LOW);
      fbbuf = analogRead(PCF);  //read C
      digitalWrite(PAH, HIGH);  //switch off
      return fbbuf;
    }
    case 2:{  //AC-B
      digitalWrite(PAH, LOW);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);
      fbbuf = analogRead(PBF);  //read B
      digitalWrite(PAH, HIGH);  //switch off
      return fbbuf;
    }
    case 3:{  //BC-A
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, LOW);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, HIGH);
      fbbuf = analogRead(PAF);  //read A
      digitalWrite(PBH, HIGH);  //switch off
      return fbbuf;
    }
    case 4:{  //BA-C
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBH, LOW);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, HIGH);
      digitalWrite(PCL, LOW);
      fbbuf = analogRead(PCF);  //read C
      digitalWrite(PBH, HIGH);  //switch off
      return fbbuf;
    }
    case 5:{  //CA-B
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, HIGH);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, LOW);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);   
      fbbuf = analogRead(PBF);  //read B
      digitalWrite(PCH, HIGH);  //switch off
      return fbbuf;
    }
    case 6:{  //CB-A
      digitalWrite(PAH, HIGH);
      digitalWrite(PAL, LOW);
      digitalWrite(PBH, HIGH);
      digitalWrite(PBL, HIGH);
      digitalWrite(PCH, LOW);
      digitalWrite(PCL, LOW);      
      fbbuf = analogRead(PAF);  //read A
      digitalWrite(PCH, HIGH);  //switch off
      return fbbuf;
    }
  }
}

/*
 * scan for movement, and update step if there's any
 * return value: 
 *  1 - there was a change
 *  0 - no change
 */
bool update(){
  switch(step){
    case 1:{
      fbbuf = scan_one(6);
      if (fbbuf > TRU) step = 2;  //step up
      if (fbbuf < TRL) step = 6;  //step down
      if (step == 1) return 0; else return 1;
    }
    case 2:{
      fbbuf = scan_one(4);
      if (fbbuf > TRU) step = 3;  //step up
      if (fbbuf < TRL) step = 1;  //step down
      if (step == 2) return 0; else return 1;
    }
    case 3:{
      fbbuf = scan_one(2);
      if (fbbuf > TRU) step = 4;  //step up
      if (fbbuf < TRL) step = 2;  //step down
      if (step == 3) return 0; else return 1;
    }
    case 4:{
      fbbuf = scan_one(6);
      if (fbbuf > TRU) step = 5;  //step up
      if (fbbuf < TRL) step = 3;  //step down
      if (step == 4) return 0; else return 1;
    }
    case 5:{
      fbbuf = scan_one(4);
      if (fbbuf > TRU) step = 6;  //step up
      if (fbbuf < TRL) step = 4;  //step down
      if (step == 5) return 0; else return 1;
    }
    case 6:{
      fbbuf = scan_one(2);
      if (fbbuf > TRU) step = 1;  //step up
      if (fbbuf < TRL) step = 5;  //step down
      if (step == 6) return 0; else return 1;
    }
  }
  return 0;
}

/*
 * graph all, step by step, used for testing
 */
void plot_all(){
  step++;
  if (step > 6) step=1;
  set_bridges();
  delay(1000);
  clear();
  delay(1);
  scan_all();
  Serial.print(' ');
  Serial.print(step*1000);
  Serial.print('\n');
  scan_all();
  Serial.print(' ');
  Serial.print(step*1000);
  Serial.print('\n');
  scan_all();
  Serial.print(' ');
  Serial.print(step*1000);
  Serial.print('\n');
  scan_all();
  Serial.print(' ');
  Serial.print(step*1000);
  Serial.print('\n');
}

 
void setup() {  
  pinMode(PAH, OUTPUT);
  pinMode(PAL, OUTPUT);
  pinMode(PBH, OUTPUT);
  pinMode(PBL, OUTPUT);
  pinMode(PCH, OUTPUT);
  pinMode(PCL, OUTPUT);
  pinMode(PAF, INPUT_ANALOG);
  pinMode(PBF, INPUT_ANALOG);
  pinMode(PCF, INPUT_ANALOG);
  Serial.begin(9600);
  
  delay(500);

  haptic = 1; //haptic enabled by default
  
  //we try to move it to the initial position:
  step = 1;
  set_bridges();
  delay(500);
  clear();
  delay(1);
  //try to find it's position (should be 1):
  scan_one(6);
  if (fbbuf > TRU) step = 2;
  if (fbbuf < TRL) step = 3;
  if ((fbbuf > TRL) && (fbbuf < TRU)) step = 1;
  Serial.println("init pos:");
  Serial.println(step);
}

void loop() {
  
  delay(10);  //this affects the sampling frequency and haptic strength. 10 is a good basic value for me.
  
  //plot_all();
  
  if ( update() ) Serial.println(step);

  if (Serial.available() > 0) {
    // read the incoming byte:
    incomingByte = Serial.read();
    //overwrite current step and enable haptic, send a number 1-6:
    if ((incomingByte > '0') && (incomingByte < '7')) {
      step = incomingByte-'0'; 
      haptic = 1;
      Serial.println(step);
      //my motor needs a  little extra time to get into position:
      set_bridges();
      delay(100);
      }
    //disable haptic, send 0:
    if (incomingByte == '0') haptic = 0;
  }

  if (haptic) set_bridges();  



}
