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

#include "command_functions.h"

void command_exit(int fd) {
	close(fd);
	pthread_exit(0);
}

void command_help(string* args) {

	string comm = args[1];

	if(comm.compare("run") == 0) {
		cout << "Runs a Program\nUSAGE: run [PROGRAM]" << endl;
	}
	else if(comm.compare("upload") == 0) {
		cout << "Uploads a Program to all clients\nUSAGE: upload [FILE]" << endl;
	}
	else if(comm.compare("list") == 0) {
		cout << "Lists all connected clients\nUSAGE: list (OS)" << endl;
	}
	else if(comm.compare("compile") == 0) {
		cout << "Compiles a Client\nUSAGE: compile (flags)" << endl;
	}
	else if(comm.compare("exit") == 0) {
		cout << "Exits server and closes all client connections\nUSAGE: exit" << endl;
	}
	else {
		cout << "List of Commands: help, run, upload, list, compile, exit\nType help [COMMAND] for more information" << endl;
	}
}

void command_run(string* args, int fd) {

}

void command_upload(string* args, int fd) {
	//TODO
}

void command_list(string* args) {
	//TODO
}

void command_compile(string* args) {
	//TODO
}
