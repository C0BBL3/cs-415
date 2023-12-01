#ifndef ACCOUNT_H
#define ACCOUNT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include "string_parser.h"

#define MAX_ACCOUNTS 16
#define MAX_LINE 256
#define UPDATE_THRESHOLD 5000

typedef struct {
    char account_number[17];
    char password[9];
    double balance;
    double reward_rate;
    double transaction_tracker;
    FILE* output_file;
    FILE* input_file;
    int start_line;
    int num_lines;
    char* current_line;
    pthread_mutex_t ac_lock;
} account;

int initialize_accounts(char *argv[]); 

// Function to find an account by its account number
account* find_account_by_number(const char* account_number);

// Function to check if transaction is a valid transaction
int check_if_transaction_is_valid(command_line transaction_token);

// Function to process a transaction for a single account
void process_transaction(command_line account_info_tokens);

// Function to iterate over every line
void* process_worker(void* arg);

// Function to update the balance for all accounts accross different threads
void* process_update_balance(void* arg);

#endif /* ACCOUNT_H_ */