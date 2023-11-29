#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"

#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100

account accounts[MAX_ACCOUNTS];
int num_accounts;

// Function to process a transaction for a single account
void process_transaction(account* acc, char* transaction) {
    char action;
    char src_account[17];
    char password[9];
    char dest_account[17];
    double transfer_amount;

    sscanf(transaction, "%c %s %s %lf", &action, src_account, password, &transfer_amount);

    pthread_mutex_lock(&acc->ac_lock);  // Lock the account for thread safety

    // Validate password
    if (strcmp(password, acc->password) != 0) {
        // Invalid password, handle accordingly
        pthread_mutex_unlock(&acc->ac_lock);
        return;
    }

    // Implement logic for processing different transaction types
    switch (action) {
        case 'T':
            // Transfer funds
            // Withdraw from source account
            acc->balance -= transfer_amount;
            
            // Find the destination account
            account* dest_acc = find_account_by_number(accounts, num_accounts, dest_account);
            
            if (dest_acc != NULL) {
                // Deposit to destination account
                pthread_mutex_lock(&dest_acc->ac_lock);  // Lock the destination account
                dest_acc->balance += transfer_amount;
                pthread_mutex_unlock(&dest_acc->ac_lock);  // Unlock the destination account
            } else {
                // Handle destination account not found
            }
            break;

        case 'C':
            // Check balance
            // Print or store the account balance
            printf("%s balance: %.2f\n", acc->account_number, acc->balance);
            break;

        case 'D':
            // Deposit
            acc->balance += transfer_amount;
            acc->transaction_tracker += 1.0;  // Increment the transaction tracker
            break;

        case 'W':
            // Withdraw
            if (acc->balance >= transfer_amount) {
                acc->balance -= transfer_amount;
                acc->transaction_tracker += 1.0;  // Increment the transaction tracker
            } else {
                // Handle insufficient funds
            }
            break;

        default:
            // Handle invalid transaction type
            break;
    }

    pthread_mutex_unlock(&acc->ac_lock);  // Unlock the account
}


// Function to update the balance for all accounts
void update_balance(account* accounts, int num_accounts) {
    // Calculate the total transaction tracker for all accounts
    double total_transaction_tracker = 0.0;
    for (int i = 0; i < num_accounts; i++) {
        total_transaction_tracker += accounts[i].transaction_tracker;
    }

    // Update balances based on reward rate and transaction tracker
    for (int i = 0; i < num_accounts; i++) {
        pthread_mutex_lock(&accounts[i].ac_lock);  // Lock the account for thread safety

        // Calculate the additional balance based on reward rate and transaction tracker
        double additional_balance = accounts[i].reward_rate * total_transaction_tracker;

        // Update the account balance
        accounts[i].balance += additional_balance;

        // Reset the transaction tracker for the next round
        accounts[i].transaction_tracker = 0.0;

        pthread_mutex_unlock(&accounts[i].ac_lock);  // Unlock the account
    }
}

int main() {
    // Read input file and initialize accounts
    FILE* inputFile = fopen("input.txt", "r");
    if (inputFile == NULL) {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    account accounts[MAX_ACCOUNTS];
    int num_accounts;

    // Read the total number of accounts
    fscanf(inputFile, "%d", &num_accounts);

    // Read account information
    for (int i = 0; i < num_accounts; i++) {
        fscanf(inputFile, "%*d %s %s %lf %lf", accounts[i].account_number, accounts[i].password,
               &accounts[i].balance, &accounts[i].reward_rate);
        accounts[i].transaction_tracker = 0.0;
        pthread_mutex_init(&accounts[i].ac_lock, NULL);
    }

    // Read and process transactions for each account
    for (int i = 0; i < num_accounts; i++) {
        char transaction[MAX_TRANSACTIONS];
        while (fscanf(inputFile, "%s", transaction) != EOF) {
            process_transaction(&accounts[i], transaction);
        }
    }

    // Close the input file
    fclose(inputFile);

    // Update balances for all accounts
    update_balance(accounts, num_accounts);

    // Print the final balances to the output file
    FILE* outputFile = fopen("output.txt", "w");
    if (outputFile == NULL) {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_accounts; i++) {
        fprintf(outputFile, "%d balance:\t%.2f\n", i, accounts[i].balance);
    }

    // Close the output file
    fclose(outputFile);

    // Clean up and free resources
    for (int i = 0; i < num_accounts; i++) {
        pthread_mutex_destroy(&accounts[i].ac_lock);
    }

    return 0;
}
