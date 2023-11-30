#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "account.h"
#include "string_parser.h"

int num_accounts;
account accounts[MAX_ACCOUNTS];
pthread_t worker_threads[MAX_ACCOUNTS];

pthread_mutex_t balance_mutex = PTHREAD_MUTEX_INITIALIZER;

// Function to find an account by its account number
account* find_account_by_number(const char* account_number) 
{
    for (int i = 0; i < num_accounts; i++) 
    {
        if (strcmp(accounts[i].account_number, account_number) == 0) 
        {
            return &accounts[i]; // Return a pointer to the found account
        }
    }

    return NULL; // Account not found
}

// Function to process a transaction for a single account
void process_transaction(command_line transaction_tokens) {
    char action;
    char account_number[17];
    char password[9];
    char dest_account[17];
    double transfer_amount;

    action = transaction_tokens.command_list[0][0];
    strcpy(account_number, transaction_tokens.command_list[1]);
    strcpy(password, transaction_tokens.command_list[2]);

    // Find the source account
    account* active_account = find_account_by_number(account_number);

    // Validate password
    if (strcmp(password, active_account->password) != 0) 
    {
        // Invalid password, handle accordingly
        return;
    }

    // Implement logic for processing different transaction types
    switch (action) 
    {
        case 'T':
            // Transfer funds
            // Find the destination account
            strcpy(dest_account, transaction_tokens.command_list[3]);
            account* dest_acc = find_account_by_number(dest_account);

            // Validate destination account
            if (dest_acc == NULL) 
            {
                // Handle invalid destination account
                break;
            }

            // Get transfer amount
            transfer_amount = atof(transaction_tokens.command_list[4]);

            // Withdraw from source account
            pthread_mutex_lock(&balance_mutex);
            active_account->balance -= transfer_amount;
            active_account->transaction_tracker += transfer_amount;  // Increment the transaction tracker
            pthread_mutex_unlock(&balance_mutex);

            // Deposit to destination account
            pthread_mutex_lock(&balance_mutex);
            dest_acc->balance += transfer_amount;
            pthread_mutex_unlock(&balance_mutex);
            break;

        case 'C':
            // Check balance
            // Print or store the account balance
            //printf("%s balance: %.2f\n", active_account->account_number, active_account->balance);
            break;

        case 'D':
            // Get transfer amount
            transfer_amount = atof(transaction_tokens.command_list[3]);

            // Deposit
            pthread_mutex_lock(&balance_mutex);
            active_account->balance += transfer_amount;
            active_account->transaction_tracker += transfer_amount;  // Increment the transaction tracker
            pthread_mutex_unlock(&balance_mutex);
            break;

        case 'W':
            
            // Get transfer amount
            transfer_amount = atof(transaction_tokens.command_list[3]);

            // Withdraw
            pthread_mutex_lock(&balance_mutex);
            active_account->balance -= transfer_amount;
            active_account->transaction_tracker += transfer_amount;  // Increment the transaction tracker
            pthread_mutex_unlock(&balance_mutex);
            break;

        default:
            // Handle invalid transaction type
            break;
    }
}

// Function to iterate over every line
void* process_worker(void* arg) 
{
    account* acc = (account*)arg;

    for (int i = 1; i <= acc->start_line; i++) {
        if (fgets(acc->current_line, MAX_LINE, acc->input_file) == NULL) {
            // Handle unexpected end of file
            return NULL;
        }
    }

    // Read and process every transaction
    for (int i = 0; i <= acc->num_lines; i++) 
    {
        // Use string parser to tokenize the transaction
        command_line transaction_tokens = str_filler(acc->current_line, " ");

        // Validate the number of transaction_tokens
        if (transaction_tokens.num_token < 3) 
        {
            // Free memory allocated for transaction_tokens
            free_command_line(&transaction_tokens);

            continue;
        }
        // Process each tokenized transaction
        process_transaction(transaction_tokens);

        fflush(stdout);

        // Free memory allocated for transaction_tokens
        free_command_line(&transaction_tokens);

        // Move to the next line
        if (fgets(acc->current_line, MAX_LINE, acc->input_file) == NULL) {
            // Handle unexpected end of file
            break;
        }
    }

    fclose(acc->input_file);
    free(acc->current_line);

    printf("Finished lines %d to %d\n", acc->start_line, acc->start_line+acc->num_lines);

    return NULL;
}

// Function to update the balance for all accounts accross different threads
void* process_update_balance(void* arg) 
{
    // account* acc = (account*)arg;
    for (int i = 0; i < num_accounts; i++) 
    {
        // Calculate the additional balance based on reward rate and transaction tracker
        double additional_balance = accounts[i].reward_rate * accounts[i].transaction_tracker;

        // Update the account balance
        pthread_mutex_lock(&balance_mutex);
        accounts[i].balance += additional_balance;
        pthread_mutex_unlock(&balance_mutex);

        // Reset the transaction tracker for the next round
        pthread_mutex_lock(&balance_mutex);
        accounts[i].transaction_tracker = 0.0;
        pthread_mutex_unlock(&balance_mutex);
    }

    return NULL;
}

int main(int argc, char *argv[]) 
{
    // Check command line arguments and open the input file
    if (argc != 2)
    {
        fprintf(stderr, "Usage: %s input_file\n", argv[0]);
        exit(1);
    }

    FILE *input_file = fopen(argv[1], "r");
    if (input_file == NULL) 
    {
        perror("Error opening input file");
        return EXIT_FAILURE;
    }

    // Read the total number of accounts
    char line[MAX_LINE];
    fgets(line, MAX_LINE, input_file);

    // Initialize account array
    num_accounts = atoi(line);

    // Get and set account information
    for (int i = 0; i < num_accounts; i++) 
    {
        fgets(line, MAX_LINE, input_file);

        // Use string parser to tokenize the account information
        command_line index_line = str_filler(line, " ");

        // Check if current line tells us which account index it is (if it is an index)
        if (index_line.num_token < 2 || strcmp(index_line.command_list[0], "index") != 0) 
        {
            // Free memory allocated for index_line
            free_command_line(&index_line);
            fclose(input_file);
            return EXIT_FAILURE;
        }

        // Number of given accounts exceeds number of accounts given
        if (atoi(index_line.command_list[1]) > num_accounts) 
        {
            // Free memory allocated for index_line
            free_command_line(&index_line);
            break;
        }

        // Free memory allocated for index_line
        free_command_line(&index_line);

        // Duplicate the account number line into the account info string
        fgets(line, MAX_LINE, input_file);
        char* account_info = strdup(line);

        // Concatenate the rest of the info for the account to be created
        for (int j = 1; j < 4; j++)
        {
            fgets(line, MAX_LINE, input_file);

            // Ensure that account_info has enough space for the new line
            size_t current_length = strlen(account_info);
            size_t new_line_length = strlen(line);

            // Resize the account_info buffer
            account_info = (char*)realloc(account_info, (current_length + new_line_length + 1) * sizeof(char));

            // Check if realloc succeeded
            if (account_info == NULL) {
                // Handle memory allocation failure
                fclose(input_file);
                free(account_info);
                return EXIT_FAILURE;
            }

            // Concatenate the new line to account_info
            strcat(account_info, line);
        }

        // Tokenize account info
        command_line account_info_tokens = str_filler(account_info, "\n");
        free(account_info);

        // Validate the number of account_info_tokens
        if (account_info_tokens.num_token < 4) 
        {
            // Handle invalid account information
            fclose(input_file);
            // Free memory allocated for account_info_tokens
            free_command_line(&account_info_tokens);
            return EXIT_FAILURE;
        }

        // Parse account info into account struct
        strcpy(accounts[i].account_number, account_info_tokens.command_list[0]);
        strcpy(accounts[i].password, account_info_tokens.command_list[1]);
        accounts[i].balance = atof(account_info_tokens.command_list[2]);
        accounts[i].reward_rate = atof(account_info_tokens.command_list[3]);
        accounts[i].transaction_tracker = 0.0;

        // Initialize Mutual Exclusion variable
        pthread_mutex_init(&accounts[i].ac_lock, NULL);

        // Open the input file for each account
        accounts[i].input_file = fopen(argv[1], "r");
        if (accounts[i].input_file == NULL) {
            perror("Error opening input file");
            // Handle the error
            return EXIT_FAILURE;
        }

        // Free memory allocated for account_info_tokens
        free_command_line(&account_info_tokens);
    }

    // Get the total number of lines in the file
    int num_transactions = 0;
    while (fgets(line, MAX_LINE, input_file) != NULL) 
    {
        num_transactions++;
    }

    // Close the input file
    fclose(input_file);

    // Calculate the number of lines per slice
    int num_lines = num_transactions / num_accounts;
    int remainder = num_transactions % num_accounts;

    // Create and start a thread for each worker
    for (int i = 0; i < num_accounts; i++)
    {
        // Assign the starting line for each worker
        accounts[i].start_line = i * num_lines + 2 + num_accounts * 5;

        // Calculate the number of lines to process for each worker
        accounts[i].num_lines = num_lines - 1;

        // Allocate memory for the current line
        accounts[i].current_line = (char*)malloc((MAX_LINE + 1) * sizeof(char));
    }

    // Let last thread handle remaining lines
    if (remainder > 0)
    {
        accounts[num_accounts - 1].num_lines += remainder;
    }
    
    // Create the worker threads
    for (int i = 0; i < num_accounts; i++)
    {
        pthread_create(&worker_threads[i], NULL, process_worker, (void*)&accounts[i]);
    }

    // Join worker threads
    for (int i = 0; i < num_accounts; i++) {
        pthread_join(worker_threads[i], NULL);
    }

    // Create bank thread
    pthread_t bank_thread;

    // Update balances for all account
    pthread_create(&bank_thread, NULL, process_update_balance, (void*)accounts);

    // Join bank thread
    pthread_join(bank_thread, NULL);

    // Print the final balances to the output file
    FILE* output_file = fopen("output-2.txt", "w");
    if (output_file == NULL)
    {
        perror("Error opening output file");
        return EXIT_FAILURE;
    }

    for (int i = 0; i < num_accounts; i++) 
    {
        fprintf(output_file, "%d balance:\t%.2f\n\n", i, accounts[i].balance);
    }

    // Close the output file
    fclose(output_file);

    // Clean up and free resources
    for (int i = 0; i < num_accounts; i++) {
        pthread_mutex_destroy(&accounts[i].ac_lock);
        //fclose(accounts[i].input_file);
        free(accounts[i].current_line);
    }

    return 0;
}