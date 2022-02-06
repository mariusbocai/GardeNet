/*
This file is my attempt to build a test environment for this complex algorithm.
We'll see if this works out...
*/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define TimeToTimeout 10u

unsigned char tokenEn1, tokenEn2, tokenEn3, tokenIsMine1, tokenIsMine2, tokenIsMine3;
unsigned int now1, now2, now3, nextExpectedAct1, nextExpectedAct2, nextExpectedAct3;
unsigned int timeClock;
unsigned int iterations, Kill1, Kill2, Kill3;
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
 
 unsigned int millisec()
{
	return timeClock;
}

 void setup() {
  tokenEn1 = 0;
  tokenEn2 = 0;
  tokenEn3 = 0;
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
  timeClock = 0;
  nextExpectedAct1 = millisec() + TimeToTimeout;
}

void PassTheToken(unsigned char toNode)
{
	
	if (toNode == 1)
	{
		if((Kill1 != 0)&&(Kill1 <= timeClock))
		{
			printf("Did not pass the token to Node1\n");
		}
		else
		{
			tokenIsMine1 = 1;
		}
		//printf("Pass the token to Node1\n");
	}
	if (toNode == 2)
	{
		if((Kill2 != 0)&&(Kill2 <= timeClock))
		{
			printf("Did not pass the token to Node2\n");
		}
		else
		{
			tokenIsMine2 = 1;
		}
		//printf("Pass the token to Node2\n");
	}
	if (toNode == 3)
	{
		if((Kill3 != 0)&&(Kill3 <= timeClock))
		{
			printf("Did not pass the token to Node3\n");
		}
		else
		{
			tokenIsMine3 = 1;
		}
		//printf("Pass the token to Node3\n");
	}
}

void StateMachine1(void)
{
  switch(currentState1)
  {
    case Idle:
    {
      if(tokenEn1 == 1)
      {
        currentState1 = Startup;
      }
      break;
    }
    case Startup:
    {
	  printf("Node1 passing token to Node2\n");
      PassTheToken(2); /*Pass the token to Node2*/
      tokenIsMine1 = 0;
      currentState1 = Waiting;
      break;
    }
    case Waiting:
    {
      now1 = millisec();
      if (now1 > nextExpectedAct1)
      {
        currentState1 = Idle;
        tokenEn1 = 0;
        tokenIsMine1 = 0; //might not be needed...
        /*No reply received, a reset will be performed!!!!*/
		printf("Reset Node1\n");
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
      now1 = millisec();
      if(now1 > nextExpectedAct1)
      {
        nextExpectedAct1 += TimeToTimeout;
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
      if((tokenEn2 == 1) && (tokenIsMine2 == 1))
      {
        currentState2 = Waiting1;
        now2 = millisec();
		printf("Node2 passing token to Node3\n");
        PassTheToken(3); /*Pass the token to Node3*/
        tokenIsMine2 = 0;
        nextExpectedAct2 = now2 + TimeToTimeout; 
      }
      else
      {
        if(tokenEn2 == 0)
        {
          tokenIsMine2 = 0;   //just as a precaution...
        }
      }
      break;
    }
    case Idle:
    {
      now2 = millisec();
      //nextExpectedAct2 = now2 + TimeToTimeout; //set next timeout
      if(tokenEn2 == 1)
      {
        if (tokenIsMine2 == 1)
        {
          now2 = millisec();
		  printf("Node2 passing token to Node3\n");
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
            tokenEn2 = 0;
            nextExpectedAct2 = 0;
            currentState2 = Startup;
            printf("Reset Node2\n");
          }
        }

      }
      else
      {
        tokenIsMine2 = 0;
        nextExpectedAct2 = 0;
        currentState2 = Startup;
		printf("Node2 disabled\n");
        //stop the whole thing, token has been disabled
      }
      break;
    }
    case Waiting1:
    {
      now2 = millisec();
      if (now2 > nextExpectedAct2)
      {
        currentState2 = Startup;
        tokenIsMine2 = 0;
        tokenEn2 = 0;
        nextExpectedAct2 = 0;
        /*No reply received, a reset will be performed!!!!*/
        printf("Reset Node2\n");
      }
      else
      {
        if(tokenIsMine2 == 1)
        {
          printf("Node2 passing token to Node1\n");
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
      now2 = millisec();
      if(now2 > nextExpectedAct2)
      {
		nextExpectedAct2 += TimeToTimeout;
        currentState2 = Idle;
		//printf("Node2 going to Idle\n");
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
      if((tokenEn3 == 1) && (tokenIsMine3 == 1))
      {
        currentState3 = Wait;
        now3 = millisec();
		printf("Node3 passing token to Node2\n");
        PassTheToken(2); /*Pass the token to Node2*/
        tokenIsMine3 = 0;
        nextExpectedAct3 = now3 + TimeToTimeout; 
      }
      else
      {
        if(tokenEn3 == 0)
        {
          tokenIsMine3 = 0;   //just as a precaution...
        }
      }
      break;
    }
    case Idle:
    {
      if(tokenEn3 == 1)
      {
        now3 = millisec();
        if (tokenIsMine3 == 1)
        {
          nextExpectedAct3 = now3 + TimeToTimeout; //set next cycle end time
		  printf("Node3 passing token to Node2\n");
          PassTheToken(2); /*Pass the token to Node3*/
          tokenIsMine3 = 0;
          currentState3 = Wait;
        }
        else
        {
          if(now3 > nextExpectedAct3)
          {
            //did not receive the token from Node 1
            tokenIsMine3 = 0;
            tokenEn3 = 0;
            nextExpectedAct3 = 0;
            currentState3 = Startup;
            printf("Reset Node3\n");
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
      now3 = millisec();
      if(now3 > nextExpectedAct3)
      {
        currentState3 = Idle;
        nextExpectedAct3 += TimeToTimeout;  //set the timeout +TODO
      }
    }
  }
}

void callSms(void)
{
	StateMachine1();
	StateMachine2();
	StateMachine3();
	StateMachine2();
	StateMachine3();
	StateMachine2();
}

int main(int argc, char **argv)
{
	unsigned int iterations,i,j;
	
	for (j = 1; j < argc; j++) 
	{
		if (!strcmp(argv[j],"--KillNode1")) 
		{
			Kill1 = atoi(argv[++j]);
			printf("Killing Node1 at %i \n", Kill1);
		}
		if (!strcmp(argv[j],"--KillNode2")) 
		{
			Kill2 = atoi(argv[++j]);
			printf("Killing Node2 at %i \n", Kill2);
		}
		if (!strcmp(argv[j],"--KillNode3")) 
		{
			Kill3 = atoi(argv[++j]);
			printf("Killing Node3 at %i \n", Kill3);
		}
	}
	
	setup();
	callSms();
	tokenEn1 = 1;
	tokenEn2 = 1;
	tokenEn3 = 1;
	printf("Token is enabled\n");
    iterations = atoi(argv[1]);
	for(i = 0; i < iterations; i++)
	{
		timeClock++;
		callSms();
		printf("======  Time is %i \n", timeClock);
	}
	//scanf("%d", &iterations);
}