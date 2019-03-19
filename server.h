#include "dependencies.h"
#include "structs.h"
#include "constants.h"
#include "local_functions.h"

// Functions

void* initialize_client_handler(void* arg);

void* initialize_terminal(void* arg);

void run_command(string command, string* command_args);

void* handle_client(void* arg);
