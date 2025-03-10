//include vex.h and the header file you created
#include "vex.h"
#include "drive.h"
#include "odom.h"
#include "FrontVision.h"
#include <iostream>
#include "cmath"

//declare the namespace you created
using namespace S;
//also include namespace vex
using namespace vex;

double joystickCurveValue = 0;

void drive::setJoystickCurve(double curveValue){
    joystickCurveValue = curveValue;
}

//Function to stop the motors
void drive::stop(){
    RD1.stop(brake);
    RD2.stop(brake);
    RD3.stop(brake);
    RD4.stop(brake);
    LD1.stop(brake);
    LD2.stop(brake);
    LD3.stop(brake);
    LD4.stop(brake);

}


//Function to stop all the drive motors with a specified brake type
void drive::stop(brakeType type){
    RD1.stop(brake);
    RD2.stop(brake);
    RD3.stop(brake);
    RD4.stop(brake);
    LD1.stop(brake);
    LD2.stop(brake);
    LD3.stop(brake);
    LD4.stop(brake);
}

//Function to spin all the drive motors at the specified power percentage
void drive::spin(float pwr){
    RD1.spin(directionType::fwd,pwr,pct);
    RD2.spin(directionType::fwd,pwr,pct);
    RD3.spin(directionType::fwd,pwr,pct);
    RD4.spin(directionType::fwd,pwr,pct); 
    LD1.spin(directionType::fwd,pwr,pct);
    LD2.spin(directionType::fwd,pwr,pct);
    LD3.spin(directionType::fwd,pwr,pct);
    LD4.spin(directionType::fwd,pwr,pct); 
}

//Function to spin the left and right drive motors at the their specified power percentages
void drive::spin(float leftpower,float rightpower){
    RD1.spin(directionType::fwd,rightpower,pct);
    RD2.spin(directionType::fwd,rightpower,pct);
    RD3.spin(directionType::fwd,rightpower,pct);
    RD4.spin(directionType::fwd,rightpower,pct); 
    LD1.spin(directionType::fwd,leftpower,pct);
    LD2.spin(directionType::fwd,leftpower,pct);
    LD3.spin(directionType::fwd,leftpower,pct);
    LD4.spin(directionType::fwd,leftpower,pct); 
}

double drive::joystickCurve(double joystickValue){
    return exp(((abs(joystickValue) - 100) * joystickCurveValue) / 1000) * joystickValue;
}


//Function that uses a PID loop to turn the robot to a specified angle at a specified maximum power
void drive::turn(double angle, double maxPwr){

     //set and initalize variables
  float lastError = 0;
  float P = 0;
  float I = 0;
  float D = 0;

  timer PIDTimer = timer();
  double pwr = 0; 
  /**********adjust pI and dI to tune*********/
  float kP = 1;
  float kI = 0;
  float kD = 0;

  //set turn target
  int turnTarget = angle;

  while(true){

    //calculate the P
    P = turnTarget - finalAngle;

    //calculate the I
    I += P * 10;

    //Calculate the D
    D = (P - lastError)/10;
    lastError = P;
    //calculate drive power
    float total = P*kP + I*kI - D*kD;

    //setting power value
    if(fabs(total) > maxPwr){
      pwr = maxPwr;
    }else if(fabs(total) < 4){
      pwr = 4;
    }else{
      pwr = fabs(total);
    }
    //check if turning left
    if(P < 0){
      pwr = -1*pwr;
    }
    //set motors to spin
    spin(-pwr,pwr);  
    
    //check if we have reached our target
    if(fabs(P) > 0.5){
      PIDTimer.clear(); 
    }
    if(PIDTimer.time(msec) > 50)
    {
      break;
    }
    
    wait(10,msec);
  }

//stop the drive
stop(brake);

}

//Function to turn to face the coordinates specified at full speed
void drive::turnToPoint(double x, double y){

    //turn to the inverse tangent of the point specified
    float turnTarget = (atan2(y - finalPosY,x - finalPosY)*180/pi);

    //make any turn less than 180 degrees
    if(turnTarget - finalAngle >= 180)
    {
        turnTarget -= 360;
    }
    else if(turnTarget - finalAngle <= -180)
    {
        turnTarget += 360;
    } 

    //turn to the calculated angle
    turn(turnTarget,100);

    //stop the drive
    stop();
}

//Function to turn to face the coordinates specified with a specified maximum power
void drive::turnToPoint(double x, double y, double pwr){

    //turn to the inverse tangent of the point specified
    float turnTarget = (atan2(y - finalPosY,x - finalPosY)*180/pi);

    //make any turn less than 180 degrees
    if(turnTarget - finalAngle >= 180)
    {
        turnTarget -= 360;
    }
    else if(turnTarget - finalAngle <= -180)
    {
        turnTarget += 360;
    } 

    //turn to the calculated angle
    turn(turnTarget,pwr);

    //stop the drive
    stop();
}

//Function to have the robot move to the specified coordinates and end at the specified angle
void drive::moveToPoint(double x, double y, double angle){
    //
    if(fabs(finalAngle - angle) > 30){
        turnToPoint(x,y);
    }
    //**Implement Rest of move to point code here**

}

//Function to have the robot move to the specified coordinates with a specified maximum power and end at the specified angle
void drive::moveToPoint(double x, double y, double angle, double maxPwr){
    if(fabs(finalAngle - angle) > 30){
        turnToPoint(x,y,maxPwr);
    }
    //**Implement Rest of move to point code here**
}




//Task to run the drive and associated mechanisms
int Drive(){
    while(true){
        base.spin(base.joystickCurve(Controller1.Axis3.position()),base.joystickCurve(Controller1.Axis2.position()));
        wait(10,msec);
    }
}