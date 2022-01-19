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

#include "commands.h"

void command_run(char* args) {
	string tokens[30];

	bool changed_pos = false;
	int num = tokenize(args, tokens);
	char* arg_conv[num];
	for(int i = 0; i < num; i++) {
		arg_conv[i] = const_cast<char*>(tokens[i].c_str());
	}
	arg_conv[num] = NULL;

	char* program = strstr(arg_conv[1], ".py");

	if(program != NULL) {
		program = arg_conv[1];
		arg_conv[0] = const_cast<char*>("python");
		changed_pos = true;
	}
	else {
		program = strstr(arg_conv[1], ".pl");
		if(program != NULL) {
			program = arg_conv[1];
			arg_conv[0] = const_cast<char*>("perl");
			changed_pos = true;
		}
		else {
			program = strstr(arg_conv[1], ".rb");
			if(program != NULL) {
				program = arg_conv[1];
				arg_conv[0] = const_cast<char*>("ruby");
				changed_pos = true;
			}
		}
	}

	int ret = fork();
	if(ret == 0) {
		char prog[1024];
		bzero(prog, 1024);

		if(changed_pos) {
			cout << "running here" << endl;
			strcpy(prog, arg_conv[0]);
			char new_prog[1024];
			bzero(new_prog, 1024);
			strcpy(new_prog, "programs/");
			strcat(new_prog, arg_conv[1]);
			arg_conv[1] = new_prog;

			cout << prog << endl;
			cout << arg_conv[1] << endl;
		}
		else {
			strcpy(prog, "programs/");
			strcat(prog, arg_conv[1]);

			if(access(prog, F_OK) != -1) {
				arg_conv[1] = prog;
			}
			else {
				bzero(prog, 1024);
				strcpy(prog, arg_conv[1]);
			}

			for(int i = 0; i < num; i++) {
				arg_conv[i] = arg_conv[i+1];
			}
		}

		execvp(prog, arg_conv);
		cout << "Error running program, " << strerror(errno) << endl;
		exit(-1);
	}
}

void get_file(char* name, int fd) {
	
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
