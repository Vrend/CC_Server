#include "dependencies.h"

#ifndef STRUCTS_H
#define STRUCTS_H

// Structs

//Arguments to pass to the client handler
typedef struct client_handler_initialize_args {
	char arg1[20];
	char arg2[20];
} server_initialize_args;

//Arguments to pass to a client thread
typedef struct client_args {
	int client_fd;
	int client_read;
} client_args;

//Client object
typedef struct client {
	pthread_t client;
	int client_write;
} client;

#endif
// End of structs
