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

void command_run(string* args) {
	int num = sizeof(args)/sizeof(args[0]);
	char* arg_conv[num];
	for(int i = 0; i < num; i++) {
		arg_conv[i] = const_cast<char*>(args[i].c_str());
	}
	int ret = fork();
	if(ret == 0) {
		char* prog = strcat(const_cast<char*>("programs/"), arg_conv[0]);
		execv(prog, arg_conv);
		cout << "Error running program" << endl;
		exit(-1);
	}
}

//Split user input into the options called tokens
void tokenize(char* input, string* result) {
	char* token;
	int i = 0;

	token = strtok(input, " ");
	
	while(token != NULL) {
		result[i] = string(token);
		token = strtok(NULL, " ");
		i++;
	}
}
