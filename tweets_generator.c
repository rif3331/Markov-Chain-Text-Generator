#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "markov_chain.h"

#define FILE_PATH_ERROR "Error: incorrect file path"
#define NUM_ARGS_ERROR "Usage: invalid number of arguments"
#define DELIMITERS " \n\t\r"
#define BEGIN_ASSIGNMENT -1
#define NUMBER_ARG0 2
#define NUMBER_ARG1 3
#define NUMBER_ARG2 4
#define MAX_COUNT_TO_ROW 1000
#define RADIX 10
#define MAX_LENGTH_TWEET 20

/**
 * Fills the MarkovChain database with data read from a file.
 * @param fp Pointer to the input file.
 * @param words_to_read Number of words to read from the file.
 * @param markov_chain Pointer to the MarkovChain.
 * @return 0 on success, 1 on failure.
 */
int fill_database(FILE *fp, int words_to_read, MarkovChain *markov_chain) {
    // Read a line from the input file
    char my_string[MAX_COUNT_TO_ROW];
    while (fgets(my_string, MAX_COUNT_TO_ROW, fp)) {
        // Tokenize the line using the defined delimiters
        char *current_token = strtok(my_string, DELIMITERS);
        while (current_token != NULL && words_to_read != 0) {
            // Get the next token
            char *next_token = strtok(NULL, DELIMITERS);
            // Add the current token to the MarkovChain database
            Node *current_node = add_to_database(markov_chain, current_token);
            if (current_node == NULL) {
                return 1; // Return 1 if addition fails
            }

            // If the current token does not end with
            // a period, add the next token to the database
            if (current_token[strlen(current_token) - 1] != '.') {
                Node *next_node = add_to_database(markov_chain, next_token);
                if (next_node == NULL) {
                    return 1; // Return 1 if addition fails
                }
                // Add the next node to the frequency list of the current node
                int x = add_node_to_frequency_list(current_node->data,
                    next_node->data);
                if (x == 0) {
                    return 1; // Return 1 if addition fails
                }
            }
            // Move to the next token
            current_token = next_token;
            words_to_read--;
        }
    }
    return 0; // Return 0 on success
}

/**
 * Main function to execute the Markov Chain tweet generation.
 * @param argc Number of command-line arguments.
 * @param argv Array of command-line arguments.
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on failure.
 */
int main(int argc, char *argv[]) {
    // Check for valid number of arguments
    if (argc != NUMBER_ARG1 + 1 && argc != NUMBER_ARG2 + 1) {
        printf("%s", NUM_ARGS_ERROR); // Print error message if invalid
        return EXIT_FAILURE;
    }
    // Initialize random seed from command-line argument
    unsigned int seed = (unsigned int)strtol(argv[1], NULL, RADIX);
    srand(seed);
    // Open the input file
    FILE *in_file = fopen(argv[NUMBER_ARG1], "r");
    if (in_file == NULL) {
        // Print error message and exit if file opening fails
        printf("%s", FILE_PATH_ERROR);
        return EXIT_FAILURE;
    }
    // Initialize the MarkovChain
    MarkovChain markov_chain = {NULL};
    // Determine the number of words to read from the file
    int counter_words = BEGIN_ASSIGNMENT;
    if (argc == NUMBER_ARG2 + 1) {
        counter_words = strtol(argv[NUMBER_ARG2], NULL, RADIX);
    }
    // Fill the MarkovChain database with data from the file
    int succsess = fill_database(in_file, counter_words, &markov_chain);
    fclose(in_file); // Close the input file

    // Check if filling the database was successful
    if (succsess == 1) {
        MarkovChain* x = &markov_chain;
        free_database(&x); // Free the database on failure
        return EXIT_FAILURE;
    }
    // Generate and print tweets
    int count_tweets = strtol(argv[NUMBER_ARG0], NULL, RADIX);
    for (int i = 1; i <= count_tweets; i++) {
        printf("Tweet %d: ", i);
        generate_tweet(get_first_random_node(&markov_chain),
            MAX_LENGTH_TWEET);}
    MarkovChain* x = &markov_chain;// Free the database
    free_database(&x);
    return EXIT_SUCCESS; // Return success
}