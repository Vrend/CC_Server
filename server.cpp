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

#include "server.h"

//Synchronization
pthread_mutex_t lock;

//Number of clients
int num_clients = 0;

//Array of clients
client clients[MAX_CLIENTS];

/* Creates 3 threads, the command handler which waits for input from terminal and sends command to
** clients, terminal thread which waits for input and sends command to command handler thread, and
** server thread which waits for connections, and creates a client thread
**/
int main(int argc, char** argv) {

	print_license();

	//Requires 4 arguments
	if(argc != 4) {
		print_usage();
		return 1;
	}

	//If the ip or port are invalid, throw exception
	if(!valid_params(argv[1], argv[2])) {
		cout << "Invalid Port or IP" << endl;
		print_usage();
		return 1;
	}

	//threads declared
	pthread_t terminal;
	pthread_t client_handler;

	//initiate mutex
	pthread_mutex_init(&lock, NULL);

	//Check the mode of input, currently only terminal is being implemented
	if(strcmp(argv[3], "terminal") == 0 || strcmp(argv[3], "t") == 0) {
		cout << "Running in terminal mode" << endl;
	}
	else if(strcmp(argv[3], "web") == 0 || strcmp(argv[3], "w") == 0) {
		cout << "Running in web mode" << endl;
		cout << "Web mode not currently available" << endl;
		return 1;
	}
	else {
		print_usage();
		return 1;
	}

	//Initializes arguments to pass to the server thread
	client_handler_initialize_args* args = (client_handler_initialize_args*) malloc(sizeof(client_handler_initialize_args));
	strcpy(args->arg1, argv[1]);
	strcpy(args->arg2, argv[2]);


	//creates all three threads
	pthread_create(&client_handler, NULL, initialize_client_handler, (void*) args);
	pthread_create(&terminal, NULL, initialize_terminal, NULL);

	//Once all threads exit, game over
	pthread_join(terminal, NULL);
	pthread_join(client_handler, NULL);

	//Properly destroy mutex and return
	pthread_mutex_destroy(&lock);
	return 0;
}

//Handles gathering clients
void* initialize_client_handler(void* arg) {
	//Arguments that are passed to the server
	server_initialize_args* sia = (server_initialize_args*) arg;
	//IP
	char* remote_ip = (char*) malloc(strlen(sia->arg1)+1);
	//Port
	char* port = (char*) malloc(strlen(sia->arg2)+1);
	strcpy(remote_ip, sia->arg1);
	strcpy(port, sia->arg2);
	free(arg);
	sia = NULL;

	cout << "Starting Server on Port " << port << "With ip " << remote_ip << endl;

	//Generally address struct and size
	struct sockaddr_in address;

	int asize = sizeof(address);

	//general socket used to create client sockets
	int master_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(master_socket == 0) {
		cout << "Failed to Create Socket" << endl;
		free(remote_ip);
		free(port);
		pthread_exit(0);
	}

	//address struct initialization
	address.sin_family = AF_INET;
	address.sin_addr.s_addr = INADDR_ANY;
	address.sin_port = htons(atoi(port));

	//bind the program to the port
	int bres = bind(master_socket, (struct sockaddr*) &address, asize);

	if(bres < 0) {
		cout << "Failed to Bind" << endl;
		free(remote_ip);
		free(port);
		pthread_exit(0);
	}

	//listen for connections
	int lres = listen(master_socket, MAX_CLIENTS);

	if(lres < 0) {
		cout << "Failed to Listen" << endl;
		free(remote_ip);
		free(port);
		pthread_exit(0);
	}

	cout << "Waiting for connections..." << endl;

	while(true) {
		//new client connection
		int new_client = accept(master_socket, (struct sockaddr*) &address, (socklen_t*) &asize);

		if(new_client < 0) {
			cout << "Failed to Acquire Client" << endl;
		}

		//can't have too many connections
		pthread_mutex_lock(&lock);
		if(num_clients == MAX_CLIENTS) {
			close(new_client);
			cout << "Too many clients" << endl;
		}
		else {
			//prepare data to give to the client
			client_args* ca = (client_args*) malloc(sizeof(client_args));

			//create pipe for communication from command handler to client
			int p[2];
			if(pipe(p) < 0) {
				cout << "Error creating command pipe" << endl;
			}

			//client arguments are initialized
			ca->client_fd = new_client;
			ca->client_read = p[0];

			//create client thread
			int res = pthread_create(&(clients[num_clients].client), NULL, handle_client, (void*) ca);
			if(res < 0) {
				cout << "Failed to Create Thread" << endl;
			}
			else {
				cout << "Client Acquired" << endl;
				//add the pipe write to the clients and detach client thread
				clients[num_clients].client_write = p[1];
				pthread_detach(clients[num_clients].client);
				num_clients++;
			}
		}
		pthread_mutex_unlock(&lock);
	}

	//on exit cleanup
	close(master_socket);
	free(remote_ip);
	free(port);
}

//terminal thread
void* initialize_terminal(void* arg) {

	cout << "Starting Terminal Session..." << endl;
	//collect input and check if it's a valid command
	while(true) {
		string input;
		cout << ">> ";
		getline(cin, input);
		string command = input.substr(0, input.find(" "));
		string command_args[20];
		tokenize(input, command_args);
		if(check_command(command)) {
			//run the actual command
			run_command(command, command_args);
		}
		else {
			cout << "Invalid Command" << endl;
		}
	}
}

//TODO: Send command via pipe to command handler
void run_command(string command, string* command_args) {
	pthread_mutex_lock(&lock);
	for(int i = 0; i < num_clients; i++) {
			//for each client thread, send it the command via its write pipe
			write(clients[i].client_write, command_args, sizeof(command_args));
	}
	pthread_mutex_unlock(&lock);

	if(command.compare("exit") == 0) {
		pthread_exit(0);
	}
}

//Client thread runs this
void* handle_client(void* arg) {
	//Arguments grabbed and heap element freed
	int connection = ((client_args*) arg)->client_fd;
	int cread = ((client_args*) arg)->client_read;
	free(arg);

	//wait for pipe from command handler
	while(true) {
		string cbuff[20];
		int res = read(cread, cbuff, sizeof(cbuff));
		if(res < 0) {
			cout << "Client pipe read error" << endl;
		}
		else {
			string command = cbuff[0];

			if(command.compare("exit")) {
				command_exit();
			}
			else if(command.compare("run")) {
				command_run(cbuff);
			}
			else if(command.compare("list")) {
				command_list(cbuff);
			}
			else if(command.compare("upload")) {
				command_upload(cbuff);
			}
			else if(command.compare("run")) {
				command_run(cbuff);
			}
			else if(command.compare("compile")) {
				command_compile(cbuff);
			}
			else {
				cout << "Unknown Command" << endl;
			}
		}
	}
	pthread_exit(0);
}
