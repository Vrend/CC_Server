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

#include "local_functions.h"

//Confirms that given IP and Port are valid
bool valid_params(const char* ip, char* port) {
	if(strcmp(ip, "localhost") == 0) {
		ip = "127.0.0.1";
	}
	struct sockaddr_in test;
	int ip_address = inet_pton(AF_INET, ip, &(test.sin_addr));
	if(ip_address < 1) {
		return false;
	}

	int port_num = atoi(port);

	if(port_num < 0 || port_num > 65535) {
		return false;
	}

	return true;
}

//Prints usage
void print_usage() {
		cout << "USAGE: ./server [REMOTE_IP] [PORT] [INPUT TYPE]\n" << endl;
}

//Split user input into the options called tokens
void tokenize(string input, string* result) {
	char* token;
	int i = 0;

	char* cinput = strdup(input.c_str());

	token = strtok(cinput, " ");

	while(token != NULL) {
		result[i] = string(token);
		token = strtok(NULL, " ");
		i++;
	}
	free(cinput);
}

//Check if command is a valid command
bool check_command(const char* command) {
	for(int i = 0; i < COMMAND_NUM; i++) {
		if(strcmp(command, commands[i].c_str()) == 0) {
			return true;
		}
	}
	return false;
}

void print_license() {
	cout <<  "\nCC_Server version " << VERSION << ", Copyright (C) 2019 Vrend\nCC_Server comes with ABSOLUTELY NO WARRANTY.\nThis is free software, and you are welcome to redistribute it under certain conditions.\nFor more information, see the LICENSE file and the README.\n" << endl;
}
