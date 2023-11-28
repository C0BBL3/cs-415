#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <pthread.h>
#include "string_parser.h"

#define MAX_ACCOUNTS 10
#define MAX_TRANSACTIONS 100
#define MAX_LINE 256

typedef struct {
    char account_number[17];
    char password[9];
    double balance;
    double reward_rate;
    double transaction_tracker;
    char out_file[64];
    pthread_mutex_t ac_lock;
} account;

// Function to find an account by its account number
account* find_account_by_number(account* accounts, const char* account_number);

// Function to process a transaction for a single account
void process_transaction(account* accounts, account* acc, command_line account_info_tokens);

// Function to update the balance for all accounts
void update_balance(account* accounts);

#endif /* ACCOUNT_H_ */