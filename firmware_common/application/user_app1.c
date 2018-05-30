/**********************************************************************************************************************
File: user_app1.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app1 as a template:
 1. Copy both user_app1.c and user_app1.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app1" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp1" with "YourNewTaskName"
 6. Use ctrl-h to find and replace all instances of "USER_APP1" with "YOUR_NEW_TASK_NAME"
 7. Add a call to YourNewTaskNameInitialize() in the init section of main
 8. Add a call to YourNewTaskNameRunActiveState() in the Super Loop section of main
 9. Update yournewtaskname.h per the instructions at the top of yournewtaskname.h
10. Delete this text (between the dashed lines) and update the Description below to describe your task
----------------------------------------------------------------------------------------------------------------------

Description:
This is a user_app1.c file template 

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
All Global variable names shall start with "G_UserApp1"
***********************************************************************************************************************/
/* New variables */
volatile u32 G_u32UserApp1Flags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern u32 G_u32AntApiCurrentMessageTimeStamp;                    /* From ant_api.c */
extern AntApplicationMessageType G_eAntApiCurrentMessageClass;    /* From ant_api.c */
extern u8 G_au8AntApiCurrentMessageBytes[ANT_APPLICATION_MESSAGE_BYTES];  /* From ant_api.c */
extern AntExtendedDataType G_sAntApiCurrentMessageExtData;                /* From ant_api.c */

extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */
extern u8 G_au8DebugScanfBuffer[];                    /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                    /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;              /* The state machine function pointer */
static u32 UserApp1_u32Timeout;                        /* Timeout counter used across states */

static AntAssignChannelInfoType UserApp1_sChannelInfo; /* ANT setup parameters */

static u8 UserApp1_au8MessageFail[] = "\n\r***ANT channel setup failed***\n\n\r";
static u8 au8UserRole[] = "\n\r***Your Role***\n\rPress 1:Adult  Press 2:Athlete\n\rPress 3:Baby  Press 4:Older\n\rPress Enter:Ensure";
static u8 au8UserCondition[] = "\n\r***Your Condition***\n\rPress 1:Relax Press 2:Exercise\n\rPress Enter:Ensure\n\r";
static u8 au8UserHint[] = "\n\rPlease Choose your state in 3 Seconds";
static u8 au8UserLower[4][2] = {{60,120},{50,80},{110,135},{55,80}};
static u8 au8UserUpper[4][2] = {{100,150},{70,110},{150,160},{78,100}};
static u8 u8TestNumber[1] = {0}; /*Store the last heartbeat*/
static u8 u8Role = 0;
static u8 u8Condition = 0;
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
  static u8 au8Welcom[]="Welcom";
  static u8 au8NETWORK_KEY[8]={0xB9, 0xA5, 0x21, 0xFB, 0xBD, 0x72, 0xC3, 0x45};
  LCDCommand(LCD_CLEAR_CMD);
  LCDMessage(LINE1_START_ADDR, au8Welcom);
  
 /* Configure ANT for this application */
  UserApp1_sChannelInfo.AntChannel          = ANT_CHANNEL_USERAPP;
  UserApp1_sChannelInfo.AntChannelType      = ANT_CHANNEL_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodLo  = ANT_CHANNEL_PERIOD_LO_USERAPP;
  UserApp1_sChannelInfo.AntChannelPeriodHi  = ANT_CHANNEL_PERIOD_HI_USERAPP;
 
  UserApp1_sChannelInfo.AntDeviceIdLo       = ANT_DEVICEID_LO_USERAPP;
  UserApp1_sChannelInfo.AntDeviceIdHi       = ANT_DEVICEID_HI_USERAPP;
  UserApp1_sChannelInfo.AntDeviceType       = ANT_DEVICE_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntTransmissionType = ANT_TRANSMISSION_TYPE_USERAPP;
  UserApp1_sChannelInfo.AntFrequency        = ANT_FREQUENCY_USERAPP;
  UserApp1_sChannelInfo.AntTxPower          = ANT_TX_POWER_USERAPP;

  UserApp1_sChannelInfo.AntNetwork = ANT_NETWORK_DEFAULT;
  for(u8 i = 0; i < ANT_NETWORK_NUMBER_BYTES; i++)
  {
    UserApp1_sChannelInfo.AntNetworkKey[i] = au8NETWORK_KEY[i];
  }
  
  /* Attempt to queue the ANT channel setup */
  if( AntAssignChannel(&UserApp1_sChannelInfo) )
  {
    UserApp1_u32Timeout = G_u32SystemTime1ms;
    UserApp1_StateMachine = UserApp1SM_AntChannelAssign;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    DebugPrintf(UserApp1_au8MessageFail);
    UserApp1_StateMachine = UserApp1SM_Error;
  }

} /* end UserApp1Initialize() */

  
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
/* Wait for ANT channel assignment */
static void UserApp1SM_AntChannelAssign()
{
  if( AntRadioStatusChannel(ANT_CHANNEL_USERAPP) == ANT_CONFIGURED)
  {
    /* Channel assignment is successful, so open channel and
    proceed to Idle state */
    AntOpenChannelNumber(ANT_CHANNEL_USERAPP);
    UserApp1_StateMachine = UserApp1SM_Show;
    //UserApp1_StateMachine = UserApp1SM_UserChoose;
  }
  
  /* Watch for time out */
  if(IsTimeUp(&UserApp1_u32Timeout, 3000))
  {
    DebugPrintf(UserApp1_au8MessageFail);
    UserApp1_StateMachine = UserApp1SM_Error;    
  }
     
} /* end UserApp1SM_AntChannelAssign */

/*-------------------------------------------------------------------------------------------------------------------*/
//The User is Who?
static void UserApp1SM_UserChoose(void)
{
  static u32 u32TimeCounter1;
  static u8 u8Role1 = 0;
  static bool bCondition = FALSE;
  u8 u8Buffer;
  
  
  
   //Debug show   Role
  if(0 == u8Role1)
  {
    DebugLineFeed();
    DebugPrintf(au8UserHint);
    DebugPrintf(au8UserRole);
    u8Buffer = DebugScanf(G_au8DebugScanfBuffer);
    u8Role1 =1;
  }
  //Choose User's Role
  if(1 == u8Role1)
  {
    u32TimeCounter1++;
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x0D)
    {
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x31)
      {
        u32TimeCounter1 = 0;
        u8Role = 0;
        u8Role1 = 2;
      }
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x32)
      {
        u32TimeCounter1 = 0;
        u8Role = 1;
        u8Role1 = 2;
      }
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x33)
      {
        u32TimeCounter1 = 0;
        u8Role = 2;
        u8Role1 = 2;
      }
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x34)
      {
        u32TimeCounter1 = 0;
        u8Role = 3;
        u8Role1 = 2;
      }
    }
    //The User donot Choose his role
    if(u32TimeCounter1 >= 5100)
    {
      u32TimeCounter1 = 0;
      u8Role1 = 0;
      UserApp1_StateMachine = UserApp1SM_Show;
    }
  }
  
 //Choose User's Condition
  if(2 == u8Role1)
  {
    if(bCondition == FALSE)
    {
    DebugLineFeed();
    DebugPrintf(au8UserHint);
    DebugPrintf(au8UserCondition);
    u8Buffer = DebugScanf(G_au8DebugScanfBuffer);
    bCondition = TRUE;
    }
     //Choose User's Condition
    if(bCondition)
    {
      u32TimeCounter1++;
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x0D)
      {
        if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x31)
        {
          u32TimeCounter1 = 0;
          u8Condition = 0;
          u8Role1 = 0;
          UserApp1_StateMachine = UserApp1SM_Show;
        }
        if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x32)
        {
          u32TimeCounter1 = 0;
          u8Condition = 1;
          u8Role1 = 0;
          UserApp1_StateMachine = UserApp1SM_Show;
        }
      }
      //User donot Choose his condition
      if(u32TimeCounter1 >= 5100)
      {
        u32TimeCounter1 = 0;
        u8Role1 = 0;
        UserApp1_StateMachine = UserApp1SM_Show;
      } 
    }
  }   
}

/*show your heartbeat in LCD*/
static void UserApp1SM_Show(void)
{
  static u8 u8HR = 0;
  static u8 u8Difference;
  static bool bCompare = FALSE;
  static u32 u32TimeCounter;
  static bool bHealth = TRUE;
  
  //Real-Time HR Show in LCD
  if( AntReadAppMessageBuffer() )
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA)
    {
      LedOn(GREEN);
      u8HR = G_au8AntApiCurrentMessageBytes[7];
       ShowRate();
    }
  }
  
  /*Pulse frequency increases fast
    Slow pulse frequency     According to your Choice*/  
  if(u8HR<au8UserLower[u8Role][u8Condition] || u8HR>au8UserUpper[u8Role][u8Condition])
  {
    u32TimeCounter++;
  }
  if(u32TimeCounter > 2000)
  {
    bHealth = FALSE;
    Warning();
  }
  if(u8HR>au8UserLower[u8Role][u8Condition] && u8HR<au8UserUpper[u8Role][u8Condition])
  {
    u32TimeCounter=0;
    bHealth = TRUE;
  }
  
  /*if your heart is changing too fast, Warning
  Law is not the whole of arteries and veins ÂöÂÊ²»Õû*/
  if(u8TestNumber[0] != 0)
  {
    if(u8HR != u8TestNumber[0])
    {
      bCompare = TRUE;
    }
    if(u8HR == u8TestNumber[0])
    {
      bCompare = FALSE;
    }
    //Difference value
    if(bCompare)
    {
      if(u8HR > u8TestNumber[0])
      {
        u8Difference = u8HR - u8TestNumber[0];
      }
      if(u8HR < u8TestNumber[0])
      {
        u8Difference = u8TestNumber[0] - u8HR ;
      }
      u8TestNumber[0] = u8HR;
    }
  }
  
  if(u8TestNumber[0] == 0)
  {
    u8TestNumber[0] = u8HR;
  }
  if(u8Difference >= 20)
  {
    Warning();
  }
  if(u8Difference == 0)
  {
    //Warning();
  }
  if(u8Difference < 20 && u8Difference > 0 && bHealth == TRUE)
  {
    PWMAudioOff(BUZZER1);
    LedOff(RED);
  }
   
  //The Staus of Battery
  if (WasButtonPressed(BUTTON0))
  {
    ButtonAcknowledge(BUTTON0);
    UserApp1_StateMachine = UserApp1SM_Battery;
  }
  
  //Change User's Role & Condition
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    UserApp1_StateMachine = UserApp1SM_UserChoose;
  }
}

//Battery
static void UserApp1SM_Battery(void)
{
  static u8 u8Status = 0x00;
  u8 au8CommonMeaasge[] = {0x46,0xFF,0xFF,0xFF,0xFF,0x80,0x07,0x01};
  
  
  if( AntReadAppMessageBuffer())
  {
    if(G_eAntApiCurrentMessageClass == ANT_DATA )
    {
     if( (G_au8AntApiCurrentMessageBytes[0] == 0x07) )
     {
       u8Status = G_au8AntApiCurrentMessageBytes[3] & 0x70;
       ShowBattery(u8Status);
     }     
    }
    if (G_eAntApiCurrentMessageClass == ANT_TICK)
    {
      AntQueueAcknowledgedMessage(ANT_CHANNEL_USERAPP,au8CommonMeaasge);
    }
  }
  
  //Return Show HR
  if (WasButtonPressed(BUTTON1))
  {
    ButtonAcknowledge(BUTTON1);
    LCDCommand(LCD_CLEAR_CMD);
    UserApp1_StateMachine = UserApp1SM_Show;
  }
}

//Show Real-Time Rate
static void ShowRate(void)
{
  LCDCommand(LCD_CLEAR_CMD);
  u8 u8RateDec;
  u8 au8Show[3] = {0,0,0};
  static u8 au8Hint1[]="HR:";
  static u8 au8Hint2[]="B0:Battery";
  static u8 au8Hint3[] = "B1:Choose";
  
  u8RateDec = HexToDec(G_au8AntApiCurrentMessageBytes[7]);
  au8Show[0] = (u8RateDec/100) + '0';
  au8Show[1] = ( (u8RateDec%100)/10 ) + '0';
  au8Show[2] = ( (u8RateDec%100)%10 ) + '0'; 
  
  if (au8Show[0] == '0')
  {
    au8Show[0] = ' ';
  }
  
  LCDMessage(LINE1_START_ADDR , au8Hint1);
  LCDMessage(LINE1_START_ADDR + 5, au8Show);
  LCDMessage(LINE2_START_ADDR , au8Hint2);
  LCDMessage(LINE2_START_ADDR+11 , au8Hint3);
  return;
}


static void Warning (void)
{
  LedOn(RED );
  PWMAudioSetFrequency(BUZZER1, 2000);
  PWMAudioOn(BUZZER1);
  return;
  
}

void ShowBattery(u8 u8Status)
{
  static u8 au8Return [] = "B1:Return";
  static u8 au8Battery[] = "Battery:";
  static u8 au8Status1[] = "New";
  static u8 au8Status2[] = "Good";
  static u8 au8Status3[] = "OK";
  static u8 au8Status4[] = "Low";
  static u8 au8Status5[] = "Critical";
  
  if (u8Status == 0x10)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Battery);
    LCDMessage(LINE1_START_ADDR + 8, au8Status1);
    LCDMessage(LINE2_START_ADDR, au8Return);
  }
  if (u8Status == 0x20)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Battery);
    LCDMessage(LINE1_START_ADDR + 8, au8Status2);
    LCDMessage(LINE2_START_ADDR, au8Return);
  }
  if (u8Status == 0x30)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Battery);
    LCDMessage(LINE1_START_ADDR + 8, au8Status3);
    LCDMessage(LINE2_START_ADDR, au8Return);
  }
  if (u8Status == 0x40)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Battery);
    LCDMessage(LINE1_START_ADDR + 8, au8Status4);
    LCDMessage(LINE2_START_ADDR, au8Return);
  }
  if (u8Status == 0x50)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Battery);
    LCDMessage(LINE1_START_ADDR + 8, au8Status5);
    LCDMessage(LINE2_START_ADDR, au8Return);
  }
}

u8 HexToDec(u8 u8Char_)
{
  u8 au8Change[2] = {0,0};
  u8 u8ReturnValue = 0; 
  
  au8Change[0] = (u8Char_ /16) * 16;
  au8Change[1] = u8Char_ %16;
  u8ReturnValue = au8Change[0] + au8Change[1];
  return(u8ReturnValue);
}
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error (for now, do nothing) */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/