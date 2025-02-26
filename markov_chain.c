#include "markov_chain.h"
#include <string.h>

/**
 * Get random number between 0 and max_number [0, max_number).
 * @param max_number Maximum value for the random number.
 * @return Random number
 */
int get_random_number(int max_number)
{
    // Generate a random number between 0 and max_number-1
    return rand() % max_number;
}

/**
 * Get a node from the MarkovChain database containing the specified data.
 * @param markov_chain The MarkovChain to search within.
 * @param data_ptr The data to search for.
 * @return Node containing the data or NULL if not found.
 */
Node* get_node_from_database(MarkovChain *markov_chain, char *data_ptr)
{
    // Check if the database is NULL
    if (markov_chain->database == NULL) {
        return NULL;
    }
    LinkedList *database = markov_chain->database;
    // Check for empty list before iterating
    if (database->first == NULL || database->size <= 0) {
        return NULL;
    }
    // Start iterating from the first node
    Node *current_node = markov_chain->database->first;
    // Iterate through the linked list
    for (int i = 0; current_node != NULL; i++) {
        // Compare the data of the current node with the provided data
        if (!strcmp(current_node->data->data, data_ptr)) {
            // Return the node if data matches
            return current_node;
        }
        // Move to the next node
        current_node = current_node->next;
    }
    // Return NULL if the data is not found
    return NULL;
}

/**
 * Add a new node to the MarkovChain database or return an
 * existing one if it already exists.
 * @param markov_chain The MarkovChain to add to.
 * @param data_ptr The data for the new node.
 * @return The added or existing node.
 */
Node* add_to_database(MarkovChain *markov_chain, char *data_ptr) {
    // Check if the node with the given data already exists
    Node *is_find = get_node_from_database(markov_chain, data_ptr);
    if (is_find != NULL) { return is_find; }
    LinkedList *new_database = NULL;
    // If the database is NULL, allocate memory for a new LinkedList
    if (markov_chain->database == NULL) {
        new_database = calloc(1, sizeof(LinkedList));
        if (new_database == NULL) {
            printf("%s", ALLOCATION_ERROR_MASSAGE);return NULL;}
        // Initialize the new database
        markov_chain->database = new_database;new_database->size = 0;
        new_database->first = NULL;new_database->last = NULL;}
    // Allocate memory for a new MarkovNode
    MarkovNode *new_markov_node = calloc(1, sizeof(MarkovNode));
    if (new_markov_node == NULL) {free(new_database);
        printf("%s", ALLOCATION_ERROR_MASSAGE);return NULL;}
    // Allocate memory for the data in the new MarkovNode
    int len_data = strlen(data_ptr) + 1;
    new_markov_node->data = calloc(len_data, sizeof(char));
    if (new_markov_node->data == NULL) {
        free(new_database);free(new_markov_node);
        printf("%s", ALLOCATION_ERROR_MASSAGE);return NULL;}
    // Copy the data to the new MarkovNode
    strcpy(new_markov_node->data, data_ptr);
    // Allocate memory for the frequency list in the new MarkovNode
    new_markov_node->frequency_list = calloc(1, sizeof(MarkovNodeFrequency));
    if (new_markov_node->frequency_list == NULL) {
        free(new_database);free(new_markov_node);
        free(new_markov_node->data);printf("%s", ALLOCATION_ERROR_MASSAGE);
        return NULL;}

    new_markov_node->size_NodesFreq = 0;// Initialize the new MarkovNode
    new_markov_node->frequency_list->markov_node = NULL;
    new_markov_node->frequency_list->frequency = 0;
    // Add the new MarkovNode to the database
    int sucss_add = add(markov_chain->database, new_markov_node);
    if (sucss_add != 0) {
        free(new_database);free(new_markov_node);
        free(new_markov_node->data);free(new_markov_node->frequency_list);
        printf("%s", ALLOCATION_ERROR_MASSAGE);return NULL;}
    return markov_chain->database->last;// Return the newly added node
}
/**
 * Add a node to the frequency list of another node.
 * @param first_node The node whose frequency list will be updated.
 * @param second_node The node to add to the frequency list.
 * @return 1 if successful, 0 if reallocation fails.
 */
int add_node_to_frequency_list(MarkovNode *first_node,
    MarkovNode *second_node) {
    // Traverse the frequency list to find if second_node already exists
    MarkovNodeFrequency *counter_pointer = first_node->frequency_list;
    for (int i = 0; i < first_node->size_NodesFreq; i++) {
        // If the node already exists in the frequency list
        if (strcmp(counter_pointer[i].markov_node->data,
            second_node->data) == 0) {
            counter_pointer[i].frequency++;
            return 1; // Node already exists, frequency incremented
        }
    }

    // Reallocate memory for the frequency list if necessary
    MarkovNodeFrequency *temp = realloc(first_node->frequency_list,
                                        (first_node->size_NodesFreq + 1)
                                        * sizeof(MarkovNodeFrequency));
    if (temp == NULL) {
        printf("%s", ALLOCATION_ERROR_MASSAGE);
        return 0; // Realloc failed
    }
    first_node->frequency_list = temp;

    // Add the new node to the frequency list
    first_node->frequency_list[first_node->size_NodesFreq].markov_node
    = second_node;
    first_node->frequency_list[first_node->size_NodesFreq].frequency = 1;
    first_node->size_NodesFreq++;
    return 1; // Successfully added new entry
}

/**
 * Free the memory allocated for the MarkovChain database.
 * @param ptr_chain Pointer to the MarkovChain.
 */
void free_database(MarkovChain **ptr_chain) {
    if (ptr_chain == NULL || *ptr_chain == NULL
        || (*ptr_chain)->database == NULL) {
        return;
    }
    LinkedList *database = (*ptr_chain)->database;
    Node *current_node = database->first;
    Node *next_node;
    // Traverse the database and free the memory for each node and its data
    while (current_node != NULL) {
        next_node = current_node->next;
        MarkovNode *markov_node = current_node->data;
        if (markov_node != NULL) {
            free(markov_node->data);
            free(markov_node->frequency_list);free(markov_node);
        }
        free(current_node);
        current_node = next_node;
    }
    // Free the memory for the database itself
    free(database);
    (*ptr_chain)->database = NULL;
}

/**
 * Get the first random node from the MarkovChain database.
 * @param markov_chain The MarkovChain to get the node from.
 * @return A pointer to the first random MarkovNode
 * or NULL if the database is empty.
 */
MarkovNode* get_first_random_node(MarkovChain *markov_chain) {
    LinkedList *database = markov_chain->database;
    if (database == NULL || database->size == 0) {
        return NULL; // Database is empty
    }

    Node *current_node = NULL;
    int attempts = 0;

    // Loop until a valid node is found
    while (true) {
        current_node = database->first;
        int i = get_random_number(markov_chain->database->size);
        for (int j = 0; j < i; j++) {
            current_node = current_node->next;
        }
        // Break if the node does not end with a period
        if (current_node->data->
            data[strlen(current_node->data->data) - 1] != '.') {
            break;
        }
        attempts++;
        // Return NULL if all nodes end with a period
        if (attempts >= markov_chain->database->size) {
            return NULL;
        }
    }
    return current_node->data;
}

/**
 * Get the next random node from the frequency list of the current node.
 * @param cur_markov_node The current MarkovNode.
 * @return A pointer to the next random MarkovNode.
 */
MarkovNode* get_next_random_node(MarkovNode *cur_markov_node) {
    int count = 0;
    int right_index = 0;

    // Calculate the total frequency count
    for (int i = 0; i < cur_markov_node->size_NodesFreq; i++) {
        count += (*cur_markov_node).frequency_list[i].frequency;
    }
    // Get a random index based on the frequency count
    int i = get_random_number(count);
    bool break2 = false;
    // Find the corresponding node for the random index
    for (int j = 0; j < cur_markov_node->size_NodesFreq; j++) {
        for (int k = 0; k <
            (*cur_markov_node).frequency_list[j].frequency; k++) {
            if (break2 == true) {
                break;
            }
            if (i == 0) {
                right_index = j;
                break2 = true;
                break;
            }
            i--;
        }
    }

    return (*cur_markov_node).frequency_list[right_index].markov_node;
}

/**
 * Generate a tweet starting from the given node and with a maximum length.
 * @param first_node The starting MarkovNode.
 * @param max_length The maximum number of nodes in the tweet.
 */
void generate_tweet(MarkovNode *first_node, int max_length) {
    MarkovNode *current_markov_node = first_node;

    // Generate the tweet by traversing the MarkovChain
    for (int i = 0; i < max_length; i++) {
        // Print the current node's data if it does not end with a period
        if (current_markov_node->
            data[strlen(current_markov_node->data) - 1] != '.') {
            printf("%s ", current_markov_node->data);
        }

        // Print the current node's data and end
        // the tweet if it ends with a period
        if (current_markov_node->
            data[strlen(current_markov_node->data) - 1] == '.') {
            printf("%s\n", current_markov_node->data);
            break;
        }

        // Get the next random node
        current_markov_node = get_next_random_node(current_markov_node);
    }
}
