/*
This file is my attempt to build a test environment for this complex algorithm.
We'll see if this works out...
*/

/*Working variables*/
unsigned char tokenEn, tokenIsMine;
unsigned long now, nextExpectedAct;
static unsigned char currentState = 0;

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
  tokenIsMine = 0;
  now = 0;
  nextExpectedAct = 0;
  currentState = Idle;
}

void StateMachine1(void)
{
  switch(currentState)
  {
    case Idle:
    {
      if(tokenEn == 1)
      {
        currentState = StartUp;
      }
      break;
    }
    case StartUp:
    {
      client.publish("TokenPass", "2"); /*Pass the token to Node2*/
      tokenIsMine = 0;
      currentState = Waiting;
      break;
    }
    case Waiting:
    {
      now = millis();
      if (now > nextExpectedAct)
      {
        currentState = Idle;
        tokenEn = 0;
        tokenIsMine = 0; //might not be needed...
        /*No reply received, a reset will be performed!!!!*/
        digitalWrite(RelayPin1,HIGH);
        delay(1000);    /*1 second reset should be enough*/
        digitalWrite(RelayPin1,LOW);
      }
      else
      {
        if(tokenIsMine == 1)
        {
          currentState = EndAll;
        }
      }
      break;
    }
    case EndAll:
    {
      /*Cycle finished succesfully, wait to cycle end and restart the process*/
      now = millis();
      if(now > nextExpectedAct)
      {
        nextExpectedAct += 3600000;
        currentState = Idle;
      }
      break;
    }
  }
}

void StateMachine2(void)
{
  switch(currentState)
  {
    case Startup:
    {
      if((tokenEn == 1) && (tokenIsMine == 1))
      {
        currentState = Waiting1;
        now = millis();
        client.publish("TokenPass", "3"); /*Pass the token to Node3*/
        tokenIsMine = 0;
        nextExpectedAct = now + 3600000; 
      }
      else
      {
        if(tokenEn == 0)
        {
          tokenIsMine = 0;   //just as a precaution...
        }
      }
      break;
    }
    case Idle:
    {
      now = millis();
      nextExpectedAct = now + 3600000; //set next timeout
      if(tokenEn == 1)
      {
        if (tokenIsMine == 1)
        {
          now = millis();
          client.publish("TokenPass", "3"); /*Pass the token to Node3*/
          tokenIsMine = 0;
          currentState = Waiting1;
        }
        else
        {
          if(now > nextExpectedAct)
          {
            //did not receive the token from Node 1
            tokenIsMine = 0;
            tokenEn = 0;
            nextExpectedAct = 0;
            currentState = Startup;
            digitalWrite(RelayPin1,HIGH);
            delay(1000);    /*1 second reset should be enough*/
            digitalWrite(RelayPin1,LOW);
          }
        }

      }
      else
      {
        tokenIsMine = 0;
        nextExpectedAct = 0;
        currentState = Startup;
        //stop the whole thing, token has been disabled
      }
      break;
    }
    case Waiting1:
    {
      now = millis();
      if (now > nextExpectedAct)
      {
        currentState = Startup;
        tokenIsMine = 0;
        tokenEn = 0;
        nextExpectedAct = 0;
        /*No reply received, a reset will be performed!!!!*/
        digitalWrite(RelayPin1,HIGH);
        delay(1000);    /*1 second reset should be enough*/
        digitalWrite(RelayPin1,LOW);
      }
      else
      {
        if(tokenIsMine == 1)
        {
          client.publish("TokenPass", "1"); /*Pass the token to Node1*/
          tokenIsMine = 0;
          currentState = EndAll;
        }
      }
      break;
    }
    case EndAll:
    {
      /*Cycle finished succesfully, wait to cycle end and restart the process*/
      now = millis();
      if(now > nextExpectedAct)
      {
        currentState = Idle;
      }
      break;
    }
  }
}

void StateMachine(void)
{
  switch(currentState)
  {
    case Startup:
    {
      if((tokenEn == 1) && (tokenIsMine == 1))
      {
        currentState = Wait;
        now = millis();
        client.publish("TokenPass", "2"); /*Pass the token to Node2*/
        tokenIsMine = 0;
        nextExpectedAct = now + 3600000; 
      }
      else
      {
        if(tokenEn == 0)
        {
          tokenIsMine = 0;   //just as a precaution...
        }
      }
      break;
    }
    case Idle:
    {
      if(tokenEn == 1)
      {
        if (tokenIsMine == 1)
        {
          now = millis();
          nextExpectedAct = now + 3600000; //set next cycle end time
          now = millis();
          client.publish("TokenPass", "2"); /*Pass the token to Node3*/
          tokenIsMine = 0;
          currentState = Wait;
        }
        else
        {
          if(now > nextExpectedAct)
          {
            //did not receive the token from Node 1
            tokenIsMine = 0;
            tokenEn = 0;
            nextExpectedAct = 0;
            currentState = Startup;
            digitalWrite(RelayPin1,HIGH);
            delay(1000);    /*1 second reset should be enough*/
            digitalWrite(RelayPin1,LOW);
          }
        }

      }
      else
      {
        tokenIsMine = 0;
        nextExpectedAct = 0;
        currentState = Startup;
        //stop the whole thing, token has been disabled
      }
      break;
    }
    case Wait:
    {
      now = millis();
      if(now > nextExpectedAct)
      {
        currentState = Idle;
        nextExpectedAct += 3600000;  //set the timeout
      }
    }
  }
}
