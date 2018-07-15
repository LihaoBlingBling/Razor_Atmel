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


/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp1_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp1_StateMachine;            /* The state machine function pointer */
//static u32 UserApp1_u32Timeout;                      /* Timeout counter used across states */
static u8 u8States = 2;                              /* The initial state*/
static bool bChange = TRUE;                          /*You can Change States ?*/
static u8 u8Volume = 6;                              /*The number of Volume*/
static bool bCorrect = FALSE;                        /*The Volume is Correct ?*/
static u8 au8UserWarning[]="Not Correct";            /*The Volume is not Correct*/       

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
  AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
  AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
  AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
  AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;
  LedOn(GREEN);
  LedOff(BLUE);
  LedOff(PURPLE);
  Show(u8States);
 
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
  /*When you press the button, the Red light flashes for 1.5 seconds*/
  u16 i;
  /*turn the volume up*/
  if( WasButtonPressed(BUTTON0) )
  {
    ButtonAcknowledge(BUTTON0);
    for(i=0;i<=1500;i++)
    {
      LedOn(RED);
    }
    LedOff(RED);
    UserApp1_StateMachine = UserApp1SM_Upper;
  }
  
  /*turn the volume down*/
  if( WasButtonPressed(BUTTON1) )
  {
    ButtonAcknowledge(BUTTON1);
    for(i=0;i<=1500;i++)
    {
      LedOn(RED);
    }
    LedOff(RED);
    UserApp1_StateMachine = UserApp1SM_Lower;
  }
  
  /*Change the audio channel*/
  if( WasButtonPressed(BUTTON3) )
  {
    ButtonAcknowledge(BUTTON3);
    for(i=0;i<=1500;i++)
    {
      LedOn(RED);
    }
    LedOff(RED);
    if(bChange)
    {
      bChange = FALSE;
      UserApp1_StateMachine = UserApp1SM_Change;
    }
  }
  
  /*Correct volume*/
  if( WasButtonPressed(BUTTON2) )
  {
    ButtonAcknowledge(BUTTON2);
    for(i=0;i<=1500;i++)
    {
      LedOn(RED);
    }
    LedOff(RED);
    LedOn(WHITE);
    UserApp1_StateMachine = UserApp1SM_ADC;
    
  }
}/* end UserApp1SM_Idle() */

/*Update volume data*/
static void UserApp1SM_ADC(void)
{
  static u32 u32Result;
  u16 i;
  //AD start
  if(bCorrect == FALSE)
  {
    AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
    AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
    AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;
    AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
    Adc12StartConversion(ADC12_CH2);
    bCorrect = TRUE;
  }
  
  if(Adc12StartConversion(ADC12_CH2) == TRUE)
  {
    //LedOn(YELLOW);
    u32Result = AT91C_BASE_ADC12B->ADC12B_CDR[ADC12_CH2];
    u32Result = u32Result/41;
    u8Volume = u32Result/10;
    
    for(i=0;i<250;i++)
    {
      
    }
    LedOff(WHITE);
    UserApp1_StateMachine = UserApp1Initialize;
    Show(u8States);
  }
  
  //AD finished
  /*if(bCorrect)
  {
   
  }*/
}

/*Switch function
  -state1 Blue on said MIC  
  -state2 Green on said Phone
  -state3 Purple on said mute*/
static void UserApp1SM_Change(void)
{
  //Based on the previous state; change state
  switch(u8States)
  {
  case 1:AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
         AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
         AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
         LedOff(BLUE);
         LedOn(GREEN);
         LedOff(PURPLE);
         u8States = 2;
         bChange = TRUE;
         break;
  case 2:AT91C_BASE_PIOA->PIO_CODR = PA_16_BLADE_CS;
         AT91C_BASE_PIOB->PIO_SODR = PB_04_BLADE_AN1;
         AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
         LedOff(BLUE);
         LedOff(GREEN);
         LedOn(PURPLE);
         u8States = 3;
         bChange = TRUE;
         break;
  case 3:AT91C_BASE_PIOA->PIO_SODR = PA_16_BLADE_CS;
         AT91C_BASE_PIOB->PIO_CODR = PB_04_BLADE_AN1;
         AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
         LedOn(BLUE);
         LedOff(GREEN);
         LedOff(PURPLE);
         u8States = 1;
         bChange = TRUE;
         break;
  }
  Show(u8States);
  UserApp1_StateMachine = UserApp1SM_Idle;
}/* end UserApp1SM_Change() */


/*volume up*/
static void UserApp1SM_Upper(void)
{
  u8 i;
  u16 j;
  AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
  AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;
  for(i=0;i<=10;i++)
  {
    AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;
    //Wait
    for(j=0;j<100;j++)
    {
      
    }
    AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
  }
  if(bCorrect)
  {
  //Upper limit
    if(u8Volume<=8)
    {
      u8Volume++;
    }
    if(u8Volume == 9)
    {
      u8Volume = 9;
    }
  }
  Show(u8States);
  UserApp1_StateMachine = UserApp1SM_Idle;
}/* end UserApp1SM_Upper() */


/*volume down*/
static void UserApp1SM_Lower(void)
{
  u8 i;
  u16 j;
  AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
  AT91C_BASE_PIOA->PIO_CODR = PA_13_BLADE_MISO;
  for(i=0;i<=10;i++)
  {
    AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;
    for(j=0;j<100;j++)
    {
      
    }
    AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
  }
  if(bCorrect)
  {
    //Lower limit
    if(u8Volume>=1)
    {
      u8Volume--;
    }
    if(u8Volume == 0)
    {
      u8Volume = 0;
    }
  }
  Show(u8States);
  UserApp1_StateMachine = UserApp1SM_Idle;
}/* end UserApp1SM_Lower() */


/*Show music in Line1
  Show volume in Line2*/
void Show(u8 u8States)
{
  static u8 au8Channel1[]="MIC";
  static u8 au8Channel2[]="Phone";
  static u8 au8Channel3[]="Mute";
  static u8 au8Volume[1] ="";
  
  au8Volume[0] = u8Volume + 48;//To ASCII
  
  //states
  if(u8States == 1)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Channel1);
  }
  if(u8States == 2)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Channel2);
  }
  if(u8States == 3)
  {
    LCDCommand(LCD_CLEAR_CMD);
    LCDMessage(LINE1_START_ADDR, au8Channel3);
  }
  
  //volume
  if(bCorrect)
  {
    LCDMessage(LINE2_START_ADDR, au8Volume);
  }
  if(bCorrect == FALSE)
  {
    LCDMessage(LINE2_START_ADDR,au8UserWarning);
  }
  
}/*end show*/


/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
