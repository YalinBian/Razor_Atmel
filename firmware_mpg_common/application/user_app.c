/**********************************************************************************************************************
File: user_app.c                                                                

----------------------------------------------------------------------------------------------------------------------
To start a new task using this user_app as a template:
 1. Copy both user_app.c and user_app.h to the Application directory
 2. Rename the files yournewtaskname.c and yournewtaskname.h
 3. Add yournewtaskname.c and yournewtaskname.h to the Application Include and Source groups in the IAR project
 4. Use ctrl-h (make sure "Match Case" is checked) to find and replace all instances of "user_app" with "yournewtaskname"
 5. Use ctrl-h to find and replace all instances of "UserApp" with "YourNewTaskName"
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
void UserAppInitialize(void)
Runs required initialzation for the task.  Should only be called once in main init section.

void UserAppRunActiveState(void)
Runs current task state.  Should only be called once in main loop.


**********************************************************************************************************************/

#include "configuration.h"

/***********************************************************************************************************************
Global variable definitions with scope across entire project.
All Global variable names shall start with "G_"
***********************************************************************************************************************/
/* New variables */
u8 G_u8NameShowUpTime;                                  /* count how many times my name showed up */
u8 G_u8NameBuffer[9];                                   /* a array to store the part of my name have been typed */
u16 G_u16TurnOnTime = 0;                               /* count for the celebration time */
volatile u32 G_u32UserAppFlags;                       /* Global state flags */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Existing variables (defined in other files -- should all contain the "extern" keyword) */
extern volatile u32 G_u32SystemFlags;                  /* From main.c */
extern volatile u32 G_u32ApplicationFlags;             /* From main.c */

extern volatile u32 G_u32SystemTime1ms;                /* From board-specific source file */
extern volatile u32 G_u32SystemTime1s;                 /* From board-specific source file */

extern u8 G_au8DebugScanfBuffer[];                     /* From debug.c */
extern u8 G_u8DebugScanfCharCount;                     /* From debug.c */
/***********************************************************************************************************************
Global variable definitions with scope limited to this local application.
Variable names shall start with "UserApp_" and be declared as static.
***********************************************************************************************************************/
static fnCode_type UserApp_StateMachine;            /* The state machine function pointer */
static u32 UserApp_u32Timeout;                      /* Timeout counter used across states */

static u8 UserApp_au8MyName[] = "A3.Bian Ya Lin";           /* firs line of LCD */
static u8 UserApp_au8UpperCaseName[] = "BIANYALIN";         /* uppercase of my name to match with what i typed*/
static u8 UserApp_au8LowerCaseName[] = "bianyalin";         /* lowercase of my name to match with what i typed */
static u8 UserApp_au8UserInputBuffer[1000]={0};             /* Char buffer */

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
Function: UserAppInitialize

Description:
Initializes the State Machine and its variables.

Requires:
  -

Promises:
  - 
*/
void UserAppInitialize(void)
{
  /* show my name on the first line of lcd ,with my favorite backlight color all the time*/
  LCDMessage(LINE1_START_ADDR, UserApp_au8MyName);
  LCDClearChars(LINE1_START_ADDR + 14, 6);
  LedOn(LCD_BLUE);
  LedOff(LCD_RED);
  LedOn(LCD_GREEN);
 
  /* If good initialization, set state to Idle */
  if( 1 )
  {
    UserApp_StateMachine = UserAppSM_Idle;
  }
  else
  {
    /* The task isn't properly initialized, so shut it down and don't run */
    UserApp_StateMachine = UserAppSM_FailedInit;
  }

} /* end UserAppInitialize() */


/*----------------------------------------------------------------------------------------------------------------------
Function UserAppRunActiveState()

Description:
Selects and runs one iteration of the current state in the state machine.
All state machines have a TOTAL of 1ms to execute, so on average n state machines
may take 1ms / n to execute.

Requires:
  - State machine function pointer points at current state

Promises:
  - Calls the function to pointed by the state machine function pointer
*/
void UserAppRunActiveState(void)
{
  UserApp_StateMachine();

} /* end UserAppRunActiveState */


/*--------------------------------------------------------------------------------------------------------------------*/
/* Private functions                                                                                                  */
/*--------------------------------------------------------------------------------------------------------------------*/


/**********************************************************************************************************************
State Machine Function Definitions
**********************************************************************************************************************/

/*-------------------------------------------------------------------------------------------------------------------*/
/* Wait for a message to be queued */
static void UserAppSM_Idle(void)
{
  static u8 u8CharCount = 0;                               
  static u16 u16Count = 0;                                /* the number of character i typed */
  static u8 u8time = 50;
  static u8 uTwentyBufferADDR = 0;                      /* address of TwentyBuffer */
  static u8 u8TwentyBuffer[21];                         /* a array to show on the LCD */
  static u8 u8TotalCharsMessage[] = "\n\rTotal Characters received: ";
  static u8 u8ClearCharsMessage[] = "\n\rCharacter count cleared!";
  static u8 u8NameCharsMessage[] = "\n\rNameBuffer:";
  static u8 u8EmptyMessage[] = "\n\rNameBuffer is empty!";
  static bool bIsNotEmpty = FALSE;                      /* to judge if the array is empty */
  static u8 u8NameADDR = 0;                             /* address of namebuffer array*/
  
  /* the time of my whole name have showed up*/
  u8time--;
  if(u8time == 0)
  {
    u8time = 50;
    /* G_u8DebugScanfCharCount represent that im typing something,so use it to tell it should store */
    if(G_u8DebugScanfCharCount)
    { 
      u8CharCount = DebugScanf(UserApp_au8UserInputBuffer + u16Count);
      /* copy the character  typed to the twentybuffer array,end by '\0' */
      u8TwentyBuffer[uTwentyBufferADDR] = UserApp_au8UserInputBuffer[u16Count];
      /* compare to the letter of my name,no matter uppercase or lowercase,if matched,copy it to namebuffer*/
      if((u8TwentyBuffer[uTwentyBufferADDR] == UserApp_au8UpperCaseName[u8NameADDR]) || (u8TwentyBuffer[uTwentyBufferADDR] == UserApp_au8LowerCaseName[u8NameADDR]))
      {
        G_u8NameBuffer[u8NameADDR] = u8TwentyBuffer[uTwentyBufferADDR];
        bIsNotEmpty = TRUE;
        u8NameADDR++;
        G_u8NameBuffer[u8NameADDR] = '\0';
        if(u8NameADDR == 9)
        {
          u8NameADDR = 0;
          G_u8NameShowUpTime++;
          /*if namebuffer is full,celebration time = 5000ms*/
          G_u16TurnOnTime = 5000;
        }
      }
      uTwentyBufferADDR++;
      u16Count++;
      u8TwentyBuffer[uTwentyBufferADDR] = '\0';
      if(uTwentyBufferADDR == 20)
      {
        
        uTwentyBufferADDR = 0;       
      }
      /* show on the lcd*/
      LCDClearChars(LINE2_START_ADDR , 20);
      LCDMessage(LINE2_START_ADDR,u8TwentyBuffer);
    }
    /* Press BUTTON0 to clear the line of text,so the next character 
       starts from the beginning*/
    if(WasButtonPressed(BUTTON0))
    {
      ButtonAcknowledge(BUTTON0);
      LCDClearChars(LINE2_START_ADDR , 20);
      uTwentyBufferADDR = 0;
    }
    /* Press BUTTON1 to print the total number of character received 
       from the debug port*/
    if(WasButtonPressed(BUTTON1))
    {
      ButtonAcknowledge(BUTTON1);
      DebugPrintf(u8TotalCharsMessage);
      DebugPrintNumber(u16Count);
      DebugLineFeed();
    }
    /* Press BUTTON2 to clear the totable number and report a message*/
    if(WasButtonPressed(BUTTON2))
    {
      ButtonAcknowledge(BUTTON2);
      DebugPrintf(u8ClearCharsMessage);
      u16Count = 0;
      DebugLineFeed();
    }
    /* Press BUTTON3 to prints the current letter buffer that is 
       storing my name*/
    if(WasButtonPressed(BUTTON3))
    {
      ButtonAcknowledge(BUTTON3);
      DebugPrintf(u8NameCharsMessage);
      if(bIsNotEmpty)
      {
        DebugPrintf(G_u8NameBuffer);
        DebugLineFeed();
      }
      else
      {
        DebugPrintf(u8EmptyMessage);
      }
    } 
  }
} /* end UserAppSM_Idle() */
     

/*-------------------------------------------------------------------------------------------------------------------*/
/* Handle an error */
static void UserAppSM_Error(void)          
{
  
} /* end UserAppSM_Error() */


/*-------------------------------------------------------------------------------------------------------------------*/
/* State to sit in if init failed */
static void UserAppSM_FailedInit(void)          
{
    
} /* end UserAppSM_FailedInit() */


/*--------------------------------------------------------------------------------------------------------------------*/
/* End of File                                                                                                        */
/*--------------------------------------------------------------------------------------------------------------------*/
