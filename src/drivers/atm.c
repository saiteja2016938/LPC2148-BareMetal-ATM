
//atm.c                                                         // Source file name comment for ATM application logic
                                                                // Blank line after file header comment
#include "kpm.h"                                                // Include keypad driver declarations
#include "defines_atm.h"                                        // Include ATM-specific macro definitions
#include <LPC21xx.h>                                            // Include LPC21xx microcontroller SFR definitions
#include <string.h>                                             // Include standard C string manipulation functions
#include <stdio.h>                                              // Include standard C I/O formatting functions
#include "lcd_defines.h"                                        // Include LCD command and pin macro definitions
#include "lcd.h"                                                // Include LCD driver declarations
#include "delay.h"                                              // Include delay routine declarations
#include "atm.h"                                                // Include ATM function prototypes
#include "rtc.h"                                                // Include RTC driver declarations
#include "intrrupt.h"                                           // Include external interrupt declarations
#include "uart.h"                                               // Include UART driver declarations
                                                                // Blank line separating includes from macros/globals
                                                                // Blank line before buzzer and history macros
#define BUZZER_PIN (1U << 5)                                    // Define buzzer pin bitmask on Port 0 Pin 5 (P0.5)
#define MAX_HISTORY 10                                          // Define maximum number of stored transactions
#define HISTORY_LENGTH 32                                       // Define maximum length of each transaction log string
char msg[HISTORY_LENGTH];                                       // Buffer used to format transaction messages
char transaction_log[MAX_HISTORY][HISTORY_LENGTH];              // 2D array holding stored transaction history entries
u32 history_count = 0;                                          // Counter for current number of logged transactions
                                                                // Blank line separating history globals from account globals
unsigned int acc_no = 1001;                                     // Default account number initialized to 1001
unsigned int pin = 1234;                                        // Default ATM PIN initialized to 1234
unsigned int balance = 25000;                                   // Default account balance initialized to 25000
unsigned char login_status = 0;                                 // Session flag: 0 = logged out, 1 = logged in
unsigned int dob = 13022005;                                   // Default date of birth (DDMMYYYY) for PIN recovery
                                                                // Blank line separating account variables from comment block
                                                                // Blank line before ReadNumber comment
                                                                // Blank line before block comment
/* Read a multi-digit number from the keypad.                   // Block comment start explaining ReadNumber input
   Press # when finished. */                                    // Block comment end noting '#' confirms entry
#define INPUT_CANCEL 0xFFFFFFFF                                 // Sentinel return value when user cancels entry ('D')
                                                                // Blank line separating INPUT_CANCEL from DisplayDateTime
void DisplayDateTime(void)                                      // Define function to display RTC date and time on LCD
{                                                               // Opening brace for DisplayDateTime body
    RTC_READ_TIME();	                                            // Read current hours, minutes, and seconds from RTC
    RTC_READ_DATE();                                            // Read current date, month, and year from RTC
                                                                // Blank line before clearing LCD and setting cursor
    CmdLCD(CLEAR_LCD);                                          // Clear LCD display screen
    CmdLCD(GOTO_LINE1_POS0);                                    // Move LCD cursor to start of line 1
                                                                // Blank line before displaying date fields
	                                                              // Indented blank line inside DisplayDateTime
    U32LCD(rdate);                                              // Display RTC day of month on LCD
    CharLCD('/');                                               // Print '/' date separator on LCD
                                                                // Blank line between day and month display
    U32LCD(rmonth);                                             // Display RTC month on LCD
    CharLCD('/');                                               // Print '/' date separator on LCD
                                                                // Blank line between month and year display
    U32LCD(ryear);                                              // Display RTC year on LCD
                                                                // Blank line before moving cursor to line 2
    CmdLCD(GOTO_LINE2_POS0);                                    // Move LCD cursor to start of line 2 for time
                                                                // Blank line before formatting hours
	if(rhrs<10)                                                   // Check if hours value is single-digit (< 10)
	   CharLCD('0');                                              // Print leading '0' for two-digit hour display
    U32LCD(rhrs);                                               // Display RTC hours on LCD
    CharLCD(':');                                               // Print ':' time separator on LCD
                                                                // Blank line before formatting minutes
	if(rmin<10)                                                   // Check if minutes value is single-digit (< 10)
	   CharLCD('0');                                              // Print leading '0' for two-digit minute display
    U32LCD(rmin);                                               // Display RTC minutes on LCD
    CharLCD(':');                                               // Print ':' time separator on LCD
                                                                // Blank line before formatting seconds
	if(rsec<10)                                                   // Check if seconds value is single-digit (< 10)
	   CharLCD('0');                                              // Print leading '0' for two-digit second display
    U32LCD(rsec);                                               // Display RTC seconds on LCD
}                                                               // Closing brace for DisplayDateTime body
void UARTDateTime(void)                                         // Define function to send RTC date and time over UART0
{                                                               // Opening brace for UARTDateTime body
    RTC_READ_TIME();	                                            // Read current time from RTC registers
    RTC_READ_DATE();                                            // Read current date from RTC registers
                                                                // Blank line before transmitting date over UART
                                                                // Indented blank line inside UARTDateTime
	U0_TxU32(rdate);                                              // Transmit RTC day over UART0
    UART0_Tx('/');                                              // Transmit '/' date separator over UART0
                                                                // Blank line between day and month output
    U0_TxU32(rmonth);                                             // Transmit RTC month over UART0
    UART0_Tx('/');                                              // Transmit '/' date separator over UART0
                                                                // Blank line between month and year output
    U0_TxU32(ryear);                                              // Transmit RTC year over UART0
                                                                // Blank line before space separator
    UART0_Tx(' ');                                              // Transmit space between date and time over UART0
                                                                // Blank line before hour output
	if(rhrs<10)                                                   // Check if hours value is less than 10
	   UART0_Tx('0');                                             // Transmit leading '0' for hours over UART0
    U0_TxU32(rhrs);                                               // Transmit RTC hours over UART0
    UART0_Tx(':');                                              // Transmit ':' time separator over UART0
   f                                                            // Stray character 'f' on line in pasted snippet
	if(rhrs<10)                                                   // Check if hours value is less than 10 before minutes
	   UART0_Tx('0');                                             // Transmit leading '0' over UART0
    U0_TxU32(rmin);                                               // Transmit RTC minutes over UART0
    UART0_Tx(':');                                              // Transmit ':' time separator over UART0
                                                                // Blank line before second output
	if(rhrs<10)                                                   // Check if hours value is less than 10 before seconds
	   UART0_Tx('0');                                             // Transmit leading '0' over UART0
    U0_TxU32(rsec);                                               // Transmit RTC seconds over UART0
                                                                // Blank line before trailing space
	UART0_Tx(' ');                                              // Transmit trailing space after timestamp over UART0
}                                                               // Closing brace for UARTDateTime body
void UART_PrintHeader(void)                                     // Define function to transmit log banner header to UART0
{                                                               // Opening brace for UART_PrintHeader body
U0_TxStr("\r\n");                                               // Transmit CRLF to start on a new line
U0_TxStr("================================================\r\n"); // Transmit top border line over UART0
U0_TxStr(" LPC2148 ATM SYSTEM\r\n");                           // Transmit system name over UART0
U0_TxStr(" TRANSACTION LOG\r\n");                             // Transmit log title over UART0
U0_TxStr("================================================\r\n"); // Transmit bottom border line over UART0
}                                                               // Closing brace for UART_PrintHeader body
void ShowEventTime(void)                                        // Define function to briefly display current RTC timestamp
{                                                               // Opening brace for ShowEventTime body
    CmdLCD(CLEAR_LCD);                                          // Clear LCD before displaying timestamp
    DisplayDateTime();                                          // Show current date and time on LCD
    delay_ms(2000);                                             // Keep timestamp visible on LCD for 2000 ms (2 seconds)
}                                                               // Closing brace for ShowEventTime body
                                                                // Blank line separating ShowEventTime and ReadNumber
u32 ReadNumber(void)                                            // Define function to read multi-digit number from keypad
{                                                               // Opening brace for ReadNumber body
    char buf[11];                                               // Buffer for up to 10 ASCII digits plus null terminator
    u32 len = 0;                                                // Current count of entered digits
    u32 keyV;                                                   // Decoded keypad key value
    u32 i;                                                      // Loop index variable
    u32 num = 0;                                                // Accumulated unsigned integer result
                                                                // Blank line before initializing buffer
    buf[0] = '\0';                                              // Initialize buffer as empty string
                                                                // Blank line before input loop
    while(1)                                                    // Loop until '#' confirms or 'D' cancels input
    {                                                           // Opening brace for ReadNumber while loop
        keyV = KeyScan();                                       // Wait for and read next pressed keypad key
                                                                // Blank line before waiting for key release
        while(ColScan() == 0);                                  // Wait until pressed key is released
                                                                // Blank line before checking key value
        if(keyV == '#')          /* Confirm */                  // Check if '#' confirmation key was pressed
        {                                                       // Opening brace for '#' branch
            if(len == 0)                                        // Check if no digits have been entered yet
                continue;                                       // Ignore '#' if empty and continue waiting
                                                                // Blank line before break
            break;                                              // Exit loop to convert entered digits to integer
        }                                                       // Closing brace for '#' branch
        else if(keyV == 'D')     /* Back to previous menu */    // Check if 'D' cancel/back key was pressed
        {                                                       // Opening brace for 'D' branch
            return INPUT_CANCEL;                                // Return cancel sentinel value
        }                                                       // Closing brace for 'D' branch
        else if(keyV == '*')     /* Clear complete field */     // Check if '*' clear key was pressed
        {                                                       // Opening brace for '*' branch
            len = 0;                                            // Reset digit count to 0
            buf[0] = '\0';                                      // Clear buffered string
        }                                                       // Closing brace for '*' branch
        else if(keyV == 'A')     /* Delete last digit */      // Check if 'A' backspace/delete key was pressed
        {                                                       // Opening brace for 'A' branch
            if(len > 0)                                         // Check if buffer has at least one digit
            {                                                   // Opening brace for delete non-empty branch
                len--;                                          // Decrement digit count
                buf[len] = '\0';                                // Null-terminate buffer at updated length
            }                                                   // Closing brace for delete non-empty branch
        }                                                       // Closing brace for 'A' branch
        else if(keyV >= '0' && keyV <= '9')                     // Check if pressed key is a digit ('0'..'9')
        {                                                       // Opening brace for digit branch
            if(len < 10)                                        // Enforce maximum 10 digits for u32 value
            {                                                   // Opening brace for append digit branch
                buf[len++] = (char)keyV;                        // Append digit character and increment length
                buf[len] = '\0';                                // Maintain null terminator
            }                                                   // Closing brace for append digit branch
        }                                                       // Closing brace for digit branch
        else                                                    // Any other unmapped key
        {                                                       // Opening brace for unhandled key branch
            continue;                                           // Skip LCD redraw and keep scanning
        }                                                       // Closing brace for unhandled key branch
                                                                // Blank line before LCD redraw
        /* Redraw the current input */                          // Comment noting LCD line 2 refresh
        CmdLCD(GOTO_LINE2_POS0);                                // Move cursor to start of line 2
        StrLCD("                ");                             // Clear line 2 with 16 spaces
        CmdLCD(GOTO_LINE2_POS0);                                // Return cursor to start of line 2
                                                                // Blank line before printing buffer digits
        for(i = 0; i < len; i++)                                // Loop through each entered digit
            CharLCD(buf[i]);                                    // Print digit onto LCD line 2
    }                                                           // Closing brace for ReadNumber while loop
                                                                // Blank line before numeric conversion
    for(i = 0; i < len; i++)                                    // Iterate over confirmed digits in buf
        num = num * 10 + (buf[i] - '0');                        // Accumulate base-10 integer from ASCII digits
                                                                // Blank line before return
    return num;                                                 // Return parsed unsigned 32-bit number
}                                                               // Closing brace for ReadNumber body
                                                                // Blank line separating ReadNumber and welcome
void welcome(void)                                              // Define main ATM welcome screen loop
{                                                               // Opening brace for welcome body
    u32 choice;                                                 // Variable storing user menu choice key
                                                                // Blank line before welcome loop
    while(1)                                                    // Infinite loop displaying welcome menu
    {                                                           // Opening brace for welcome while loop
	   // CheckInterruptRequest();                                 // Commented-out call to check RTC setup flag
                                                                // Blank line before LCD welcome banner
        CmdLCD(CLEAR_LCD);                                      // Clear LCD display
        CmdLCD(GOTO_LINE1_POS0);                                // Move cursor to line 1, position 0
        StrLCD("WELCOME TO ATM");                               // Display welcome banner on line 1
                                                                // Blank line before line 2 menu prompt
        CmdLCD(GOTO_LINE2_POS0);                                // Move cursor to line 2, position 0
        StrLCD("1.Login 2.Forgot");                             // Display login and forgot-PIN options on line 2
                                                                // Blank line before commented RTC flag block
      /* if (rtc_setup_requested)                               // Commented-out check for rtc_setup_requested
       {                                                        // Commented-out opening brace
        rtc_setup_requested = 0;                                // Commented-out reset of rtc_setup_requested
        RTC_SetupMode();                                        // Commented-out call to RTC_SetupMode()
       }*/                                                       // Commented-out closing brace
                                                                // Blank line before reading choice
        choice = KeyScan();                                     // Scan keypad for user selection
        while(ColScan() == 0);                                  // Wait until key is released
                                                                // Blank line before branching on choice
        if(choice == '1')                                       // Check if user selected '1' (Login)
        {                                                       // Opening brace for login branch
            login();                                            // Start login flow
        }                                                       // Closing brace for login branch
        else if(choice == '2')                                  // Check if user selected '2' (Forgot PIN)
        {                                                       // Opening brace for forgot_pin branch
            forgot_pin();                                       // Start PIN recovery flow
        }                                                       // Closing brace for forgot_pin branch
    }                                                           // Closing brace for welcome while loop
}                                                               // Closing brace for welcome body
                                                                // Closing single quote from pasted snippet
                                                                
void login(void)                                                                                      // Define function to authenticate account number and PIN
{                                                                                                     // Opening brace for login function body
    u32 a, p;                                                                                         // Variables for entered account number ('a') and entered PIN ('p')
    u32 attempts = 0;                                                                                 // Counter tracking failed login attempts
                                                                                                      // Blank line before resetting login_status
    login_status = 0;                                                                                 // Mark session as logged out at start of login flow
                                                                                                      // Blank line before login attempt loop
    while(attempts < 3)                                                                               // Allow up to 3 login attempts
    {                                                                                                 // Opening brace for login attempts loop
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen before account prompt
        StrLCD("Enter Acc No:");                                                                      // Prompt user to enter account number on line 1
        CmdLCD(GOTO_LINE2_POS0);                                                                      // Move cursor to line 2 for account number input
                                                                                                      // Blank line before reading account number
        a = ReadNumber();                                                                             // Read entered account number from keypad
                                                                                                      // Blank line before cancel check on account number
        if(a == INPUT_CANCEL)                                                                         // Check if user pressed cancel ('D')
            return;                                                                                   // Return early to previous menu
                                                                                                      // Blank line before prompting for PIN
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen before PIN prompt
        StrLCD("Enter PIN:");                                                                         // Prompt user to enter PIN on line 1
        CmdLCD(GOTO_LINE2_POS0);                                                                      // Move cursor to line 2 for PIN input
                                                                                                      // Blank line before reading PIN
        p = ReadNumber();                                                                             // Read entered PIN from keypad
                                                                                                      // Blank line before cancel check on PIN
        if(p == INPUT_CANCEL)                                                                         // Check if user pressed cancel ('D')
            return;                                                                                   // Return early to previous menu
                                                                                                      // Blank line before verifying account number and PIN
        if(a == acc_no && p == pin)                                                                   // Check if entered account number and PIN match stored credentials
        {                                                                                             // Opening brace for valid login branch
            login_status = 1;                                                                         // Set active session flag to logged in (1)
                                                                                                      // Blank line before login success display
            CmdLCD(CLEAR_LCD);                                                                        // Clear LCD display
            StrLCD("Login Success");                                                                  // Display login success message on LCD
		//	AddTransaction("Login Success");                                                              // Commented-out call to log login event in transaction history
                                                                                                      // Blank line before UART login logging
           UARTDateTime();                                                                            // Send current RTC date and time over UART0
           U0_TxStr(" | ACCOUNT : ");                                                                 // Send account prefix over UART0
           U0_TxU32(acc_no);                                                                          // Send account number over UART0
           U0_TxStr(" | LOGIN SUCCESS\r\n");                                                          // Send login success message and CRLF over UART0
            delay_ms(1000);                                                                           // Hold login success message for 1 second
					  ShowEventTime();                                                                            // Display timestamp of login event on LCD
                                                                                                      // Blank line before opening main ATM menu
            atm_menu();                                                                               // Enter authenticated main ATM menu
            return;                                                                                   // Return after exiting ATM menu
        }                                                                                             // Closing brace for valid login branch
                                                                                                      // Blank line before incrementing failed attempts
        attempts++;                                                                                   // Increment failed login attempt counter
		if (attempts == 3)                                                                            // Check if 3 failed login attempts have occurred
        {                                                                                             // Opening brace for account locked branch
            CmdLCD(CLEAR_LCD);                                                                        // Clear LCD display
            StrLCD("ACCOUNT LOCKED");                                                                 // Display account locked alert on LCD
            BuzzerAlert();                                                                            // Sound buzzer alert on failed login lockout
            login_status = 0;                                                                         // Ensure login_status remains 0
            return;                                                                                   // Exit login function
        	                                                                                            // Indented blank line inside attempts == 3 branch
		}                                                                                             // Closing brace for account locked branch
                                                                                                      // Blank line before invalid login notification
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD display
        StrLCD("Invalid Login");                                                                      // Inform user that login credentials were invalid
        delay_ms(1000);                                                                               // Hold invalid login message for 1 second
                                                                                                      // Blank line before next attempt loop iteration
    }                                                                                                 // Closing brace for login attempts loop
                                                                                                      // Blank line before fallback ATM locked message
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD display
    StrLCD("ATM LOCKED");                                                                             // Display ATM locked message on LCD
    delay_ms(2000);                                                                                   // Hold ATM locked message for 2 seconds
                                                                                                      // Blank line before resetting login_status
    login_status = 0;                                                                                 // Ensure login_status is set to 0
}                                                                                                     // Closing brace for login function body
                                                                                                      // Blank line separating login and atm_menu
void atm_menu(void)                                                                                   // Define main ATM operations menu loop
{                                                                                                     // Opening brace for atm_menu function body
    u32 choice;                                                                                       // Variable holding user's menu selection key
                                                                                                      // Blank line before authenticated menu loop
    while(login_status)                                                                               // Repeat menu while user is logged in
    {                                                                                                 // Opening brace for atm_menu while loop
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD before drawing menu
        StrLCD("1.Bal 2.Withdraw");                                                                   // Display balance enquiry and withdrawal options on line 1
        CmdLCD(GOTO_LINE2_POS0);                                                                      // Move cursor to start of line 2
        StrLCD("3.Dep 4.PIN");                                                                        // Display deposit and PIN change options on line 2
                                                                                                      // Blank line before reading keypad choice
        choice = KeyScan();                                                                           // Scan keypad for selected menu option
        while(ColScan() == 0);                                                                        // Wait until pressed key is released
                                                                                                      // Blank line before switch dispatch
        switch(choice)                                                                                // Select operation corresponding to pressed key
        {                                                                                             // Opening brace for menu switch statement
            case '1':                                                                                 // Option '1': Balance enquiry
                balance_enquiry();                                                                    // Display balance and log enquiry
                break;                                                                                // Break out of switch to refresh menu
                                                                                                      // Blank line separating switch cases
            case '2':                                                                                 // Option '2': Cash withdrawal
                withdraw();                                                                           // Execute cash withdrawal workflow
                break;                                                                                // Break out of switch to refresh menu
                                                                                                      // Blank line separating switch cases
            case '3':                                                                                 // Option '3': Cash deposit
                deposit();                                                                            // Execute cash deposit workflow
                break;                                                                                // Break out of switch to refresh menu
                                                                                                      // Blank line separating switch cases
            case '4':                                                                                 // Option '4': Change PIN
                change_pin();                                                                         // Execute PIN change workflow
                break;                                                                                // Break out of switch to refresh menu
                                                                                                      // Blank line separating switch cases
            case '5':                                                                                 // Option '5': Transaction history
                transaction_history();                                                                // View stored transaction history on LCD
                break;                                                                                // Break out of switch to refresh menu
                                                                                                      // Blank line separating switch cases
            case '6':                                                                                 // Option '6': Logout
                logout();                                                                             // Log out user session and return to welcome screen
                return;                                                                               // Return from atm_menu
                                                                                                      // Blank line separating switch cases
            default:                                                                                  // Any other unmapped key
                CmdLCD(CLEAR_LCD);                                                                    // Clear LCD display
                StrLCD("Invalid Choice");                                                             // Display invalid choice message on LCD
                delay_ms(1000);                                                                       // Hold error message for 1 second
                break;                                                                                // Break out of switch
        }                                                                                             // Closing brace for menu switch statement
    }                                                                                                 // Closing brace for atm_menu while loop
}                                                                                                     // Closing brace for atm_menu function body
                                                                                                      // Blank line before balance_enquiry
                                                                                                      // Blank line before balance_enquiry
void balance_enquiry(void)                                                                            // Define function to display account balance and log enquiry
{                                                                                                     // Opening brace for balance_enquiry function body
    unsigned int b;                                                                                   // Working copy of balance used for ASCII conversion
                                                                                                      // Blank line before LCD heading
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Balance:");                                                                               // Display "Balance:" prompt on line 1
                                                                                                      // Blank line before moving cursor to line 2
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to start of line 2
                                                                                                      // Blank line before copying balance
    b = balance;                                                                                      // Load current account balance into b
                                                                                                      // Blank line before manual digit-to-ASCII block
    /* Display balance one digit at a time */                                                         // Comment explaining digit-by-digit conversion to string
    {                                                                                                 // Opening brace for local formatting block
        char str[10];                                                                                 // Buffer holding ASCII digits of balance
        int i = 0, j;                                                                                 // Index i for digit count, j for reversal loop
        char temp;                                                                                    // Temporary char used while reversing string
                                                                                                      // Blank line before zero check
        if(b == 0)                                                                                    // Check if balance is 0
            str[i++] = '0';                                                                           // Store '0' character when balance is zero
                                                                                                      // Blank line before digit extraction loop
        while(b > 0)                                                                                  // Loop while digits remain in b
        {                                                                                             // Opening brace for digit extraction loop
            str[i++] = (b % 10) + '0';                                                                // Extract least-significant digit as ASCII char
            b = b / 10;                                                                               // Remove least-significant digit by dividing by 10
        }                                                                                             // Closing brace for digit extraction loop
                                                                                                      // Blank line before null termination
        str[i] = '\0';                                                                                // Null-terminate reversed digit string
                                                                                                      // Blank line before string reversal loop
        for(j = 0; j < i / 2; j++)                                                                    // Swap characters from ends toward middle to reverse order
        {                                                                                             // Opening brace for reversal loop
            temp = str[j];                                                                            // Save character from front index j
            str[j] = str[i - j - 1];                                                                  // Move character from corresponding back index to front
            str[i - j - 1] = temp;                                                                    // Place saved front character into back index
        }                                                                                             // Closing brace for reversal loop
                                                                                                      // Blank line before displaying formatted balance
        StrLCD((s8 *)str);                                                                            // Display formatted balance string on LCD line 2
    }                                                                                                 // Closing brace for local formatting block
                                                                                                      // Blank line before UART balance log
    delay_ms(2000);                                                                                   // Hold balance display on LCD for 2 seconds
        U0_TxStr("\r\n");                                                                             // Transmit CRLF over UART0
        UARTDateTime();                                                                               // Transmit current RTC date and time over UART0
        U0_TxStr(" | BALANCE ENQUIRY\r\n");                                                           // Log balance enquiry header over UART0
        U0_TxStr(" ACCOUNT : ");                                                                         // Log account label over UART0
        U0_TxU32(acc_no);                                                                             // Log account number over UART0
        U0_TxStr("\r\n");                                                                             // Transmit CRLF over UART0
        U0_TxStr(" AVAILABLE BALANCE : Rs.");                                                           // Log available balance label over UART0
        U0_TxU32(balance);                                                                            // Log account balance over UART0
        U0_TxStr("\r\n");                                                                             // Transmit CRLF over UART0
        U0_TxStr("----------------------------------------\r\n");                                     // Transmit separator line over UART0
		sprintf(msg, "BALANCE Rs.%u", balance);                                                         // Format balance summary into msg buffer
        AddTransaction(msg);                                                                          // Add balance enquiry entry to transaction history
                                                                                                      // Blank line before showing event timestamp
		ShowEventTime();                                                                              // Display timestamp of balance enquiry on LCD
}                                                                                                     // Closing brace for balance_enquiry function body
                                                                                                      // Blank line before withdraw
                                                                                                      // Blank line before withdraw
                                                                                                      // Blank line before withdraw
void withdraw(void)                                                                                   // Define cash withdrawal transaction function
{                                                                                                     // Opening brace for withdraw function body
    unsigned int amount;                                                                              // Variable storing requested withdrawal amount
                                                                                                      // Blank line before withdrawal prompt
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter Amount:");                                                                          // Prompt user for withdrawal amount on line 1
                                                                                                      // Blank line before reading withdrawal amount
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2 for amount entry
    amount = ReadNumber();                                                                            // Read withdrawal amount from keypad
	  if(amount == INPUT_CANCEL)                                                                        // Check if user cancelled entry using 'D'
    return;                                                                                           // Return to menu without withdrawing
                                                                                                      // Blank line before amount validation
    if(amount == 0)                                                                                   // Reject withdrawal amount of 0
    {                                                                                                 // Opening brace for zero-amount branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Invalid Amount");                                                                     // Display invalid amount message on LCD
    }else if(amount % 100 !=0 )                                                                       // Check if amount is not a multiple of Rs.100
    {                                                                                                 // Opening brace for non-multiple-of-100 branch
     CmdLCD(CLEAR_LCD);                                                                               // Clear LCD screen
     StrLCD("Use multiples");                                                                         // Display denomination rule on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2
     StrLCD("of Rs.100");                                                                             // Display "of Rs.100" on line 2
     delay_ms(1000);                                                                                  // Hold message for 1 second
     return;                                                                                          // Return early from withdraw
                                                                                                      // Blank line inside non-multiple-of-100 branch
    }else if(amount > balance)                                                                        // Check if requested amount exceeds available balance
    {                                                                                                 // Opening brace for low-balance branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Low Balance");                                                                        // Display low balance warning on LCD
		BuzzerAlert();                                                                                // Trigger buzzer alert on insufficient funds
		IOCLR0 = 1 << 21;                                                                             // Clear GPIO Port 0 pin 21 via IOCLR0
    }                                                                                                 // Closing brace for low-balance branch
    else                                                                                              // Withdrawal amount is valid and within balance
    {                                                                                                 // Opening brace for successful withdrawal branch
        balance = balance - amount;                                                                   // Deduct withdrawn amount from balance
                                                                                                      // Blank line before withdrawal success message
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Withdraw Success");                                                                   // Display withdrawal confirmation on LCD
		                                                                                                // Indented blank line inside successful withdrawal branch
    }                                                                                                 // Closing brace for successful withdrawal branch
                                                                                                      // Blank line before post-withdrawal delay and log
    delay_ms(1500);                                                                                   // Hold status message for 1.5 seconds
		/* Call only after withdrawal succeeds and balance is updated. */                                 // Comment noting UART and history logging step
         U0_TxStr("\r\n");                                                                            // Transmit CRLF over UART0
         UARTDateTime();                                                                              // Transmit current RTC date and time over UART0
         U0_TxStr(" | CASH WITHDRAWAL\r\n");                                                          // Log cash withdrawal header over UART0
         U0_TxStr(" ACCOUNT : ");                                                                        // Log account label over UART0
         U0_TxU32(acc_no);                                                                            // Log account number over UART0
         U0_TxStr("\r\n");                                                                            // Transmit CRLF over UART0
         U0_TxStr(" AMOUNT : Rs.");                                                                    // Log withdrawal amount label over UART0
         U0_TxU32(amount);                                                                            // Log withdrawal amount over UART0
         U0_TxStr("\r\n");                                                                            // Transmit CRLF over UART0
         U0_TxStr(" REMAINING BALANCE : Rs.");                                                         // Log remaining balance label over UART0
         U0_TxU32(balance);                                                                           // Log updated balance over UART0
         U0_TxStr("\r\n");                                                                            // Transmit CRLF over UART0
         U0_TxStr("----------------------------------------\r\n");                                    // Transmit separator line over UART0
        sprintf(msg, "WITHDRAW Rs.%u", amount);                                                       // Format withdrawal log entry into msg
        AddTransaction(msg);                                                                          // Append withdrawal entry to transaction history
		                                                                                                // Indented blank line before ShowEventTime
		ShowEventTime();                                                                              // Display timestamp of withdrawal on LCD
}                                                                                                     // Closing brace for withdraw function body
                                                                                                      // Blank line before deposit
                                                                                                      // Blank line before deposit
                                                                                                      // Blank line before deposit
                                                                                                      // Blank line before deposit
void deposit(void)                                                                                    // Define cash deposit transaction function
{                                                                                                     // Opening brace for deposit function body
    unsigned int amount;                                                                              // Variable storing deposit amount entered by user
                                                                                                      // Blank line before deposit prompt
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter Amount:");                                                                          // Prompt user for deposit amount on line 1
                                                                                                      // Blank line before reading deposit amount
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2 for amount entry
    amount = ReadNumber();                                                                            // Read deposit amount from keypad
                                                                                                      // Blank line before commented ReadNumber call
//	amount = ReadNumber();                                                                            // Commented-out duplicate ReadNumber call
   if (amount == INPUT_CANCEL)                                                                        // Check if user cancelled entry using 'D'
   {                                                                                                  // Opening brace for cancel branch
    return;                                                                                           // Return to menu without depositing
   }                                                                                                  // Closing brace for cancel branch
                                                                                                      // Blank line before deposit validation checks
   if(amount == 0)                                                                                    // Reject deposit amount of 0
    {                                                                                                 // Opening brace for zero-amount branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Invalid Amount");                                                                     // Display invalid amount message on LCD
    }else if(amount % 100 !=0 )                                                                       // Check if deposit amount is not a multiple of Rs.100
   {                                                                                                  // Opening brace for non-multiple-of-100 branch
     CmdLCD(CLEAR_LCD);                                                                               // Clear LCD screen
     StrLCD("Use multiples");                                                                         // Display denomination rule on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2
     StrLCD("of Rs.100");                                                                             // Display "of Rs.100" on line 2
    delay_ms(1000);                                                                                  // Hold message for 1 second
    return;                                                                                          // Return early from deposit
   }else if(amount > 4294967295U - balance)                                                           // Prevent 32-bit unsigned overflow when adding to balance
    {                                                                                                 // Opening brace for overflow protection branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Amount Too High");                                                                    // Display amount too high warning on LCD
    }                                                                                                 // Closing brace for overflow protection branch
    else                                                                                              // Deposit amount is valid and within safe range
    {                                                                                                 // Opening brace for successful deposit branch
        balance = balance + amount;                                                                   // Add deposit amount to account balance
                                                                                                      // Blank line before deposit success message
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Deposit Success");                                                                    // Display deposit confirmation on LCD
    }                                                                                                 // Closing brace for successful deposit branch
                                                                                                      // Blank line before post-deposit delay and log
    delay_ms(1500);                                                                                   // Hold status message for 1.5 seconds
/* Call only after deposit succeeds and balance is updated. */                                      // Comment noting UART and history logging step
               U0_TxStr("\r\n");                                                                      // Transmit CRLF over UART0
               UARTDateTime();                                                                        // Transmit current RTC date and time over UART0
               U0_TxStr(" | CASH DEPOSIT\r\n");                                                   // Log cash deposit header over UART0
               U0_TxStr(" ACCOUNT : ");                                                                  // Log account label over UART0
               U0_TxU32(acc_no);                                                                      // Log account number over UART0
               U0_TxStr("\r\n");                                                                      // Transmit CRLF over UART0
               U0_TxStr(" AMOUNT : Rs.");                                                             // Log deposit amount label over UART0
               U0_TxU32(amount);                                                                      // Log deposited amount over UART0
               U0_TxStr("\r\n");                                                                      // Transmit CRLF over UART0
               U0_TxStr(" UPDATED BALANCE : Rs.");                                                    // Log updated balance label over UART0
               U0_TxU32(balance);                                                                     // Log new balance over UART0
               U0_TxStr("\r\n");                                                                      // Transmit CRLF over UART0
               U0_TxStr("----------------------------------------\r\n");                              // Transmit separator line over UART0
                                                                                                      // Blank line before formatting deposit log
		sprintf(msg, "DEPOSIT Rs.%u", amount);                                                        // Format deposit log entry into msg
        AddTransaction(msg);                                                                          // Append deposit entry to transaction history
}                                                                                                     // Closing brace for deposit function body
                                                                                                      // Blank line separating deposit and logout
                                                                                                      // Blank line before logout
void logout(void)                                                                                     // Define user session logout function
{                                                                                                     // Opening brace for logout function body
    login_status = 0;                                                                                 // Clear login_status flag to end session
                                                                                                      // Blank line before logout LCD message
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Logged Out");                                                                             // Display logout message on LCD
                                                                                                      // Blank line before logout UART log
    delay_ms(1500);                                                                                   // Hold logout message for 1.5 seconds
	    UARTDateTime();                                                                               // Transmit current RTC date and time over UART0
        U0_TxStr(" | ACCOUNT : ");                                                                     // Log account label over UART0
        U0_TxU32(acc_no);                                                                             // Log account number over UART0
        U0_TxStr(" | LOGOUT\r\n");                                                                    // Log logout event over UART0
        U0_TxStr("----------------------------------------\r\n");                                     // Transmit divider line over UART0
        U0_TxStr(" SESSION COMPLETED\r\n");                                                            // Log session completion message over UART0
        U0_TxStr("================================================\r\n");                             // Transmit closing banner over UART0
                                                                                                      // Blank line before showing logout timestamp
	  ShowEventTime();                                                                                // Display logout timestamp on LCD
    welcome();                                                                                        // Return to welcome screen
}                                                                                                     // Closing brace for logout function body
                                                                                                      // Closing single quote from pasted snippet
void change_pin(void)                                                                                 // Define function to change the user's ATM PIN
{                                                                                                     // Opening brace for change_pin function body
    u32 old_pin, new_pin;                                                                             // Variables holding entered existing PIN and replacement PIN
                                                                                                      // Blank line before prompting for old PIN
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter Old PIN:");                                                                         // Prompt user to enter current PIN on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2 for old PIN input
                                                                                                      // Blank line before reading old PIN
    old_pin = ReadNumber();                                                                           // Read old PIN from keypad
                                                                                                      // Blank line before cancel check on old PIN
    if(old_pin == INPUT_CANCEL)                                                                       // Check if user pressed cancel ('D')
        return;                                                                                       // Return to menu without changing PIN
                                                                                                      // Blank line before verifying old PIN
    if(old_pin != pin)                                                                                // Check if entered old PIN does not match stored PIN
    {                                                                                                 // Opening brace for wrong PIN branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Wrong PIN");                                                                          // Display wrong PIN message on LCD
        delay_ms(1500);                                                                               // Hold error message for 1.5 seconds
        return;                                                                                       // Abort PIN change and return to menu
    }                                                                                                 // Closing brace for wrong PIN branch
                                                                                                      // Blank line before prompting for new PIN
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter New PIN:");                                                                         // Prompt user to enter new PIN on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2 for new PIN input
                                                                                                      // Blank line before reading new PIN
    new_pin = ReadNumber();                                                                           // Read new PIN from keypad
                                                                                                      // Blank line before cancel check on new PIN
    if(new_pin == INPUT_CANCEL)                                                                       // Check if user pressed cancel ('D')
        return;                                                                                       // Return to menu without updating PIN
                                                                                                      // Blank line before updating stored PIN
    pin = new_pin;                                                                                    // Store new_pin into active pin variable
                                                                                                      // Blank line before PIN changed confirmation
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("PIN Changed");                                                                          // Display PIN change confirmation on LCD
    delay_ms(1500);                                                                                   // Hold confirmation message for 1.5 seconds
                                                                                                      // Blank line before UART PIN change log
       UARTDateTime();                                                                                // Transmit current RTC date and time over UART0
       U0_TxStr(" | ACCOUNT : ");                                                                     // Transmit account prefix over UART0
       U0_TxU32(acc_no);                                                                             // Transmit account number over UART0
       U0_TxStr(" | PIN CHANGED\r\n");                                                                // Transmit PIN changed status and CRLF over UART0
                                                                                                      // Blank line before commented AddTransaction call
	//	AddTransaction("PIN CHANGED");                                                                // Commented-out call to record PIN change in history buffer
		ShowEventTime();                                                                              // Display timestamp of PIN change event on LCD
}                                                                                                     // Closing brace for change_pin function body
                                                                                                      // Blank line separating change_pin and AddTransaction
void AddTransaction(char *message)                                                                    // Define function to append a log entry into transaction_log
{                                                                                                     // Opening brace for AddTransaction function body
     u32 i;                                                                                           // Loop counter used when shifting entries in full buffer
      /* If history is full, remove the oldest transaction */                                         // Comment explaining eviction of oldest entry when full
       if (history_count == MAX_HISTORY)                                                              // Check if transaction_log has reached MAX_HISTORY entries
       {                                                                                              // Opening brace for full-buffer branch
            for (i = 0; i < MAX_HISTORY - 1; i++)                                                     // Shift older entries left from index 1..MAX_HISTORY-1 to 0..MAX_HISTORY-2
            {                                                                                         // Opening brace for shift loop
              strcpy(transaction_log[i], transaction_log[i + 1]);                                     // Copy next transaction string into current slot
            }                                                                                         // Closing brace for shift loop
           history_count--;                                                                           // Decrement count so new entry is placed at last slot
        }                                                                                             // Closing brace for full-buffer branch
   strcpy(transaction_log[history_count], message);                                                   // Copy new transaction message into transaction_log
   history_count++;                                                                                   // Increment stored transaction count
}                                                                                                     // Closing brace for AddTransaction function body
                                                                                                      // Blank line before transaction_history
                                                                                                      // Blank line before transaction_history
                                                                                                      // Blank line before transaction_history
void transaction_history(void)                                                                        // Define function to browse stored transactions on LCD
 {                                                                                                    // Opening brace for transaction_history function body
        u32 index;                                                                                    // Index of currently displayed transaction entry
        u32 i;u32 key;                                                                                // Column loop counter i and keypad scan key value
      if (history_count == 0)                                                                         // Check if no transactions are recorded yet
      {                                                                                               // Opening brace for empty history branch
          CmdLCD(CLEAR_LCD);                                                                          // Clear LCD screen
          StrLCD("Transaction");                                                                      // Display "Transaction" on line 1
          CmdLCD(GOTO_LINE2_POS0);                                                                    // Move cursor to start of line 2
          StrLCD("History Empty");                                                                    // Display "History Empty" on line 2
          delay_ms(2000);                                                                             // Hold empty message for 2 seconds
          return;                                                                                     // Return to ATM menu
       }                                                                                              // Closing brace for empty history branch
       index = history_count - 1;                                                                     // Start at most recent transaction entry
      while (1)                                                                                       // Loop allowing user to navigate history or exit
      {                                                                                               // Opening brace for history viewer loop
          CmdLCD(CLEAR_LCD);                                                                          // Clear LCD before rendering transaction entry
          /* Display transaction number */                                                            // Comment indicating line 1 shows transaction number
           StrLCD("Transaction ");                                                                    // Print "Transaction " label on line 1
           U32LCD(index + 1);                                                                         // Print 1-based transaction number on line 1
          /* Display transaction details */                                                           // Comment indicating line 2 shows transaction message
           CmdLCD(GOTO_LINE2_POS0);                                                                   // Move cursor to start of line 2
          for (i = 0; i < 16; i++)                                                                    // Loop across all 16 character positions on line 2
          {                                                                                           // Opening brace for 16-column display loop
            if (transaction_log[index][i] != '\0')                                                    // Check if character is before string null terminator
                CharLCD(transaction_log[index][i]);                                                   // Display character on LCD
            else                                                                                      // End of transaction string reached
                CharLCD(' ');                                                                         // Pad remaining columns with space character
           }                                                                                          // Closing brace for 16-column display loop
           key = KeyScan();                                                                           // Scan keypad for navigation or exit key
           while (ColScan() == 0);                                                                    // Wait until key is released
         if (key == 'A')                                                                              // Check if 'A' (Previous transaction) was pressed
         {                                                                                            // Opening brace for 'A' previous branch
        /* Previous transaction */                                                                    // Comment noting move to older transaction (lower index)
        if (index > 0)                                                                                // Ensure index does not go below 0
        index--;                                                                                      // Decrement index to view previous transaction
         }                                                                                            // Closing brace for 'A' previous branch
         else if (key == 'B')                                                                         // Check if 'B' (Next transaction) was pressed
         {                                                                                            // Opening brace for 'B' next branch
          /* Next transaction */                                                                      // Comment noting move to newer transaction (higher index)
          if (index < history_count - 1)index++;                                                      // Increment index if not already at latest transaction
         }                                                                                            // Closing brace for 'B' next branch
        else if (key == 'D')                                                                          // Check if 'D' (Exit history) was pressed
         {                                                                                            // Opening brace for 'D' exit branch
          /* Exit history */                                                                          // Comment noting return to previous menu
          return;                                                                                     // Exit transaction_history function
          }                                                                                           // Closing brace for 'D' exit branch
      }                                                                                               // Closing brace for history viewer loop
}                                                                                                     // Closing brace for transaction_history function body
                                                                                                      // Blank line before forgot_pin
                                                                                                      // Blank line before forgot_pin
                                                                                                      // Blank line before forgot_pin
                                                                                                      // Blank line before forgot_pin
void forgot_pin(void)                                                                                 // Define PIN recovery function using account number and DOB
{                                                                                                     // Opening brace for forgot_pin function body
    u32 entered_acc;                                                                                  // Account number entered by user
    u32 entered_dob;                                                                                  // Date of birth entered by user
    u32 new_pin;                                                                                      // New PIN entered after successful verification
                                                                                                      // Blank line before Step 1 account entry
    /* Step 1: Enter account number */                                                                // Step 1 comment: prompt for and check account number
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter Acc No:");                                                                          // Prompt user for account number on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2
                                                                                                      // Blank line before reading entered_acc
    entered_acc = ReadNumber();                                                                       // Read account number from keypad
                                                                                                      // Blank line before cancel check on entered_acc
    if(entered_acc == INPUT_CANCEL)                                                                   // Check if user cancelled entry ('D')
        return;                                                                                       // Return to welcome screen
                                                                                                      // Blank line before checking account match
    if(entered_acc != acc_no)                                                                         // Verify entered account number matches stored acc_no
    {                                                                                                 // Opening brace for invalid account branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Invalid Account");                                                                    // Display invalid account message on LCD
        delay_ms(1500);                                                                               // Hold message for 1.5 seconds
        return;                                                                                       // Abort PIN recovery
    }                                                                                                 // Closing brace for invalid account branch
                                                                                                      // Blank line before Step 2 DOB verification
    /* Step 2: Verify date of birth */                                                                // Step 2 comment: prompt for and check date of birth
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter DOB:");                                                                             // Prompt user for DOB on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2
                                                                                                      // Blank line before reading entered_dob
    entered_dob = ReadNumber();                                                                       // Read DOB from keypad
                                                                                                      // Blank line before cancel check on entered_dob
    if(entered_dob == INPUT_CANCEL)                                                                   // Check if user cancelled entry ('D')
        return;                                                                                       // Return to welcome screen
                                                                                                      // Blank line before checking DOB match
    if(entered_dob != dob)                                                                            // Verify entered DOB matches stored dob
    {                                                                                                 // Opening brace for wrong DOB branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Wrong DOB");                                                                              // Display wrong DOB message on LCD
        delay_ms(1500);                                                                               // Hold message for 1.5 seconds
        return;                                                                                       // Abort PIN recovery
    }                                                                                                 // Closing brace for wrong DOB branch
                                                                                                      // Blank line before Step 3 new PIN entry
    /* Step 3: Enter new PIN */                                                                       // Step 3 comment: prompt user for new PIN
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter New PIN:");                                                                         // Prompt user for new PIN on line 1
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Move cursor to line 2
                                                                                                      // Blank line before reading new_pin
    new_pin = ReadNumber();                                                                           // Read new PIN from keypad
                                                                                                      // Blank line before cancel check on new_pin
    if(new_pin == INPUT_CANCEL)                                                                       // Check if user cancelled entry ('D')
        return;                                                                                       // Return without updating PIN
                                                                                                      // Blank line before Step 4 updating PIN
    /* Step 4: Update PIN */                                                                          // Step 4 comment: update stored PIN and notify user
    pin = new_pin;                                                                                    // Save new_pin as active ATM PIN
                                                                                                      // Blank line before PIN updated confirmation
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("PIN Updated");                                                                          // Display PIN updated confirmation on LCD
    delay_ms(1500);                                                                                   // Hold confirmation for 1.5 seconds
}                                                                                                     // Closing brace for forgot_pin function body
                                                                                                      // Blank line separating forgot_pin and CheckInterruptRequest
void CheckInterruptRequest(void)                                                                      // Define helper to process pending RTC setup request flag
{                                                                                                     // Opening brace for CheckInterruptRequest function body
    if (rtc_setup_requested)                                                                          // Check if RTC setup request flag is non-zero
    {                                                                                                 // Opening brace for rtc_setup_requested branch
        rtc_setup_requested = 0;                                                                      // Clear RTC setup request flag
        RTC_SetupMode();                                                                              // Launch RTC setup menu
    }                                                                                                 // Closing brace for rtc_setup_requested branch
	rtc_setup_requested = 0;                                                                          // Ensure rtc_setup_requested flag is cleared to 0
}                                                                                                     // Closing brace for CheckInterruptRequest function body
 /*                                                                                                   // Start of commented-out legacy RTC_SetupMode block
void RTC_SetupMode(void)                                                                              // Commented-out prototype for legacy RTC_SetupMode
{                                                                                                     // Commented-out opening brace
    u32 date_value, time_value;                                                                       // Commented-out variables for packed date and time numbers
    u32 day, month, year;                                                                             // Commented-out date field variables
    u32 hour, minute, second;                                                                         // Commented-out time field variables
                                                                                                      // Commented-out blank line
    CmdLCD(CLEAR_LCD);                                                                                // Commented-out LCD clear call
    StrLCD("Date DDMMYYYY:");                                                                         // Commented-out date prompt string
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Commented-out cursor move to line 2
                                                                                                      // Commented-out blank line
    date_value = ReadNumber();                                                                        // Commented-out call reading DDMMYYYY value
                                                                                                      // Commented-out blank line
    if (date_value == INPUT_CANCEL)                                                                   // Commented-out cancel check on date input
        return;                                                                                       // Commented-out early return on cancel
                                                                                                      // Commented-out blank line
    day = date_value / 1000000U;                                                                      // Commented-out extraction of day component
    month = (date_value / 10000U) % 100U;                                                             // Commented-out extraction of month component
    year = date_value % 10000U;                                                                       // Commented-out extraction of year component
                                                                                                      // Commented-out blank line
    if (day < 1 || day > 31 ||                                                                        // Commented-out day lower/upper range check
        month < 1 || month > 12 ||                                                                    // Commented-out month range check
        year < 2000 || year > 2099)                                                                   // Commented-out year range check
    {                                                                                                 // Commented-out opening brace
        CmdLCD(CLEAR_LCD);                                                                            // Commented-out LCD clear call
        StrLCD("Invalid Date");                                                                       // Commented-out invalid date message
        delay_ms(1500);                                                                               // Commented-out 1.5s delay
        return;                                                                                       // Commented-out early return on invalid date
    }                                                                                                 // Commented-out closing brace
                                                                                                      // Commented-out blank line
    CmdLCD(CLEAR_LCD);                                                                                // Commented-out LCD clear call
    StrLCD("Time HHMMSS:");                                                                           // Commented-out time prompt string
    CmdLCD(GOTO_LINE2_POS0);                                                                          // Commented-out cursor move to line 2
                                                                                                      // Commented-out blank line
    time_value = ReadNumber();                                                                        // Commented-out call reading HHMMSS value
                                                                                                      // Commented-out blank line
    if (time_value == INPUT_CANCEL)                                                                   // Commented-out cancel check on time input
        return;                                                                                       // Commented-out early return on cancel
                                                                                                      // Commented-out blank line
    hour   = time_value / 10000U;                                                                     // Commented-out extraction of hour component
    minute = (time_value / 100U) % 100U;                                                              // Commented-out extraction of minute component
    second = time_value % 100U;                                                                       // Commented-out extraction of second component
                                                                                                      // Commented-out blank line
    if (hour > 23 || minute > 59 || second > 59)                                                      // Commented-out time range check
    {                                                                                                 // Commented-out opening brace
        CmdLCD(CLEAR_LCD);                                                                            // Commented-out LCD clear call
        StrLCD("Invalid Time");                                                                       // Commented-out invalid time message
        delay_ms(500);                                                                                // Commented-out 500ms delay
        return;                                                                                       // Commented-out early return on invalid time
    }                                                                                                 // Commented-out closing brace
                                                                                                      // Commented-out blank line
    RTC_SET_DATE(day, month, year);                                                                   // Commented-out RTC date write call
    RTC_SET_TIME(second, minute, hour);                                                               // Commented-out RTC time write call
                                                                                                      // Commented-out blank line
    CmdLCD(CLEAR_LCD);                                                                                // Commented-out LCD clear call
    StrLCD("RTC Updated");                                                                          // Commented-out RTC updated message
	return;                                                                                             // Commented-out return statement
                                                                                                      // Commented-out blank line
//    delay_ms(500);                                                                                  // Commented-out delay statement
	                                                                                                    // Commented-out indented blank line
}                                                                                                     // Commented-out closing brace
 */                                                                                                   // End of commented-out legacy RTC_SetupMode block
                                                                                                      // Blank line before active RTC_SetupMode
void RTC_SetupMode(void)                                                                              // Define interactive RTC setup mode menu
{                                                                                                     // Opening brace for RTC_SetupMode function body
    u32 choice;                                                                                       // Variable holding user's RTC setup choice
    while(1)                                                                                          // Loop displaying RTC setup options
    {                                                                                                 // Opening brace for RTC_SetupMode while loop
     CmdLCD(CLEAR_LCD);                                                                               // Clear LCD screen
     StrLCD("1.Time 2.Date");                                                                         // Display Time and Date menu options on line 1
     CmdLCD(GOTO_LINE2_POS0);                                                                         // Move cursor to line 2
     StrLCD("3.Exit");                                                                                 // Display Exit option on line 2
     choice = KeyScan();                                                                              // Scan keypad for setup selection
     while(ColScan() == 0);                                                                           // Wait until key is released
     switch(choice)                                                                                   // Dispatch RTC setup submenu based on choice
     {                                                                                                // Opening brace for RTC_SetupMode switch
      case '1':                                                                                       // Option '1': Configure RTC time
      RTC_TimeMenu();                                                                                 // Enter time configuration submenu
      break;                                                                                          // Break out of switch
      case '2':                                                                                       // Option '2': Configure RTC date
      RTC_DateMenu();                                                                                 // Enter date configuration submenu
      break;                                                                                          // Break out of switch
      case '3':                                                                                       // Option '3': Exit RTC setup mode
      CmdLCD(CLEAR_LCD);                                                                              // Clear LCD screen
      StrLCD("RTC Settings");                                                                         // Display "RTC Settings" on line 1
      CmdLCD(GOTO_LINE2_POS0);                                                                        // Move cursor to line 2
      StrLCD("Exited");                                                                               // Display "Exited" on line 2
      delay_ms(1000);                                                                                 // Hold exit message for 1 second
       welcome();                                                                                     // Return to welcome screen
      }                                                                                               // Closing brace for RTC_SetupMode switch
    }                                                                                                 // Closing brace for RTC_SetupMode while loop
}                                                                                                     // Closing brace for RTC_SetupMode function body
                                                                                                      // Blank line before RTC_TimeMenu
                                                                                                      // Blank line before RTC_TimeMenu
void RTC_TimeMenu(void)                                                                               // Define submenu for configuring RTC hours, minutes, and seconds
{                                                                                                     // Opening brace for RTC_TimeMenu function body
         u32 choice;                                                                                  // Variable holding selected time field option
         u32 value;                                                                                   // Variable holding numeric time value entered
         while(1)                                                                                     // Loop displaying time menu until '4' (Back) is chosen
      {                                                                                               // Opening brace for RTC_TimeMenu while loop
       CmdLCD(CLEAR_LCD);                                                                             // Clear LCD screen
       StrLCD("1.Hour 2.Min");                                                                          // Display Hour and Min options on line 1
       CmdLCD(GOTO_LINE2_POS0);                                                                       // Move cursor to line 2
       StrLCD("3.Sec 4.Back");                                                                         // Display Sec and Back options on line 2
       choice = KeyScan();                                                                            // Scan keypad for time menu selection
       while(ColScan() == 0);                                                                         // Wait until key is released
       switch(choice)                                                                                 // Dispatch time field update based on choice
        {                                                                                             // Opening brace for RTC_TimeMenu switch
       case '1':                                                                                      // Option '1': Set RTC hour
       CmdLCD(CLEAR_LCD);                                                                             // Clear LCD screen
       StrLCD("Enter Hour:");                                                                         // Prompt user to enter hour on line 1
       value = ReadNumber();                                                                          // Read hour value from keypad
       if(value == INPUT_CANCEL) break;                                                               // Cancel hour update if 'D' pressed
       if(value <= 23)                                                                                // Validate 24-hour range (0..23)
       {                                                                                              // Opening brace for valid hour branch
        RTC_READ_TIME();                                                                              // Read current RTC seconds and minutes
        RTC_SET_TIME(rsec, rmin, value);                                                              // Update RTC time with new hour value
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Hour Updated");                                                                       // Display hour update confirmation on LCD
}                                                                                                     // Closing brace for valid hour branch
else                                                                                                  // Hour value is greater than 23
{                                                                                                     // Opening brace for invalid hour branch
        CmdLCD(CLEAR_LCD);                                                                            // Clear LCD screen
        StrLCD("Invalid Hour");                                                                       // Display invalid hour message on LCD
}                                                                                                     // Closing brace for invalid hour branch
       delay_ms(1000);                                                                                // Hold status message for 1 second
       break;                                                                                         // Break out of switch
      case '2':                                                                                       // Option '2': Set RTC minute
      CmdLCD(CLEAR_LCD);                                                                              // Clear LCD screen
      StrLCD("Enter Minute:");                                                                        // Prompt user to enter minute on line 1
       value = ReadNumber();                                                                          // Read minute value from keypad
     if(value == INPUT_CANCEL) break;                                                                 // Cancel minute update if 'D' pressed
     if(value <= 59)                                                                                  // Validate minute range (0..59)
      {                                                                                               // Opening brace for valid minute branch
      RTC_READ_TIME();                                                                                // Read current RTC seconds and hours
      RTC_SET_TIME(rsec, value, rhrs);                                                                // Update RTC time with new minute value
      CmdLCD(CLEAR_LCD);                                                                              // Clear LCD screen
      StrLCD("Minute Updated");                                                                       // Display minute update confirmation on LCD
}                                                                                                     // Closing brace for valid minute branch
else                                                                                                  // Minute value is greater than 59
{                                                                                                     // Opening brace for invalid minute branch
      CmdLCD(CLEAR_LCD);                                                                              // Clear LCD screen
      StrLCD("Invalid Minute");                                                                       // Display invalid minute message on LCD
}                                                                                                     // Closing brace for invalid minute branch
       delay_ms(1000);                                                                                // Hold status message for 1 second
      break;                                                                                          // Break out of switch
    case '3':                                                                                         // Option '3': Set RTC second
    CmdLCD(CLEAR_LCD);                                                                                // Clear LCD screen
    StrLCD("Enter Second:");                                                                          // Prompt user to enter second on line 1
      value = ReadNumber();                                                                           // Read second value from keypad
     if(value == INPUT_CANCEL) break;                                                                 // Cancel second update if 'D' pressed
     if(value <= 59)                                                                                  // Validate second range (0..59)
     {                                                                                                // Opening brace for valid second branch
       RTC_READ_TIME();                                                                               // Read current RTC minutes and hours
       RTC_SET_TIME(value, rmin, rhrs);                                                               // Update RTC time with new second value
       CmdLCD(CLEAR_LCD);                                                                             // Clear LCD screen
       StrLCD("Second Updated");                                                                      // Display second update confirmation on LCD
     }                                                                                                // Closing brace for valid second branch
    else                                                                                              // Second value is greater than 59
    {                                                                                                 // Opening brace for invalid second branch
          CmdLCD(CLEAR_LCD);                                                                          // Clear LCD screen
          StrLCD("Invalid Second");                                                                   // Display invalid second message on LCD
}                                                                                                     // Closing brace for invalid second branch
          delay_ms(1000);                                                                             // Hold status message for 1 second
          break;                                                                                      // Break out of switch
        case '4':                                                                                     // Option '4': Return to RTC_SetupMode menu
         return;                                                                                      // Exit RTC_TimeMenu
    }                                                                                                 // Closing brace for RTC_TimeMenu switch
   }                                                                                                  // Closing brace for RTC_TimeMenu while loop
}                                                                                                     // Closing brace for RTC_TimeMenu function body
                                                                                                      // Blank line before RTC_DateMenu
                                                                                                      // Blank line before RTC_DateMenu
                                                                                                      // Blank line before RTC_DateMenu
void RTC_DateMenu(void)                                                                               // Define submenu for configuring RTC day, month, and year
{                                                                                                     // Opening brace for RTC_DateMenu function body
     u32 choice;                                                                                      // Variable holding selected date field option
     u32 value;                                                                                       // Variable holding numeric date value entered
while(1)                                                                                              // Loop displaying date menu until '4' (Back) is chosen
{                                                                                                     // Opening brace for RTC_DateMenu while loop
      CmdLCD(CLEAR_LCD);                                                                              // Clear LCD screen
      StrLCD("1.Day 2.Month");                                                                         // Display Day and Month options on line 1
       CmdLCD(GOTO_LINE2_POS0);                                                                       // Move cursor to line 2
       StrLCD("3.Year 4.Back");                                                                        // Display Year and Back options on line 2
choice = KeyScan();                                                                                   // Scan keypad for date menu selection
while(ColScan() == 0);                                                                                // Wait until key is released
switch(choice)                                                                                        // Dispatch date field update based on choice
{                                                                                                     // Opening brace for RTC_DateMenu switch
case '1':                                                                                             // Option '1': Set RTC day
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Enter Day:");                                                                                 // Prompt user to enter day on line 1
value = ReadNumber();                                                                                 // Read day value from keypad
if(value == INPUT_CANCEL) break;                                                                      // Cancel day update if 'D' pressed
if(value >= 1 && value <= 31)                                                                         // Validate day range (1..31)
{                                                                                                     // Opening brace for valid day branch
RTC_READ_DATE();                                                                                      // Read current RTC month and year
RTC_SET_DATE(value, rmonth, ryear);                                                                   // Update RTC date with new day value
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Day Updated");                                                                                // Display day update confirmation on LCD
}                                                                                                     // Closing brace for valid day branch
else                                                                                                  // Day value outside 1..31 range
{                                                                                                     // Opening brace for invalid day branch
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Invalid Day");                                                                                // Display invalid day message on LCD
}                                                                                                     // Closing brace for invalid day branch
delay_ms(1000);                                                                                       // Hold status message for 1 second
break;                                                                                                // Break out of switch
case '2':                                                                                             // Option '2': Set RTC month
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Enter Month:");                                                                               // Prompt user to enter month on line 1
value = ReadNumber();                                                                                 // Read month value from keypad
if(value == INPUT_CANCEL) break;                                                                      // Cancel month update if 'D' pressed
if(value >= 1 && value <= 12)                                                                         // Validate month range (1..12)
{                                                                                                     // Opening brace for valid month branch
RTC_READ_DATE();                                                                                      // Read current RTC day and year
RTC_SET_DATE(rdate, value, ryear);                                                                    // Update RTC date with new month value
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Month Updated");                                                                              // Display month update confirmation on LCD
}                                                                                                     // Closing brace for valid month branch
else                                                                                                  // Month value outside 1..12 range
{                                                                                                     // Opening brace for invalid month branch
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Invalid Month");                                                                              // Display invalid month message on LCD
}                                                                                                     // Closing brace for invalid month branch
delay_ms(1000);                                                                                       // Hold status message for 1 second
break;                                                                                                // Break out of switch
case '3':                                                                                             // Option '3': Set RTC year
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Enter Year:");                                                                                // Prompt user to enter year on line 1
value = ReadNumber();                                                                                 // Read year value from keypad
if(value == INPUT_CANCEL) break;                                                                      // Cancel year update if 'D' pressed
if(value >= 2000 && value <= 2099)                                                                    // Validate year range (2000..2099)
{                                                                                                     // Opening brace for valid year branch
RTC_READ_DATE();                                                                                      // Read current RTC day and month
RTC_SET_DATE(rdate, rmonth, value);                                                                   // Update RTC date with new year value
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Year Updated");                                                                               // Display year update confirmation on LCD
}                                                                                                     // Closing brace for valid year branch
else                                                                                                  // Year value outside 2000..2099 range
{                                                                                                     // Opening brace for invalid year branch
CmdLCD(CLEAR_LCD);                                                                                    // Clear LCD screen
StrLCD("Invalid Year");                                                                               // Display invalid year message on LCD
}                                                                                                     // Closing brace for invalid year branch
delay_ms(1000);                                                                                       // Hold status message for 1 second
break;                                                                                                // Break out of switch
case '4':                                                                                             // Option '4': Return to RTC_SetupMode menu
return;                                                                                               // Exit RTC_DateMenu
}                                                                                                     // Closing brace for RTC_DateMenu switch
}                                                                                                     // Closing brace for RTC_DateMenu while loop
}                                                                                                     // Closing brace for RTC_DateMenu function body
                                                                                                      // Trailing single quotes from pasted snippet

// Leading blank line from pasted snippet
void InitBuzzer(void)                                           // Define function to initialize buzzer GPIO pin (P0.5)
{                                                               // Opening brace for InitBuzzer function body
// Configure P0.5 as GPIO                                       // Comment noting P0.5 pin function selection via PINSEL0
PINSEL0 &= ~(3U << 10);                                         // Clear bits [11:10] of PINSEL0 to select GPIO mode for P0.5
// Set output LOW first: buzzer OFF                             // Comment noting initial safe low output state
IOCLR0 = BUZZER_PIN;                                            // Drive P0.5 LOW initially so buzzer starts turned off
// Configure P0.5 as output                                     // Comment noting direction register configuration for P0.5
IODIR0 |= BUZZER_PIN;                                           // Set bit 5 of IODIR0 to configure P0.5 as an output pin
}                                                               // Closing brace for InitBuzzer function body
void BuzzerAlert(void)                                          // Define function to trigger buzzer alert for 2 seconds
{                                                               // Opening brace for BuzzerAlert function body
// Buzzer ON                                                    // Comment indicating buzzer activation step
IOSET0 = BUZZER_PIN;                                            // Set P0.5 HIGH via IOSET0 to turn buzzer ON
delay_ms(2000);                                                 // Keep buzzer ON for 2000 ms (2 seconds)
// Buzzer OFF                                                   // Comment indicating buzzer deactivation step
IOCLR0 = BUZZER_PIN;                                            // Clear P0.5 LOW via IOCLR0 to turn buzzer OFF
}                                                               // Closing brace for BuzzerAlert function body
                                                                // Trailing single quotes from pasted snippet
