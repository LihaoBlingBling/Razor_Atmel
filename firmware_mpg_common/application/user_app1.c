/**********************************************************************************************************************
File: user_app1.c                                                                

Description:
Provides a Tera-Term driven system to display, read and write an LED command list.

Test1.
Test2 from Engenuics.
Test3.

------------------------------------------------------------------------------------------------------------------------
API:

Public functions:
None.

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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[DEBUG_SCANF_BUFFER_SIZE]; /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                        /* From debug.c */


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 au8UserMenu1[]="************************************\n\rPress 1 to program LED command sequence\n\rPress 2 to show current USER program\n\r************************************\n\r";
static u8 au8UserMessage1[]="\n\rEnter commands as LED-ONTIME-OFFTIME and press Enter\n\rTime is in milliseconds, max 100 commands";
static u8 au8UserMessage2[]="\n\rLED colors: R(r),O(o),Y(y),G(g),C(c),B(b),P(p),W(w)\n\rExample: R-100-200 (RED on at 100ms and off at 200ms)\n\rPress Enter on blank line to end\n\r";
static u8 au8UserMessage3[]="\n\rLED   ON TIME   OFF TIME\n\r-----------------------------------\n\r";
static u8 au8Error1[]="\n\rThe Ledname is wrong\n\r";
static u8 au8Error2[]="\n\rThe character after Ledname is wrong\n\r";
static u8 au8Error3[]="\n\rStart time cannot include character which is not Number\n\r";
static u8 au8Error5[]="\n\rEnd time cannot include character which is not Number\n\r";
static u8 au8Error6[]="End time is less than Start time\n\r";
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
  u8 au8UserApp1Start1[] = "LED program task started\n\r";
  
  
  /* Turn off the Debug task command processor and announce the task is ready */
  DebugSetPassthrough();
  DebugPrintf(au8UserApp1Start1);
  DebugLineFeed();
  DebugPrintf(au8UserMenu1);

  
  
    /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp1_StateMachine = UserApp1SM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp1_StateMachine = UserApp1SM_FailedInit;
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
/* Wait for input */
static void UserApp1SM_Idle(void)
{
  static LedCommandType C1;
  static u8 u8Menu = 0;
  static u8 u8Error = 0;
  static u8 au8LedNames1[] = {'W','P','B','C','G','Y','O','R'};
  static u8 au8LedNames2[] = {'w','p','b','c','g','y','o','r'};
  static u8 au8Led[] = { WHITE,WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  static u8 u8Number = 0;
  static u32 u32Time1;
  static u32 u32Time2;
  static u8 u8Number1 = 0;
  static u8 u8Number2 = 0;
  static u8 u8Address1 = 1;
  static u8 u8Address2 = 1;
  static u32 au32Time[]={1,1,10,100,1000,10000,100000};
  static bool bSign = FALSE;
  static u8 u8Sign = 0;
  static u32 u32Count = 0;
  static u8 u8List = 0;
  static u8 u8Count = 0;
  static bool bInput = TRUE;
  static bool bAgain = FALSE;
  
  if(u8Menu == 0)
  {
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x31)
    {
      u8Menu=DebugScanf(G_au8DebugScanfBuffer);
      DebugPrintf(au8UserMessage1);
      DebugPrintf(au8UserMessage2);
      DebugLineFeed();
      u32Count=0;
    }  
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x32)
    {
      u8Count=0;
      u8Menu=DebugScanf(G_au8DebugScanfBuffer)+1;
      DebugPrintf(au8UserMessage3);
      DebugLineFeed();
    }
  }
  if(u8Menu==1)
  {
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x0D && bInput==TRUE )
    {
      bAgain = FALSE;
      if(u32Count < G_u8DebugScanfCharCount)
      {
        if(bSign==FALSE && u8Number<=7)
        {
          if(G_au8DebugScanfBuffer[u32Count]==au8LedNames1[u8Number] || G_au8DebugScanfBuffer[u32Count]==au8LedNames2[u8Number])
          {
            bSign=TRUE;
          }
          u8Number++;
        }
        if(bSign==FALSE && u8Number>=8)
        {
          u8Error=1;
        }
        
        if(bSign)
        {
          if(u8Sign == 0 && u8Error == 0)
          {
            u32Count++;
            if(G_au8DebugScanfBuffer[u32Count]==0x2D)
            {
              u8Sign=1;
            }
            else
            {
              u8Sign=4;
              u8Error=2;
            }
          }
          
          if(u8Sign == 1)
          {
            u32Count++;
            u8Number1++;
            if(G_au8DebugScanfBuffer[u32Count]==0x2D)
            {
              u8Sign=2;
              for(u8Address1=1;u8Address1<u8Number1;u8Address1++)
              {
                if(G_au8DebugScanfBuffer[u32Count-u8Address1]>=48 && G_au8DebugScanfBuffer[u32Count-u8Address1]<=57)
                {
                  u32Time1=u32Time1+(G_au8DebugScanfBuffer[u32Count-u8Address1]-48)*au32Time[u8Address1];
                }
                else
                {
                  u8Error=3;
                }
              }
            }
          }
          
          if(u8Sign == 2 && u8Error==0 )
          {
            if(G_au8DebugScanfBuffer[u32Count]!=0x0D)
            {
              u32Count++;
              u8Number2++;
            }
            else
            {
              bInput=FALSE;
              u8Sign=3;
              for(u8Address2=1;u8Address2<u8Number2;u8Address2++)
              {
                if(G_au8DebugScanfBuffer[u32Count-u8Address2]>=48 && G_au8DebugScanfBuffer[u32Count-u8Address2]<=57)
                {
                  u32Time2=u32Time2+(G_au8DebugScanfBuffer[u32Count-u8Address2]-48)*au32Time[u8Address2];
                }
                else
                {
                  u8Error=5;
                }
              }
              DebugLineFeed();
              if(u8Error == 0 && u32Time2<=u32Time1)
              {
                u8Error=6;
              }
            }
          }
        }
      }
    }
    
    if( u8Error==0 && bInput==FALSE)
    { 
      if(bAgain == FALSE)
      {
        C1.eLED = au8Led[u8Number];
        C1.u32Time = u32Time1;
        C1.bOn = TRUE;
        C1.eCurrentRate = LED_PWM_100;
        LedDisplayAddCommand(USER_LIST , &C1);
        
        C1.eLED = au8Led[u8Number];
        C1.u32Time = u32Time2;
        C1.bOn = FALSE;
        C1.eCurrentRate = LED_PWM_0;
        LedDisplayAddCommand(USER_LIST , &C1);
        bAgain=TRUE;
        u8List++;
      }
      if(bAgain)
      {
        
        bSign=FALSE;
        u8Number = 0;
        u8Number1 = 0;
        u8Number2 = 0;
        u8Address1 = 1;
        u8Address2 = 1;
        u8Sign = 0;
        u32Time1=0;
        u32Time2=0;
      }
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] != 0x0D)
      {
        u32Count++;
        bInput=TRUE;
      }
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1] == 0x0D &&G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2] == 0x0D )
      {
        u8Menu=0;
        u8Count=DebugScanf(G_au8DebugScanfBuffer);
      }
    }
    if(u8Error!=0)
    {
      bSign=FALSE;
      u8Number = 0;
      u8Number1 = 0;
      u8Number2 = 0;
      u8Address1 = 1;
      u8Address2 = 1;
      u8Sign = 0;
      u32Time1=0;
      u32Time2=0;
      u32Count=G_u8DebugScanfCharCount;
      bInput=TRUE;
      switch(u8Error)
      {
      case 1 :
        u8Error=0;
        DebugPrintf(au8Error1);
        break;
      case 2 :
        u8Error=0;
        DebugPrintf(au8Error2);
        break;
      case 3 :
        u8Error=0;
        DebugPrintf(au8Error3);
        break;
      case 5 :
        u8Error=0;
        DebugPrintf(au8Error5);
        break;
      case 6 :
        u8Error=0;
        DebugPrintf(au8Error6);
        break;
      }
      
    }
  }
  if(u8Menu==2)
  {
    u8Menu=0;
    for(u8Count=0;u8Count<=u8List;u8Count++)
    {
      LedDisplayPrintListLine(u8Count);
    }
    DebugPrintf(au8UserMenu1);
    bInput=TRUE;
    bSign=FALSE;
    u8Number = 0;
    u8Number1 = 0;
    u8Number2 = 0;
    u8Address1 = 1;
    u8Address2 = 1;
    u8Sign = 0;
    u32Time1=0;
    u32Time2=0;
    u32Count=0;
    
  }
  
  
} /* end UserApp1SM_Idle() */
                      
            
#if 0
/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */
#endif


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserApp1SM_FailedInit(void)          
{
    
} /* end UserApp1SM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
