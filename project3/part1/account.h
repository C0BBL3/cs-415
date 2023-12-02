#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "string_parser.h"

#define MAX_ACCOUNTS 16
#define MAX_LINE 256

typedef struct {
    char account_number[17];
    char password[9];
    double balance;
    double reward_rate;
    double transaction_tracker;
} account;

// Function to find an account by its account number
account* find_account_by_number(const char* account_number);

// Function to check if transaction is a valid transaction
int check_if_transaction_is_valid(command_line transaction_token);

// Function to process a transaction for a single account
void process_transaction(command_line transaction_tokens);

// Function to update the balance for all accounts accross different threads
void* process_update_balance();

#endif /* ACCOUNT_H_ */