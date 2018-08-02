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
static u8 au8LedData[80]={0};
static u8 au8PrepareData[116] = {0};
static u8 au8Prepare[16][14]={0};
static u8 au8Test0[]={0x00,0x01,0x00,0x01,0xFC,0x7F,0x80,0x03,
0x40,0x05,0x20,0x09,0x18,0x31,0x06,0xC1,
0xE0,0x0F,0x40,0x00,0x80,0x00,0xFE,0xFF,
0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02};/*"李",0*/
static u8 au8Test1[]={0x20,0x08,0x40,0x04,0xFC,0x7F,0x00,0x01,
0x00,0x01,0xF8,0x3F,0x00,0x01,0x00,0x01,
0xFE,0xFF,0x00,0x01,0x00,0x01,0xFC,0x7F,
0x80,0x02,0x40,0x04,0x30,0x18,0x0E,0xE0};/*"美",1*/
static u8 au8Test2[]={0xF8,0x3F,0x00,0x01,0xFE,0x7F,0x02,0x41,
0x74,0x9D,0x00,0x01,0x70,0x1D,0x00,0x02,
0x00,0x01,0xFC,0x7F,0x20,0x08,0x40,0x04,
0x80,0x03,0x40,0x04,0x30,0x18,0x0E,0xE0};/*"雯",2*/
static u8 au8Test3[32]={0xF0,0x1F,0x10,0x10,0x10,0x10,0xF0,0x1F,
0x10,0x10,0x10,0x10,0xF0,0x1F,0x00,0x00,
0xFE,0xFF,0x00,0x01,0x00,0x11,0xF8,0x11,
0x00,0x11,0x00,0x29,0x00,0x45,0xFE,0x83};/*"是",3*/
static u8 au8Test4[] ={0x00,0x01,0x00,0x01,0x00,0x01,0x00,0x01,
0x00,0x01,0x10,0x11,0x08,0x11,0x04,0x11,
0x04,0x21,0x02,0x21,0x02,0x41,0x02,0x81,
0x00,0x01,0x00,0x01,0x00,0x05,0x00,0x02};/*"小",4*/
static u8 au8Test5[] = {0x40,0x10,0x40,0x10,0x40,0x10,0x40,0x20,
0x44,0x24,0x44,0x64,0x44,0x64,0x44,0xA4,
0x44,0x24,0x44,0x24,0x44,0x24,0x44,0x24,
0x44,0x24,0xFC,0x27,0x04,0x20,0x00,0x20};/*"仙",5*/
static u8 au8Test6[] = {0x00,0x02,0x00,0x02,0x00,0x02,0x00,0x02,
0x00,0x04,0xFE,0xFF,0x20,0x04,0x20,0x08,
0x20,0x08,0x40,0x10,0x40,0x18,0x80,0x06,
0x80,0x01,0x60,0x06,0x10,0x18,0x08,0x60};/*"女",6*/
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
  //MBI5026GF
  AT91C_BASE_PIOA->PIO_PER |= (PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  AT91C_BASE_PIOA->PIO_PDR &= ~(PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  AT91C_BASE_PIOA->PIO_OER |= (PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  AT91C_BASE_PIOA->PIO_ODR &= ~(PA_11_BLADE_UPIMO|PA_12_BLADE_UPOMI|PA_14_BLADE_MOSI|PA_15_BLADE_SCK);
  
  //CD4515BM
  AT91C_BASE_PIOA->PIO_PER |= (PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  AT91C_BASE_PIOA->PIO_PDR &= ~(PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  AT91C_BASE_PIOA->PIO_OER |= (PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  AT91C_BASE_PIOA->PIO_ODR &= ~(PA_03_HSMCI_MCCK|PA_04_HSMCI_MCCDA|PA_05_HSMCI_MCDA0|PA_06_HSMCI_MCDA1|PA_07_HSMCI_MCDA2|PA_08_SD_CS_MCDA3);
  
  //GT21L16S2W
  AT91C_BASE_PIOB->PIO_PER |= (PB_05_TP56|PB_06_TP58|PB_07_TP60 |PB_08_TP62);
  AT91C_BASE_PIOB->PIO_PDR &= ~(PB_05_TP56|PB_06_TP58|PB_07_TP60 |PB_08_TP62);
  AT91C_BASE_PIOB->PIO_OER |= (PB_05_TP56|PB_07_TP60 |PB_08_TP62);
  AT91C_BASE_PIOB->PIO_ODR &= ~(PB_05_TP56|PB_07_TP60 |PB_08_TP62);
  AT91C_BASE_PIOB->PIO_OER &= ~(PB_06_TP58);
  AT91C_BASE_PIOB->PIO_ODR |= (PB_06_TP58);
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
  u8 i;
  static u8 j = 0;
  static u8 u8Line = 0;
  static bool bPrepare = FALSE;
  static u32 u32Timer = 0;
  static u16 u16State = 0;
  static u16 u16Length = 0;

 
  static u16 word = '男';
  
  static u8 MSB;
  static u8 LSB;
  static u32 u32Address;
  
  MSB = word >> 8;
  LSB = word & 0x00FF;
  
  if(MSB >= 0xA4 && MSB <= 0xA8 && LSB >= 0xA1)
  {
    u32Address = 0;
  }
  else if(MSB >= 0xA1 && MSB <= 0xA9 && LSB >= 0xA1)
  {
    u32Address =((MSB - 0xA1)*94 + (LSB -0xA1))*32 + 0;
  }
  else if(MSB >= 0xB0 && MSB <= 0xF7 && LSB >= 0xA1)
  {
    u32Address =((MSB - 0xB0)*94 + (LSB -0xA1) + 846)*32 + 0;
  }
  
  ReadData(u32Address);
  
  
  if(bPrepare == FALSE)
  {
    bPrepare = TRUE;
    for(i=0;i<16;i++)
    {
      for(u8Line=0;u8Line<2;u8Line++)
      {
        au8Prepare[i][u8Line]=au8Test6[j];
        au8Prepare[i][u8Line+2]=au8Test5[j];
        au8Prepare[i][u8Line+4]=au8Test4[j];
        au8Prepare[i][u8Line+6]=au8Test3[j];
        au8Prepare[i][u8Line+8]=au8Test2[j];
        au8Prepare[i][u8Line+10]=au8Test1[j];
        au8Prepare[i][u8Line+12]=au8Test0[j];
        j++;
      }
      if(u8Line>=2)
      {
        u8Line = 0;
      }
      if(j>=32)
      {
        j=0;
      }
    }
  }
  
  
  
  //UserApp1_StateMachine = UserApp1SM_LedData;

  
  
  //MBI5026GF（16*5）

  for(i=0;i<16;i++)
  {
    LineData(i);
    LedData(u16Length);
    MBI_Data(i);
    AT91C_BASE_PIOA->PIO_SODR = PA_11_BLADE_UPIMO;
    Delay(5);
    AT91C_BASE_PIOA->PIO_SODR = PA_12_BLADE_UPOMI;
    Delay(5);
    AT91C_BASE_PIOA->PIO_CODR = PA_12_BLADE_UPOMI;
    Delay(5);
    CD_Data(i);
    AT91C_BASE_PIOA->PIO_CODR = PA_11_BLADE_UPIMO;
    Delay(5);
  }
  
  u32Timer++;
  if(u32Timer>=15)
  {
    u16State++;
    u32Timer = 0;
  }
  
  if(u16State<80 && u16State != u16Length)
  {
    u16Length++;
  }
  
  if(u16Length>=79)
  {
    u16Length = 80;
  }
  //UserApp1_StateMachine = UserApp1SM_TestData;

  
  
}/* end UserApp1SM_Idle() */
    
void MBI_Data(u8 k)
{
  u8 i;
  static u8 u8Data;
  for(i=0;i<80;i++)
  {
    u8Data = au8LedData[i];

      AT91C_BASE_PIOA->PIO_CODR = PA_15_BLADE_SCK;
      if( u8Data == 0)
      {
        AT91C_BASE_PIOA->PIO_CODR = PA_14_BLADE_MOSI;
      }
      else if(u8Data != 0x00)
      {
        AT91C_BASE_PIOA->PIO_SODR = PA_14_BLADE_MOSI;
      }
      Delay(5);
      AT91C_BASE_PIOA->PIO_SODR = PA_15_BLADE_SCK;
      Delay(5);
      
    
  }
}

void CD_Data(u8 m)
{
  static u8 u8Line;
  u8Line = m;
  AT91C_BASE_PIOA->PIO_CODR = PA_04_HSMCI_MCCDA ;
  AT91C_BASE_PIOA->PIO_SODR = PA_03_HSMCI_MCCK ;
  
  
  switch(u8Line)
  {
  case 0 : 
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 1 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 2 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 3 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 4 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 5 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 6 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 7 :
    AT91C_BASE_PIOA->PIO_CODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 8:
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 9:
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 10 :
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 11 :
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 12 :
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 13 :
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 14 :
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_CODR = PA_05_HSMCI_MCDA0 ;
    break;
  case 15 :
    AT91C_BASE_PIOA->PIO_SODR = PA_08_SD_CS_MCDA3 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_07_HSMCI_MCDA2 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_06_HSMCI_MCDA1 ;
    AT91C_BASE_PIOA->PIO_SODR = PA_05_HSMCI_MCDA0 ;
    break;
  default :
    break;
    
  }
}

void ReadData(u32 a)
{
  static u8 u8Insturction = 0x03;
  static u32 u32Address;
  u8 i;
  
  
  AT91C_BASE_PIOB->PIO_CODR = PB_05_TP56;
  
  for(i=0;i<8;i++)
  {
    AT91C_BASE_PIOB->PIO_CODR = PB_07_TP60;
    if((0x10&u8Insturction) == 0)
    {
      AT91C_BASE_PIOB->PIO_CODR = PB_08_TP62;
    }
    else if((0x10&u8Insturction) != 0)
    {
      AT91C_BASE_PIOB->PIO_SODR = PB_08_TP62;
    }
    u8Insturction = u8Insturction << 1;
    AT91C_BASE_PIOB->PIO_SODR = PB_07_TP60;
  }
  
  for(i=0;i<24;i++)
  {
    
  }
  
  
}


/*static void UserApp1SM_TestData(void)
{
  static u8 u8Choose = 0;
  static u8 u8Number = 1;
  static u16 u16Timer = 0;
  u8 i;
  u8 l;
  static u8 m;
  static u8 d;
  static s8 p;
  static bool bChoose = FALSE;
  
  u16Timer++;
  
  if(u8Choose<=9)//从11列向后Number个列;改变字数这个if需动
  {
    u8Number = u8Choose +1;
    for(l=0;l<16;l++)
    {
      for(m=0;m<u8Number;m++)
      {
        au8TestData[l][u8Number-1 - m] = au8Prepare[l][13 - m];//+2
      }
    }
  }
  
  if(u8Choose >9)//字数改变只需动这个if
  {
    bChoose = TRUE;
    d = 9;
    u8Number = 10;
    if(u8Choose > 24)//+2
    {
      u8Choose = 10;
    }
    
    for(i=0;i<10;i++)
    {
      if(bChoose)
      {
        bChoose = FALSE;
        if(u8Choose == 23)//+2
        {
          p = 14;//+2
        }
        if(u8Choose == 24)//+2
        {
          p = 13; //+2
        }
        if(u8Choose <22)//+2
        {
          p = 22 - u8Choose;//+2
        }
        if(u8Choose == 22)//+2
        {
          p = 0;
        }
      }
      

      for(l=0;l<16;l++)
      {
        au8TestData[l][d] = au8Prepare[l][p];
      }
      if(p>=0)
      {
        p--;
      }
      if(p<0)
      {
        p=14;//+2
      }
      d--;
      
    }
  }
  
  if(u16Timer>=100)
  {
    u8Choose++;
    u16Timer = 0;
  }
  UserApp1_StateMachine = UserApp1SM_Idle;
}*/


void LineData(u8 i)
{
  u8 j;
  u8 n;
  static u8 u8TestData;
  
  for(j=0;j<14;j++)
  {
    u8TestData=au8Prepare[i][j];
    for(n=0;n<8;n++)
    {
      if((0x01&u8TestData) == 0x00)
      {
        au8PrepareData[n + j*8 +4]=0;  ////
      }
      if((0x01&u8TestData) != 0x00)
      {
        au8PrepareData[n + j*8 +4]=1;  /////
      }
      u8TestData = u8TestData >> 1;
    }
  }
  
  
  
}


void LedData(u16 i)
{
  u8 j;
  u8 l;
  s8 p;
  static u8 u8Choose;
  static bool bChoose = FALSE;
  static u16 u16Timer = 0;
  
  
  
  if(i<80)
  {
    for(j=0;j<i;j++)
    {
      au8LedData[i -1 -j] = au8PrepareData[115 -j];  ////
    }
    return;
    
  }
  
  if(i==80)
  {
    u16Timer++;
    bChoose = TRUE;
    if(u16Timer>=250)
    {
      u8Choose++;
      u16Timer = 0;
    }
    //u8Choose++;
    if(u8Choose>116)  /////
    {
      u8Choose = 1;
    }
    l=79;
    
    for(j=0;j<80;j++)
    {
      if(bChoose)
      {
        bChoose = FALSE;
        if(u8Choose<=116) ////
        {
          p=116-u8Choose; ///
        }
        
      }
      au8LedData[l] = au8PrepareData[p];
      if(p>=0)
      {
        p--;
      }
      if(p<0)
      {
        p=115;   ////
      }
      l--;
    }
    return;
    
    
  }
   
}

void Delay(u8 n)
{
  u8 i;
  for(i=0;i<n;i++)
  {
    
  }
}
/*----------------------
---------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserApp1SM_Error(void)          
{
  
} /* end UserApp1SM_Error() */



/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/