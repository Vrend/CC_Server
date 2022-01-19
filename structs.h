/*
Copyright (C) 2019 Vrend

This file is part of CC_Server.

CC_Server is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

CC_Server is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
long with CC_Server.  If not, see <https://www.gnu.org/licenses/>.
*/

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
	bool active = false;
} client;

#endif
// End of structs
