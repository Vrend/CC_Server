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
		cout << "USAGE: ./server [REMOTE_IP] [PORT] [INPUT TYPE]" << endl;
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
bool check_command(string command) {
	for(int i = 0; i < COMMAND_NUM; i++) {
		if(command.compare(commands[i]) == 0) {
			return true;
		}
	}
	return false;
}
