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
extern volatile u32 G_u32SystemFlags;                   /* From main.c */
extern volatile u32 G_u32ApplicationFlags;              /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                 /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                  /* From board-specific source file */
extern u8 G_au8DebugScanfBuffer[];                      /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                      /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 UserApp_au8Menu1[] = "B0:Name  B1:User";
static u8 UserApp_au8Menu2[] = "B3:Menu";
static u32 au32Address[36][5]={ {0,1,2,3,4},
                                {1,2,3,4,5},
                                {2,3,4,5,6},
                                {3,4,5,6,7},
                                {4,5,6,7,8},
                                {5,6,7,8,9},
                                {6,7,8,9,10},
                                {7,8,9,10,11},
                                {8,9,10,11,12},
                                {9,10,11,12,13},
                                {10,11,12,13,14},
                                {11,12,13,14,15},
                                {12,13,14,15,16},
                                {13,14,15,16,17},
                                {14,15,16,17,18},
                                {15,16,17,18,19},
                                {16,17,18,19,64},
                                {17,18,19,64,65},
                                {18,19,64,65,66},
                                {19,64,65,66,67},
                                {64,65,66,67,68},
                                {65,66,67,68,69},
                                {66,67,68,69,70},
                                {67,68,69,70,71},
                                {68,69,70,71,72},
                                {69,70,71,72,73},
                                {70,71,72,73,74},
                                {71,72,73,74,75},
                                {72,73,74,75,76},
                                {73,74,75,76,77},
                                {74,75,76,77,78},
                                {75,76,77,78,79},
                                {76,77,78,79,80},
                                {77,78,79,80,81},
                                {78,79,80,81,82},
                                {79,80,81,82,83}};
static u16 au16Tone[]={0,523,578,628,698,784,880,988 };
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
   LCDCommand(LCD_CLEAR_CMD);
   LCDMessage(LINE1_START_ADDR, UserApp_au8Menu1);
   LCDMessage(LINE2_START_ADDR, UserApp_au8Menu2);
   
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
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
/* Wait for ??? */
static void UserApp1SM_Idle(void)
{
  static u8 u8Number= 0;        //The Number of Address
  static u32 u32Count = 0;
  static u8 u8Sign = 0;         //Warning
  static u8 u8Direction = 0;
  static u8 u8Tone = 0;
  
  static u8 u8Menu = 0;
  static u8 au8UesrHint[]="\n\rPlease Input:";
  static u8 au8Message1[]="B0:Finish B1:Display";
  static u8 au8Message2[]="B2:Again";
  static u8 au8Warning[]="Warning";
  static u8 u8End = 0;          //The end of input
  static u8 au8Line1[20]="";
  static u8 au8Line2[20]="";
  static u8 au8Line3[20]="";
  static u32 u32InputNumber = 0;
  static u8 u8LineNumber1 = 0;
  static u8 u8LineNumber2 = 0;
  static u8 u8LineNumber3 = 0;
  static u8 u8Print=0;
  static u16 u16Counter = 0;    //The Counter for Roll
  static u8 u8State = 0;
  static u16 u16Sign = 0;
  static u8 u8i=0;              //empty au8Line1 & au8Line2 & au8Line3
  
  u32Count++;
  
  if(u8Menu==0)
  {
    if( WasButtonPressed(BUTTON0) )//Menu1:Name
    {
      ButtonAcknowledge(BUTTON0);
      u8Menu=1;
    }
    if( WasButtonPressed(BUTTON1) )//Menu2:Input
    {
      ButtonAcknowledge(BUTTON1);
      u8Menu=2;
    }
    
  }    
  if( WasButtonPressed(BUTTON3) )//Go to Menu at any time
  {
    ButtonAcknowledge(BUTTON3);
    u8Menu=0;
    u8Direction = 0;
    u8Sign = 0;
    u8Tone = 0;
    LedOff(GREEN);
    LedOff(BLUE);
    LedOff(RED);
    PWMAudioOff(BUZZER1);
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, UserApp_au8Menu1);
    LCDMessage(LINE2_START_ADDR, UserApp_au8Menu2);
    u8End = 0;
    u8Print = 0;
    u32InputNumber = 0;
    u8LineNumber1 = 0;
    u8LineNumber2 = 0;
    u8LineNumber3 = 0;
    u16Counter = DebugScanf(G_au8DebugScanfBuffer);
    u8State = 0;
    u16Sign = 0;
    u8i=0;
    if(u8i<=19)
    {
      au8Line1[u8i]=0;
      au8Line2[u8i]=0;
      au8Line3[u8i]=0;
      u8i++;
    }
  }
  
  if(u8Menu==1)                         //My name Rolling
  {
    if( WasButtonPressed(BUTTON0) )     //Left
    {
      ButtonAcknowledge(BUTTON0);
      PWMAudioOff(BUZZER1);
      u8Sign=0;
      u8Direction=2;
    }
    if( WasButtonPressed(BUTTON1) )     //Right
    {
      ButtonAcknowledge(BUTTON1);
      PWMAudioOff(BUZZER1);
      u8Sign=0;
      u8Direction=1;
    }
    
    
    if(u32Count>=500 && u8Sign==0)
    {
      u32Count=0;
      
      PWMAudioSetFrequency(BUZZER1, au16Tone[u8Tone]);
      PWMAudioOn(BUZZER1);
      
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(au32Address[u8Number][0], "L");
      LCDMessage(au32Address[u8Number][1], "i");
      LCDMessage(au32Address[u8Number][2], "H");
      LCDMessage(au32Address[u8Number][3], "a");
      LCDMessage(au32Address[u8Number][4], "o");        //Move once per 500ms
      
      if(u8Direction==1)        //Right
      {
        LedOff(RED);
        LedOff(BLUE);
        LedOn(GREEN);
        u8Tone++;
        if(u8Number==36)        //Arrive at the right border
        {
          u8Number=35;
          u8Sign=1;
        }
        u8Number++;
      }
      
      if(u8Direction==2)        //Left
      {
        LedOff(RED);
        LedOff(GREEN);
        LedOn(BLUE);
        u8Tone--;
        if(u8Number==0)         //Arrive at the left border
        {
          u8Number=1;
          u8Sign=1;
        }
        u8Number--;
      }
      /*music*/
      if(u8Tone>8 && u8Direction==1)
      {
        u8Tone=1;
      }
      if(u8Tone==0 && u8Direction==2)
      {
        u8Tone=8;
      }
    }
    /*Warning*/
    if(u8Sign==1)
    {
      LedOff(GREEN);
      LedOff(BLUE);
      LedOn(RED);
      u8Tone=0;
      PWMAudioSetFrequency(BUZZER1, 1976);
      PWMAudioOn(BUZZER1);
    }
  }
  
  /*User can input*/
  if(u8Menu==2)
  {
    
    if(u8Print==0) /*Prompting*/
    {
      LCDCommand(LCD_CLEAR_CMD);
      LCDMessage(LINE1_START_ADDR, au8Message1);
      LCDMessage(LINE2_START_ADDR, au8Message2);
      DebugPrintf(au8Message1);
      DebugPrintf(au8Message2);
      DebugLineFeed();
      DebugPrintf(au8UesrHint);
      u8Print=1;
    }
    
    /*Finish inputting*/
    if( WasButtonPressed(BUTTON0) )
    {
      ButtonAcknowledge(BUTTON0);
      u8End=1;
    }
    /*Input Again*/
    if( WasButtonPressed(BUTTON2) ) 
    {
      ButtonAcknowledge(BUTTON2);
      u8End = 0;
      u8Print = 0;
      u32InputNumber = 0;
      u8LineNumber1 = 0;
      u8LineNumber2 = 0;
      u8LineNumber3 = 0;
      u16Counter = DebugScanf(G_au8DebugScanfBuffer);
      u8State = 0;
      u16Sign = 0;
      u8i=0;
      if(u8i<=19)
      {
        au8Line1[u8i]=0;
        au8Line2[u8i]=0;
        au8Line3[u8i]=0;
        u8i++;
      }
    }
    
    if(u8End == 1)
    {
      /*<=20*/
      if(G_u8DebugScanfCharCount<=20)
      {
        if( WasButtonPressed(BUTTON1) )
        {
          LCDCommand(LCD_CLEAR_CMD);
          ButtonAcknowledge(BUTTON1);
          LCDMessage(LINE1_START_ADDR, G_au8DebugScanfBuffer);
        }
      }
      
      /*>20 && <=40*/
      if(G_u8DebugScanfCharCount<=40 && G_u8DebugScanfCharCount>20)
      {
        if(u32InputNumber<=G_u8DebugScanfCharCount)
        {
          /*Change au8Line1*/
          if( u32InputNumber<=19)
          {
            au8Line1[u8LineNumber1]= G_au8DebugScanfBuffer[u32InputNumber];
            u8LineNumber1++;
            u32InputNumber++;
          }
          /*Change au8Line2*/
          if(u32InputNumber>19)
          {
            au8Line2[u8LineNumber2]= G_au8DebugScanfBuffer[u32InputNumber];
            u8LineNumber2++;
            u32InputNumber++;
          }
        }
        /*Diaplay*/
        if( WasButtonPressed(BUTTON1) )
        {
          LCDCommand(LCD_CLEAR_CMD);
          ButtonAcknowledge(BUTTON1);
          LCDMessage(LINE1_START_ADDR, au8Line1);
          LCDMessage(LINE2_START_ADDR, au8Line2);
        }
      }
      
      /*>40 && <=60*/
      if(G_u8DebugScanfCharCount>40 && G_u8DebugScanfCharCount<=60)
      {
        
        if(u32InputNumber<=G_u8DebugScanfCharCount)
        {
          /*au8Line1*/
          if( u32InputNumber<=19)
          {
            au8Line1[u8LineNumber1]= G_au8DebugScanfBuffer[u32InputNumber];
            u8LineNumber1++;
            u32InputNumber++;
          }
          /*au8Line2*/
          if(u32InputNumber>19 && u32InputNumber<=39)
          {
            au8Line2[u8LineNumber2]= G_au8DebugScanfBuffer[u32InputNumber];
            u8LineNumber2++;
            u32InputNumber++;
          }
          /*au8Line3*/
          if(u32InputNumber>39)
          {
            au8Line3[u8LineNumber3]= G_au8DebugScanfBuffer[u32InputNumber];
            u8LineNumber3++;
            u32InputNumber++;
          }
        }
        if( WasButtonPressed(BUTTON1) )
        {
          ButtonAcknowledge(BUTTON1);
          u8Print=2;
        }
        
        if(u8Print==2)
        {
          u16Counter++;
          if(u16Counter >= 500)
          {
            u16Counter=0;
            u8State++;
            u16Sign++;
          }
          if(u8State == 3)
          {
            u8State=0;
          } 
        }
        
        /*u8State have changed   Display*/
        if(u16Sign == 1)
        {
          u16Sign = 0;
          switch(u8State)
          {
          case 0:
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR, au8Line3);
            LCDMessage(LINE2_START_ADDR, au8Line1);
            break;
          case 1:
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR, au8Line1);
            LCDMessage(LINE2_START_ADDR, au8Line2);
            break;
          case 2:
            LCDCommand(LCD_CLEAR_CMD);
            LCDMessage(LINE1_START_ADDR, au8Line2);
            LCDMessage(LINE2_START_ADDR, au8Line3);
            break;
          default:
            break;
          }
        } 
      }
      
      /*>60 Warning*/
      if(G_u8DebugScanfCharCount>60)
      {
        
        if( WasButtonPressed(BUTTON1) )
        {
          ButtonAcknowledge(BUTTON1);
          LCDCommand(LCD_CLEAR_CMD);
          LCDMessage(LINE1_START_ADDR,au8Warning);
        }
      }
    }
  }
    
  
  

  
} /* end UserApp1SM_Idle() */
    

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
