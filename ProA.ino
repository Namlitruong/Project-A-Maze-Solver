//RMIT UNIVERSITY VIETNAM
//OENG1065 - ENGINEERING PROJECT A
//
//PROJECT NAME: Maze Solving Robot
//TEAM MEMBERS:   Truong Phan Hoai Nam - s3518702
//                Bui Khac Phuong Uyen - s3618720
//                Park Su Young        - s3634788
//                Le Gia Bao           - s3595082
//                Tran Thi Hong Phuong - s3623386

//PROJECT DESCRIPTION: In this project, we design and build a microcontroller-based mobile robot
//programmed in C language to solve a line maze using line tracking technique.
//=======================================================

//=====DECLARATION=====
unsigned long count;
uint8_t sensor[5];
char Case;
//uint8_t Situation = 0;

  // VARIABLES FOR SENSOR READING
float activeSensor = 0; // Count active sensors
float totalSensor = 0;  // Total sensor readings
float avgSensor =3;     // Average sensor reading

  // VARIABLES FOR PID CONTROLLER
float Kp = 66;
float Ki = 0.05;
float Kd = 2;
float error = 0;
float *er_pt = &error;      // error pointer
float previousError = 0;
float totalError = 0;
int PIDstatus = 1;
int i;
float power = 0;

  // VARIABLES FOR ADJUSTING SPEED
int turnSpeed =200;
int adj = 3;
int adjRight =2;
int flag;

  // VARIABLES FOR PATH OPTIMISATION
bool finish = false;
char option[50];
int index = 0;
//========================

//=====DELAY FUNCTION=====
void delay_ms (uint16_t millisecond) {
  unsigned long sec;
  sec = ((16000000/12)/1000)*millisecond;
  for (count = 0; count < sec; count ++);
}

//=====MOTOR FUNCTIONS=====
void setPWM_leftmotor (uint8_t PWM6){
  OCR0A = PWM6;
}

void setPWM_rightmotor (uint8_t PWM5){
  OCR0B = PWM5;
}
 
void Forward (){
PORTB = 0b00001010;
}

void Backward (){
 PORTB = 0b00000101;
}

void Stop (){
  PORTB = 0b00000000;
  setPWM_leftmotor(0);
  setPWM_rightmotor(0);
}

void TurnLeft (){
PORTB = 0b00000110;
}

void TurnRight (){
 PORTB = 0b00001001;
}
//========================

//=====READ SENSORS AND ASSIGN VALUE TO SENSOR ARRAY=====
void ReadSensors (){
  // Sensor 0
    if (PINC & (1<<0)){
      sensor[0] = 1;
    }else{
      sensor[0] = 0;}

// Sensor 1
    if (PINC & (1<<1)){
      sensor[1] = 1;
    }else{
      sensor[1] = 0;}

// Sensor 2
    if (PINC & (1<<2)){
      sensor[2] = 1;
    }else{
      sensor[2] = 0;}

// Sensor 3
    if (PINC & (1<<3)){
      sensor[3] = 1;
    }else{
      sensor[3] = 0;}

// Sensor 4
    if (PINC & (1<<4)){
      sensor[4] = 1;
    }else{
      sensor[4] = 0;}

     //Print in the serial monitor 
    Serial.print (sensor[0]);
    Serial.print ("   ");
    Serial.print (sensor[1]);
    Serial.print ("   ");
    Serial.print (sensor[2]);
    Serial.print ("   ");
    Serial.print (sensor[3]);
    Serial.print ("   ");
    Serial.print (sensor[4]);
    Serial.print ("   ");
}
//========================

//=====DETERMINE RUNNING CASE=====
void SensorsCondition (){
    ReadSensors ();
    delay_ms(5);
    // STRAIGHT
     if ((sensor[0]==0 && sensor[1]==1 && sensor[2]==1 && sensor[3]==0 && sensor[4]==0)
    || (sensor[0]==1 && sensor[1]==1 && sensor[2]==0 && sensor[3]==0 && sensor[4]==0)
    || (sensor[0]==1 && sensor[1]==0 && sensor[2]==0 && sensor[3]==0 && sensor[4]==0)
    || (sensor[0]==0 && sensor[1]==0 && sensor[2]==0 && sensor[3]==1 && sensor[4]==0)
    || (sensor[0]==0 && sensor[1]==0 && sensor[2]==1 && sensor[3]==1 && sensor[4]==0)
    || (sensor[0]==0 && sensor[1]==0 && sensor[2]==0 && sensor[3]==1 && sensor[4]==1)
    || (sensor[0]==0 && sensor[1]==0 && sensor[2]==0 && sensor[3]==0 && sensor[4]==1)
    || (sensor[0]==0 && sensor[1]==1 && sensor[2]==0 && sensor[3]==0 && sensor[4]==0)
    ||(sensor[0]==0 && sensor[1]==0 && sensor[2]==1 && sensor[3]==0 && sensor[4]==0) 
    ) {    
      Case = 'S'; // Straight 00100
    }
  
   // FINISH
    else if(sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1){
      //if ( flag == 1)
      Case = 'F'; // Finish 11111
    }
    // TURN LEFT
    else if ((sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==0 && sensor[4]==0)
          || (sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==0)) {
      Case = 'L'; // Turn Left 11100
                  //           11110
    }

// TURN RIGHT
    else if ((sensor[0]==0 && sensor[1]==0 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1)
          || (sensor[0]==0 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1)) {
      Case = 'R'; // Turn Right 00111
                  //            01111
    }

// DEADEND
    else if (sensor[0]==0 && sensor[1]==0 && sensor[2]==0 && sensor[3]==0 && sensor[4]==0){
      Case = 'D'; // Dead End 00000
      //Situation = 0;
    }
}

//=====TURNING FUNCTIONS=====
void turnRight(){
  setPWM_leftmotor (turnSpeed);
  setPWM_rightmotor (turnSpeed);
  TurnRight();
  //PID_program();
  delay_ms((adjRight*90)+1);
  Stop();
  delay_ms(20);
}
void turnLeft () {
  setPWM_leftmotor (turnSpeed);
  setPWM_rightmotor (turnSpeed);
  TurnLeft();
  delay_ms((adj*90)+15);
  Stop();
  //delay_ms(20);
}
//========================

//=====CONTROL ROBOT ACCORDING TO OUTPUT FROM SENSORS=====
void RunCase (){
   SensorsCondition ();
   switch (Case){

      // GO STRAIGHT
      case 'S':
      //Situation =0;
        Forward ();
        if (PIDstatus ==1)
        PID_program();
        Serial.println ("Straight");
        break;
 
      case 'R':
        // DETERMINE IF IT'S RIGHT-ONLY OR STRAIGHT-OR-RIGHT
        PIDstatus =0;
        // Move 1 step forward
        //PORTB ^= 1 << PORTB5;
        setPWM_leftmotor (turnSpeed);
        setPWM_rightmotor (turnSpeed);
        Forward();
        delay_ms(30);
        Stop();
        delay_ms(20);
        ReadSensors();
        
        // TURN RIGHT
        if (sensor[2]==0) {
          turnRight();
          // Add to option array
          option[index] = 'R';
          index ++;
          Serial.println ("Turn Right");
        }
        
        // GO STRAIGHT
        else {
          setPWM_leftmotor (190);
          setPWM_rightmotor (190);
          Forward ();
          delay_ms(20);
          
          option[index] = 'S';
          index ++;
          Serial.println("Straight00");
        }
        PIDstatus=1;
        break;
        
      case 'L': // Left 
        PIDstatus=0;
        setPWM_leftmotor (turnSpeed);
        setPWM_rightmotor (turnSpeed);
        Forward();
        delay_ms(30);
        Stop();
        delay_ms(20);
        turnLeft();
        PIDstatus=1;
        // Add to option array
        option[index] = 'L';
        index ++;
        Serial.println ("TurnLeft");
        break;

      case 'D': // Deadend 
        //PIDstatus=0;
       // if ( lacPhai ==0){
        setPWM_leftmotor(200);
        setPWM_rightmotor(200);
        TurnRight();
        delay_ms(110);
        ReadSensors();       
        if (sensor[0] == 1 || sensor [1] ==1 || sensor [2] ==1 || sensor[3] ==1|| sensor [4] ==1){
            break;
        }
        else {
          setPWM_leftmotor(200);
          setPWM_rightmotor(200);
          TurnLeft();
          delay_ms(210);
          ReadSensors();
          if (sensor[0] == 1 || sensor [1] ==1 || sensor [2] ==1 || sensor[3] ==1|| sensor [4] ==1) {
            break;
          }
          else {
            setPWM_leftmotor (180);
            setPWM_rightmotor (180);
            TurnLeft();
            delay_ms(600);
            //PIDstatus=1;
            Serial.println ("DeadEnd");
            // Add to option array
            option[index] = 'U';
            index ++;
            //Situation = 1;
            break;                    
          }
        }
        
        //Stop();
        //delay_ms(50);
      
      case 'F': // All black
        // DETERMINE IF FINISH OR CROSS INTERSECTION
        //PIDstatus=0;
        Stop();
        delay_ms(250);
        // Move 1 step forward
        PORTB ^= 1 << PORTB5;
        //goStraight();
        ReadSensors();
        
        // FINISH
        //if (sensor[0]==0 && sensor[1]==0 && sensor[2]==1 && sensor[3]==0 && sensor[4]==0
         if (sensor[0]==1 && sensor[1]==1 && sensor[2]==1 && sensor[3]==1 && sensor[4]==1) {
          Stop();
          delay_ms(1000);
          //finish = true;
          //flag = 0;
          Serial.println ("Finish");
        }
        
        // TURN LEFT
        else {
          turnLeft ();
          // Add to option array
          option[index] = 'L';
          index ++;
          Serial.println("Turn Left");
        }
        //PIDstatus=1;
        break;
      //case 'P':
        
      default:
        Stop ();
        Serial.println ("Errors");
        break;
    }
}
//=================================

//=====MAIN PROGRAM=====
void setup() {
//  DDRD |= (1<<5) | (1<<6);
  Serial.begin (115200);
  TCCR0A = 0b10100011;
  TCCR0B = 0b00000001;
  DDRD = 0b01100000;
  DDRB = 0b00001111;
  DDRC = 0b00000000;
  PORTD = 0b01100000;
  DDRD &= ~(1<<DDD2);
  DDRB |= 1<<DDB5;
  EICRA |= (1<<ISC00);
  EIMSK |= (1<<INT0);
  sei();
}

void loop() {
    //TestCodeMotor ();
    RunCase ();

    // Toggle straight forward path
    if (finish == true) {
    }
}
//=================================

//=====PID CONTROLLER=====
void PID_program()
{ 
    Error(er_pt);
    
    previousError = error; // save previous error for differential 
 
    totalError += error; // Accumulate error for integral
    
    power = (Kp*error) + (Kd*(error-previousError)) + (Ki*totalError);
    
    if( power>255 ) { power = 255; }
    if( power<-255 ) { power = -255; }
    
    if(power<0) // Turn left
    {
      setPWM_rightmotor(180);
      setPWM_leftmotor(185 - abs(int(power)));
    }
    
    else // Turn right
    {
      setPWM_rightmotor(180 - int(power));
      setPWM_leftmotor(185);
    }
    //delay_ms(10);
}

void Error(float *error) {
  for(int i=0; i<=4; i++) 
    {
      if(sensor[i]==1) {
        activeSensor+=1; 
        }
      totalSensor += sensor[i] * (i+1);
    }
      
    avgSensor = totalSensor/activeSensor;
    *error = (avgSensor - 3);
    activeSensor = 0; totalSensor = 0;
}
//=================================

//=====INTERRUPT - MOVE EXTRA INCH=====
ISR(INT0_vect){
  
  setPWM_leftmotor (turnSpeed-10);
  setPWM_rightmotor (turnSpeed-10);
  Forward();
  //for (int i=0;i <100;i++){
    //Backward();
    //PID_program();
  //}
   delay_ms(100);
   Stop();
   //delay_ms(50); 
}
//=================================
