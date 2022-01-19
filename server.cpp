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


#include "server.h"

//Synchronization
pthread_mutex_t lock;

//Number of clients
int num_clients = 0;

//Array of clients
client clients[MAX_CLIENTS];

bool running;

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
	// pthread_mutex_lock(&lock);
	// for(int i = 0; i < MAX_CLIENTS; i++) {
	// 	if(clients[i].active) {
	// 		pthread_join(clients[i].client, NULL);
	// 	}
	// }
	// pthread_mutex_unlock(&lock);

	//Properly destroy mutex and return
	pthread_mutex_destroy(&lock);
	pthread_exit(0);
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

	running = true;

	cout << "Starting Server on Port " << port << " With ip " << remote_ip << endl;

	//Generally address struct and size
	struct sockaddr_in address, client_address;

	int asize = sizeof(address);
	int client_size;

	//general socket used to create client sockets
	int master_socket = socket(AF_INET, SOCK_STREAM, 0);

	if(master_socket == 0) {
		cout << "Failed to Create Socket" << endl;
		free(remote_ip);
		free(port);
		pthread_exit(0);
	}

	int flags = fcntl(master_socket, F_GETFL);
	fcntl(master_socket, F_SETFL, flags | O_NONBLOCK);

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
		exit(1);
	}

	//listen for connections
	int lres = listen(master_socket, MAX_CLIENTS);

	if(lres < 0) {
		cout << "Failed to Listen" << endl;
		free(remote_ip);
		free(port);
		exit(1);
	}

	client_size = sizeof(client_address);

	cout << "Waiting for connections..." << endl;

	signal(SIGUSR1, handle_exit);
	signal(SIGINT, handle_sigint);

	while(running) {
		//new client connection
		sleep(1);
		int new_client = accept(master_socket, (struct sockaddr*) &client_address, (socklen_t*) &client_size);

		if(new_client < 0) {
			continue;
		}

		int optval = 1;
		int optsize = sizeof(optval);
		int set_sock_opt = setsockopt(new_client, SOL_SOCKET, SO_KEEPALIVE, &optval, optsize);
		int optval2 = 300;
		int optsize2 = sizeof(optval2);
		int set_keep_alive_time = setsockopt(new_client, SOL_TCP, TCP_KEEPIDLE, &optval2, optsize2);

		if(set_sock_opt < 0 || set_keep_alive_time < 0) {
			cout << "Failed to set KEEP_ALIVE for client socket" << endl;
			close(new_client);
			continue;
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

			int index = find_empty_client_index();

			int res = pthread_create(&(clients[index].client), NULL, handle_client, (void*) ca);

			if(res < 0) {
				cout << "Failed to Create Thread" << endl;
			}
			else {
				cout << "Client Acquired" << endl;
				//add the pipe write to the clients
				clients[index].client_write = p[1];
				clients[index].active = true;
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
		const char* command = (input.substr(0, input.find(" "))).c_str();
		if(check_command(command)) {
			char* command_char = (char*) malloc(1024);
			memset(command_char, '\0', 1024);
			strcpy(command_char, input.c_str());
			run_command(command, command_char);
			free(command_char);
		}
		else if(strcmp(command, "") == 0) {
			continue;
		}
		else {
			cout << "Invalid Command" << endl;
		}
	}
}

//TODO: Send command via pipe to client
void run_command(const char* command, char* command_args) {
	pthread_mutex_lock(&lock);
	if(strcmp(command, "exit") == 0) {
		for(int i = 0; i < MAX_CLIENTS; i++) {
				//for each client thread, send it the command via its write pipe
				if(clients[i].active) {
					//cout << "exiting client with ID " << i << "..." << endl;
					write(clients[i].client_write, command_args, 1024);
				}
		}
		kill(getpid(), SIGUSR1);
		free(command_args);
		pthread_mutex_unlock(&lock);
		pthread_exit(0);
	}
	else if(strcmp(command, "run") == 0) {
		//cout << "Running " << command_args[1] << "...." << endl;
		for(int i = 0; i < MAX_CLIENTS; i++) {
			//for each client thread, send it the command via its write pipe
			if(clients[i].active) {
					write(clients[i].client_write, command_args, 1024);
			}
		}
	}
	else if(strcmp(command, "upload") == 0) {
		for(int i = 0; i < MAX_CLIENTS; i++) {
			//for each client thread, send it the command via its write pipe
			if(clients[i].active) {
					write(clients[i].client_write, command_args, 1024);
			}
		}
	}
	else {
		string comarg[20];
		tokenize(command_args, comarg);

		if(strcmp(command, "list") == 0) {
			command_list(comarg);
		}
		else if(strcmp(command, "compile") == 0) {
			command_compile(comarg);
		}
		else if(strcmp(command, "help") == 0) {
			command_help(comarg);
		}
		else {
			cout << "Unknown Command" << endl;
		}
	}
	pthread_mutex_unlock(&lock);
}

//Client thread runs this
void* handle_client(void* arg) {
	//Arguments grabbed and heap element freed
	pthread_detach(pthread_self());
	int connection = ((client_args*) arg)->client_fd;
	int cread = ((client_args*) arg)->client_read;
	free(arg);

	//wait for pipe from command handler
	while(true) {
		char cbuff[1024];
		int res = read(cread, cbuff, 1024);
		if(res < 0) {
			//cout << "Client pipe read error" << endl;
		}
		else {
			string tokens[20];
			tokenize(cbuff, tokens);
			string command = tokens[0];
			if(command.compare("exit\n") == 0 || command.compare("exit") == 0) {
				close(cread);
				command_exit(connection);
			}
			else if(command.compare("upload") == 0) {
				char prog[1024];
				bzero(prog, 1024);
				strcpy(prog, "programs/");
				strcat(prog, tokens[1]);
				if(access(prog, F_OK) != -1) {
					write(connection, cbuff, 1024);
					//TODO
				}
				else {
					cout << "Invalid File: " << tokens[1] << endl;
				}
			}
			else {
				write(connection, cbuff, 1024);
				bzero(cbuff, 1024);
				read(connection, cbuff, 1024);
				cout << cbuff << endl;
			}
			//cout << command << endl;
		}
	}
	pthread_exit(0);
}

/*
Function needs to be called inside of the mutex lock
*/
int find_empty_client_index() {
	for(int i = 0; i < MAX_CLIENTS; i++) {
		if(!clients[i].active) {
			return i;
		}
	}
	return -1;
}

void handle_exit(int sig) {
	cout << "Shutting down server..." << endl;
	running = false;
}

void handle_sigint(int sig) {
	cout << "Force shutting down server..." << endl;
	sleep(1);
	exit(1);
}
