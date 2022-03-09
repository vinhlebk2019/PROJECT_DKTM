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
  #define MAX_DISTANCE_FRONT 50
  #define MIN_DISTANCE_FRONT 1
  #define MAX_DISTANCE_LATERAL 20
  #define MIN_DISTANCE_LATERAL 1
  #define MAX_SPEED 320
  #define MIN_SPEED 0
  

// declaration pins to be used in due arduino
    #define ENABLEPIN_RIGHT        3  // PWM pin
    #define ENABLEPIN_LEFT         5  // PWM pin
    #define OUTPUTPIN_RIGHT_1      9
    #define OUTPUTPIN_RIGHT_2      6
    #define OUTPUTPIN_LEFT_1      11
    #define OUTPUTPIN_LEFT_2      10
    #define TRIGPIN_HS         12
    #define ECHOPIN_HS         13
    #define TRIGPIN_FS          7  
    #define ECHOPIN_FS          8
    #define TRIGPIN_BS          2 
    #define ECHOPIN_BS          4 

       
// variables to be used
  float outputLeft,outputRight,distanceHS,distanceFS,distanceBS;

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
            pinMode(TRIGPIN_HS,OUTPUT);
            pinMode(ECHOPIN_HS,INPUT);
            pinMode(TRIGPIN_FS,OUTPUT);
            pinMode(ECHOPIN_FS,INPUT);
            pinMode(TRIGPIN_BS,OUTPUT);
            pinMode(ECHOPIN_BS,INPUT);
            // Serial.begin(9600);


//  create  two new FuzzyInput objects
    FuzzyInput* HSSensor = new FuzzyInput(1);
    FuzzyInput* FSSensor = new FuzzyInput(2);
    FuzzyInput* BSSensor = new FuzzyInput(3);
 
 //  create the FuzzySets that  copose the FuzzyInput objects 
    FuzzySet* smallHS = new FuzzySet(MIN_DISTANCE_FRONT,MIN_DISTANCE_FRONT,MAX_DISTANCE_FRONT/4,MAX_DISTANCE_FRONT/2);
    HSSensor->addFuzzySet(smallHS);// add the FuzzySet in the FuzzyInput object
    FuzzySet* smallFS = new FuzzySet(MIN_DISTANCE_LATERAL,MIN_DISTANCE_LATERAL,MAX_DISTANCE_LATERAL/4,MAX_DISTANCE_LATERAL/2);
    FSSensor->addFuzzySet(smallFS);
    FuzzySet* smallBS = new FuzzySet(MIN_DISTANCE_LATERAL,MIN_DISTANCE_LATERAL,MAX_DISTANCE_LATERAL/4,MAX_DISTANCE_LATERAL/2);
    BSSensor->addFuzzySet(smallBS);
    
    FuzzySet*  safeHS = new FuzzySet(MAX_DISTANCE_FRONT/4,MAX_DISTANCE_FRONT/2,MAX_DISTANCE_FRONT/2,MAX_DISTANCE_FRONT*3/4);
    HSSensor->addFuzzySet(safeHS);
    FuzzySet*  safeFS = new FuzzySet(MAX_DISTANCE_LATERAL/4,MAX_DISTANCE_LATERAL/2,MAX_DISTANCE_LATERAL/2,MAX_DISTANCE_LATERAL*3/4);
    FSSensor->addFuzzySet(safeFS);
    FuzzySet*  safeBS = new FuzzySet(MAX_DISTANCE_LATERAL/4,MAX_DISTANCE_LATERAL/2,MAX_DISTANCE_LATERAL/2,MAX_DISTANCE_LATERAL*3/4);
    BSSensor->addFuzzySet(safeBS);
    
    FuzzySet*  bigHS = new FuzzySet(MAX_DISTANCE_FRONT/2,MAX_DISTANCE_FRONT*3/4,MAX_DISTANCE_FRONT,MAX_DISTANCE_FRONT);
    HSSensor->addFuzzySet(bigHS);
    FuzzySet*  bigFS = new FuzzySet(MAX_DISTANCE_LATERAL/2,MAX_DISTANCE_LATERAL*3/4,MAX_DISTANCE_LATERAL,MAX_DISTANCE_LATERAL);
    FSSensor->addFuzzySet(bigFS);
    FuzzySet*  bigBS = new FuzzySet(MAX_DISTANCE_LATERAL/2,MAX_DISTANCE_LATERAL*3/4,MAX_DISTANCE_LATERAL,MAX_DISTANCE_LATERAL);
    BSSensor->addFuzzySet(bigBS);
    
  //  add the FuzzyInputs in the Fuzzy Object
    fuzzy->addFuzzyInput(HSSensor);
    fuzzy->addFuzzyInput(FSSensor);
    fuzzy->addFuzzyInput(BSSensor);
  
  // create two new FuzzyOutput objects
    FuzzyOutput* rightWheelSpeed = new FuzzyOutput(1);
    FuzzyOutput* leftWheelSpeed = new FuzzyOutput(2);

  //  create the FuzzySets that  copose the FuzzyOutput objects 
    FuzzySet* lowerRightSpeed = new FuzzySet(MIN_SPEED,MIN_SPEED,MAX_SPEED/4,MAX_SPEED/2);
    rightWheelSpeed->addFuzzySet(lowerRightSpeed);// add the FuzzySet in the FuzzyInput object
    FuzzySet* lowerLeftSpeed = new FuzzySet(MIN_SPEED,MIN_SPEED,MAX_SPEED/4,MAX_SPEED/2);
    leftWheelSpeed->addFuzzySet(lowerLeftSpeed);

    FuzzySet* averageRightSpeed = new FuzzySet(MAX_SPEED/4,MAX_SPEED/2,MAX_SPEED/2,MAX_SPEED*3/4);
    rightWheelSpeed->addFuzzySet(averageRightSpeed);
    FuzzySet* averageLeftSpeed = new FuzzySet(MAX_SPEED/4,MAX_SPEED/2,MAX_SPEED/2,MAX_SPEED*3/4);
    leftWheelSpeed->addFuzzySet(averageLeftSpeed);

    FuzzySet* fastRightSpeed = new FuzzySet(MAX_SPEED/2,MAX_SPEED*3/4,MAX_SPEED,MAX_SPEED);
    rightWheelSpeed->addFuzzySet(fastRightSpeed);
    FuzzySet* fastLeftSpeed = new FuzzySet(MAX_SPEED/2,MAX_SPEED*3/4,MAX_SPEED,MAX_SPEED);
    leftWheelSpeed->addFuzzySet(fastLeftSpeed);


  // add the FuzzyOutputs in the Fuzzy object
    fuzzy->addFuzzyOutput(rightWheelSpeed);
    fuzzy->addFuzzyOutput(leftWheelSpeed);


  // building the antecedents fuzzy rules
   FuzzyRuleAntecedent* smallHSAndSmallFS = new FuzzyRuleAntecedent();
      smallHSAndSmallFS->joinWithAND(smallHS,smallFS);
      
   FuzzyRuleAntecedent* smallHSAndSmallFSAndSmallBS = new FuzzyRuleAntecedent();
      smallHSAndSmallFSAndSmallBS->joinWithAND(smallBS,smallHSAndSmallFS);
   FuzzyRuleAntecedent* smallHSAndSmallFSAndSafeBS = new FuzzyRuleAntecedent();
      smallHSAndSmallFSAndSafeBS->joinWithAND(safeBS,smallHSAndSmallFS);
   FuzzyRuleAntecedent* smallHSAndSmallFSAndBigBS = new FuzzyRuleAntecedent();
      smallHSAndSmallFSAndBigBS->joinWithAND(bigBS,smallHSAndSmallFS);
      
   FuzzyRuleAntecedent* smallHSAndSafeFS = new FuzzyRuleAntecedent();
      smallHSAndSafeFS->joinWithAND(smallHS,safeFS);
      
   FuzzyRuleAntecedent* smallHSAndSafeFSAndSmallBS = new FuzzyRuleAntecedent();
      smallHSAndSafeFSAndSmallBS->joinWithAND(smallBS,smallHSAndSafeFS);
   FuzzyRuleAntecedent* smallHSAndSafeFSAndSafeBS = new FuzzyRuleAntecedent();
      smallHSAndSafeFSAndSafeBS->joinWithAND(safeBS,smallHSAndSafeFS);
   FuzzyRuleAntecedent* smallHSAndSafeFSAndBigBS = new FuzzyRuleAntecedent();
     smallHSAndSafeFSAndBigBS->joinWithAND(bigBS,smallHSAndSafeFS);
     
   FuzzyRuleAntecedent* smallHSAndBigFS = new FuzzyRuleAntecedent();
      smallHSAndBigFS->joinWithAND(smallHS,bigFS);
      
   FuzzyRuleAntecedent* smallHSAndBigFSAndSmallBS = new FuzzyRuleAntecedent();
      smallHSAndBigFSAndSmallBS->joinWithAND(smallBS,smallHSAndBigFS);
   FuzzyRuleAntecedent* smallHSAndBigFSAndSafeBS = new FuzzyRuleAntecedent();
      smallHSAndBigFSAndSafeBS->joinWithAND(safeBS,smallHSAndBigFS);
   FuzzyRuleAntecedent* smallHSAndBigFSAndBigBS = new FuzzyRuleAntecedent();
     smallHSAndBigFSAndBigBS->joinWithAND(bigBS,smallHSAndBigFS);

/////

FuzzyRuleAntecedent* safeHSAndSmallFS = new FuzzyRuleAntecedent();
     safeHSAndSmallFS->joinWithAND(safeHS,smallFS);
      
   FuzzyRuleAntecedent* safeHSAndSmallFSAndSmallBS = new FuzzyRuleAntecedent();
      safeHSAndSmallFSAndSmallBS->joinWithAND(smallBS,safeHSAndSmallFS);
   FuzzyRuleAntecedent* safeHSAndSmallFSAndSafeBS = new FuzzyRuleAntecedent();
     safeHSAndSmallFSAndSafeBS->joinWithAND(safeBS,safeHSAndSmallFS);
   FuzzyRuleAntecedent* safeHSAndSmallFSAndBigBS = new FuzzyRuleAntecedent();
      safeHSAndSmallFSAndBigBS->joinWithAND(bigBS,safeHSAndSmallFS);
      
   FuzzyRuleAntecedent* safeHSAndSafeFS = new FuzzyRuleAntecedent();
      safeHSAndSafeFS->joinWithAND(safeHS,safeFS);
      
   FuzzyRuleAntecedent* safeHSAndSafeFSAndSmallBS = new FuzzyRuleAntecedent();
      safeHSAndSafeFSAndSmallBS->joinWithAND(smallBS,safeHSAndSafeFS);
   FuzzyRuleAntecedent* safeHSAndSafeFSAndSafeBS = new FuzzyRuleAntecedent();
      safeHSAndSafeFSAndSafeBS->joinWithAND(safeBS,safeHSAndSafeFS);
   FuzzyRuleAntecedent* safeHSAndSafeFSAndBigBS = new FuzzyRuleAntecedent();
   safeHSAndSafeFSAndBigBS->joinWithAND(bigBS,safeHSAndSafeFS);

   FuzzyRuleAntecedent* safeHSAndBigFS = new FuzzyRuleAntecedent();
      safeHSAndBigFS->joinWithAND(safeHS,bigFS);
      
   FuzzyRuleAntecedent* safeHSAndBigFSAndSmallBS = new FuzzyRuleAntecedent();
      safeHSAndBigFSAndSmallBS->joinWithAND(smallBS,safeHSAndBigFS);
   FuzzyRuleAntecedent* safeHSAndBigFSAndSafeBS = new FuzzyRuleAntecedent();
      safeHSAndBigFSAndSafeBS->joinWithAND(safeBS,safeHSAndBigFS);
   FuzzyRuleAntecedent* safeHSAndBigFSAndBigBS = new FuzzyRuleAntecedent();
    safeHSAndBigFSAndBigBS->joinWithAND(bigBS,safeHSAndBigFS);

////
   
   FuzzyRuleAntecedent* bigHSAndSmallFS = new FuzzyRuleAntecedent();
    bigHSAndSmallFS->joinWithAND(bigHS,smallFS);
      
   FuzzyRuleAntecedent* bigHSAndSmallFSAndSmallBS = new FuzzyRuleAntecedent();
      bigHSAndSmallFSAndSmallBS->joinWithAND(smallBS,bigHSAndSmallFS);
   FuzzyRuleAntecedent* bigHSAndSmallFSAndSafeBS = new FuzzyRuleAntecedent();
     bigHSAndSmallFSAndSafeBS->joinWithAND(safeBS,bigHSAndSmallFS);
   FuzzyRuleAntecedent* bigHSAndSmallFSAndBigBS = new FuzzyRuleAntecedent();
      bigHSAndSmallFSAndBigBS->joinWithAND(bigBS,bigHSAndSmallFS);
      
   FuzzyRuleAntecedent*  bigHSAndSafeFS = new FuzzyRuleAntecedent();
      bigHSAndSafeFS->joinWithAND( bigHS,safeFS);
      
   FuzzyRuleAntecedent* bigHSAndSafeFSAndSmallBS = new FuzzyRuleAntecedent();
      bigHSAndSafeFSAndSmallBS->joinWithAND(smallBS, bigHSAndSafeFS);
   FuzzyRuleAntecedent* bigHSAndSafeFSAndSafeBS = new FuzzyRuleAntecedent();
     bigHSAndSafeFSAndSafeBS->joinWithAND(safeBS, bigHSAndSafeFS);
   FuzzyRuleAntecedent* bigHSAndSafeFSAndBigBS = new FuzzyRuleAntecedent();
     bigHSAndSafeFSAndBigBS->joinWithAND(bigBS, bigHSAndSafeFS);

   FuzzyRuleAntecedent* bigHSAndBigFS = new FuzzyRuleAntecedent();
      bigHSAndBigFS->joinWithAND(bigHS,bigFS);
      
   FuzzyRuleAntecedent* bigHSAndBigFSAndSmallBS = new FuzzyRuleAntecedent();
      bigHSAndBigFSAndSmallBS->joinWithAND(smallBS,bigHSAndBigFS);
   FuzzyRuleAntecedent* bigHSAndBigFSAndSafeBS = new FuzzyRuleAntecedent();
      bigHSAndBigFSAndSafeBS->joinWithAND(safeBS,bigHSAndBigFS);
   FuzzyRuleAntecedent* bigHSAndBigFSAndBigBS = new FuzzyRuleAntecedent();
    bigHSAndBigFSAndBigBS->joinWithAND(bigBS,bigHSAndBigFS);
     
    
   /* FuzzyRuleAntecedent* smallFrontDistanceAndSmallLateralDistance = new FuzzyRuleAntecedent();
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
      bigFrontDistanceAndBigLateralDistance->joinWithAND(bigFrontDistance,bigLateralDistance); */


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

 ////////////////////////////////////////////   
  // connection between antecedent and consequent rules
    FuzzyRule* fuzzyRule01 = new FuzzyRule(1,smallHSAndSmallFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule01);
    FuzzyRule* fuzzyRule02 = new FuzzyRule(2,smallHSAndSmallFSAndSmallBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule02);
    FuzzyRule* fuzzyRule03 = new FuzzyRule(3,smallHSAndSmallFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule03);
    FuzzyRule* fuzzyRule04 = new FuzzyRule(4,smallHSAndSmallFSAndSafeBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule04);
    FuzzyRule* fuzzyRule05 = new FuzzyRule(5,smallHSAndSmallFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule05);
    FuzzyRule* fuzzyRule06 = new FuzzyRule(6,smallHSAndSmallFSAndBigBS,lowerLeftSpeedWheel);  
      fuzzy->addFuzzyRule(fuzzyRule06);
    FuzzyRule* fuzzyRule07 = new FuzzyRule(7,smallHSAndSafeFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule07);
    FuzzyRule* fuzzyRule08 = new FuzzyRule(8,smallHSAndSafeFSAndSmallBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule08);
    FuzzyRule* fuzzyRule09 = new FuzzyRule(9,smallHSAndSafeFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule09);
    FuzzyRule* fuzzyRule10 = new FuzzyRule(10,smallHSAndSafeFSAndSafeBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule10);
    FuzzyRule* fuzzyRule11 = new FuzzyRule(11,smallHSAndSafeFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule11);
    FuzzyRule* fuzzyRule12 = new FuzzyRule(12,smallHSAndSafeFSAndBigBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule12);
    FuzzyRule* fuzzyRule13 = new FuzzyRule(13,smallHSAndBigFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule13);
    FuzzyRule* fuzzyRule14 = new FuzzyRule(14,smallHSAndBigFSAndSmallBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule14);
    FuzzyRule* fuzzyRule15 = new FuzzyRule(15,smallHSAndBigFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule15);
    FuzzyRule* fuzzyRule16 = new FuzzyRule(16,smallHSAndBigFSAndSafeBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule16);
    FuzzyRule* fuzzyRule17 = new FuzzyRule(17,smallHSAndBigFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule17);
    FuzzyRule* fuzzyRule18 = new FuzzyRule(18,smallHSAndBigFSAndBigBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule18);


    FuzzyRule* fuzzyRule19 = new FuzzyRule(19,safeHSAndSmallFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule19);
    FuzzyRule* fuzzyRule20 = new FuzzyRule(20,safeHSAndSmallFSAndSmallBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule20);
    FuzzyRule* fuzzyRule21 = new FuzzyRule(21,safeHSAndSmallFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule21);
    FuzzyRule* fuzzyRule22 = new FuzzyRule(22,safeHSAndSmallFSAndSafeBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule22);
    FuzzyRule* fuzzyRule23 = new FuzzyRule(23,safeHSAndSmallFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule23);
    FuzzyRule* fuzzyRule24 = new FuzzyRule(24,safeHSAndSmallFSAndBigBS,lowerLeftSpeedWheel);  
      fuzzy->addFuzzyRule(fuzzyRule24);
      
    FuzzyRule* fuzzyRule25 = new FuzzyRule(25,safeHSAndSafeFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule25);
    FuzzyRule* fuzzyRule26 = new FuzzyRule(26,safeHSAndSafeFSAndSmallBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule26);
    FuzzyRule* fuzzyRule27 = new FuzzyRule(27,safeHSAndSafeFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule27);
    FuzzyRule* fuzzyRule28 = new FuzzyRule(28,safeHSAndSafeFSAndSafeBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule28);
    FuzzyRule* fuzzyRule29 = new FuzzyRule(29,safeHSAndSafeFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule29);
    FuzzyRule* fuzzyRule30 = new FuzzyRule(30,safeHSAndSafeFSAndBigBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule30);  
    FuzzyRule* fuzzyRule31 = new FuzzyRule(31,safeHSAndBigFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule31);
    FuzzyRule* fuzzyRule32 = new FuzzyRule(32,safeHSAndBigFSAndSmallBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule32);
    FuzzyRule* fuzzyRule33 = new FuzzyRule(33,safeHSAndBigFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule33);
    FuzzyRule* fuzzyRule34 = new FuzzyRule(34,safeHSAndBigFSAndSafeBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule34);
    FuzzyRule* fuzzyRule35 = new FuzzyRule(35,safeHSAndBigFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule35);
    FuzzyRule* fuzzyRule36 = new FuzzyRule(36,safeHSAndBigFSAndBigBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule36); 



    FuzzyRule* fuzzyRule37 = new FuzzyRule(37,bigHSAndSmallFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule37);
    FuzzyRule* fuzzyRule38 = new FuzzyRule(38,bigHSAndSmallFSAndSmallBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule38);
    FuzzyRule* fuzzyRule39 = new FuzzyRule(39,bigHSAndSmallFSAndSafeBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule39);
    FuzzyRule* fuzzyRule40 = new FuzzyRule(40,bigHSAndSmallFSAndSafeBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule40);
    FuzzyRule* fuzzyRule41 = new FuzzyRule(41,bigHSAndSmallFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule41);
    FuzzyRule* fuzzyRule42 = new FuzzyRule(42,bigHSAndSmallFSAndBigBS,lowerLeftSpeedWheel);  
      fuzzy->addFuzzyRule(fuzzyRule42);
    FuzzyRule* fuzzyRule43 = new FuzzyRule(43,bigHSAndSafeFSAndSmallBS,lowerRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule43);
    FuzzyRule* fuzzyRule44 = new FuzzyRule(44,bigHSAndSafeFSAndSmallBS,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule44);
    FuzzyRule* fuzzyRule45 = new FuzzyRule(45,bigHSAndSafeFSAndSafeBS,lowerRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule45);
    FuzzyRule* fuzzyRule46 = new FuzzyRule(46,bigHSAndSafeFSAndSafeBS,lowerLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule46);
    FuzzyRule* fuzzyRule47 = new FuzzyRule(47,bigHSAndSafeFSAndBigBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule47);
    FuzzyRule* fuzzyRule48 = new FuzzyRule(48,bigHSAndSafeFSAndBigBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule48);
    FuzzyRule* fuzzyRule49 = new FuzzyRule(49,bigHSAndBigFSAndSmallBS,fastRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule49);
    FuzzyRule* fuzzyRule50 = new FuzzyRule(50,bigHSAndBigFSAndSmallBS,averageLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule50);
    FuzzyRule* fuzzyRule51 = new FuzzyRule(51,bigHSAndBigFSAndSafeBS,lowerRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule51);
    FuzzyRule* fuzzyRule52 = new FuzzyRule(52,bigHSAndBigFSAndSafeBS,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule52);
    FuzzyRule* fuzzyRule53 = new FuzzyRule(53,bigHSAndBigFSAndBigBS,averageRightSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule53);
    FuzzyRule* fuzzyRule54 = new FuzzyRule(54,bigHSAndBigFSAndBigBS,fastLeftSpeedWheel);
      fuzzy->addFuzzyRule(fuzzyRule54);      
    
}

void loop() {
  //  put your main code here, to run repeatedly:

    // set values ​​of control pins to allow the forward movement of the robot
          digitalWrite(OUTPUTPIN_RIGHT_1,LOW);
          digitalWrite(OUTPUTPIN_RIGHT_2,HIGH);
          digitalWrite(OUTPUTPIN_LEFT_1,LOW);
          digitalWrite(OUTPUTPIN_LEFT_2,HIGH);

        // calculating the distance from the front sensor
          distanceHS = calculateDistanceSensor(TRIGPIN_HS,ECHOPIN_HS);

        // calculating the distance from the right sensor
          distanceFS = calculateDistanceSensor(TRIGPIN_FS,ECHOPIN_FS);
           distanceBS = calculateDistanceSensor(TRIGPIN_BS,ECHOPIN_BS);


        // modifying the fuzzy inputs according to the calculated distances
          fuzzy->setInput(1,distanceHS);
          fuzzy->setInput(2,distanceFS);
          fuzzy ->setInput(3,distanceBS);

          fuzzy->fuzzify();


        // receives the output values ​​of the fuzzy system

        // if the lateral sensor is on the right side then the outputs are:
          outputRight = fuzzy->defuzzify(1);
          outputLeft = fuzzy->defuzzify(2);


        // if the lateral sensor is on the left side then the outputs are:
          // outputRight = fuzzy->defuzzify(2);
          // outputLeft = fuzzy->defuzzify(1);

          analogWrite(ENABLEPIN_RIGHT,int(outputRight));
          analogWrite(ENABLEPIN_LEFT,int(outputLeft));

        // Serial.print(distanceFront);
        // Serial.print("|");
        // Serial.print(distanceLateral);
        // Serial.print("|");
        // Serial.print(int(outputRight));
        // Serial.print("|");
        // Serial.println(int(outputLeft));
        
          delay(200);
        

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
