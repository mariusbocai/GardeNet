/*
This file is my attempt to build a test environment for this complex algorithm.
We'll see if this works out...
*/

unsigned char tokenEn, tokenIsMine1, tokenIsMine2, tokenIsMine3;
unsigned long now1, now2, now3, nextExpectedAct1, nextExpectedAct2, nextExpectedAct3, globalTime;
unsigned char currentState1, currentState2, currentState3;

 enum States{
  Startup = 0,
  Idle,
  Wait,
  Waiting,
  Waiting1,
  Act1,
  Waiting2,
  Act2,
  EndAll};
  
 void setup() {
  tokenEn = 0;
  tokenIsMine1 = 0;
  tokenIsMine2 = 0;
  tokenIsMine3 = 0;
  now1 = 0;
  now2 = 0;
  now3 = 0;
  nextExpectedAct1 = 0;
  nextExpectedAct2 = 0;
  nextExpectedAct3 = 0;
  currentState1 = Idle;
  currentState2 = Startup;
  currentState3 = Startup;
}

void PassTheToken(unsigned char toNode)
{
	if (toNode == 1)
	{
		tokenIsMine1 = 1;
		printf("Pass the token to Node1/n");
	}
	if (toNode == 2)
	{
		tokenIsMine2 = 1;
		printf("Pass the token to Node2/n");
	}
	if (toNode == 3)
	{
		tokenIsMine3 = 1;
		printf("Pass the token to Node3/n");
	}
}

void StateMachine1(void)
{
  switch(currentState1)
  {
    case Idle:
    {
      if(tokenEn == 1)
      {
        currentState1 = StartUp;
      }
      break;
    }
    case StartUp:
    {
      PassTheToken(2); /*Pass the token to Node2*/
      tokenIsMine1 = 0;
      currentState1 = Waiting;
      break;
    }
    case Waiting:
    {
      now1 = millis();
      if (now1 > nextExpectedAct1)
      {
        currentState1 = Idle;
        tokenEn = 0;
        tokenIsMine1 = 0; //might not be needed...
        /*No reply received, a reset will be performed!!!!*/
		printf("Reset Node1/n");
      }
      else
      {
        if(tokenIsMine1 == 1)
        {
          currentState1 = EndAll;
        }
      }
      break;
    }
    case EndAll:
    {
      /*Cycle finished succesfully, wait to cycle end and restart the process*/
      now1 = millis();
      if(now1 > nextExpectedAct1)
      {
        nextExpectedAct1 += 3600000;
        currentState1 = Idle;
      }
      break;
    }
  }
}

void StateMachine2(void)
{
  switch(currentState2)
  {
    case Startup:
    {
      if((tokenEn == 1) && (tokenIsMine2 == 1))
      {
        currentState2 = Waiting1;
        now2 = millis();
        PassTheToken(3); /*Pass the token to Node3*/
        tokenIsMine2 = 0;
        nextExpectedAct2 = now2 + 3600000; 
      }
      else
      {
        if(tokenEn == 0)
        {
          tokenIsMine2 = 0;   //just as a precaution...
        }
      }
      break;
    }
    case Idle:
    {
      now2 = millis();
      nextExpectedAct2 = now2 + 3600000; //set next timeout
      if(tokenEn == 1)
      {
        if (tokenIsMine2 == 1)
        {
          now2 = millis();
          PassTheToken(3); /*Pass the token to Node3*/
          tokenIsMine2 = 0;
          currentState2 = Waiting1;
        }
        else
        {
          if(now2 > nextExpectedAct2)
          {
            //did not receive the token from Node 1
            tokenIsMine2 = 0;
            tokenEn = 0;
            nextExpectedAct2 = 0;
            currentState2 = Startup;
            printf("Reset Node2/n");
          }
        }

      }
      else
      {
        tokenIsMine2 = 0;
        nextExpectedAct2 = 0;
        currentState2 = Startup;
        //stop the whole thing, token has been disabled
      }
      break;
    }
    case Waiting1:
    {
      now2 = millis();
      if (now2 > nextExpectedAct2)
      {
        currentState2 = Startup;
        tokenIsMine2 = 0;
        tokenEn = 0;
        nextExpectedAct2 = 0;
        /*No reply received, a reset will be performed!!!!*/
        printf("Reset Node2/n");
      }
      else
      {
        if(tokenIsMine2 == 1)
        {
          PassTheToken(1); /*Pass the token to Node1*/
          tokenIsMine2 = 0;
          currentState2 = EndAll;
        }
      }
      break;
    }
    case EndAll:
    {
      /*Cycle finished succesfully, wait to cycle end and restart the process*/
      now2 = millis();
      if(now2 > nextExpectedAct2)
      {
        currentState2 = Idle;
      }
      break;
    }
  }
}

void StateMachine3(void)
{
  switch(currentState3)
  {
    case Startup:
    {
      if((tokenEn == 1) && (tokenIsMine3 == 1))
      {
        currentState3 = Wait;
        now3 = millis();
        client.publish("TokenPass", "2"); /*Pass the token to Node2*/
        tokenIsMine3 = 0;
        nextExpectedAct3 = now3 + 3600000; 
      }
      else
      {
        if(tokenEn == 0)
        {
          tokenIsMine3 = 0;   //just as a precaution...
        }
      }
      break;
    }
    case Idle:
    {
      if(tokenEn == 1)
      {
        if (tokenIsMine3 == 1)
        {
          now3 = millis();
          nextExpectedAct3 = now3 + 3600000; //set next cycle end time
          client.publish("TokenPass", "2"); /*Pass the token to Node3*/
          tokenIsMine3 = 0;
          currentState3 = Wait;
        }
        else
        {
          if(now3 > nextExpectedAct3)
          {
            //did not receive the token from Node 1
            tokenIsMine3 = 0;
            tokenEn = 0;
            nextExpectedAct3 = 0;
            currentState3 = Startup;
            digitalWrite(RelayPin1,HIGH);
            delay(1000);    /*1 second reset should be enough*/
            digitalWrite(RelayPin1,LOW);
          }
        }

      }
      else
      {
        tokenIsMine3 = 0;
        nextExpectedAct3 = 0;
        currentState3 = Startup;
        //stop the whole thing, token has been disabled
      }
      break;
    }
    case Wait:
    {
      now3 = millis();
      if(now3 > nextExpectedAct3)
      {
        currentState3 = Idle;
        nextExpectedAct3 += 3600000;  //set the timeout
      }
    }
  }
}
