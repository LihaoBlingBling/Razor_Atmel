/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app.c file template 

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:


Protected System functions:
void UserApp1Initialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserApp1RunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentDataTimeStamp;                       /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;        /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */



/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static u8 u8Role = 0x02;
static bool bSeek = TRUE;
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static AntAssignChannelInfoType sAntSetupData;

/**********************************************************************************************************************
Function Definitions
**********************************************************************************************************************/

/*--------------------------------------------------------------------------------------------------------------------*/
/* Public functions                                                                                                   */
/*--------------------------------------------------------------------------------------------------------------------*/


/*--------------------------------------------------------------------------------------------------------------------*/
/* Protected functions                                                                                                */
/*--------------------------------------------------------------------------------------------------------------------*/

/*--------------------------------------------------------------------------------------------------------------------
Function: UserApp1Initialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserApp1Initialize(void)
{
  u8 au8GameName[] = "Hide and Go Seek!";
  u8 au8HintMessage[] = "Press B0 to Start"; 
  /* Clear screen and place start messages */

  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8GameName); 
  LCDMessage(LINE2_START_ADDR, au8HintMessage); 

 /* Configure ANT for this application */
  sAntSetupData.AntChannel          = ANT_CHANNEL_USERAPP;
  sAntSetupData.AntChannelType      = ANT_CHANNEL_TYPE2_USERAPP;
  sAntSetupData.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  sAntSetupData.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
  
  sAntSetupData.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  sAntSetupData.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  sAntSetupData.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  sAntSetupData.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  sAntSetupData.AntFrequency        = ANT_FREQUENCY_USERAPP;
  sAntSetupData.AntTxPower          = ANT_TX_POWER_USERAPP;

  sAntSetupData.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    sAntSetupData.AntNetworkKey[i] = ANT_DEFAULT_NETWORK_KEY;
  }
    
  /* If good initialization, set state to Idle */
  if( AntAssignChannel(&sAntSetupData) )
  {
    /* Channel is configured, so change LED to yellow */  
    UserApp1_StateMachine = UserApp1SM_WaitChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_Error;
  }

}/* end UserApp1Initialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserApp1RunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserApp1RunActiveState(void)
{
  UserApp1_StateMachine();

} /* end UserApp1RunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for the ANT channel assignment to finish */
static void UserApp1SM_WaitChannelAssign(void)
{
  /* Check if the channel assignment is complete */
  if(AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  
  /* Monitor for timeout */
  if( IsTimeUp(&UserApp1_u32Timeout, 3000) )
  {
    DebugPrintf("\n\r***Channel assignment timeout***\n\n\r");
    UserApp1_StateMachine = UserApp1SM_Error;
  }
      
} /* end UserApp1SM_WaitChannelAssign() */


  /*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserApp1SM_Idle(void)
{
  if(WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    LCDCommand(LCD_CLEAR_CMD); 
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP); 
    UserApp1_StateMachine = UserApp1SM_WaitOpen;
  }   
} /* end UserApp1SM_Idle() */


static void UserApp1SM_RoleChange(void)
{
  if (u8Role == 0x01)
  {
    sAntSetupData.AntChannelType = ANT_CHANNEL_TYPE1_USERAPP;
    if( AntAssignChannel(&sAntSetupData))
    {
      UserApp1_StateMachine = UserApp1SM_WaitOpen;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Error;
    }
  }
  if (u8Role == 0x02)
  {
    sAntSetupData.AntChannelType = ANT_CHANNEL_TYPE2_USERAPP;
    if( AntAssignChannel(&sAntSetupData))
    {
      UserApp1_StateMachine = UserApp1SM_WaitOpen;
    }
    else
    {
      UserApp1_StateMachine = UserApp1SM_Error;
    }
  }
}


static void UserApp1SM_WaitOpen(void)
{
 if (AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED) 
 {
   AntOpenChannelNumber(ANT_CHANNEL_USERAPP); 
   UserApp1_StateMachine = UserApp1SM_CountDown;
 }
}


static void UserApp1SM_CountDown(void)
{
  u8 au8Role1Message[] = "       Seeker     ";
  u8 au8Role2Message[] = "       Hider     ";
  static u8 au8TimeCounter[3] = {0};
  static u32 u32TimeCount = 0;
  u8 au8CountDownMessage[] = "CountDown";
  u8 au8Message1[] = "   Ready or not ?";
  u8 au8Message2[] = "   Here I come !";
  
  
  if (u32TimeCount == 0)
  {
    au8TimeCounter[0] = '0';
    LCDCommand(LCD_CLEAR_CMD);
    if(u8Role==1)
    {
      LCDMessage(LINE1_START_ADDR , au8Role1Message);
    }
    if(u8Role==2)
    {
      LCDMessage(LINE1_START_ADDR , au8Role2Message);
    }
    LCDMessage(LINE2_START_ADDR,au8CountDownMessage); 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 1000)
  {
    au8TimeCounter[0] = '1'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 2000)
  {
    au8TimeCounter[0] = '2'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 3000)
  {
    au8TimeCounter[0] = '3'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 4000)
  {
    au8TimeCounter[0] = '4'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 5000)
  {
    au8TimeCounter[0] = '5'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 6000)
  {
    au8TimeCounter[0] = '6'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 7000)
  {
    au8TimeCounter[0] = '7'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 8000)
  {
    au8TimeCounter[0] = '8'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 9000)
  {
    au8TimeCounter[0] = '9'; 
    LCDMessage(LINE2_START_ADDR + 10, au8TimeCounter); 
  }
  if (u32TimeCount == 10000)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Message1);
    LCDMessage(LINE2_START_ADDR, au8Message2);
  }  
  u32TimeCount++;
  if (u32TimeCount == 11000)
  {
    u32TimeCount = 0;
    if(u8Role==0x01)
    {
      UserApp1_StateMachine = UserApp1SM_Seek;
    }
    if(u8Role==0x02)
    {
      UserApp1_StateMachine = UserApp1SM_Hide;
    }
    
  }
}


static void UserApp1SM_Seek(void)
{
  static bool bBuzzer = TRUE;
  static bool bLedOn = FALSE;
  u8 au8FoundMessage[] = {1,1,1,1,1,1,1,1};
  static s8 s8Rssi = 0;
    
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      s8Rssi = G_sAntApiCurrentMessageExtData.s8RSSI;
      if (bSeek == TRUE && bLedOn == TRUE)
      {
        if (s8Rssi && s8Rssi <= -96)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,100);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOff(PURPLE);    
          LedOff(BLUE);    
          LedOff(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);       
        }
        if (-96 < s8Rssi && s8Rssi <= -80)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,300);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOff(BLUE);    
          LedOff(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);      
        }
        if (-80 < s8Rssi && s8Rssi <= -74)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,500);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOff(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);        
        }
        if (-74 < s8Rssi && s8Rssi <= -68)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,700);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOff(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);      
        }
        if (-68 < s8Rssi && s8Rssi <= -62)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,900);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOff(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED);       
        }
        if (-62 < s8Rssi && s8Rssi <= -56)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,1100);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOn(YELLOW);    
          LedOff(ORANGE);    
          LedOff(RED); 
        }
         if (-56 < s8Rssi && s8Rssi <= -50)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,1400);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOn(YELLOW);    
          LedOn(ORANGE);    
          LedOff(RED); 
        }
         if (-50 < s8Rssi && s8Rssi <= -44)
        {
          if (bBuzzer == TRUE)
          {
            PWMAudioSetFrequency(BUZZER1,1700);
            PWMAudioOn(BUZZER1);
          }
          LedOn(WHITE);
          LedOn(PURPLE);    
          LedOn(BLUE);    
          LedOn(CYAN);    
          LedOn(GREEN);    
          LedOn(YELLOW);    
          LedOn(ORANGE);    
          LedOn(RED); 
          s8Rssi = 0;
          AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8FoundMessage);
          UserApp1_StateMachine = UserApp1SM_Found;
        }
      }
    }
  }
  if (bBuzzer == TRUE)
  {
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      PWMAudioOff(BUZZER1);
      bBuzzer = FALSE;
    }
  }
  if (bBuzzer == FALSE)
  {
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      bBuzzer = TRUE;
      PWMAudioOn(BUZZER1);
    }
  }
 
  
}


static void UserApp1SM_Found(void)
{
  PWMAudioOff(BUZZER1);
  u8 au8Found[]= "Found you !";

  AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR +5, au8Found);
  LedOff(WHITE);
  LedOff(PURPLE);    
  LedOff(BLUE);    
  LedOff(CYAN);
  LedBlink(GREEN,LED_2HZ);    
  LedOff(YELLOW);    
  LedOff(ORANGE);    
  LedOff(RED); 
  u8Role = 0x02; 
  UserApp1_StateMachine = UserApp1SM_CloseWait;
  
}

static void UserApp1SM_Hide(void)
{
  PWMAudioOff(BUZZER1);
  static u8 u8Check = 0;
  static bool bShow = TRUE;
  u8 au8TestMessage[] = {0,0,0,0,0,0,0,0};
  u8 au8Role2Message[] = "       Hide     ";
  u8 au8FoundData [] = {2,2,2,2,2,2,2,2};
  u8 au8FoundMessage [] ="   You Found Me   ";
  
 
  if (bShow == TRUE)
  {
    LedOff(GREEN);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Role2Message);
    bShow = FALSE;
  }
  if(AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      for(u8 i=0;i<ANT_DATA_BYTES;i++)
      {
        if(G_au8AntApiCurrentMessageBytes[i] == au8FoundData[i])
        {
          u8Check++;
        }
      }
      if (u8Check == 8)
      {
        AntCloseChannelNumber(ANT_CHANNEL_USERAPP);
        bShow = TRUE;
        bSeek = TRUE;
        LCDCommand(LCD_CLEAR_CMD);
        LCDMessage(LINE1_START_ADDR, au8FoundMessage);
        UserApp1_StateMachine = UserApp1SM_CloseWait;
        u8Role = 0x01;
      }
      u8Check = 0;
    }
       if(G_eAntApiCurrentMessageClass == ANT_TICK)
    {
       AntQueueBroadcastMessage(ANT_CHANNEL_USERAPP, au8TestMessage);
    }
  }
  
}


static void UserApp1SM_CloseWait(void)
{
  static u32 u32TimeCounter = 0;
    
  if (AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CLOSED)
  {
    u32TimeCounter++;
  }
  if (u32TimeCounter >= 5000)
  {
    u32TimeCounter = 0;
    AntUnassignChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_RoleChange; 
  }
}




     




/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{

} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
