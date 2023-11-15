#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <time.h>
#include <math.h>

struct cstmrInfo
{
  char firstName[30], lastName[30];
  char phone[12];
  float acctBalance;
  long int acctNumber;
  char Password[20];
};

int createAcct(int *createStatus, long int *acctNumber);
long int genAcctNumber();
int homeScreen(void);
int loginMenu();
int loginAcct();
void acctStatement(struct cstmrInfo cstmrInfo1);
int deposit(struct cstmrInfo *cstmrInfo1);
bool transfer(struct cstmrInfo *cstmrInfo1);
int reMove(struct cstmrInfo *cstmrInfo1);

long int genAcctNumber()
{
  srand(time(NULL));
  int recurringDigits = 21;
  long int randEightDigitNum = 0;

  for (int i = 0; i < 8; i++)
  {
    randEightDigitNum = (randEightDigitNum * 10) + (rand() % 10);
  }

  long int generatedAcctNumber = (long int)(recurringDigits * (pow(10, 8))) + randEightDigitNum;

  return labs(generatedAcctNumber);
}

int createAcct(int *createStatus, long int *acctNumber)
{
  struct cstmrInfo cstmrInfo1;

  FILE *pCstmrInfo;

  char userFileName[30];

  printf("To create your account, enter the following details\n\n");

  printf("Enter your first name:\n");
  scanf("%s", cstmrInfo1.firstName);

  printf("Enter your last name:\n");
  scanf("%s", cstmrInfo1.lastName);

  printf("Enter your phone number:\n");
  scanf("%s", cstmrInfo1.phone);

  printf("Enter your password (at least 8 characters):\n");
  scanf("%s", cstmrInfo1.Password);

  cstmrInfo1.acctNumber = *acctNumber;

  // Concatenate, open, and save user's general info (struct cstmrInfo)
  snprintf(userFileName, sizeof(userFileName), "%ld.bin", cstmrInfo1.acctNumber);

  pCstmrInfo = fopen(userFileName, "wb");

  if (pCstmrInfo != NULL)
  {
    fwrite(&cstmrInfo1, sizeof(cstmrInfo1), 1, pCstmrInfo);
    cstmrInfo1.acctBalance = 0;
    *acctNumber = cstmrInfo1.acctNumber;
    *createStatus = 1;
    fclose(pCstmrInfo);
    return 1;
  }
  else
  {
    printf("Failed to open %s. Account registration failed. Try again.\n", userFileName);
    return 0;
  }
}

int homeScreen(void)
{
  int opt;
  printf("This is the New Bank.\n\nWhat would you want to do?\n\n");
  printf("1. Create an account\n\n2. Login to an existing account\n\n");
  printf("Choose an option: ");
  scanf("%d", &opt);
  return opt;
}

int loginMenu()
{
  int opt;
  printf("Select an option below\n\n");
  printf("1. Credit your account\n");
  printf("2. Make a transfer\n");
  printf("3. Check account balance\n");
  printf("4. Delete your account\n");
  printf("5. Log out\n\n");
  printf("Enter option: ");
  scanf("%d", &opt);
  return opt;
}

int loginAcct()
{
  struct cstmrInfo cstmrInfo1;
  long int acctNumber;
  char password[20];

  printf("Enter your account number: ");
  scanf("%ld", &acctNumber);
  printf("Enter your password: ");
  scanf("%s", password);

  char userFileName[30];
  snprintf(userFileName, sizeof(userFileName), "%ld.bin", acctNumber);

  FILE *pCstmrInfo;

  pCstmrInfo = fopen(userFileName, "rb");
  if (pCstmrInfo != NULL)
  {
    fread(&cstmrInfo1, sizeof(cstmrInfo1), 1, pCstmrInfo);
    if (strcmp(password, cstmrInfo1.Password) == 0)
    {
      fclose(pCstmrInfo);
      return 1;
    }
    else
    {
      printf("\nPassword mismatch! Try again.\n");
      fclose(pCstmrInfo);
      return 0;
    }
  }
  else
  {
    printf("\nAccount not registered. Proceed to create an account.\n");
    return 0;
  }
}

void acctStatement(struct cstmrInfo cstmrInfo1)
{
  printf("Your account balance is %.2f\n\n", cstmrInfo1.acctBalance);
}

int deposit(struct cstmrInfo *cstmrInfo1)
{
  float amount;
  FILE *pCstmrInfo;

  printf("Enter the amount you want to deposit: ");
  scanf("%f", &amount);

  char userFileName[30];
  snprintf(userFileName, sizeof(userFileName), "%ld.bin", cstmrInfo1->acctNumber);

  pCstmrInfo = fopen(userFileName, "rb+");
  if (pCstmrInfo != NULL)
  {
    fread(&cstmrInfo1, sizeof(struct cstmrInfo), 1, pCstmrInfo);
    long offset = ftell(pCstmrInfo) - sizeof(float);
    cstmrInfo1->acctBalance += amount;
    fseek(pCstmrInfo, offset, SEEK_SET);
    fwrite(&cstmrInfo1->acctBalance, sizeof(float), 1, pCstmrInfo);
    fclose(pCstmrInfo);
    return 1;
  }
  else
  {
    printf("Processing error: Unable to retrieve account information. Try again.\n");
    return 0;
  }
}

bool transfer(struct cstmrInfo *cstmrInfo1)
{
  long int acctNumber[2];
  float amount;

  printf("Enter your account number: ");
  scanf("%ld", &acctNumber[0]);
  printf("Enter the account number to transfer to: ");
  scanf("%ld", &acctNumber[1]);
  printf("Enter the amount to transfer: ");
  scanf("%f", &amount);

  char userFileName[30];
  char password[20];
  FILE *pCstmrInfo;
  struct cstmrInfo cstmrInfo2;

  // Concatenating the SENDER'S acctNumber to check if such a file exists before updating the balance
  snprintf(userFileName, sizeof(userFileName), "%ld.bin", acctNumber[0]);
  pCstmrInfo = fopen(userFileName, "rb+");
  if (pCstmrInfo != NULL)
  {
    fread(cstmrInfo1, sizeof(struct cstmrInfo), 1, pCstmrInfo);
    long offset = ftell(pCstmrInfo) - sizeof(float);
    cstmrInfo1->acctBalance -= amount;
    fseek(pCstmrInfo, offset, SEEK_SET);
    fwrite(&cstmrInfo1->acctBalance, sizeof(float), 1, pCstmrInfo);
    fclose(pCstmrInfo);
    printf("\nTransfer successful.\n");

    // Concatenating the RECEIVER'S acctNumber to update its balance
    snprintf(userFileName, sizeof(userFileName), "%ld.bin", acctNumber[1]);
    pCstmrInfo = fopen(userFileName, "rb+");
    if (pCstmrInfo != NULL)
    {
      fread(&cstmrInfo2, sizeof(struct cstmrInfo), 1, pCstmrInfo);
      long offset = ftell(pCstmrInfo) - sizeof(float);
      cstmrInfo2.acctBalance += amount;
      fseek(pCstmrInfo, offset, SEEK_SET);
      fwrite(&cstmrInfo2.acctBalance, sizeof(float), 1, pCstmrInfo);
      fclose(pCstmrInfo);
      printf("\nTransfer successful.\n");
      return true;
    }
    else
    {
      printf("\nTransfer error: Receiver's account not found.\n");
      return false;
    }
  }
  else
  {
    printf("\nTransfer error: Sender's account not found.\n");
    return false;
  }
}

int reMove(struct cstmrInfo *cstmrInfo1)
{
  long int acctNumber;

  printf("Enter your account number: ");
  scanf("%ld", &acctNumber);

  char userFileName[30];
  snprintf(userFileName, sizeof(userFileName), "%ld.bin", acctNumber);

  int status = remove(userFileName);

  if (status == 0)
  {
    printf("Account deleted successfully.\n");
  }
  else
  {
    printf("Account not found. Delete operation failed.\n");
  }

  return 0;
}

int main(void)
{
  int input;
  long int acctNumber;
  int loginStatus, createStatus;
  int menuOption;
  printf("Welcome to the New Bank. We keep your money in a way that's new.\n\n");

  while (1)
  {
    // Initialize createStatus to 0 to signify account not yet created
    createStatus = 0;

    input = homeScreen();

    // Create an account
    if (input == 1)
    {
      acctNumber = genAcctNumber();
      createStatus = createAcct(&createStatus, &acctNumber);
      if (createStatus == 1)
      {
        printf("Account created successfully!\n\n");
      }
    }
    // Login
    else if (input == 2)
    {
      loginStatus = loginAcct();
      if (loginStatus == 1)
      {
        struct cstmrInfo cstmrInfo1;
        // Load user's information for later use
        char userFileName[30];
        snprintf(userFileName, sizeof(userFileName), "%ld.bin", acctNumber);

        FILE *pCstmrInfo;
        pCstmrInfo = fopen(userFileName, "rb");
        if (pCstmrInfo != NULL)
        {
          fread(&cstmrInfo1, sizeof(struct cstmrInfo), 1, pCstmrInfo);
          fclose(pCstmrInfo);
        }

        // Entering into the user's main banking interface
        while (1)
        {
          menuOption = loginMenu();
          // Credit account
          if (menuOption == 1)
          {
            if (deposit(&cstmrInfo1))
            {
              printf("Your account has been credited successfully!\n\n");
              acctStatement(cstmrInfo1);
            }
            else
            {
              printf("Failed to complete the transaction\n");
            }
          }
          // Make a transfer
          else if (menuOption == 2)
          {
            if (transfer(&cstmrInfo1))
            {
              printf("Transaction complete.\n");
              acctStatement(cstmrInfo1);
            }
            else
            {
              printf("Transaction incomplete.\n");
            }
          }
          // Check balance
          else if (menuOption == 3)
          {
            acctStatement(cstmrInfo1);
          }
          // Delete account
          else if (menuOption == 4)
          {
            reMove(&cstmrInfo1);
            createStatus = 0;
            break;
          }
          // Log out
          else if (menuOption == 5)
          {
            break;
          }
          else
          {
            printf("Invalid option.\n");
          }
        }
      }
    }
    // Invalid option
    else
    {
      printf("Invalid option\n");
    }
  }

  return 0;
}
