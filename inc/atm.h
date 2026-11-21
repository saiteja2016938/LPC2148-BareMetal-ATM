//atm.h                                               // Header file name comment for ATM system declarations

#ifndef ATM_H                                         // Start include guard: check if ATM_H is not yet defined
#define ATM_H                                         // Define ATM_H to prevent multiple inclusion of this header

void welcome(void);                                   // Declare function to display the welcome screen
void create_account(void);                            // Declare function to create a new user account
void login(void);                                     // Declare function to authenticate an existing user
void atm_menu(void);                                  // Declare function to display and handle the main ATM menu
void balance_enquiry(void);                           // Declare function to check and display account balance
void withdraw(void);                                  // Declare function to withdraw cash from the account
void deposit(void);                                   // Declare function to deposit funds into the account
void change_pin(void);                                // Declare function to change the user's PIN
void transaction_history(void);                       // Declare function to view past account transactions
void logout(void);                                    // Declare function to log out of the current session
void forgot_pin(void);                                // Declare function to recover or reset a forgotten PIN
void DisplayDateTime(void);                           // Declare function to display current RTC date and time
void ShowEventTime(void);                             // Declare function to display recorded event timestamp
void CheckInterruptRequest(void);                     // Declare function to check pending interrupt requests
void RTC_SetupMode(void);                             // Declare function to configure Real-Time Clock (RTC) settings

                                                      // Blank line separating declarations from include guard end
#endif                                                // End include guard for ATM_H
                                                      // Trailing newline at end of header file
