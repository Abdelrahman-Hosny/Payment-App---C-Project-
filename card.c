// card.c  by abdelrahman Hosny 14/12/2022 

/* Includes */
#include <stdio.h>
#include <string.h>
#include "card.h"


/**************************************************  Functions Definitions  **********************************************************/
/* 1- Get Name */
EN_cardError_t getCardHolderName(ST_cardData_t* cardData)
{
	printf("\nCardholder's Name: ");
	char temp[1000] = { 0 };   /* input buffer to avoid overflow error */
	gets(temp);

	/* check for input length validity (20:24)*/
	if (strlen(temp) < 20  || strlen(temp) > 24)
	{
		printf("Wrong_Name, length!\n");
		return WRONG_NAME;
	}

	/* check for correct input format (alphabets and spaces only) */
	int i = 0;
	for (i = 0; i < strlen(temp); i++)
	{
		/* non alphabet character ? */
		if ( (*(temp+i) > 90 && *(temp + i) < 97)  ||  (*(temp + i) > 122 || *(temp + i) < 65) )
		{
			/* is it a space? */
			if (*(temp + i) != 32)
			{
				printf("Wrong_Name, not alphabet!\n");
				return WRONG_NAME;
			}
			else
			{
				/* all is good .. now store in the cardData*/
				cardData->cardHolderName[i] = *(temp + i);
			}
		}
	}
	//printf("Name_OK\n");
	return CARD_OK;
};


/* 2- Get Expity date function definition */
EN_cardError_t getCardExpiryDate(ST_cardData_t* cardData)
{
	printf("Card Expity Date in MM/YY format: ");
	char temp[1000] = {0};   /* input buffer to avoid overflow error */
	gets(temp);

	/* check for input length validity */
	if (strlen(temp)<5 || strlen(temp)>6)
	{
		printf("Wrong_EXP_Date, length\n");		
		return WRONG_EXP_DATE;
	}
	
	/* correct slash place */
	if (*(temp+2) != '/')
	{
		printf("Wrong_EXP_Date, the slash '/'\n");
		return WRONG_EXP_DATE;
	}

	/* User must enter digits only */
	if (  !isdigit(*(temp + 0)) || !isdigit(*(temp + 1)) || !isdigit(*(temp + 3)) || !isdigit(*(temp + 4))  )
	{
		printf("Wrong_EXP_Date, not digit\n");
		return WRONG_EXP_DATE;
	}

	/* months must be <= 12 */
	if (*(temp + 0) > (char)49)
	{
		printf("Wrong_EXP_Date, wrong month\n");
		return WRONG_EXP_DATE;
	}
	else if (*(temp + 0) == (char)49 && *(temp + 0) > (char)50)
	{
		printf("Wrong_EXP_Date, wrong month\n");
		return WRONG_EXP_DATE;
	}
		
	/* All is good .. now store in the cardData */
	int i = 0;
	for (i = 0; i < 6; i++)
	{
		cardData->cardExpirationDate[i] = *(temp + i);
	}

	//printf("Exp_Date_OK\n");
	return CARD_OK;
};


/* 3- Get Card PAN function definition */
EN_cardError_t getCardPAN(ST_cardData_t* cardData)
{
	printf("Card PAN: ");
	char temp[1000] = { 0 };
	gets( temp);

	/* check for input length validity */
	if (strlen(temp) < 16 || strlen(temp) > 19)
	{
		printf("WRONG PAN, length\n");
		return WRONG_PAN;
	}

	/* all input must be digits only */
	int i = 0;
	for (i = 0; i < strlen(temp); i++)
	{
		/* all chars are digits? */
		if (!isdigit(*(temp + 1)))
		{
			printf("WRONG PAN, non digits input\n");
			return WRONG_PAN;
		}
		else
		{
			/* All is good .. now store in cardData*/
			cardData->primaryAccountNumber[i] = *(temp + i);
		}
	}
	//printf("PAN_OK\n");
	return CARD_OK;
};




/**************************************************  Testing Functions definitions  **********************************************************/
/* 1- Testing the getName function */
void getCardHolderNameTest1(void) 
{
	ST_cardData_t new_card = {0};                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;       // pointer to the new card structure 

	getCardHolderName(new_card_p); 
};


/* 2- Testing the expiryDate function */
void getCardExpiryDateTest(void) 
{
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 

	getCardExpiryDate(new_card_p);
};


/* 3- Testing the card number function */
void getCardPANTest(void)
{
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 

	getCardPAN(new_card_p);
};




/**************************************************  Performing the tests  **********************************************************/
//int main() {

/* 1- Get Card Holder's Name */
/*	printf("Tester Name: Abdelrahman Hosny\n");
	printf("Function Name: getCardHolderName\n");

	printf("Test Case 1:\n");
	printf("Input Data: NULL\n");     // null
	printf("Expected Result: WRONG_NAME\n");
	printf("Actual Result: ");
	getCardHolderNameTest1();

	printf("\nTest Case 2:\n");
	printf("Input Data: hhh\n");    // too short (<20)
	printf("Expected Result: WRONG_NAME\n");
	printf("Actual Result: ");
	getCardHolderNameTest1();

	printf("\nTest Case 3:\n");
	printf("Input Data: hhhhhhhhhhhhhhhhhhhhhhhhhhhh\n");    // too long (>25) 
	printf("Expected Result: WRONG_NAME\n");
	printf("Actual Result: ");
	getCardHolderNameTest1();

	printf("\nTest Case 4:\n");
	printf("Input Data: Abdelrahman1 Ibrahim\n");    // contains non alphabet character
	printf("Expected Result: WRONG_NAME, not an alphabet\n");
	printf("Actual Result: ");
	getCardHolderNameTest1();

	printf("\nTest Case 5:\n");
	printf("Input Data: Abdelrahman Ibrahim \n");    // Correct Input
	printf("Expected Result: NAME_OK\n");
	printf("Actual Result: ");
	getCardHolderNameTest1();




/* 2- Get Card Expity Date */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: getCardExpiryDate\n");

	printf("Test Case: 1\n");
	printf("Input Data: 0\n");     // too short
	printf("Expected Result: WRONG_EXP_DATE\n");
	printf("Actual Result: \n ");
	getCardExpiryDateTest();

	printf("\nTest Case: 2\n");
	printf("Input Data: 12 25\n");     // wrong slash
	printf("Expected Result: WRONG_EXP_DATE\n");
	printf("Actual Result: \n ");
	getCardExpiryDateTest();

	printf("\nTest Case: 3\n");
	printf("Input Data: 12/a0\n");     // not digits
	printf("Expected Result: WRONG_EXP_DATE\n");
	printf("Actual Result: \n ");
	getCardExpiryDateTest();

	printf("\nTest Case: 4\n");
	printf("Input Data: 12/12/2022\n");     // too long
	printf("Expected Result: WRONG_EXP_DATE\n");
	printf("Actual Result: \n ");
	getCardExpiryDateTest();

	printf("\nTest Case: 5\n");
	printf("Input Data: 25/25\n");     // wrong month 
	printf("Expected Result: WRONG_EXP_DATE\n");
	printf("Actual Result: \n ");
	getCardExpiryDateTest();

	printf("\nTest Case: 6\n");
	printf("Input Data: 12/25\n");     // correct
	printf("Expected Result: WRONG_EXP_DATE\n");
	printf("Actual Result: \n ");
	getCardExpiryDateTest();




/* 3- Get Card PAN */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: getCardPAN\n");

	printf("Test Case 1:\n");
	printf("Input PAT: 0\n");                         // too short
	printf("Expected Result: WRONG_PAN\n");
	printf("Actual Result: ");
	getCardPANTest();

	printf("\nTest Case 2:\n");
	printf("Input PAT: 123456789012345678912\n");     // too long
	printf("Expected Result: WRONG_PAN\n");
	printf("Actual Result: ");
	getCardPANTest();

	printf("\nTest Case 3:\n");
	printf("Input PAT: 5vdv546231v5df451v\n");     // wrong format 
	printf("Expected Result: WRONG_PAN\n");
	printf("Actual Result: ");
	getCardPANTest();

	printf("\nTest Case: 4\n");
	printf("Input PAT: 012345678901234567\n");     // correct input
	printf("Expected Result: PAN_OK\n");
	printf("Actual Result: ");
	getCardPANTest();





	return 0;
}*/
