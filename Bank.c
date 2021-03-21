#include <stdio.h>
#include <string.h>
#include <stdbool.h>
#include <stdlib.h>

// STANDARD DEFINITIONS TO PROVIDE CROSS PLATFORM clear screen and pause commands
#if defined(_WIN32) || defined(_WIN64)
    #define clrscr() system("cls")
    #define pause() system("pause")
#else
    #define clrscr() printf("\x1B[1;1H\x1B[2J")
    #define pause() fflush(stdout); system("read -r -p \"Press any key to continue...\" key")
#endif

typedef struct{ //Stores the users first name, last name, card #, pin, cheqiung account balance, savings account balance, other account balance and whether they have overdraft protection(shown as a bool but stored in database as 1 or 0)

    char fName[50], lName[50];

    int chequing, savings, other, card, pin; 

    bool ODP;

    }

    Customer;

typedef struct{//Structure made so strings aren't directly held in 2-dimensional arrays (the latter of which proved to be problematic).

    char action[150];

}
History;

int withdrawFunds(int wallet,int withdrawAmount){//Finds the amount of $50 and $20 needed to complete the withdrawal of funds from the respective account.

    int num20=0, num50=0, remainingAmount;

    while (withdrawAmount%10!=0||withdrawAmount==10||withdrawAmount==30){//Railroads user into making sure they enter a valid input.

        printf("Please enter a valid amount to withdraw: ");
        scanf("%i",&withdrawAmount);

    }

    remainingAmount=withdrawAmount;

    while (remainingAmount>=0){//Loops until the whole amount is accounted for by 20 or 50 dollar bills.

        if (remainingAmount<100){

            if (remainingAmount%20==0){

                num20 = remainingAmount/20;
                remainingAmount -= num20*20;
                break;

            }

            else{

                num50++;
                remainingAmount-=50;//If the remaining amount is not cleanly divisible by 20, then another $50 is accounted for.

            }

        }

        else if (remainingAmount>=100){//Accounts for $50 if the remaining amount is higher than or equal to 100$.
        
            num50++;
            remainingAmount-=50;

        }

    }
    
    
    wallet = wallet-withdrawAmount*100;
    printf("\nYou obtained: \n\n%i x $50 bills \n%i x $20 bills\n\n",num50,num20);
    pause();

    return wallet;
}
void endMessage(){//Posts an ending message and exits the program on the user's input

    printf("Thank you for banking with Washington Mutual Bank. We hope you have a good day, and look forward to doing business in the future.\n");
    pause();
    exit(0); 

}
void addSessionHistory(int actionType,int *actionNum, int acc,char billerFirstName[50],char billerLastName[50],float transaction,History *history){//Adds individual actions as actions taken in the session.
    
    int z;
    char tempAction[150],account[20];
    z=*actionNum;
    
    if (acc==1){
        
        strcpy(account,"chequing");

    }

    else if (acc==2){

        strcpy(account,"savings");

    }

    else if (acc==3){

        strcpy(account,"TFSA/RRSP");

    }

    if (actionType==1){//Withdraw Action

        sprintf(history[z].action,"%i. Withdrew $%.2f from %s account\n",z+1, transaction,account);
    
    }

    else if (actionType==2){//Billing Action

        sprintf(history[z].action,"%i. Billed $%.2f from %s account to %s %s\n",z+1, transaction,account,billerFirstName,billerLastName);
    
    }

    else if (actionType==3){//Deposit Action

        sprintf(history[z].action,"%i. Deposited $%.2f to %s account\n",z+1, transaction,account);
    
    }
    
    *actionNum = *actionNum + 1;

}

void resetTransactionHistory(History *history,int *actionNum){//Deletes all previous session's transaction history. Useful for when starting a new session .
    
    int i;

    for (i=0;i<=(*actionNum);i++){

        strcpy(history[i].action,"");

    }

    *actionNum = 0;

}

void addCustomer(int *numCus,char firstName[50],char lastName[50], int cardNum, int pin,bool odp, Customer *customer){//Adds a customer by indexing the customer to the next empty space in the data structure.
    
    int z;
    
    z = *numCus;

    strcpy(customer[z].fName,firstName);
    strcpy(customer[z].lName,lastName);

    customer[z].card = cardNum;
    customer[z].pin = pin;
    customer[z].ODP = odp;

    customer[z].chequing = 0.0;
    customer[z].savings = 0.0;
    customer[z].other = 0.0;

    *numCus= *numCus+1;
}

void loadDatabase(int *numCus, Customer *customer) {//Loads in the whole database into the data structure Customer.

    int y,ODPchecker; 
    float chequing, savings, other;
    FILE* fp;

    fp = fopen("atmUsers.txt","r");

    fscanf(fp,"%i",numCus);

    for(y = 0; y<=*numCus;y++){//Scans in all of the data used in the Customer data structure

        fscanf(fp,"%s",customer[y].fName); 
        fscanf(fp,"%s",customer[y].lName);
        fscanf(fp,"%i",&customer[y].card); 
        fscanf(fp,"%i",&customer[y].pin);
        fscanf(fp,"%f",&chequing);
        fscanf(fp,"%f",&savings);
        fscanf(fp,"%f",&other);
        fscanf(fp,"%i",&ODPchecker); 

        if (ODPchecker == 0){//Sets the overdarft protection boolean according to whether there is a 1 or a 0 in its place

            customer[y].ODP = false;

        }

        else if(ODPchecker == 1){

            customer[y].ODP = true;

        }

        customer[y].other=other*100;
        customer[y].savings=savings*100;
        customer[y].chequing=chequing*100;

    }

    fclose(fp); 

} 

void userMenu(int cheq,int sav,int oth,char firstName[50],char lastName[50],bool overdraft){//Shows the user their balances, their name, along with a brief welcome message.

    printf("Hello %s %s! \nHere is your account overview:\n\n",firstName,lastName);

    if (overdraft==true){

        printf("Chequing Account:\t\t\t$%.2f + $1000.00 Overdraft Protection\n",(float)cheq/100);//Adds an overdaft text if the user does have overdraft protection.
    
    }

    else{

        printf("Chequing Account:\t\t\t$%.2f\n",(float)cheq/100);
    }

    printf("Savings Account:\t\t\t$%.2f\n",(float)sav/100);

    printf("TFSA/RRSP Account:\t\t\t$%.2f\n\n",(float)oth/100);

}
void updateDatabase(Customer *customer,int *numCustomers){//Deletes and completely rewrites database with the updated data structure
    int i;
    FILE *fp;

    fp = fopen("atmUsers.txt","w+");//File is effectively erased

    fprintf(fp,"%i\n",*numCustomers);//An integer keeps track of the number of accounts in the database, which helps when looping through the array of the Customer structure to find something, or to add a new user to the next empty slot.

    for (i=0;i<*numCustomers;i++){//New contents are added back into the file.

         fprintf(fp,"%s %s %i %i %.2f %.2f %.2f %i \n",customer[i].fName,customer[i].lName,customer[i].card,customer[i].pin,(float)customer[i].chequing/100,(float)customer[i].savings/100,(float)customer[i].other/100,customer[i].ODP);
    
    }

    fclose(fp);

}


int main(void) {
    char newFname[50],newLname[50],billerFName[50],billerLName[50];
    int enteredCard,enteredPin,numCustomers,x,y,n,billerCard,ODP,sessionCount=0,selection[99];
    bool cardMatch=false, billMatch=false,logout=false,quit=false, newODP=false,cont=true,invalidCard=false,fnameMatch=false,lnameMatch=false,billerCardMatch=false;
    float moneyWithdrawn=0,moneyBilled=0,moneyDeposited=0;
    Customer account[99];
    History  session[99];
    loadDatabase(&numCustomers,account);//Loads up database.

    while (quit==false){

        cardMatch=false, billMatch=false,logout=false,quit=false, newODP=false,cont=true,invalidCard=false;
        resetTransactionHistory(session,&sessionCount);//Makes sure transaction history from previous session is deleted, this would be a problem if the user chose to log out and this function didn't exist.
        clrscr();

        printf("Hello, welcome to Washington Mutual Bank! \nHow may we help you today?\n\n1.\tLogin \n2.\tMake a new account\n\n0.\tQuit\n\nCHOICE: ");
        scanf("%i",&selection[0]);

        while (selection[0]<0 && selection[0]>=2){//Error trap

            printf("Please enter a proper input: ");
            scanf("%i",&selection[0]);

        }

            if (selection[0]==1){

                while (logout==false){//Loop  to kick user out of "logged in" menus and actions, boolean used here is changed when user chooses to logout.

                    clrscr();

                    printf("Please enter your 8-digit card number: ");
                    scanf("%i",&enteredCard);

                    for (x=0;x<=numCustomers;x++){//Authentication of card number.

                        if(enteredCard==account[x].card){

                            cardMatch = true;
                            break;

                        }

                    }

                    if (!cardMatch){//Ask again for card # until it finds a match.

                        printf("Card number not found. Please try again\n");

                    }

                    else if(cardMatch){//Card number is authenticated.

                        clrscr();

                        printf("Please enter your 4-digit PIN:");//2nd round of security.
                        scanf("%i",&enteredPin);

                        while (account[x].pin!=enteredPin){//Error trapping the PIN input.

                            clrscr();

                            printf("Incorrect PIN, please enter your 4-digit PIN: ");
                            scanf("%i",&enteredPin);

                        }

                        if (enteredPin==account[x].pin){

                            while (cont==true){//Loop to continue to let customers take actions on their accounts without having to log in after each action. Also reversed when user selects to log out.

                                clrscr();
                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                printf("Please select one of the following:\n\n1.\tWithdraw funds\n2.\tPay bills\n3.\tDeposit funds\n4.\tTransaction History\n5.\tLog Out\n\n0.\tQuit\n\nCHOICE: ");
                                scanf("%i",&selection[1]);//User presented with actions they may take.

                                while (selection[1]<0||selection[1]>5){//Error trapping selection[1].

                                    clrscr();
                                    userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                    printf("Please enter a proper input.\n ");
                                    printf("Please select one of the following:\n\n1.\tWithdraw funds\n2.\tPay bills\n3.\tDeposit funds\n4.\tTransaction History\n\n0.\tQuit\n\nCHOICE: ");
                                    scanf("%i",&selection[1]);

                                }
                                //Withdrawing funds.
                                if (selection[1]==1){

                                    clrscr();
                                    userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                    printf("Type one of:\n1.\tChequing\n2.\tSavings\n3.\tOther/TFSA/RRSP\n\n0.\tQuit\n\nCHOICE: ");
                                    scanf("%i",&selection[2]);

                                    while (selection[2]>4&&selection[2]<0){//Error trapping selection[2].

                                        clrscr();

                                        printf("Please enter one of the following:\n1.\tChequing\n2.\tSavings\n3.\tOther/TFSA/RRSP\n\n0.\tQuit\n\nCHOICE:  ");
                                        scanf("%i",&selection[2]);

                                    }

                                    if (selection[2]==1){//Chequing

                                        if (account[x].chequing>2000||(account[x].ODP==true&&account[x].chequing+100000>2000)){//Bars user from withdrawing funds if their overdraft and/or their balance does not allow for any further withdrawals, because of how the withdrawals happen in either $20 or $50 bills.

                                            clrscr();

                                            userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                            printf("Please enter the amount you would like to withdraw. (Must be able to withdraw using only $50 and $20 bills): ");

                                            if (account[x].ODP==true){//Customer has overdraft protection (ODP)

                                                scanf("%f",&moneyWithdrawn);

                                                while (moneyWithdrawn*100>account[x].chequing+100000||moneyWithdrawn<=0){//ODP gives customer extra leeway with withdrawals.

                                                    printf("Please enter a valid amount to withdraw. (Must be able to withdraw using only $50 and $20 bills): ");
                                                    scanf("%f",&moneyWithdrawn);

                                                }

                                                account[x].chequing=withdrawFunds(account[x].chequing,moneyWithdrawn);
                                                addSessionHistory(selection[1],&sessionCount, selection[2],"", "", moneyWithdrawn, session);

                                                printf("Your new balance:$%.2f\n",(float)account[x].chequing/100);
                                                updateDatabase(account, &numCustomers);

                                            }

                                            else if (account[x].ODP==false){//Customer doesn't have ODP.

                                                scanf("%f",&moneyWithdrawn);

                                                while (moneyWithdrawn*100>account[x].chequing||moneyWithdrawn<=0){//Customer's balance not allowed to go below $0.

                                                    printf("Please enter a valid amount to withdraw. (Must be able to withdraw using only $50 and $20 bills): ");
                                                    scanf("%f",&moneyWithdrawn);

                                                }

                                                account[x].chequing=withdrawFunds(account[x].chequing,moneyWithdrawn);
                                                addSessionHistory(selection[1],&sessionCount, selection[2],"", "", moneyWithdrawn, session);

                                                printf("Your new balance:$%.2f\n",(float)account[x].chequing/100);
                                                updateDatabase(account, &numCustomers);//Update database with now new information.

                                            }

                                        }

                                        else if (account[x].chequing<=0){

                                            printf("You do not have enough funds to withdraw from this account. Please try another account or another action\n");
                                            pause();

                                        }

                                    }

                                    else if (selection[2]==2){//Savings

                                        if (account[x].savings>2000){//Checks to see if customer can actually withdraw a valid amount of money.

                                            clrscr();
                                            userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                            printf("Please enter the amount you would like to withdraw. (Must be able to withdraw using only $50 and $20 bills): ");
                                            scanf("%f",&moneyWithdrawn);

                                                while (moneyWithdrawn*100>account[x].savings||moneyWithdrawn<=0){//Error traps the amount withdrawn.

                                                    printf("Please enter a valid amount to withdraw. (Must be able to withdraw using only $50 and $20 bills): ");
                                                    scanf("%f",&moneyWithdrawn);

                                                }

                                                account[x].savings=withdrawFunds(account[x].savings,moneyWithdrawn);
                                                addSessionHistory(selection[1],&sessionCount, selection[2],"", "", moneyWithdrawn, session);

                                                printf("Your new balance:$%.2f\n",(float)account[x].savings/100);
                                                updateDatabase(account, &numCustomers);//Update database with now new information.

                                        }

                                        else if (account[x].savings<=0){//Error message if customer doesn't have enough funds.

                                            printf("You do not have enough funds to withdraw from this account. Please try another account or another action\n");
                                            pause();

                                        }

                                    }

                                    else if (selection[2]==3){//Other/TFSA/RRSP

                                        if (account[x].other>2000){//Checks to see if customer can actually withdraw a valid amount of money.

                                            clrscr();
                                            userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                            printf("Please enter the amount you would like to withdraw. (Must be able to withdraw using only $50 and $20 bills):");
                                            scanf("%f",&moneyWithdrawn);

                                                while (moneyWithdrawn*100>account[x].other||moneyWithdrawn<=0){//Error traps the amount withdrawn.

                                                    printf("Please enter a valid amount to withdraw. (Must be able to withdraw using only $50 and $20 bills): ");
                                                    scanf("%f",&moneyWithdrawn);

                                                }

                                                account[x].other=withdrawFunds(account[x].other,moneyWithdrawn);
                                                addSessionHistory(selection[1],&sessionCount, selection[2],"", "", moneyWithdrawn, session);

                                                printf("Your new balance:$%.2f\n",(float)account[x].other/100);
                                                updateDatabase(account, &numCustomers);//Update database with now new information.
                                                
                                        }

                                        else if (account[x].other<=0){//Error message if customer doesn't have enough funds.

                                            printf("You do not have enough funds to withdraw from this account. Please try another account or another action\n");
                                            pause();

                                        }

                                    }

                                    else if (selection[2]==0){//Quitting program.

                                        endMessage();

                                    }

                                }

                                else if (selection[1]==2){//Billing.

                                    clrscr();
                                    userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                    printf("Select an account to pay bills from: \n1.\tChequing \n2.\tSavings \n3.\tOther/TFSA/RRSP\n\nCHOICE:  ");
                                    scanf("%i",&selection[2]);

                                    while (selection[2]<1&&selection[2]>3){//Error traps selection[2].

                                        clrscr();
                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                        printf("Please enter a valid input: ");
                                        scanf("%i",&selection[2]);

                                    }

                                    if (selection[2]==1){//Chequing account.

                                        if (account[x].chequing>0||(account[x].ODP==true&&account[x].chequing+100000>0)){//Checks to see if customer has any money left to bill to someone else.

                                            clrscr();
                                            userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                            printf("Please enter the biller's first name: ");
                                            scanf("%s",billerFName);
                                            printf("Please enter the biller's last name: ");
                                            scanf("%s",billerLName);
                                            printf("Please enter the biller's 8-digit card number: ");
                                            scanf("%i",&billerCard);

                                            for (y=0;y<=numCustomers;y++){//Attempts to find all of the information scanned above in the database.

                                                if(strcmp(billerFName,account[y].fName)==0&&billerCard==account[y].card&&strcmp(billerLName,account[y].lName)==0){

                                                    billMatch = true;
                                                    break;

                                                }

                                            }

                                            while (billMatch==false){//Error trapping bill info.

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                printf("No matching account on record\n");
                                                printf("Please enter the biller's first name: ");
                                                scanf("%s",billerFName);
                                                printf("Please enter the biller's last name: ");
                                                scanf("%s",billerLName);
                                                printf("Please enter the biller's 8-digit card number: ");
                                                scanf("%i",&billerCard);

                                                for (y=0;y<=numCustomers;y++){//Attempts to find all of the information scanned above in the database.

                                                    if(strcmp(billerFName,account[y].fName)==0&&billerCard==account[y].card&&strcmp(billerLName,account[y].lName)==0){

                                                        billMatch = true;
                                                        break;

                                                    }

                                                }
                                            
                                            }
                                            if (billMatch==true){//Goes into this chunk when the biller's info mathces with someone from the database.

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                printf("Please enter the amount to be billed: ");
                                                scanf("%f",&moneyBilled);

                                                if (account[x].ODP==true){//Extra leeway given to customers with ODP.

                                                    while (moneyBilled<=0||moneyBilled*100>account[x].chequing+100000){//Error trapping moneyBilled with ODP in mind.

                                                        clrscr();
                                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                        printf("Please enter a valid amount to bill from your account: ");
                                                        scanf("%f",&moneyBilled);

                                                    }

                                                }

                                                else if (account[x].ODP==false){//Normal balance required if customer doesn't have ODP.

                                                    while (moneyBilled<=0||moneyBilled*100>account[x].chequing){//Normal error trapping moneyBilled

                                                        clrscr();
                                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                        printf("Please enter a valid amount to bill from your account: ");
                                                        scanf("%f",&moneyBilled);

                                                    }

                                                }

                                                account[y].chequing+=moneyBilled*100;
                                                account[x].chequing-=moneyBilled*100;

                                                addSessionHistory(selection[1],&sessionCount, selection[2],account[y].fName, account[y].lName, moneyBilled, session);//Adds their type of action, their action count (sessionCount),the account they took out the funds from, the reciever's first and last name, the amount of money transfered, and the structure used to store all that information in.
                                                printf("Successfully billed $%.2f to %s %s\n",moneyBilled,account[y].fName,account[y].lName);

                                                updateDatabase(account, &numCustomers);//Updates entire database with new information, so both reciever and giver have updated information.
                                                pause();

                                            }

                                        }

                                        else if (account[x].chequing<=0){//Error message if customer doesn't have enough funds.

                                            printf("You do not have enough funds to withdraw from this account. Please try another account or another action\n");
                                            pause();

                                        }  

                                    }

                                    else if (selection[2]==2){//Savings

                                        if (account[x].savings>0){//Checks to see if customer has any money left to bill to someone else.

                                            clrscr();
                                            userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                            printf("Please enter the biller's first name: ");
                                            scanf("%s",billerFName);
                                            printf("Please enter the biller's last name: ");
                                            scanf("%s",billerLName);
                                            printf("Please enter the biller's 8-digit card number: ");
                                            scanf("%i",&billerCard);

                                            for (y=0;y<=numCustomers;y++){//Attempts to find all of the information scanned above in the database.

                                                if(strcmp(billerFName,account[y].fName)==0&&billerCard==account[y].card&&strcmp(billerLName,account[y].lName)==0){

                                                    billMatch = true;
                                                    break;

                                                }

                                            }

                                            while (billMatch==false){//Error trapping bill info.

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                printf("No matching account on record\n");
                                                printf("Please enter the biller's first name: ");
                                                scanf("%s",billerFName);
                                                printf("Please enter the biller's last name: ");
                                                scanf("%s",billerLName);
                                                printf("Please enter the biller's 8-digit card number: ");
                                                scanf("%i",&billerCard);
                                                
                                                for (y=0;y<=numCustomers;y++){//Attempts to find all of the information scanned above in the database.

                                                    if(strcmp(billerFName,account[y].fName)==0&&billerCard==account[y].card&&strcmp(billerLName,account[y].lName)==0){

                                                        billMatch = true;
                                                        break;

                                                    }

                                                }

                                            }
                                            if (billMatch==true){//Goes into this chunk when the biller's info mathces with someone from the database.

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                printf("Please enter the amount to be billed: ");
                                                scanf("%f",&moneyBilled);

                                                    while (moneyBilled<=0||moneyBilled*100>account[x].savings){//Error traps moneyBilled

                                                        printf("Please enter a valid amount to bill from your account: ");
                                                        scanf("%f",&moneyBilled);

                                                    }
                                                
                                                account[y].chequing+=moneyBilled*100;
                                                account[x].savings-=moneyBilled*100;

                                                addSessionHistory(selection[1],&sessionCount, selection[2],account[y].fName, account[y].lName, moneyBilled, session);//Adds their type of action, their action count (sessionCount),the account they took out the funds from, the reciever's first and last name, the amount of money transfered, and the structure used to store all that information in.
                                                printf("Successfully billed $%.2f to %s %s\n",moneyBilled,account[y].fName,account[y].lName);

                                                updateDatabase(account, &numCustomers);//Updates entire database with new information, so both reciever and giver have updated information.
                                                pause();

                                            } 

                                        }

                                        else if (account[x].savings<=0){

                                            printf("You do not have enough funds to withdraw from this account. Please try another account or another action\n");
                                            pause();

                                        }

                                    }
                                    else if (selection[2]==3){//Other/TSFA/RRSP

                                        if (account[x].other>0){//Checks to see if customer has any money left to bill to someone else.

                                            clrscr();
                                            userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                            printf("Please enter the biller's first name: ");
                                            scanf("%s",billerFName);
                                            printf("Please enter the biller's last name: ");
                                            scanf("%s",billerLName);
                                            printf("Please enter the biller's 8-digit card number: ");
                                            scanf("%i",&billerCard);

                                            for (y=0;y<=numCustomers;y++){//Attempts to find all of the information scanned above in the database.

                                                if(strcmp(billerFName,account[y].fName)==0&&billerCard==account[y].card&&strcmp(billerLName,account[y].lName)==0){

                                                    billMatch = true;
                                                    break;

                                                }

                                            }
                                            while (billMatch==false){//Error trapping bill info.

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                printf("No matching account on record\n");
                                                printf("Please enter the biller's first name: ");
                                                scanf("%s",billerFName);
                                                printf("Please enter the biller's last name: ");
                                                scanf("%s",billerLName);
                                                printf("Please enter the biller's 8-digit card number: ");
                                                scanf("%i",&billerCard);

                                                for (y=0;y<=numCustomers;y++){//Attempts to find all of the information scanned above in the database.

                                                    if(strcmp(billerFName,account[y].fName)==0&&billerCard==account[y].card&&strcmp(billerLName,account[y].lName)==0){

                                                        billMatch = true;
                                                        break;

                                                    }
                                                }
                                            }
                                            if (billMatch==true){//Goes into this chunk when the biller's info mathces with someone from the database.

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);

                                                printf("Please enter the amount to be billed: ");
                                                scanf("%f",&moneyBilled);

                                                    while (moneyBilled<=0||moneyBilled*100>account[x].other){//Error traps moneyBilled

                                                        clrscr();
                                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                                        printf("Please enter a valid amount to bill from your account: ");
                                                        scanf("%f",&moneyBilled);

                                                    }
                                                
                                                account[y].chequing+=moneyBilled*100;
                                                account[x].other-=moneyBilled*100;

                                                addSessionHistory(selection[1],&sessionCount, selection[2],account[y].fName, account[y].lName, moneyBilled, session);//Adds their type of action, their action count (sessionCount),the account they took out the funds from, the reciever's first and last name, the amount of money transfered, and the structure used to store all that information in.
                                                printf("Successfully billed $%.2f to %s %s\n",moneyBilled,account[y].fName,account[y].lName);

                                                updateDatabase(account, &numCustomers);//Updates entire database with new information, so both reciever and giver have updated information.
                                                pause();

                                            }  
                                        }
                                        else if (account[x].other<=0){//Error message if customer doesn't have enough funds.

                                            printf("You do not have enough funds to withdraw from this account. Please try another account or another action\n");
                                            pause();

                                        }

                                    }

                                }
                                //Depositing
                                else if (selection[1]==3){

                                    clrscr();
                                    userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                    
                                    printf("Select the account you want to deposit funds into: \n1.\tChequing\n2.\tSavings\n3.\tOther/TFSA/RRSP\n\nCHOICE: ");
                                    scanf("%i",&selection[2]);

                                    while (selection[2]<1&&selection[2]>3){//Error trapping selection[2]

                                        printf("Please enter a proper input: ");
                                        scanf("%i",&selection[2]);

                                    }                    
                                    if (selection[2]==1){//Chequing

                                        clrscr();
                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                        moneyDeposited=0;

                                            while (moneyDeposited!=-1){//Loop until the user enters -1

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                                printf("Please enter how much you are going to deposit into your account(-1 to stop): ");
                                                scanf("%f",&moneyDeposited);

                                                while (moneyDeposited!=-1&&moneyDeposited<-1){//Error trapping the deposit amount.

                                                    printf("Please enter a valid amount");
                                                    scanf("%f",&moneyDeposited);

                                                }
                                                if (moneyDeposited!=-1){

                                                    account[x].chequing= account[x].chequing + moneyDeposited*100;//Adds moneyDeposited to the account balance ONLY if it isn't -1.
                                                    addSessionHistory(selection[1],&sessionCount, selection[2],"","", moneyDeposited, session);//Adds their type of action, their action count (sessionCount),the account they deposited the funds to, the amount of money deposited, and the structure used to store all that information in.
                                                    updateDatabase(account, &numCustomers);//Updates entire database with new information.

                                                }

                                            }

                                        
                                    }
                                    else if (selection[2]==2){//Savings

                                        clrscr();
                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                        moneyDeposited=0;

                                            while (moneyDeposited!=-1){//Loop until the user enters -1

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                                printf("Please enter how much you are going to deposit into your account(-1 to stop): ");
                                                scanf("%f",&moneyDeposited);

                                                while (moneyDeposited!=-1&&moneyDeposited<-1){//Error trapping the deposit amount.

                                                    printf("Please enter a valid amount");
                                                    scanf("%f",&moneyDeposited);

                                                }
                                                if (moneyDeposited!=-1){

                                                    account[x].savings= account[x].savings + moneyDeposited*100;//Adds moneyDeposited to the account balance ONLY if it isn't -1.
                                                    addSessionHistory(selection[1],&sessionCount, selection[2],"","", moneyDeposited, session);//Adds their type of action, their action count (sessionCount),the account they deposited the funds to, the amount of money deposited, and the structure used to store all that information in.
                                                    updateDatabase(account, &numCustomers);//Updates entire database with new information.

                                                }
                                            }
                                    }
                                    else if (selection[2]==3){

                                        clrscr();
                                        userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                        moneyDeposited=0;

                                            while (moneyDeposited!=-1){//Loop until the user enters -1

                                                clrscr();
                                                userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                                printf("Please enter how much you are going to deposit into your account(-1 to stop): ");
                                                scanf("%f",&moneyDeposited);
                                                
                                                while (moneyDeposited!=-1&&moneyDeposited<-1){//Error trapping the deposit amount.

                                                    printf("Please enter a valid amount");
                                                    scanf("%f",&moneyDeposited);

                                                }

                                                if (moneyDeposited!=-1){

                                                    account[x].other= account[x].other + moneyDeposited*100;//Adds moneyDeposited to the account balance ONLY if it isn't -1.
                                                    addSessionHistory(selection[1],&sessionCount, selection[2],"","", moneyDeposited, session);//Adds their type of action, their action count (sessionCount),the account they deposited the funds to, the amount of money deposited, and the structure used to store all that information in.
                                                    updateDatabase(account, &numCustomers);//Updates entire database with new information.

                                                }

                                            }

                                        
                                    }

                                }

                                else if (selection[1]==4){//Transaction History read out

                                    clrscr();
                                    userMenu(account[x].chequing,account[x].savings,account[x].other,account[x].fName,account[x].lName,account[x].ODP);
                                    
                                    for (n=0;n<=sessionCount;n++){

                                        printf("%s\n",session[n].action);

                                    }

                                    pause();

                                }

                                else if (selection[1]==5){//"Kicks" user out of the "logged in user" menu, by flipping 2 booleans that control the menu levels. User can now log in or make a new user.
                                    
                                    logout=true;
                                    cont = false;
                                    printf("Thank you for your business, and have a great day!\n");
                                    pause();

                                }

                                else if (selection[1]==0){//Quit
                                
                                    endMessage();

                                }

                        }
                    }
                }
            }
        }

        else if (selection[0]==2){//User intends to make a new user under their name.

            printf("Please enter your first name: ");
            scanf("%s",newFname);
            printf("Please enter your last name: ");
            scanf("%s",newLname);
            printf("Please enter your new card number (Must be between 10000000 and 99999999): ");
            scanf("%i",&enteredCard);

            for (x=0;x<=numCustomers;x++){//Loops through to see  if card is already in database

                if(enteredCard==account[x].card){

                    invalidCard = true;
                    break;

                }

            }

            while (enteredCard>99999999||enteredCard<10000000||invalidCard==true){//Error traps card number, so it is 8 digits, and another card number that is exactly like it doesn't exist

                if (invalidCard==true){

                    printf("That card is already registered in our system!\n");

                }
                printf("Please enter a valid card number: ");
                scanf("%i",&enteredCard);

                for (x=0;x<=numCustomers;x++){

                    if(enteredCard==account[x].card){

                        invalidCard = true;
                        break;

                    }

                    else{

                        invalidCard = false;

                    }

                }

            }


            printf("Please enter your 4-digit PIN number (Must be between 1000 and 9999): ");
            scanf("%i",&enteredPin);

            while (enteredPin>9999||enteredPin<1000){//Error traps PIN.

                printf("Please enter a valid PIN: ");
                scanf("%i",&enteredPin);

            }

            printf("Would you like to add overdraft protection to your chequing account?(1 for yes OR 0 for no): ");
            scanf("%i",&selection[4]);

            while(selection[4]==1&&selection[4]==0){//Error traps ODP selection.

                printf("Please enter a proper input: ");
                scanf("%i",&selection[4]);

            }

            if (selection[4]==1){

                ODP = true;

            }

            else if (selection[4]==0){

                ODP = false;

            }

            addCustomer(&numCustomers, newFname, newLname,enteredCard, enteredPin, ODP, account); //Adds new user to array of Customer structure.
            updateDatabase(account, &numCustomers);//Writes everything to the database.
            clrscr();

            printf("What would you like to do?:\n\n1.\tStart your banking experience\n\n0.\tQuit\n\nCHOICE: ");
            scanf("%i",&selection[0]);

            while (selection[0]==0&&selection[0]==1){//Error trapping selection[0]

                printf("Please enter a proper input: ");
                scanf("%i",&selection[0]);
                
            }

            if (selection[0]==0){

                endMessage();      

            }

        }

        else if (selection[0]==0){
            endMessage();
        }

    }

    return 0;

}
