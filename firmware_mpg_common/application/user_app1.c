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
  static u8 au8Colour[2][8]={{'W','P','B','C','G','Y','O','R'},
  {'w','p','b','c','g','y','o','r'}};
  static u8 au8LedNames[]={WHITE,WHITE, PURPLE, BLUE, CYAN, GREEN, YELLOW, ORANGE, RED};
  static u8 u8Colour1 = 0;
  static u8 u8Colour2 = 0;
  static u8 u8Sign = 0;
  static u32 u32Counter = 0;
  static u32 u32Number1 = 0;
  static u32 u32Number2 =0;
  static u32 u32Number3 = 0;
  static u32 u32Count = 0;
  static u8 u8Right = 0;
  static u32 u32Start1 = 0;
  static u32 u32Start = 0;
  static u32 u32End1 = 0;
  static u32 u32End = 0;
  static bool bInput = TRUE;
  static LedCommandType L;
  static u8 u8List = 0;
  static bool bAgain=FALSE;
  static u8 u8Menu = 0;
  static u8 u8Count = 0;
  
  if(u8Menu==0)
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
    
    if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-1]==0x0D  )
    {
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2]!=0x0D)
      {
        if(bInput==TRUE )
        {
          bAgain=FALSE;
          if(u32Counter<G_u8DebugScanfCharCount)
          {
            u32Counter++;
            if(G_au8DebugScanfBuffer[u32Counter]==0x2D)
            {
              if(u32Number1!=0)
              {
                u32Number2=u32Counter;
                u8Right++;
              }
              if(u32Number1==0)
              {
                u32Number1=u32Counter;
                u8Right++;
              }
            }
            if(G_au8DebugScanfBuffer[u32Counter]==0x0D)
            {
              u32Number3=u32Counter;
              u8Right++;
            }
          }
          
          
          
          if(u8Right==3)
          {
            if(u32Count==0)
            {
              u32Count=u32Number1-1;
            }
            
            if(u8Colour1<=2 && u8Sign==0)
            {
              if(u8Colour1==0)
              {
                if(G_au8DebugScanfBuffer[u32Count]==au8Colour[0][u8Colour2])
                {
                  u8Sign=1;
                  u8Right++;
                }
                u8Colour2++;
                
              }
              if(u8Colour1==1)
              {
                if(G_au8DebugScanfBuffer[u32Count]==au8Colour[1][u8Colour2])
                {
                  u8Sign=1;
                  u8Right++;
                }
                u8Colour2++;
              }
              if(u8Colour2>8)
              {
                u8Colour1++;
                u8Colour2=0;
              }
            }
          }
          if(u8Right==4)
          {
            if(u32Count<=u32Number1)
            {
              u32Count=u32Number1+1;
            }
            if(G_au8DebugScanfBuffer[u32Count]>='0' && G_au8DebugScanfBuffer[u32Count]<='9')
            {
              u32Start1=G_au8DebugScanfBuffer[u32Count]-'0';
              for(u16 i=0;i<(u32Number2-u32Count-1);i++)
              {
                u32Start1=u32Start1*10;
              }
              u32Count++;
              u32Start=u32Start+u32Start1;
            }
            if(u32Count>=u32Number2)
            {
              u8Right=5;
            }
          }
          if(u8Right==5)
          {
            if(u32Count<=u32Number2)
            {
              u32Count=u32Number2+1;
            }
            
            if(G_au8DebugScanfBuffer[u32Count]>='0' && G_au8DebugScanfBuffer[u32Count]<='9')
            {
              u32End1=G_au8DebugScanfBuffer[u32Count]-'0';
              for(u16 i=0;i<(u32Number3-u32Count-1);i++)
              {
                u32End1=u32End1*10;
              }
              u32Count++;      
              u32End=u32End+u32End1;
            }
            if(u32Count>=u32Number3)
            {
              u8Right++;
              bInput=FALSE;
            }
          }
          if(u8Right==6)
          {
            if(u32End>u32Start)
            {
              DebugLineFeed();
              u8Right=7;
            }
          }
        }
        
        if(bInput==FALSE)
        {
          if(u8Right==7 && bAgain==FALSE)
          {
            L.eLED = au8LedNames[u8Colour2];
            L.u32Time = u32Start;
            L.bOn = TRUE;
            L.eCurrentRate = LED_PWM_100;
            LedDisplayAddCommand(USER_LIST , &L);
            
            L.eLED = au8LedNames[u8Colour2];
            L.u32Time = u32End;
            L.bOn = FALSE;
            L.eCurrentRate = LED_PWM_0;
            LedDisplayAddCommand(USER_LIST , &L);
            bAgain=TRUE;
            u8List++;
          }
          if(bAgain)
          {
            u8Colour1 = 0;
            u8Colour2 = 0;
            u8Sign = 0;
            u32Number1 = 0;
            u32Number2 =0;
            u32Number3 = 0;
            u32Count = 0;
            u8Right = 0;
            u32Start1 = 0;
            u32Start = 0;
            u32End1 = 0;
            u32End = 0;
            bInput = TRUE;
            u32Counter=G_u8DebugScanfCharCount;
          }
        }
      }
      if(G_au8DebugScanfBuffer[G_u8DebugScanfCharCount-2]==0x0D)
      {
        u8Menu=0;
        u8Colour1 = 0;
        u8Colour2 = 0;
        u8Sign = 0;
        u32Number1 = 0;
        u32Number2 =0;
        u32Number3 = 0;
        u32Count = 0;
        u8Right = 0;
        u32Start1 = 0;
        u32Start = 0;
        u32End1 = 0;
        u32End = 0;
        bInput = TRUE;
        u32Counter=G_u8DebugScanfCharCount;
        u8Count=DebugScanf(G_au8DebugScanfBuffer);
        
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
    u8Colour1 = 0;
    u8Colour2 = 0;
    u8Sign = 0;
    u32Number1 = 0;
    u32Number2 =0;
    u32Number3 = 0;
    u32Count = 0;
    u8Right = 0;
    u32Start1 = 0;
    u32Start = 0;
    u32End1 = 0;
    u32End = 0;
    bInput = TRUE;
    u32Counter=G_u8DebugScanfCharCount;
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
