// terminal.c  by abdelrahman Hosny 14/12/2022 

/* Includes */
#include <stdio.h>
#include <string.h>
#include "terminal.h"
#include "../card/card.h"
#include "../My_Headers/std_typedefs.h"



/**************************************************  Functions Definitions  **********************************************************/

/* 1- Get Transaction Date function definition */
EN_terminalError_t getTransactionDate(ST_terminalData_t* termData) 
{
	printf("\nTransaction Date in DD/MM/YYYY foramt : ");
	char temp[100] = { 0 };   /* input buffer to avoid overflow error */
	gets(temp);

	/* 1- check for input length validity */
	if (strlen(temp) != 10)
	{
		printf("WRONG_DATE: Length ");
		return WRONG_DATE;
	}

	/* 2- check for input format validity (Slash) */     // DD/MM/YYYY
	if ( *(temp +2)  != '/' || *(temp + 5) != '/' ) 
	{
		printf("WRONG_DATE, Format - Slash");
		return WRONG_DATE;
	}

	/* 3- check for input format validity (Digits) */     // DD/MM/YYYY
	else if (!isdigit( *(temp + 0) ) || !isdigit( *(temp + 1) ) )  // Days
	{
		printf("WRONG_DATE: Format - Days");
		return WRONG_DATE;
	}
	else if (!isdigit(*(temp + 3)) || !isdigit(*(temp + 4) ) )    // Months
	{
		printf("WRONG_DATE: Format - Monthes");
		return WRONG_DATE;
	}
	else if (!isdigit(*(temp + 6)) || !isdigit(*(temp + 7)) || !isdigit(*(temp + 8)) || !isdigit(*(temp + 9) ) ) // Years 
	{
		printf("WRONG_DATE: Format - Years");
		return WRONG_DATE;
	}
	else  
	{
		/* all is good ..now store in termData */
		int i = 0;
		for (i = 0; i < strlen(temp); i++)
		{
			termData->transactionDate[i] = *(temp + i);
		}
		//printf("TERMINAL_OK, Date Accepted :)");
		return TERMINAL_OK;
	}
	
};


/* 2- Is Card Expired ? */
EN_terminalError_t isCardExpired(ST_cardData_t* cardData, ST_terminalData_t* termData)
{	
	// (MM/YY) vs (DD/MM/YYYY) 01/34/6789
	if (cardData->cardExpirationDate[3] < termData->transactionDate[8])  // Compare Years tens     
	{
		printf("\nEXPIRED_CARD!");
		return EXPIRED_CARD;
	}
	else if (cardData->cardExpirationDate[3] > termData->transactionDate[8])
	{
		return TERMINAL_OK;
	}
	else if (cardData->cardExpirationDate[4] < termData->transactionDate[9])  // Compare Years ones     
	{
		printf("\nEXPIRED_CARD!");
		return EXPIRED_CARD;
	}
	else if (cardData->cardExpirationDate[4] > termData->transactionDate[9])
	{
		return TERMINAL_OK;
	}
	else if (cardData->cardExpirationDate[0] < termData->transactionDate[3])  // Compare months tens     
	{
		printf("\nEXPIRED_CARD!");
		return EXPIRED_CARD;
	}
	else if (cardData->cardExpirationDate[0] > termData->transactionDate[3])  // Compare months tens     
	{
		return TERMINAL_OK;
	}
	else if (cardData->cardExpirationDate[1] < termData->transactionDate[4])  // Compare months ones     
	{
		printf("\nEXPIRED_CARD!");
		return EXPIRED_CARD;
	}
	else if (cardData->cardExpirationDate[1] > termData->transactionDate[4])  // Compare months ones     
	{
		return TERMINAL_OK;
	}
	else
	{
		//printf("\nTERMINAL_OK, Card is not expired");
		return TERMINAL_OK;
	}
};


/* 3- get Transaction amount */
EN_terminalError_t getTransactionAmount(ST_terminalData_t* termData)
{
	printf("Transaction amount: ");
	scanf_s(" %f", &termData->transAmount);

	if (termData->transAmount <= 0)
	{
		printf("INVALID_AMOUNT");
		return INVALID_AMOUNT;
	}
	else
	{
		//printf("TERMINAL_OK");
		return TERMINAL_OK;
	}
};


/* 4- Set the maximum transaction amount */
EN_terminalError_t setMaxAmount(ST_terminalData_t* termData, float maxAmount)
{
	if (maxAmount <= 0)
	{
		printf("\nINVALID_MAX_AMOUNT ");
		return INVALID_MAX_AMOUNT;
	}
	else 
	{
		termData->maxTransAmount = maxAmount;
		return TERMINAL_OK;
	}
};


/* 5- is the Amount below the max ? */
EN_terminalError_t isBelowMaxAmount(ST_terminalData_t* termData)
{
	if (termData->maxTransAmount < termData->transAmount)
	{
		printf("EXCEED_MAX_AMOUNT!");
		return EXCEED_MAX_AMOUNT;
	}
	else
	{
		//printf("TERMINAL_OK, Transaction Amount is below max ");
		return TERMINAL_OK;
	}
};


/* 6- is it a valid card PAN ?    (Optional)  */
EN_terminalError_t isValidCardPAN(ST_cardData_t* cardData)
{
	int length = strlen(cardData->primaryAccountNumber);
	int i = 0, odd_sum=0, even_sum=0, double_even = 0;
	if (length % 2 == 0) //the length is even (16 or 18)
	{
		/*1- add odd place */
		for (i = length - 1; i >= 0; i-=2)
		{
			odd_sum = odd_sum + cardData->primaryAccountNumber[i] - 48 ;  //(-48) to display the decimal value
		}
		//printf("\nOdd Sum = %d    ", odd_sum);

		/*2- add the double of the even places */
		for (i = length - 2; i >= 0; i-=2)
		{
			if ((int)cardData->primaryAccountNumber[i] < 5+48)
			{
				double_even = cardData->primaryAccountNumber[i] * 2 - 2 * 48;
				even_sum = even_sum + double_even;
			}
			else
			{
				double_even = 1 + ((2* (int)cardData->primaryAccountNumber[i]-2*48) % 10);
				even_sum = even_sum + double_even;
			}
		}
		//printf("even Sum = %d  \n", (int)even_sum);
	}

	else if (length % 2 == 1) //the length is odd (17 or 19)
	{
		/*1- add odd place */
		for (i = length - 2; i >= 0; i -= 2)
		{
			odd_sum = odd_sum + cardData->primaryAccountNumber[i] - 48;  //(-48) to display the decimal value
		}
		//printf("\nOdd Sum = %d   ", odd_sum);

		/*2- add the double of the even places */
		for (i = length - 1; i >= 0; i -= 2)
		{
			if ((int)cardData->primaryAccountNumber[i] < 5 + 48)
			{
				double_even = cardData->primaryAccountNumber[i] * 2 - 2 * 48;
				even_sum = even_sum + double_even;
			}
			else
			{
				double_even = 1 + ((2 * (int)cardData->primaryAccountNumber[i] - 2 * 48) % 10);
				even_sum = even_sum + double_even;
			}
		}
		//printf("even Sum = %d  \n", (int)even_sum);
	}


	/*3- (their sum)%10 == 0 ?  */
	if (((int)even_sum + (int)odd_sum) % 10 != 0 || ((((int)even_sum + (int)odd_sum)) <= 0) )
	{
		printf("Incorrect PAN!");
		return INVALID_CARD;
	}
	else
	{
		//printf("TERMINAL_OK, correct PAN");
		return TERMINAL_OK;
	}
}; 








/**************************************************  Test Functions definitions  **********************************************************/
/* 1-  get Transaction Date Test */
void getTransactionDateTest(void) 
{
	ST_terminalData_t new_terminal = { 0 };                    // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;         // pointer to the new termianl structure 

	getTransactionDate(new_terminal_p);
};


/* 2- Is Card Expired Test */
void isCardExpriedTest(void)
{
	ST_terminalData_t new_terminal = { 0 };                    // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;         // pointer to the new termianl structure  
	EN_terminalError_t tra_date = getTransactionDate(new_terminal_p);

	printf("\n\n"); // separator
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 
	EN_cardError_t card_date = getCardExpiryDate(new_card_p);

	if (!card_date && !tra_date) // No error
	{
		isCardExpired(new_card_p, new_terminal_p);
	}
	else
	{
		printf("\nInput Error");
	}
};


/* 3- get Transaction amount test */
void getTransactionAmountTest(void)
{
	ST_terminalData_t new_terminal = { 0 };                    // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;         // pointer to the new termianl structure 
	getTransactionAmount(new_terminal_p);
};


/* 4- Set the maximum transaction amount TEST */
void setMaxAmountTest(void)
{
	ST_terminalData_t new_terminal = { 0 };                    // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;         // pointer to the new termianl structure 
	printf("\nEnter Max Amount: ");
	float x;
	scanf_s("%f", &x);
	setMaxAmount(new_terminal_p,x);
};


/* 5- is the Amount below the max ? TEST*/
void isBelowMaxAmountTest(void)
{
	ST_terminalData_t new_terminal = { 0 };                    // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;         // pointer to the new termianl structure 
	setMaxAmount(new_terminal_p,10000);
	getTransactionAmount(new_terminal_p);
	isBelowMaxAmount(new_terminal_p);
};


/* 6- is it a valid card PAN TEST    (Optional)  */
void isValidCardPANTest(void)
{
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 
	getCardPAN(new_card_p);

	isValidCardPAN(new_card_p);
}






/**************************************************  Performing the tests  **********************************************************/
/*int main()
{
	
	/* 6- Correct PAN ?  */
/*	printf("Tester Name: Abdelrahman Hosny\n");
	printf("Function Name: isValidCardPAN\n");
	printf("Test Case: 1\n");
	printf("Input Data (PAN): 4863930011823834\n");
	printf("Expected Result: correct PAN\n");
	printf("Actual Result: ");
	isValidCardPANTest();

	printf("\n\nTest Case: 2\n");
	printf("Input Data (PAN): 4863930011823934\n");
	printf("Expected Result: Incorrect PAN\n");
	printf("Actual Result: ");
	isValidCardPANTest();

/* 1- Get Transaction Date */
  /*printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: getTransactionDate\n");
	printf("Test Case 1:\n");
	printf("Input Data: NULL\n");     // wrong length 
	printf("Expected Result: WRONG_DATE \n");
	printf("Actual Result: ");
	getTransactionDateTest();

	printf("\n\nTest Case 2:\n");
	printf("Input Data: 01 12 2022\n");     // wrong format (slash)
	printf("Expected Result: WRONG_DATE \n");
	printf("Actual Result: ");
	getTransactionDateTest();

	printf("\n\nTest Case 3:\n");
	printf("Input Data: a1/12/2022\n");     // wrong format (days)
	printf("Expected Result: WRONG_DATE \n");
	printf("Actual Result: ");
	getTransactionDateTest();

	printf("\n\nTest Case 4:\n");
	printf("Input Data: 01/bb/2022\n");     // wrong format (months)
	printf("Expected Result: WRONG_DATE \n");
	printf("Actual Result: ");
	getTransactionDateTest();

	printf("\n\nTest Case 5:\n");
	printf("Input Data: 01/12/aaaa\n");     // wrong format (years)
	printf("Expected Result: WRONG_DATE \n");
	printf("Actual Result: ");
	getTransactionDateTest();

	printf("\n\nTest Case 6:\n");
	printf("Input Data: 12/12/2022\n");     // Correct date
	printf("Expected Result: TERMINAL_OK \n");
	printf("Actual Result: ");
	getTransactionDateTest();




/* 2- Is card expired ? */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: isCardExpired\n");
	printf("Test Case: 1\n");
	printf("Input Data (Transaction Date): 12/12/2022\n");      // expired card
	printf("Input Data (Card Expiration Date): 12/21\n");     
	printf("Expected Result: EXPIRED_CARD \n");
	printf("Actual Result: ");
	isCardExpriedTest();

	printf("\n\nTest Case: 2\n");
	printf("Input Data (Transaction Date): 12/12/2022\n");      // input error
	printf("Input Data (Card Expiration Date): aa/22\n");
	printf("Expected Result: wrong input \n");
	printf("Actual Result: ");

	isCardExpriedTest();
	printf("\n\nTest Case: 3\n");
	printf("Input Data (Transaction Date): 12/12/2022\n");      
	printf("Input Data (Card Expiration Date): 12/25\n");      // running card
	printf("Expected Result: wrong input \n");
	printf("Actual Result: ");
	isCardExpriedTest();



/* 3- get transaction amount */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: getTransactionAmount\n");
	printf("Test Case 1:\n");
	printf("Input Data: 0\n");      // zero input
	printf("Expected Result: INVALID_AMOUNT\n");
	printf("Actual Result: ");
	getTransactionAmountTest();

	printf("\n\nTest Case 2:\n");
	printf("Input Data: 1000\n");      // int value
	printf("Expected Result: Terminal_OK\n");
	printf("Actual Result: ");
	getTransactionAmountTest();




/* 4- Set the maximum transaction amount */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: setMaxAmount\n");

	printf("Test Case 1:\n");
	printf("Input Data: 0\n");      // zero input
	printf("Expected Result: INVALID_MAX_AMOUNT\n");
	printf("Actual Result: ");
	setMaxAmountTest();

	printf("\n\nTest Case 2:\n");
	printf("Input Data: 1000\n");      // valid input
	printf("Expected Result: INVALID_MAX_AMOUNT\n");
	printf("Actual Result: ");
	setMaxAmountTest();




/* 5- is it Below Max Amount  */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: isBelowMaxAmount\n");

	printf("Test Case 1:\n");
	printf("Input Data (max amount): 10000\n");      
	printf("Input Data (Transaction Amount): 50000\n");      // Exceeding Limit 
	printf("Expected Result: EXCEED_MAX_AMOUNT\n");
	printf("Actual Result: ");
	isBelowMaxAmountTest();

	printf("\n\nTest Case: 2\n");
	printf("Input Data (max amount): 10000\n");
	printf("Input Data (Transaction Amount): 500\n");      // Below Limit 
	printf("Expected Result: TERMINAL_OK\n");
	printf("Actual Result: ");
	isBelowMaxAmountTest();







	return 0;
}*/