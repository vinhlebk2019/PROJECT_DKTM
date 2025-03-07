#include <Fuzzy.h>
#include <FuzzyComposition.h>
#include <FuzzyInput.h>
#include <FuzzyIO.h>
#include <FuzzyOutput.h>
#include <FuzzyRule.h>
#include <FuzzyRuleAntecedent.h>
#include <FuzzyRuleConsequent.h>
#include <FuzzySet.h>

// setting the maximum and minimum values
  #define MAX_DISTANCE_FRONT 100
  #define MIN_DISTANCE_FRONT 20
  #define MAX_DISTANCE_LATERAL 50
  #define MIN_DISTANCE_LATERAL 10
  #define MAX_SPEED 100
  #define MIN_SPEED 0

// declaration pins to be used in due arduino
    #define ENABLEPIN_RIGHT       5  // PWM pin
    #define ENABLEPIN_LEFT        6  // PWM pin
    #define OUTPUTPIN_RIGHT_1   0
    #define OUTPUTPIN_RIGHT_2   1
    #define OUTPUTPIN_LEFT_1    2
    #define OUTPUTPIN_LEFT_2    3
    #define TRIGPIN_FRONT       12
    #define ECHOPIN_FRONT       13
    #define TRIGPIN_LATERAL      7
    #define ECHOPIN_LATERAL      8


// variables to be used
  float outputLeft,outputRight,distanceFront,distanceLateral;

// function to calculate the distance from the wall until the sensor in cm
  float calculateDistanceSensor(int trigPin, int echoPin);

//  create a new fuzzy object
  Fuzzy* fuzzy  = new Fuzzy();

void setup() {


     // the operation mode setting pins
            pinMode(ENABLEPIN_RIGHT,OUTPUT);  
            pinMode(ENABLEPIN_LEFT,OUTPUT);  
            pinMode(OUTPUTPIN_RIGHT_1,OUTPUT);
            pinMode(OUTPUTPIN_RIGHT_2,OUTPUT);
            pinMode(OUTPUTPIN_LEFT_1,OUTPUT);
            pinMode(OUTPUTPIN_LEFT_2,OUTPUT);
            pinMode(TRIGPIN_FRONT,OUTPUT);
            pinMode(ECHOPIN_FRONT,INPUT);
            pinMode(TRIGPIN_LATERAL,OUTPUT);
            pinMode(ECHOPIN_LATERAL,INPUT);
//            Serial.begin(9600);


//  create  two new FuzzyInput objects
    FuzzyInput* frontDistanceSensor = new FuzzyInput(1);
    FuzzyInput* lateralDistanceSensor = new FuzzyInput(2);
 
 //  create the FuzzySets that  copose the FuzzyInput objects 
    FuzzySet* smallFrontDistance = new FuzzySet(20,20,40,60);
    frontDistanceSensor->addFuzzySet(smallFrontDistance);// add the FuzzySet in the FuzzyInput object
    FuzzySet* smallLateralDistance = new FuzzySet(10,10,20,30);
    lateralDistanceSensor->addFuzzySet(smallLateralDistance);
    
    FuzzySet*  safeFrontDistance = new FuzzySet(40,60,60,80);
    frontDistanceSensor->addFuzzySet(safeFrontDistance);
    FuzzySet*  safeLateralDistance = new FuzzySet(20,30,30,40);
    lateralDistanceSensor->addFuzzySet(safeLateralDistance);
    
    FuzzySet*  bigFrontDistance = new FuzzySet(60,80,100,100);
    frontDistanceSensor->addFuzzySet(bigFrontDistance);
    FuzzySet*  bigLateralDistance = new FuzzySet(30,40,50,50);
    lateralDistanceSensor->addFuzzySet(bigLateralDistance);

  //  add the FuzzyInputs in the Fuzzy Object
    fuzzy->addFuzzyInput(lateralDistanceSensor);
    fuzzy->addFuzzyInput(frontDistanceSensor);
  
  // create two new FuzzyOutput objects
    FuzzyOutput* rightWheelSpeed = new FuzzyOutput(1);
    FuzzyOutput* leftWheelSpeed = new FuzzyOutput(2);

  //  create the FuzzySets that  copose the FuzzyOutput objects 
    FuzzySet* lowerRightSpeed = new FuzzySet(MIN_SPEED,MIN_SPEED,MAX_SPEED/4,MAX_SPEED/2);
    rightWheelSpeed->addFuzzySet(lowerRightSpeed);// add the FuzzySet in the FuzzyInput object
    FuzzySet* lowerLeftSpeed = new FuzzySet(MIN_SPEED,MIN_SPEED,MAX_SPEED/4,MAX_SPEED/2);
    leftWheelSpeed->addFuzzySet(lowerLeftSpeed);

    FuzzySet* averageRightSpeed = new FuzzySet(MAX_SPEED/4,50,50,MAX_SPEED*3/4);
    rightWheelSpeed->addFuzzySet(averageRightSpeed);
    FuzzySet* averageLeftSpeed = new FuzzySet(MAX_SPEED/4,50,50,MAX_SPEED*3/4);
    leftWheelSpeed->addFuzzySet(averageLeftSpeed);

    FuzzySet* fastRightSpeed = new FuzzySet(MAX_SPEED/2,MAX_SPEED*3/4,MAX_SPEED,MAX_SPEED);
    rightWheelSpeed->addFuzzySet(fastRightSpeed);
    FuzzySet* fastLeftSpeed = new FuzzySet(MAX_SPEED/2,MAX_SPEED*3/4,MAX_SPEED,MAX_SPEED);
    leftWheelSpeed->addFuzzySet(fastLeftSpeed);


  // add the FuzzyOutputs in the Fuzzy object
    fuzzy->addFuzzyOutput(rightWheelSpeed);
    fuzzy->addFuzzyOutput(leftWheelSpeed);


  // building the antecedents fuzzy rules
    FuzzyRuleAntecedent* smallFrontDistanceAndSmallLateralDistance = new FuzzyRuleAntecedent();
      smallFrontDistanceAndSmallLateralDistance->joinWithAND(smallFrontDistance,smallLateralDistance); 
    FuzzyRuleAntecedent* smallFrontDistanceAndSafeLateralDistance = new FuzzyRuleAntecedent();
      smallFrontDistanceAndSafeLateralDistance->joinWithAND(smallFrontDistance,safeLateralDistance);
    FuzzyRuleAntecedent* smallFrontDistanceAndBigLateralDistance = new FuzzyRuleAntecedent();
      smallFrontDistanceAndBigLateralDistance->joinWithAND(smallFrontDistance,bigLateralDistance);
    FuzzyRuleAntecedent* safeFrontDistanceAndSmallLateralDistance = new FuzzyRuleAntecedent();
      safeFrontDistanceAndSmallLateralDistance->joinWithAND(safeFrontDistance,smallLateralDistance);
    FuzzyRuleAntecedent* safeFrontDistanceAndSafeLateralDistance = new FuzzyRuleAntecedent();
      safeFrontDistanceAndSafeLateralDistance->joinWithAND(safeFrontDistance,safeLateralDistance);
    FuzzyRuleAntecedent* safeFrontDistanceAndBigLateralDistance = new FuzzyRuleAntecedent();
      safeFrontDistanceAndBigLateralDistance->joinWithAND(safeFrontDistance,bigLateralDistance);
    FuzzyRuleAntecedent* bigFrontDistanceAndSmallLateralDistance = new FuzzyRuleAntecedent();
      bigFrontDistanceAndSmallLateralDistance->joinWithAND(bigFrontDistance,smallLateralDistance);
    FuzzyRuleAntecedent* bigFrontDistanceAndSafeLateralDistance = new FuzzyRuleAntecedent();
      bigFrontDistanceAndSafeLateralDistance->joinWithAND(bigFrontDistance,safeLateralDistance);
    FuzzyRuleAntecedent* bigFrontDistanceAndBigLateralDistance = new FuzzyRuleAntecedent();
      bigFrontDistanceAndBigLateralDistance->joinWithAND(bigFrontDistance,bigLateralDistance);


  // building the consequent fuzzy rules
    FuzzyRuleConsequent* fastRightSpeedWheel = new FuzzyRuleConsequent();
      fastRightSpeedWheel->addOutput(fastRightSpeed);
    FuzzyRuleConsequent* averageRightSpeedWheel = new FuzzyRuleConsequent();
      averageRightSpeedWheel->addOutput(averageRightSpeed);
    FuzzyRuleConsequent* lowerRightSpeedWheel = new FuzzyRuleConsequent();
      lowerRightSpeedWheel->addOutput(lowerRightSpeed);
    FuzzyRuleConsequent* fastLeftSpeedWheel = new FuzzyRuleConsequent();
      fastLeftSpeedWheel->addOutput(fastLeftSpeed);
    FuzzyRuleConsequent* averageLeftSpeedWheel = new FuzzyRuleConsequent();
      averageLeftSpeedWheel->addOutput(averageLeftSpeed);
    FuzzyRuleConsequent* lowerLeftSpeedWheel = new FuzzyRuleConsequent();
      lowerLeftSpeedWheel->addOutput(lowerLeftSpeed);

    
  // connection between antecedent and consequent rules
    FuzzyRule* fuzzyRule01 = new FuzzyRule(1,smallFrontDistanceAndSmallLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule01);
    FuzzyRule* fuzzyRule02 = new FuzzyRule(2,smallFrontDistanceAndSmallLateralDistance,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule02);
    FuzzyRule* fuzzyRule03 = new FuzzyRule(3,smallFrontDistanceAndSafeLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule03);
    FuzzyRule* fuzzyRule04 = new FuzzyRule(4,smallFrontDistanceAndSafeLateralDistance,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule04);
    FuzzyRule* fuzzyRule05 = new FuzzyRule(5,smallFrontDistanceAndBigLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule05);
    FuzzyRule* fuzzyRule06 = new FuzzyRule(6,smallFrontDistanceAndBigLateralDistance,averageLeftSpeedWheel);  
      fuzzy->addFuzzyRule(fuzzyRule06);
    FuzzyRule* fuzzyRule07 = new FuzzyRule(7,safeFrontDistanceAndSmallLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule07);
    FuzzyRule* fuzzyRule08 = new FuzzyRule(8,safeFrontDistanceAndSmallLateralDistance,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule08);
    FuzzyRule* fuzzyRule09 = new FuzzyRule(9,safeFrontDistanceAndSafeLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule09);
    FuzzyRule* fuzzyRule10 = new FuzzyRule(10,safeFrontDistanceAndSafeLateralDistance,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule10);
    FuzzyRule* fuzzyRule11 = new FuzzyRule(11,safeFrontDistanceAndBigLateralDistance,averageRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule11);
    FuzzyRule* fuzzyRule12 = new FuzzyRule(12,safeFrontDistanceAndBigLateralDistance,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule12);
    FuzzyRule* fuzzyRule13 = new FuzzyRule(13,bigFrontDistanceAndSmallLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule13);
    FuzzyRule* fuzzyRule14 = new FuzzyRule(14,bigFrontDistanceAndSmallLateralDistance,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule14);
    FuzzyRule* fuzzyRule15 = new FuzzyRule(15,bigFrontDistanceAndSafeLateralDistance,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule15);
    FuzzyRule* fuzzyRule16 = new FuzzyRule(16,bigFrontDistanceAndSafeLateralDistance,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule16);
    FuzzyRule* fuzzyRule17 = new FuzzyRule(17,bigFrontDistanceAndBigLateralDistance,averageRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule17);
    FuzzyRule* fuzzyRule18 = new FuzzyRule(18,bigFrontDistanceAndBigLateralDistance,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule18); 




    
}

void loop() {
  //  put your main code here, to run repeatedly:

    // set values ​​of control pins to allow the forward movement of the robot
      digitalWrite(OUTPUTPIN_RIGHT_1,HIGH);
          digitalWrite(OUTPUTPIN_RIGHT_2,LOW);
          digitalWrite(OUTPUTPIN_LEFT_1,HIGH);
          digitalWrite(OUTPUTPIN_LEFT_2,LOW);

        // calculating the distance from the front sensor
          distanceFront = calculateDistanceSensor(TRIGPIN_FRONT,ECHOPIN_FRONT);

        // calculating the distance from the right sensor
          distanceLateral = calculateDistanceSensor(TRIGPIN_LATERAL,ECHOPIN_LATERAL);


        // modifying the fuzzy inputs according to the calculated distances
          fuzzy->setInput(1,distanceFront);
          fuzzy->setInput(2,distanceLateral);

          fuzzy->fuzzify();


        // receives the output values ​​of the fuzzy system

//         if the lateral sensor is on the right side then the outputs are:
//          outputRight = fuzzy->defuzzify(1);
//          outputLeft = fuzzy->defuzzify(2);


        // if the lateral sensor is on the left side then the outputs are:
           outputRight = fuzzy->defuzzify(2);
           outputLeft = fuzzy->defuzzify(1);

          analogWrite(ENABLEPIN_RIGHT,int(outputRight));
          analogWrite(ENABLEPIN_LEFT,int(outputLeft));

//       Serial.print(distanceFront);
//         Serial.print("|");
//         Serial.print(distanceLateral);
//        Serial.print("|");
//         Serial.print(int(outputRight));
//         Serial.print("|");
//         Serial.println(int(outputLeft));        
//          delay(200);    
}
float calculateDistanceSensor(int trigPin, int echoPin){
  digitalWrite(trigPin,LOW);
        delayMicroseconds(2);
        digitalWrite(trigPin,HIGH);
        delayMicroseconds(10);
        digitalWrite(trigPin,LOW);
  
        unsigned long duration = pulseIn(echoPin,HIGH);
        return duration/58;
}
