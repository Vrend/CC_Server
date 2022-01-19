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
#include "structs.h"
#include "constants.h"
#include "local_functions.h"
#include "command_functions.h"

// Functions

void* initialize_client_handler(void* arg);

void* initialize_terminal(void* arg);

void run_command(const char* command, char* command_args);

void* handle_client(void* arg);

void handle_exit(int sig);

void handle_sigint(int sig);

int find_empty_client_index();
