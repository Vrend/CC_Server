/*
Copyright (C) 2019 John Broderick

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

#include "commands.h"

void command_run(char* args) {
	string tokens[20];
	int num = tokenize(args, tokens);
	char* arg_conv[num];
	for(int i = 0; i < num; i++) {
		arg_conv[i] = const_cast<char*>(tokens[i].c_str());
	}
	arg_conv[num] = NULL;
	int ret = fork();
	if(ret == 0) {
		char prog[1024];
		bzero(prog, 1024);
		strcpy(prog, "programs/");
		strcat(prog, arg_conv[1]);
		execv(prog, arg_conv);
		cout << "Error running program, " << strerror(errno) << endl;
		exit(-1);
	}
}

//Split user input into the options called tokens
int tokenize(char* input, string* result) {
	char* token;
	int i = 0;
	token = strtok(input, " ");
	while(token != NULL) {
		result[i] = string(token);
		token = strtok(NULL, " ");
		i++;
	}
	return i;
}
