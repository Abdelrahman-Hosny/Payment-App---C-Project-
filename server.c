// server.c  by abdelrahman Hosny 14/12/2022 

/* Includes */
#include <stdio.h>
#include <string.h>
#include "server.h"
#include "../Card/card.h"
#include "../Terminal/terminal.h"
#include "../My_Headers/std_typedefs.h"


/********************************************** Databases and global variables ********************************************************/
ST_accountsDB_t accountsDB[255] = {
	{0000.00,   0   ,"0000000000000000"},  // the null case 
	{7000.00,RUNNING,"4863930011823834"},  //correct PAN
	{10000.0,BLOCKED,"5594444042696709"},  //correct PAN
	{10330.0,RUNNING,"5807007076043875"},  
	{10000.0,BLOCKED,"1010057076043875"},
	{12000.0,RUNNING,"5807057550043800"} };

ST_transaction_t TransactionsDB[255] = { 0 }; //Initial transactions data base (No transactions yet)
int g_Transaction_num = 1;                    //Initial transaction number 
int g_accountRefrence_index = 0;              //Account reference, changed when account is found in data base by function (isValidAccount)



/**************************************************  Functions Definitions  **********************************************************/

/* 1- All server logic .. Is Valid Account */
EN_transStat_t recieveTransactionData(ST_transaction_t* transData) 
{
/* First: Assign transaction Sequence Number, despite the operation success/failure status  */
	transData->transactionSequenceNumber = g_Transaction_num;

/* 1- Search for the account, Exists ? NO? FRAUD_CARD*/
	EN_serverError_t validAcc_return = isValidAccount(&transData->cardHolderData, accountsDB);
	if (validAcc_return == ACCOUNT_NOT_FOUND)
	{
		printf("\nFRAUD_CARD");
		transData->transState = FRAUD_CARD;
		saveTransaction(transData);
		return FRAUD_CARD;
	}
	else
	{
		//printf("\nAccount index in Database = %d", g_accountRefrence_index);
	}

/* 2- amount available ? */
	EN_serverError_t ValidAmount_return = isAmountAvailable(&transData->terminalData, accountsDB);
	if (ValidAmount_return == LOW_BALANCE)
	{
		printf("\nDECLINED_INSUFFECIENT_FUND");
		transData->transState = DECLINED_INSUFFECIENT_FUND;
		saveTransaction(transData);
		return DECLINED_INSUFFECIENT_FUND;
	}

/* 3- Account blocked ? */
	EN_serverError_t AccState_return = isBlockedAccount(accountsDB);
	if (AccState_return == BLOCKED_ACCOUNT)
	{
		printf("\nDECLINED_STOLEN_CARD");
		transData->transState = DECLINED_STOLEN_CARD;
		saveTransaction(transData);
		return DECLINED_STOLEN_CARD;
	}

/* 4- Transaction Can not be saved ? */
	EN_serverError_t save_return = saveTransaction(transData);
	if (save_return == SAVING_FAILED)
	{
		printf("\nINTERNAL_SERVER_ERROR ");
		transData->transState = INTERNAL_SERVER_ERROR;
		return INTERNAL_SERVER_ERROR;
	}

/* 5- Update the database with the new balance */
	accountsDB[g_accountRefrence_index].balance = accountsDB[g_accountRefrence_index].balance - transData->terminalData.transAmount;
	printf("\nNew balance = %f", accountsDB[g_accountRefrence_index].balance);

	printf("\nAPPROVED :)\n\nTHANK YOU!\n\n");
	transData->transState = APPROVED;
	saveTransaction(transData);

	return APPROVED;
};


/* 2- is Valid Account (Card PAN is stored in server accounts database?) */
EN_serverError_t isValidAccount(ST_cardData_t* cardData, ST_accountsDB_t* accountRefrence)
{
	/* Card Exists ? .. Search for card PAN in database */
	int i = 0; 
	for (i = 0; i < 255; i++)
	{
		if ( strcmp(accountsDB[i].primaryAccountNumber, cardData->primaryAccountNumber) == 0 ) //Must be Identical
		{
			//printf("\nSERVER_OK, Account Found");
			g_accountRefrence_index = i;
			return SERVER_OK;
		}
		else
		{
			g_accountRefrence_index = 0;
		}
	}
	printf("\nAccount NOT Found");
	return ACCOUNT_NOT_FOUND;
};


/* 3- What is the account state? RUNNING/BLOCKED */
EN_serverError_t isBlockedAccount(ST_accountsDB_t* accountRefrence)
{
	if (accountsDB[g_accountRefrence_index].state == RUNNING)
	{
		printf("\nSERVER_OK, Running Account");
		return SERVER_OK;
	}
	else
	{
		printf("\nBLOCKED_ACCOUNT");
		return BLOCKED_ACCOUNT;
	}
};


/* 4- Is the amount availabel ? the amount user requests is within balance ?  */
EN_serverError_t isAmountAvailable(ST_terminalData_t* termData, ST_accountsDB_t* accountRefrence)
{
	if (termData->transAmount <= accountsDB[g_accountRefrence_index].balance)
	{
		printf("\nSERVER_OK, Balance available");
		return SERVER_OK;
	}
	else
	{
		printf("\nLOW_BALANCE!");
		return LOW_BALANCE;
	}
};


/* 5- save all transaction data in the transactions database */
EN_serverError_t saveTransaction(ST_transaction_t* transData)
{
	/* 1- store all transaction data in the transactions database. */
	if (g_Transaction_num < 255)
	{
		printf("\nSaving.. ");
		TransactionsDB[g_Transaction_num].cardHolderData = transData->cardHolderData;    //1- card data 
		TransactionsDB[g_Transaction_num].terminalData = transData->terminalData;        //2- terminal data 
		TransactionsDB[g_Transaction_num].transState = transData->transState;            //3- transaction state 
		TransactionsDB[g_Transaction_num].transactionSequenceNumber = transData->transactionSequenceNumber; //4- transaction number 
		
		printf("\nTransaction data is saved successfully");
		g_Transaction_num++; //increment the transaction number only after saving the current one 
		return SERVER_OK;
	}
	else
	{
		printf("\nDatabase Overflow!");
		return SAVING_FAILED;
	}
};


/* 6- list all Saved Transactions from database */
void listSavedTransactions(void)
{
	int i;
	for (i = 1; i < g_Transaction_num; i++)
	{
		printf("\n\n#########################\n");
		printf("Transaction Sequence Number : %d\n", TransactionsDB[i].transactionSequenceNumber);
		printf("Transaction Date : %s\n", TransactionsDB[i].terminalData.transactionDate);
		printf("Transaction Amount : %f\n", TransactionsDB[i].terminalData.transAmount);

		switch (TransactionsDB[i].transState)
		{
		case 0: printf("Transaction State : APPROVED\n"); break;
		case 1: printf("Transaction State : DECLINED_INSUFFECIENT_FUND\n"); break;
		case 2: printf("Transaction State : DECLINED_STOLEN_CARD\n"); break;
		case 3: printf("Transaction State : FRAUD_CARD\n"); break;
		case 4: printf("Transaction State : INTERNAL_SERVER_ERROR\n"); break;
		}

		printf("Terminal Max Amount : %f\n", TransactionsDB[i].terminalData.maxTransAmount);
		printf("Cardholder Name : %s\n", TransactionsDB[i].cardHolderData.cardHolderName);
		printf("PAN : %s\n", TransactionsDB[i].cardHolderData.primaryAccountNumber);
		printf("Card Expiration Date: %s\n", TransactionsDB[i].cardHolderData.cardExpirationDate);
		printf("#########################\n");
	}
};






/**************************************************  Test Functions definitions  **********************************************************/
void recieveTransactionDataTest(void) 
{
	/* card module */
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 
	getCardHolderName(new_card_p);
	getCardExpiryDate(new_card_p);
	getCardPAN(new_card_p);


	/* terminal module */
	ST_terminalData_t new_terminal = { 0 };                 // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;      // pointer to the new termianl structure
	getTransactionDate(new_terminal_p);
	isCardExpired(new_card_p, new_terminal_p);
	getTransactionAmount(new_terminal_p);
	setMaxAmount(new_terminal_p, 20000);
	isBelowMaxAmount(new_terminal_p);

	/* server module */
	ST_transaction_t new_transaction = { new_card,new_terminal };                        // new transaction local structure 
	ST_transaction_t* new_transaction_p = &new_transaction;  // pointer to the new transaction structure
	recieveTransactionData(new_transaction_p);
};


void isValidAccountTest(void) 
{
	ST_cardData_t g_new_card = { 0 };                  // new card local structure 
	ST_cardData_t* g_new_card_p = &g_new_card;         // pointer to the new card structure 
	EN_cardError_t card_date = getCardPAN(g_new_card_p);

	isValidAccount(g_new_card_p, accountsDB);
};


void isBlockedAccountTest(void)
{
	printf("Enter account reference in Accounts Data Base: ");
	int temp;
	scanf_s("%d", &temp);
	g_accountRefrence_index = temp;
	isBlockedAccount(accountsDB);
};


void isAmountAvailableTest(void) 
{
	ST_terminalData_t new_terminal = { 0 };                    // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;         // pointer to the new termianl structure 
	EN_terminalError_t terminal_return = getTransactionAmount(new_terminal_p);

	if (terminal_return == TERMINAL_OK)
	{
		isAmountAvailable(new_terminal_p, accountsDB);
	}
};


void saveTransactionTest(void)
{
	/* card module */
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 
	getCardHolderName(new_card_p);
	getCardExpiryDate(new_card_p);
	getCardPAN(new_card_p);


	/* terminal module */
	ST_terminalData_t new_terminal = { 0 };                 // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;      // pointer to the new termianl structure
	getTransactionDate(new_terminal_p);
	isCardExpired(new_card_p, new_terminal_p);
	getTransactionAmount(new_terminal_p);
	setMaxAmount(new_terminal_p, 20000);
	isBelowMaxAmount(new_terminal_p);

	/* server module */
	ST_transaction_t new_transaction = { new_card,new_terminal };                        // new transaction local structure 
	ST_transaction_t* new_transaction_p = &new_transaction;  // pointer to the new transaction structure
	recieveTransactionData(new_transaction_p);

    /* Now Save to the database */
	saveTransaction(new_transaction_p);
};


void listSavedTransactionsTest(void)
{
	/* card module */
	ST_cardData_t new_card = { 0 };                // new card local structure 
	ST_cardData_t* new_card_p = &new_card;         // pointer to the new card structure 
	getCardHolderName(new_card_p);
	getCardExpiryDate(new_card_p);
	getCardPAN(new_card_p);


	/* terminal module */
	ST_terminalData_t new_terminal = { 0 };                 // new termianl local structure 
	ST_terminalData_t* new_terminal_p = &new_terminal;      // pointer to the new termianl structure
	getTransactionDate(new_terminal_p);
	isCardExpired(new_card_p, new_terminal_p);
	getTransactionAmount(new_terminal_p);
	setMaxAmount(new_terminal_p, 20000);
	isBelowMaxAmount(new_terminal_p);

	/* server module */
	ST_transaction_t new_transaction = { new_card,new_terminal };                        // new transaction local structure 
	ST_transaction_t* new_transaction_p = &new_transaction;  // pointer to the new transaction structure
	recieveTransactionData(new_transaction_p);

	/* Now show me the saved data */
	listSavedTransactions();

};





/**************************************************  Performing the tests  **********************************************************/
//int main()
//{
 /* 1- All server logic */
/*	printf("Tester Name: Abdelrahman Hosny\n");
	printf("Function Name: recieveTransactionData\n");
	printf("Test Case: 1\n");
	printf("Input Data: Correct Data \n");      
	printf("Expected Result:  \n");
	printf("Actual Result: ");
	recieveTransactionDataTest();

	printf("\n\nTest Case: 2\n");
	printf("Input Data: Wrong PAN \n");
	printf("Expected Result:  \n");
	printf("Actual Result: ");
	recieveTransactionDataTest();

 /* 2- is Valid Account (Card PAN is stored in server accounts database?)*/
/*	printf("Tester Name: Abdelrahman Hosny\n");
	printf("Function Name: isValidAccountTest\n");
	printf("Test Case 1:\n");
	printf("Input Data (CARD PAN): 8989974615436851\n");      // Wrong Card PAN
	printf("Expected Result: ACCOUNT_NOT_FOUND \n");
	printf("Actual Result: ");
	isValidAccountTest();

	printf("\n\nTest Case 2:\n");
	printf("Input Data (CARD PAN):5594444042696709\n");      // correct Card PAN
	printf("Expected Result: SERVER_OK, Account Found \n");
	printf("Actual Result: ");
	isValidAccountTest();



 /* 3- What is the account state? RUNNING/BLOCKED */
/*	printf("Tester Name: Abdelrahman Hosny\n");
	printf("Function Name: isBlockedAccountTest\n");
	printf("Test Case 1:\n");
	printf("Input Data (refernce to account in DataBase): 0 \n");      // Running Account
	printf("Expected Result: Running Account \n");
	printf("Actual Result: ");
	isBlockedAccountTest();

	printf("\n\nTest Case 2:\n");
	printf("Input Data (refernce to account in DataBase): 1 \n");      // Blocked Account 
	printf("Expected Result: BLOCKED_ACCOUNT \n");
	printf("Actual Result: ");
	isBlockedAccountTest();

	printf("\n\nTest Case 3:\n");
	printf("Input Data (refernce to account in DataBase): 2 \n");      // Running Account 
	printf("Expected Result: Running Account \n");
	printf("Actual Result: ");
	isBlockedAccountTest();



 /* 4- Is the amount availabel ? the amount user requests is within balance ?  */
/*	printf("Tester Name: Abdelrahman Hosny\n");
	printf("Function Name: isAmountAvailable\n");

	printf("Test Case 1:\n");
	printf("Input Data (refernce to account in DataBase): 0 (balance = 2000.0) \n");      
	printf("Input Data (Transaction Amount): 100.0\n"); //within balance range 
	printf("Expected Result: SERVER_OK, Balance available \n");
	printf("Actual Result: ");
	isAmountAvailableTest();

	printf("\n\nTest Case 2:\n");
	printf("Input Data (refernce to account in DataBase): 0 (balance = 2000.0) \n");
	printf("Input Data (Transaction Amount): 30000.0\n"); //Higher than balance
	printf("Expected Result: LOW_Balance! \n");
	printf("Actual Result: ");
	isAmountAvailableTest();

	printf("\n\nTest Case 3:\n");
	printf("Input Data (refernce to account in DataBase): 0 (balance = 2000.0) \n");
	printf("Input Data (Transaction Amount): 2000.0\n"); //Same Value
	printf("Expected Result: SERVER_OK, Balance available \n");
	printf("Actual Result: ");
	isAmountAvailableTest();



/* 5- save all transaction data in the transactions database */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: saveTransaction\n");
	printf("Test Case: 1\n");
	printf("Input Data: \n"); 
	printf("Expected Result: Transaction data is saved successfully \n");
	printf("Actual Result: ");
	saveTransactionTest();

	printf("\n\n\n\nTest Case: 2\n");
	printf("Input Data: \n");
	printf("Expected Result: Transaction data is saved successfully \n");
	printf("Actual Result: ");
	saveTransactionTest();




 /* 6- list all Saved Transactions from database */
/*	printf("\n\n\n\nTester Name: Abdelrahman Hosny\n");
	printf("Function Name: listSavedTransactions\n");
	printf("Test Case: 1\n");
	printf("Input Data: \n");
	printf("Expected Result: show saved transaction \n");
	printf("Actual Result: ");
	listSavedTransactionsTest();

	printf("\n\nTest Case: 2\n");
	printf("Input Data: \n");
	printf("Expected Result: show saved transaction \n");
	printf("Actual Result: ");
	listSavedTransactionsTest();


	return 0;
}*/