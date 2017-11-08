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
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];  /* From debug.c */
extern u8 G_u8DebugScanfCharCount;  /* From debug.c */

/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */


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
  static u8     u8CharCount = 0;
  static u8     u8Sign = 0;
  static u32    u32Counter = 0;
  static u8     u8Number = 0;
  static u8     u8Count = 0;
  static bool   bTime = FALSE;
  static u8     au8NameNumber[]="\n\rThe number of your name in buffer:";
  
  if(u8CharCount<=G_u8DebugScanfCharCount-1  )
  {
    if(G_au8DebugScanfBuffer[u8CharCount] == 'L')
    {
      u8Count = 0;
      u8Sign = 1; /*the sign of the first char 'L' appearing*/
      u32Counter = 0; 
    }
    if(G_au8DebugScanfBuffer[u8CharCount] == 'i' && u8Sign == 1)
    {
      u8Count ++;
      u32Counter = 0;/*when G_au8DebugScanfBuffer is changed, clear Counter*/
    }
    
    if(G_au8DebugScanfBuffer[u8CharCount] == 'h' && u8Sign == 1)
    {
      u8Count ++;  
      u32Counter = 0;
    }
    
    if(G_au8DebugScanfBuffer[u8CharCount] == 'a' && u8Sign == 1)
    {
      u8Sign = 4;
      u8Count ++; /*the number of char before the last char'o'*/
      u32Counter = 0;
    }
    
    if(G_au8DebugScanfBuffer[u8CharCount] == 'o' && u8Sign == 4 && u8Count==3)
    {
      u8Count=0;
      u32Counter = 0;
      u8Number++; /*The number of your name*/
      bTime = TRUE; /*The number of your name has been changed*/
    }
    if(u8Count>=4) 
    {
      u8Count=0;
    }
    
    u8CharCount++;
  }
  
  if(bTime == TRUE) /* Start Counting*/
  {
    u32Counter++;
  }
  
  if(u32Counter == 3000)/*G_au8DebugScanfBuffer has not been changed in 3s*/
  {
    u32Counter = 0;
    u8CharCount = 0;
    bTime = FALSE;
    u8Count = DebugScanf(G_au8DebugScanfBuffer);  /*Clear G_au8DebugScanfBuffer*/
    DebugPrintf(au8NameNumber);
    DebugPrintNumber(u8Number);
    DebugLineFeed();
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
