// app.c  by abdelrahman Hosny 14/12/2022 

/* Includes */
#include <stdio.h>
#include <string.h>
#include "app.h"
#include "../Card/card.h"
#include "../Server/server.h"
#include "../Terminal/terminal.h"
#include "../My_Headers/std_typedefs.h"


void appStart(void)
{
/*********************************************  Card Module ****************************************************/
	ST_cardData_t new_card = {0};     /* New Card*/
	ST_cardData_t* new_card_p = &new_card;
	
	EN_cardError_t card_name = getCardHolderName(new_card_p);	/* 1- Get card holder name */
	EN_cardError_t card_date = getCardExpiryDate(new_card_p);   /* 2- Get card expiration date */
	EN_cardError_t card_PAN = getCardPAN(new_card_p);			/* 3- Get Card PAN */





/************************************************** terminal Module *****************************************************/

	ST_terminalData_t new_terminal = {0};	 /* New Terminal structure */
	ST_terminalData_t* new_terminal_p = &new_terminal;

	/* 1- Is it a valid card? Correct PAN ? */
	EN_terminalError_t valid_card_PAN = isValidCardPAN(new_card_p);      
		if (valid_card_PAN == INVALID_CARD)		
		{
			printf("\nProcess Terminated.. Terminal: INVALID_CARD");
			return 0;
		}

	/* 2- Get Transaction date */
	EN_terminalError_t term_date = getTransactionDate(new_terminal_p);
		if (term_date == WRONG_DATE)		
		{
			printf("\nProcess Terminated.. Terminal: WRONG_DATE");
			return 0;
		}


	/* 3- Is card expired? */
	EN_terminalError_t cardExpiry_return = isCardExpired(new_card_p, new_terminal_p);
		if (cardExpiry_return == EXPIRED_CARD)
		{
			printf("\nProcess Terminated.. Terminal: EXPIRED_CARD");
			return 0;
		}


	/* 4- Get Transaction Amount */
	EN_terminalError_t trans_amount = getTransactionAmount(new_terminal_p);
		if (trans_amount == INVALID_AMOUNT)
		{
			printf("\nProcess Terminated.. Terminal: INVALID_AMOUNT");
			return 0;
		}


	/* 5- Set the maximum transaction amount */
	EN_terminalError_t term_max = setMaxAmount(new_terminal_p, 8000);
		if (term_max == INVALID_MAX_AMOUNT)
		{
			printf("\nProcess Terminated.. Terminal: INVALID_MAX_AMOUNT");
			return 0;
		}


	/* 6- is the Amount below the max ?*/
	EN_terminalError_t belowMax = isBelowMaxAmount(new_terminal_p);
		if (belowMax == EXCEED_MAX_AMOUNT)
		{
			printf("\nProcess Terminated.. Terminal: EXCEED_MAX_AMOUNT");
			return 0;
		}



/**************************************************** Server Module ***********************************************/
	ST_transaction_t new_trans = { 0 };     /* New Transaction Structure */
	ST_transaction_t* new_trans_p = &new_trans;

	new_trans.cardHolderData = new_card;
	new_trans.terminalData = new_terminal;


	/* 1- Is it valid Account? */
	/* 2- Is Amount available? */
	/* 3- Update Account balance */
	EN_transStat_t Server_validation = recieveTransactionData(new_trans_p);

};
